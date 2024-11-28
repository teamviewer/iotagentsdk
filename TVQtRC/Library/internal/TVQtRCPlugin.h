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
#pragma once

#include "TVQtRC/Interface.h"

#include "internal/Communication/CommunicationAdapter.h"

#include "internal/Grabbing/Screen/AbstractScreenGrabMethod.h"
#include "internal/Grabbing/Screen/QWindowGrabNotifier.h"

#include "internal/InputSimulation/AbstractInputSimulator.h"

#include <QtCore/QMultiHash>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QtPlugin>

#include <QtGui/QWindow>

#include <memory>

namespace tvqtsdk
{
class ILogging;
class Logging;

class TVQtRCPlugin final : public QObject, public TVQtRCPluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID TVQtRCPluginInterface_iid)
	Q_INTERFACES(tvqtsdk::TVQtRCPluginInterface)

public:
	explicit TVQtRCPlugin(QObject* parent = nullptr);
	~TVQtRCPlugin() override;

	void setRemoteScreenSdkBaseUrl(QUrl remoteScreenSdkBaseUrl) override;
	BaseUrlParseResultCode setRemoteScreenSdkBaseUrlChecked(QUrl remoteScreenSdkBaseUrl) override;

	BaseUrlParseResultCode setRemoteScreenSdkUrls(QUrl baseServerUrl, QUrl agentApiUrl) override;

	void registerApplication() override;
	void deregisterApplication() override;

	void registerApplicationWindow(QWindow* window) override;
	void deregisterApplicationWindow() override;

	ControlMode getControlMode() const override;
	void setControlMode(ControlMode value) override;
	QMetaObject::Connection registerControlModeChanged(const std::function<void(ControlMode controlModeValue)>& slot, const QObject* context) override;

	bool getAccessMode(AccessControl feature, Access& requestedAccess) override;
	bool setAccessMode(AccessControl feature, Access access) override;
	bool accessConfirmationReply(AccessControl feature, bool confirmed) override;
	QMetaObject::Connection registerAccessModeChanged(const std::function<void(AccessControl feature, Access access)>& slot, const QObject* context) override;
	QMetaObject::Connection registerAccessConfirmationRequest(const std::function<void(AccessControl feature, uint32_t timeout)>& slot, const QObject* context) override;

	QMetaObject::Connection registerForInstantSupportErrorNotification(const std::function<void(InstantSupportError errorCode)>& slot, const QObject* context) override;
	QMetaObject::Connection registerForInstantSupportModifiedNotification(const std::function<void(InstantSupportData data)>& slot, const QObject* context) override;
	QMetaObject::Connection registerForInstantSupportConfirmationRequest(const std::function<void(ConfirmationResponseFunction response)>& slot, const QObject* context) override;

	bool isTeamViewerSessionRunning() const override;
	void terminateTeamViewerSessions() override;
	QMetaObject::Connection registerTeamViewerSessionStarted(const std::function<void()>& slot, const QObject* context = nullptr) override;
	QMetaObject::Connection registerTeamViewerSessionStopped(const std::function<void()>& slot, const QObject* context = nullptr) override;

	bool isAgentCommunicationEstablished() const override;
	QMetaObject::Connection registerAgentCommunicationStatusChanged(const std::function<void(bool isConnected)>& slot, const QObject* context = nullptr) override;

	void startLogging(const QString& logFolderPath) override;
	void stopLogging() override;

	bool requestInstantSupport(
		const QString& accessToken,
		const QString& name,
		const QString& group,
		const QString& description,
		const QString& sessionCode) override;

	bool requestInstantSupportV2(
		const QString& accessToken,
		const QString& name,
		const QString& group,
		const QString& description,
		const QString& sessionCode,
		const QString& email) override;

	bool closeInstantSupportCase(
		const QString& accessToken,
		const QString& sessionCode) override;

	AbstractChat* getChat() override;

	bool augmentRCSessionStartListening() override;
	bool augmentRCSessionStopListening() override;
	QMetaObject::Connection registerAugmentRCSessionInvitationReceived(const std::function<void(QUrl url)>& slot, const QObject* context) override;
	bool isFeatureAvailable(Feature feature) const override;

private:
	Q_SIGNAL void controlModeChanged(tvqtsdk::ControlMode controlModeValue);

	Q_SIGNAL void remoteControlStarted();
	Q_SIGNAL void remoteControlStopped();

	Q_SIGNAL void agentCommunicationStatusChanged(bool communicationEstablished);

	Q_SIGNAL void accessModeChanged(tvqtsdk::AccessControl feature, tvqtsdk::Access access);

	Q_SIGNAL void accessConfirmationRequested(tvqtsdk::AccessControl feature, uint32_t timeout);

	Q_SIGNAL void augmentRCSessionInvitationReceived(QUrl url);

	Q_SIGNAL void instantSupportErrorNotification(tvqtsdk::InstantSupportError errorCode);
	Q_SIGNAL void instantSupportModifiedNotification(tvqtsdk::InstantSupportData data);

	Q_SIGNAL void instantSupportConfirmationRequest(ConfirmationResponseFunction confirmationResponse);

	Q_SLOT void reactOnAccessConfirmationRequest(const tvqtsdk::AccessControl feature, uint32_t timeout);
	Q_SLOT void reactOnAccessModeChangeNotifier(tvqtsdk::AccessControl feature, Access access);
	Q_SLOT void reactOnAugmentRCSessionInvitationReceived(QUrl url);
	Q_SLOT void reactOnInstantSupportError(tvqtsdk::InstantSupportError errorCode);
	Q_SLOT void reactOnInstantSupportModified(tvqtsdk::InstantSupportData data);
	Q_SLOT void reactOnInstantSupportConnectionConfirmationRequest();
	Q_SLOT void reactOnTVSessionStarted(const tvqtsdk::ScreenGrabStrategy strategy);
	Q_SLOT void reactOnTVSessionStopped();
	Q_SLOT void reactOnEstablishedCommunication();
	Q_SLOT void reactOnLostCommunication();
	Q_SLOT void reactOnDesktopGeometryChanges();
	Q_SLOT void setAreaOfInterest();

	void startupGrabbing();
	void startupInputSimulation();
	void shutdownRemoteControl();
	void shutdownGrabbing();
	void shutdownInputSimulation();
	void subscribeForDesktopGeometryChanges();
	void unsubscribeFromDesktopGeometryChanges();
	void subscribeForScreenGeometryChanges(QScreen* screen);
	void unsubscribeFromScreenGeometryChanges(QScreen* screen);

	bool m_isAgentCommunicationEstablished = false;
	bool m_isTeamViewerSessionRunning = false;
	ControlMode m_controlMode = ControlMode::Disabled;

	const std::shared_ptr<tvqtsdk::Logging> m_logging;
	const std::shared_ptr<tvqtsdk::ILogging> m_loggingProxy;
	const std::shared_ptr<tvqtsdk::CommunicationAdapter> m_communicationAdapter;

	QPointer<tvqtsdk::AbstractScreenGrabMethod> m_grabMethod;
	QPointer<tvqtsdk::QWindowGrabNotifier> m_grabNotifier;
	QPointer<tvqtsdk::AbstractInputSimulator> m_inputSimulator;
	QPointer<tvqtsdk::AbstractChat> m_chat;

	QPointer<QWindow> m_applicationWindow;
	tvqtsdk::ScreenGrabStrategy m_strategy{tvqtsdk::ScreenGrabStrategy::NoGrabbing};

	QMetaObject::Connection m_windowDestroyedConnection;

	QMutex m_virtualDesktopMutex;
	VirtualDesktop m_virtualDesktop;
	bool m_virtualDesktopGeometryHandshakeSucceeded = false;
	QRect m_areaOfInterest;

	QMutex m_connectionsMutex;
	QMultiHash<QScreen*, QMetaObject::Connection> m_screenGeometryChangesConnections;
};

} // namespace tvqtsdk
