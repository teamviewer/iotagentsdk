//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2021 TeamViewer Germany GmbH                                     //
//                                                                                //
// Permission is hereby granted, free of charge, to any person obtaining a copy   //
// of this software and associated documentation files (the "Software"), to deal  //
// in the Software without restriction, including without limitation the rights   //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      //
// copies of the Software, and to permit persons to whom the Software is          //
// furnished to do so, subject to the following conditions:                       //
//                                                                                //
// The above copyright notice and this permission notice shall be included in all //
// copies or substantial portions of the Software.                                //
//                                                                                //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  //
// SOFTWARE.                                                                      //
//********************************************************************************//
#include "TVQtRCPlugin.h"

#include "internal/Chat/Chat.h"

#include "internal/Communication/CommunicationAdapter.h"

#include "internal/Grabbing/Screen/QWindowGrabMethod.h"

#include "internal/InputSimulation/InputSimulator.h"
#include "internal/InputSimulation/XKBMap.h"

#include "internal/Logging/Logging.h"
#include "internal/Logging/LoggingProxy.h"
#include "internal/Logging/LoggingPrivateAdapter.h"

#include "TVQtRC/InstantSupportError.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QWindow>
#include <QRegularExpression>

namespace tvqtsdk
{

namespace
{

void registerMetatypes()
{
	qRegisterMetaType<std::shared_ptr<SimulateKeyCommand>>();
	qRegisterMetaType<std::shared_ptr<SimulateMouseCommand>>();
	qRegisterMetaType<ScreenGrabStrategy>();
	qRegisterMetaType<ControlMode>();
	qRegisterMetaType<AccessControl>();
	qRegisterMetaType<Access>();
	qRegisterMetaType<ChatType>();
}

bool isValidAccessControl(AccessControl feature)
{
	switch (feature)
	{
		case AccessControl::FileTransfer:
		case AccessControl::RemoteView:
		case AccessControl::RemoteControl:
		case AccessControl::ScreenRecording:
		return true;
	}
	return false;
}

} // namespace

TVQtRCPlugin::TVQtRCPlugin(QObject* parent)
	: QObject(parent)
	, m_logging(std::make_shared<Logging>())
	, m_loggingProxy(std::make_shared<LoggingProxy>(m_logging))
	, m_communicationAdapter(CommunicationAdapter::Create(
		  m_loggingProxy,
		  std::make_shared<LoggingPrivateAdapter>(m_logging)))
{
	registerMetatypes();

	QObject::connect(
		m_communicationAdapter.get(),
		&CommunicationAdapter::agentCommunicationEstablished,
		this,
		&TVQtRCPlugin::reactOnEstablishedCommunication,
		Qt::QueuedConnection);

	QObject::connect(
		m_communicationAdapter.get(),
		&CommunicationAdapter::agentCommunicationLost,
		this,
		&TVQtRCPlugin::reactOnLostCommunication,
		Qt::QueuedConnection);

	QObject::connect(
		m_communicationAdapter.get(),
		&CommunicationAdapter::tvSessionStarted,
		this,
		&TVQtRCPlugin::reactOnTVSessionStarted,
		Qt::QueuedConnection);

	QObject::connect(
		m_communicationAdapter.get(),
		&CommunicationAdapter::tvSessionStopped,
		this,
		&TVQtRCPlugin::reactOnTVSessionStopped,
		Qt::QueuedConnection);

	QObject::connect(
		m_communicationAdapter.get(),
		&CommunicationAdapter::accessConfirmationRequested,
		this,
		&TVQtRCPlugin::reactOnAccessConfirmationRequest,
		Qt::QueuedConnection);

	QObject::connect(
		m_communicationAdapter.get(),
		&CommunicationAdapter::accessModeChangeNotified,
		this,
		&TVQtRCPlugin::reactOnAccessModeChangeNotifier,
		Qt::QueuedConnection);

	QObject::connect(
		m_communicationAdapter.get(),
		&CommunicationAdapter::augmentRCSessionInvitationReceived,
		this,
		&TVQtRCPlugin::reactOnAugmentRCSessionInvitationReceived,
		Qt::QueuedConnection);

	QObject::connect(
		m_communicationAdapter.get(),
		&CommunicationAdapter::instantSupportErrorNotification,
		this,
		&TVQtRCPlugin::reactOnInstantSupportError,
		Qt::QueuedConnection);

	QObject::connect(
		m_communicationAdapter.get(),
		&CommunicationAdapter::instantSupportModifiedNotification,
		this,
		&TVQtRCPlugin::reactOnInstantSupportModified,
		Qt::QueuedConnection);

	QObject::connect(
		m_communicationAdapter.get(),
		&CommunicationAdapter::instantSupportConnectionConfirmationRequested,
		this,
		&TVQtRCPlugin::reactOnInstantSupportConnectionConfirmationRequest,
		Qt::QueuedConnection);
}

TVQtRCPlugin::~TVQtRCPlugin()
{
	shutdownRemoteControl();

	m_isTeamViewerSessionRunning = false;
	Q_EMIT remoteControlStopped();

	m_communicationAdapter->sendStop();

	m_applicationWindow.clear();
	m_communicationAdapter->shutdown();
}

void TVQtRCPlugin::setRemoteScreenSdkBaseUrl(QUrl remoteScreenSdkBaseUrl)
{
	m_communicationAdapter->setRemoteScreenSdkBaseUrl(remoteScreenSdkBaseUrl);
}

BaseUrlParseResultCode TVQtRCPlugin::setRemoteScreenSdkBaseUrlChecked(QUrl remoteScreenSdkBaseUrl)
{
	return m_communicationAdapter->setRemoteScreenSdkBaseUrlChecked(remoteScreenSdkBaseUrl);
}

BaseUrlParseResultCode TVQtRCPlugin::setRemoteScreenSdkUrls(QUrl baseServerUrl, QUrl agentApiUrl)
{
	return m_communicationAdapter->setRemoteScreenSdkUrls(baseServerUrl, agentApiUrl);
}

void TVQtRCPlugin::registerApplication()
{
	terminateTeamViewerSessions();
	deregisterApplicationWindow();

	m_communicationAdapter->startup();

	subscribeForDesktopGeometryChanges();
}

void TVQtRCPlugin::deregisterApplication()
{
	unsubscribeFromDesktopGeometryChanges();

	terminateTeamViewerSessions();
	deregisterApplicationWindow();

	m_communicationAdapter->shutdown();
}

void TVQtRCPlugin::registerApplicationWindow(QWindow* window)
{
	shutdownRemoteControl();

	m_applicationWindow = window;

	if (m_applicationWindow)
	{
		m_windowDestroyedConnection = QObject::connect(m_applicationWindow, &QObject::destroyed, this, &TVQtRCPlugin::shutdownRemoteControl);

		if (m_isTeamViewerSessionRunning)
		{
			switch (m_controlMode)
			{
				case ControlMode::Disabled:
					shutdownRemoteControl();
					break;
				case ControlMode::ViewOnly:
					startupGrabbing();
					shutdownInputSimulation();
					break;
				case ControlMode::FullControl:
					startupGrabbing();
					startupInputSimulation();
					break;
			}
		}
		else
		{
			m_communicationAdapter->startup();
		}
	}
}

void TVQtRCPlugin::deregisterApplicationWindow()
{
	shutdownRemoteControl();

	QObject::disconnect(m_windowDestroyedConnection);
	m_applicationWindow.clear();
}

ControlMode TVQtRCPlugin::getControlMode() const
{
	return m_controlMode;
}

void TVQtRCPlugin::setControlMode(ControlMode value)
{
	if (m_controlMode != value)
	{
		m_communicationAdapter->sendControlMode(value);

		m_controlMode = value;
		Q_EMIT controlModeChanged(m_controlMode);

		if (m_isTeamViewerSessionRunning)
		{
			switch (m_controlMode)
			{
				case ControlMode::Disabled:
					shutdownRemoteControl();
					break;
				case ControlMode::ViewOnly:
					startupGrabbing();
					shutdownInputSimulation();
					break;
				case ControlMode::FullControl:
					startupGrabbing();
					startupInputSimulation();
					break;
			}
		}
	}
}

QMetaObject::Connection TVQtRCPlugin::registerControlModeChanged(const std::function<void(ControlMode)>& slot, const QObject* context)
{
	return context == nullptr ?
		QObject::connect(this, &TVQtRCPlugin::controlModeChanged, slot) :
		QObject::connect(this, &TVQtRCPlugin::controlModeChanged, context, slot);
}

bool TVQtRCPlugin::getAccessMode(AccessControl feature, Access& access)
{
	bool getAccessResult = false;
	if (isValidAccessControl(feature))
	{
		getAccessResult = m_communicationAdapter->sendGetAccessMode(feature, access);

		if (getAccessResult)
		{
			Q_EMIT accessModeChanged(feature, access);
		}
	}
	return getAccessResult;
}

bool TVQtRCPlugin::setAccessMode(AccessControl feature, Access access)
{
	if (isValidAccessControl(feature))
	{
		return m_communicationAdapter->sendSetAccessMode(feature, access);
	}
	return false;
}

bool TVQtRCPlugin::accessConfirmationReply(AccessControl feature, bool confirmed)
{
	bool confirm = false;
	if (isValidAccessControl(feature))
	{
		confirm = confirmed;
	}

	return m_communicationAdapter->sendAccessConfirmationReply(feature, confirm);
}

QMetaObject::Connection TVQtRCPlugin::registerAccessModeChanged(const std::function<void(AccessControl feature, Access access)>& slot, const QObject* context)
{
	return context == nullptr ?
		QObject::connect(this, &TVQtRCPlugin::accessModeChanged, slot) :
		QObject::connect(this, &TVQtRCPlugin::accessModeChanged, context, slot);
}

QMetaObject::Connection TVQtRCPlugin::registerAccessConfirmationRequest(const std::function<void(AccessControl feature, uint32_t timeout)>& slot, const QObject* context)
{
	return context == nullptr ?
		QObject::connect(this, &TVQtRCPlugin::accessConfirmationRequested, slot) :
		QObject::connect(this, &TVQtRCPlugin::accessConfirmationRequested, context, slot);
}

QMetaObject::Connection TVQtRCPlugin::registerForInstantSupportErrorNotification(const std::function<void(InstantSupportError errorCode)>& slot, const QObject* context)
{
	return context == nullptr ?
		QObject::connect(this, &TVQtRCPlugin::instantSupportErrorNotification, slot) :
		QObject::connect(this, &TVQtRCPlugin::instantSupportErrorNotification, context, slot);
}

QMetaObject::Connection TVQtRCPlugin::registerForInstantSupportModifiedNotification(const std::function<void(InstantSupportData data)>& slot, const QObject* context)
{
	return context == nullptr ?
		QObject::connect(this, &TVQtRCPlugin::instantSupportModifiedNotification, slot) :
		QObject::connect(this, &TVQtRCPlugin::instantSupportModifiedNotification, context, slot);
}

QMetaObject::Connection TVQtRCPlugin::registerForInstantSupportConfirmationRequest(const std::function<void(ConfirmationResponseFunction response)>& slot, const QObject* context)
{
	return context == nullptr ?
		QObject::connect(this, &TVQtRCPlugin::instantSupportConfirmationRequest, slot) :
		QObject::connect(this, &TVQtRCPlugin::instantSupportConfirmationRequest, context, slot);
}

bool TVQtRCPlugin::isTeamViewerSessionRunning() const
{
	return m_isTeamViewerSessionRunning;
}

void TVQtRCPlugin::terminateTeamViewerSessions()
{
	shutdownRemoteControl();

	m_isTeamViewerSessionRunning = false;
	Q_EMIT remoteControlStopped();

	m_communicationAdapter->sendStop();
}

QMetaObject::Connection TVQtRCPlugin::registerTeamViewerSessionStarted(const std::function<void ()>& slot, const QObject* context)
{
	return context == nullptr ?
		QObject::connect(this, &TVQtRCPlugin::remoteControlStarted, slot) :
		QObject::connect(this, &TVQtRCPlugin::remoteControlStarted, context, slot);
}

QMetaObject::Connection TVQtRCPlugin::registerTeamViewerSessionStopped(const std::function<void ()>& slot, const QObject* context)
{
	return context == nullptr ?
		QObject::connect(this, &TVQtRCPlugin::remoteControlStopped, slot) :
		QObject::connect(this, &TVQtRCPlugin::remoteControlStopped, context, slot);
}

bool TVQtRCPlugin::isAgentCommunicationEstablished() const
{
	return m_isAgentCommunicationEstablished;
}

QMetaObject::Connection TVQtRCPlugin::registerAgentCommunicationStatusChanged(const std::function<void(bool)>& slot, const QObject* context)
{
	return context == nullptr ?
		QObject::connect(this, &TVQtRCPlugin::agentCommunicationStatusChanged, slot) :
		QObject::connect(this, &TVQtRCPlugin::agentCommunicationStatusChanged, context, slot);
}

void TVQtRCPlugin::startLogging(const QString& logFolderPath)
{
	m_logging->startLogging(logFolderPath);
	m_logging->logInfo(QString("Started file logging to path %1").arg(logFolderPath));
#ifdef TV_VERSION
	m_logging->logInfo(QString("TV Agent SDK version %1").arg(TV_VERSION));
#endif
}

void TVQtRCPlugin::stopLogging()
{
	m_logging->stopLogging();
}

void TVQtRCPlugin::reactOnAccessConfirmationRequest(const tvqtsdk::AccessControl feature, uint32_t timeout)
{
	if (isValidAccessControl(feature))
	{
		Q_EMIT accessConfirmationRequested(feature, timeout);
	}
}

void TVQtRCPlugin::reactOnAccessModeChangeNotifier(tvqtsdk::AccessControl feature, Access access)
{
	if (isValidAccessControl(feature))
	{
		Q_EMIT accessModeChanged(feature, access);
	}
}

void TVQtRCPlugin::reactOnAugmentRCSessionInvitationReceived(QUrl url)
{
	Q_EMIT augmentRCSessionInvitationReceived(url);
}

void TVQtRCPlugin::reactOnInstantSupportError(tvqtsdk::InstantSupportError errorCode)
{
	Q_EMIT instantSupportErrorNotification(errorCode);
}

void TVQtRCPlugin::reactOnInstantSupportModified(tvqtsdk::InstantSupportData data)
{
	Q_EMIT instantSupportModifiedNotification(data);
}

void TVQtRCPlugin::reactOnInstantSupportConnectionConfirmationRequest()
{
	const auto commadapter = m_communicationAdapter;
	auto confirmationResponseCallback = [commadapter](ConnectionUserConfirmation confirmation)
	{
		QMetaObject::invokeMethod(
			commadapter.get(),
			"confirmConnectionRequest",
			Qt::QueuedConnection,
			Q_ARG(tvqtsdk::ConnectionType, ConnectionType::InstantSupport),
			Q_ARG(tvqtsdk::ConnectionUserConfirmation, confirmation));
	};

	Q_EMIT instantSupportConfirmationRequest(confirmationResponseCallback);
}

void TVQtRCPlugin::reactOnTVSessionStarted(const ScreenGrabStrategy strategy)
{
	if (m_isTeamViewerSessionRunning)
	{
		return;
	}

	m_strategy = strategy;

	m_isTeamViewerSessionRunning = true;
	Q_EMIT remoteControlStarted();

	m_communicationAdapter->sendControlMode(m_controlMode);

	switch (m_controlMode)
	{
		case ControlMode::Disabled:
			shutdownRemoteControl();
			break;
		case ControlMode::ViewOnly:
			startupGrabbing();
			shutdownInputSimulation();
			break;
		case ControlMode::FullControl:
			startupGrabbing();
			startupInputSimulation();
			break;
	}
}

void TVQtRCPlugin::reactOnTVSessionStopped()
{
	if (!m_isTeamViewerSessionRunning)
	{
		return;
	}

	shutdownRemoteControl();

	m_isTeamViewerSessionRunning = false;
	Q_EMIT remoteControlStopped();
}

void TVQtRCPlugin::reactOnEstablishedCommunication()
{
	if (!m_isAgentCommunicationEstablished)
	{
		m_isAgentCommunicationEstablished = true;
		Q_EMIT agentCommunicationStatusChanged(m_isAgentCommunicationEstablished);
	}
}

void TVQtRCPlugin::reactOnLostCommunication()
{
	shutdownRemoteControl();

	if (m_isAgentCommunicationEstablished)
	{
		m_isAgentCommunicationEstablished = false;
		Q_EMIT agentCommunicationStatusChanged(m_isAgentCommunicationEstablished);
	}
}

void TVQtRCPlugin::reactOnDesktopGeometryChanges()
{
	if (m_applicationWindow)
	{
		return;
	}

	const QList<QScreen*> appScreens = QGuiApplication::screens();
	Q_ASSERT(!appScreens.isEmpty());

	const bool singleVirtualDesktop =
		appScreens == QGuiApplication::primaryScreen()->virtualSiblings();
	if (!singleVirtualDesktop)
	{
		m_logging->logError("Multiple virtual desktops detected, no remote input available");
		return;
	}

	QRect virtualGeometry = QGuiApplication::primaryScreen()->virtualGeometry();

	VirtualDesktop virtualDesktop;
	virtualDesktop.width = virtualGeometry.width();
	virtualDesktop.height = virtualGeometry.height();
	virtualDesktop.screens.reserve(appScreens.size());
	for (const auto& screen: appScreens)
	{
		virtualDesktop.screens << Screen{screen->name(), screen->geometry()};
	}

	{
		QMutexLocker locker{&m_virtualDesktopMutex};
		const bool test1 = virtualDesktop.width == m_virtualDesktop.width,
			test2 = virtualDesktop.height == m_virtualDesktop.height,
			test3 = virtualDesktop.screens.size() == m_virtualDesktop.screens.size();

		if (test1 && test2 && test3 &&
			std::equal(
				std::begin(virtualDesktop.screens),
				std::end(virtualDesktop.screens),
				std::begin(m_virtualDesktop.screens),
				[](const Screen& a, const Screen& b)
				{
					return a.geometry == b.geometry && a.name == b.name;
				}))
		{
			return;
		}
		m_virtualDesktop = virtualDesktop;
	}

	const ViewGeometrySendResult result =
		m_communicationAdapter->sendVirtualDesktopGeometry(virtualDesktop);

	QMutexLocker locker{&m_virtualDesktopMutex};
	m_virtualDesktopGeometryHandshakeSucceeded = result == ViewGeometrySendResult::Ok;
}

void TVQtRCPlugin::setAreaOfInterest()
{
	QRect areaOfInterest;
	if (m_applicationWindow)
	{
		areaOfInterest = m_applicationWindow->geometry();
	}
	else
	{
		areaOfInterest = QGuiApplication::primaryScreen()->virtualGeometry();
	}

	{
		QMutexLocker locker{&m_virtualDesktopMutex};
		if (areaOfInterest == m_areaOfInterest)
		{
			return;
		}
		m_areaOfInterest = areaOfInterest;
	}

	const ViewGeometrySendResult result =
		m_communicationAdapter->sendAreaOfInterest(areaOfInterest);

	if (result == ViewGeometrySendResult::Rejected)
	{
		m_logging->logError(QStringLiteral("Failed to coordinate new area of interest"
										   "Shutting down remote control..."));
		shutdownRemoteControl();
	}
}

void TVQtRCPlugin::startupGrabbing()
{
	switch (m_strategy)
	{
		case ScreenGrabStrategy::NoGrabbing:
			if (m_applicationWindow)
			{
				QObject::connect(
					m_applicationWindow,
					&QWindow::xChanged,
					this,
					&TVQtRCPlugin::setAreaOfInterest);

				QObject::connect(
					m_applicationWindow,
					&QWindow::yChanged,
					this,
					&TVQtRCPlugin::setAreaOfInterest);

				QObject::connect(
					m_applicationWindow,
					&QWindow::widthChanged,
					this,
					&TVQtRCPlugin::setAreaOfInterest);

				QObject::connect(
					m_applicationWindow,
					&QWindow::heightChanged,
					this,
					&TVQtRCPlugin::setAreaOfInterest);
			}
			else
			{
				reactOnDesktopGeometryChanges();
			}

			// This might immediately shutdownRemoteControl,
			// therefore it cannot be directly called here.
			QMetaObject::invokeMethod(this, SLOT(setAreaOfInterest()), Qt::QueuedConnection);
			break;
		case ScreenGrabStrategy::EventDrivenByApp:
		{
			if (!m_grabMethod)
			{
				m_grabMethod = new QWindowGrabMethod(m_applicationWindow, m_loggingProxy, this);
				QObject::connect(
					m_grabMethod,
					&AbstractScreenGrabMethod::grabFinished,
					m_communicationAdapter.get(),
					&CommunicationAdapter::sendScreenGrabResult);

				QObject::connect(
					m_grabMethod,
					&AbstractScreenGrabMethod::imageDefinitionChanged,
					m_communicationAdapter.get(),
					&CommunicationAdapter::sendImageDefinitionForGrabResult);
			}
			m_grabMethod->startGrabbing();

			break;
		}
		case ScreenGrabStrategy::ChangeNotificationOnly:
		{
			if (!m_grabNotifier)
			{
				m_grabNotifier = new QWindowGrabNotifier(m_applicationWindow, m_loggingProxy, this);
				QObject::connect(
					m_grabNotifier,
					&QWindowGrabNotifier::grabRequested,
					m_communicationAdapter.get(),
					&CommunicationAdapter::sendGrabRequest);

				QObject::connect(
					m_grabNotifier,
					&QWindowGrabNotifier::imageDefinitionChanged,
					m_communicationAdapter.get(),
					&CommunicationAdapter::sendImageDefinitionForGrabRequest);
			}
			m_grabNotifier->start();

			break;
		}
	}
}

void TVQtRCPlugin::startupInputSimulation()
{
	if (!m_applicationWindow)
	{
		QMutexLocker locker{&m_virtualDesktopMutex};
		if (!m_virtualDesktopGeometryHandshakeSucceeded)
		{
			m_logging->logError(QStringLiteral("Failed to start input simulator, "
				"virtual desktop geometry handshake has failed"));
			return;
		}
	}

	if (!m_inputSimulator)
	{
		m_inputSimulator =
			new InputSimulator{m_communicationAdapter, m_loggingProxy, m_applicationWindow, this};
	}
	m_inputSimulator->enable();
}

void TVQtRCPlugin::shutdownRemoteControl()
{
	shutdownGrabbing();
	shutdownInputSimulation();
}

void TVQtRCPlugin::shutdownGrabbing()
{
	if (m_applicationWindow)
	{
		QObject::disconnect(
			m_applicationWindow,
			&QWindow::xChanged,
			this,
			&TVQtRCPlugin::setAreaOfInterest);

		QObject::disconnect(
			m_applicationWindow,
			&QWindow::yChanged,
			this,
			&TVQtRCPlugin::setAreaOfInterest);

		QObject::disconnect(
			m_applicationWindow,
			&QWindow::widthChanged,
			this,
			&TVQtRCPlugin::setAreaOfInterest);

		QObject::disconnect(
			m_applicationWindow,
			&QWindow::heightChanged,
			this,
			&TVQtRCPlugin::setAreaOfInterest);
	}

	if (m_grabMethod)
	{
		m_grabMethod->stopGrabbing();
		delete m_grabMethod;
		m_grabMethod = nullptr;
	}

	if (m_grabNotifier)
	{
		m_grabNotifier->stop();
		delete m_grabNotifier;
		m_grabNotifier = nullptr;
	}
}

void TVQtRCPlugin::shutdownInputSimulation()
{
	if (m_inputSimulator)
	{
		m_inputSimulator->disable();
		delete m_inputSimulator;
		m_inputSimulator = nullptr;
	}
}

void TVQtRCPlugin::subscribeForDesktopGeometryChanges()
{
	Q_ASSERT(qGuiApp);

	for (const auto screen: QGuiApplication::screens())
	{
		subscribeForScreenGeometryChanges(screen);
	}

	QObject::connect(
		qGuiApp,
		&QGuiApplication::screenAdded,
		this,
		&TVQtRCPlugin::subscribeForScreenGeometryChanges);

	QObject::connect(
		qGuiApp,
		&QGuiApplication::screenRemoved,
		this,
		&TVQtRCPlugin::unsubscribeFromScreenGeometryChanges);
}

void TVQtRCPlugin::unsubscribeFromDesktopGeometryChanges()
{
	Q_ASSERT(qGuiApp);

	QObject::disconnect(
		qGuiApp,
		&QGuiApplication::screenAdded,
		this,
		&TVQtRCPlugin::subscribeForScreenGeometryChanges);

	QObject::disconnect(
		qGuiApp,
		&QGuiApplication::screenRemoved,
		this,
		&TVQtRCPlugin::unsubscribeFromScreenGeometryChanges);

	for (const auto screen: QGuiApplication::screens())
	{
		unsubscribeFromScreenGeometryChanges(screen);
	}
}

void TVQtRCPlugin::subscribeForScreenGeometryChanges(QScreen* screen)
{
	Q_ASSERT(screen);

	QMutexLocker locker{&m_connectionsMutex};
	if (m_screenGeometryChangesConnections.contains(screen))
	{
		return;
	}

	m_screenGeometryChangesConnections.insert(
		screen,
		QObject::connect(
			screen,
			&QScreen::availableGeometryChanged,
			this,
			&TVQtRCPlugin::reactOnDesktopGeometryChanges));

	m_screenGeometryChangesConnections.insert(
		screen,
		QObject::connect(
			screen,
			&QScreen::geometryChanged,
			this,
			&TVQtRCPlugin::reactOnDesktopGeometryChanges));

	m_screenGeometryChangesConnections.insert(
		screen,
		QObject::connect(
			screen,
			&QScreen::virtualGeometryChanged,
			this,
			&TVQtRCPlugin::reactOnDesktopGeometryChanges));
}

void TVQtRCPlugin::unsubscribeFromScreenGeometryChanges(QScreen* screen)
{
	QMutexLocker locker{&m_connectionsMutex};
	for (auto iter = m_screenGeometryChangesConnections.find(screen);
		iter != m_screenGeometryChangesConnections.end() &&
		iter.key() == screen; ++iter)
	{
		QObject::disconnect(iter.value());
	}
	m_screenGeometryChangesConnections.remove(screen);
}

bool TVQtRCPlugin::requestInstantSupport(
	const QString &accessToken,
	const QString &name,
	const QString &group,
	const QString &description,
	const QString &sessionCode)
{
	return requestInstantSupportV2(accessToken, name, group, description, sessionCode, QString());
}

bool TVQtRCPlugin::requestInstantSupportV2(
	const QString& accessToken,
	const QString& name,
	const QString& group,
	const QString& description,
	const QString& sessionCode,
	const QString& email)
{
	QRegularExpression emailRegex("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,}\\b", QRegularExpression::CaseInsensitiveOption);
	if (!email.isEmpty() && !emailRegex.match(email).hasMatch())
	{
		Q_EMIT m_communicationAdapter->instantSupportErrorNotification(InstantSupportError::InvalidEmail);
		return false;
	}
	return m_communicationAdapter->requestInstantSupport(
		accessToken.toUtf8().constData(),
		name.toUtf8().constData(),
		group.toUtf8().constData(),
		description.toUtf8().constData(),
		sessionCode.toUtf8().constData(),
		email.toUtf8().constData());
}

AbstractChat* TVQtRCPlugin::getChat()
{
	if (!m_chat)
	{
		m_chat = new Chat(m_communicationAdapter);
	}

	return m_chat;
}

bool TVQtRCPlugin::augmentRCSessionStartListening()
{
	return m_communicationAdapter->sendAugmentRCSessionStartListening();
}

bool TVQtRCPlugin::augmentRCSessionStopListening()
{
	return m_communicationAdapter->sendAugmentRCSessionStopListening();
}

QMetaObject::Connection TVQtRCPlugin::registerAugmentRCSessionInvitationReceived(
	const std::function<void(QUrl)>& slot, const QObject* context)
{
	return context == nullptr ?
	QObject::connect(this, &TVQtRCPlugin::augmentRCSessionInvitationReceived, slot) :
	QObject::connect(this, &TVQtRCPlugin::augmentRCSessionInvitationReceived, context, slot);
}

bool TVQtRCPlugin::isFeatureAvailable(Feature feature) const
{
	const uint64_t runningServicesBitmask = m_communicationAdapter->getRunningServicesBitmask();

	auto isServiceRunning = [runningServicesBitmask](TVRemoteScreenSDKCommunication::ServiceType type) -> bool
	{
		return runningServicesBitmask &
			(1ULL << static_cast<typename std::underlying_type<TVRemoteScreenSDKCommunication::ServiceType>::type>(type));
	};

	switch (feature)
	{
	case Feature::AccessControl:
		return isServiceRunning(TVRemoteScreenSDKCommunication::ServiceType::AccessControlIn)
			&& isServiceRunning(TVRemoteScreenSDKCommunication::ServiceType::AccessControlOut);

	case Feature::InstantSupport:
		return isServiceRunning(TVRemoteScreenSDKCommunication::ServiceType::InstantSupport)
			&& isServiceRunning(TVRemoteScreenSDKCommunication::ServiceType::InstantSupportNotification);

	case Feature::TVSessionManagement:
		return isServiceRunning(TVRemoteScreenSDKCommunication::ServiceType::SessionControl)
			&& isServiceRunning(TVRemoteScreenSDKCommunication::ServiceType::SessionStatus);

	case Feature::Chat:
		return isServiceRunning(TVRemoteScreenSDKCommunication::ServiceType::ChatIn)
			&& isServiceRunning(TVRemoteScreenSDKCommunication::ServiceType::ChatOut);

	case Feature::AugmentRCSession:
		return isServiceRunning(TVRemoteScreenSDKCommunication::ServiceType::AugmentRCSessionControl)
			&& isServiceRunning(TVRemoteScreenSDKCommunication::ServiceType::AugmentRCSessionConsumer);
	}

	return false;
}

} // namespace tvqtsdk
