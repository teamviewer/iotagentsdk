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
#include "AppModel.h"

#include <TVQtRC/Interface.h>

#include <QtCore/QDateTime>
#include <QtCore/QtDebug>

namespace
{

AppModel::ControlMode toModelValue(tvqtsdk::ControlMode value)
{
	switch (value)
	{
	case tvqtsdk::ControlMode::Disabled:    return AppModel::ControlMode_Disabled;
	case tvqtsdk::ControlMode::ViewOnly:    return AppModel::ControlMode_ViewOnly;
	case tvqtsdk::ControlMode::FullControl: return AppModel::ControlMode_FullControl;
	}

	return AppModel::ControlMode_Disabled;
}

tvqtsdk::ControlMode toPluginValue(AppModel::ControlMode value)
{
	switch (value)
	{
	case AppModel::ControlMode_Disabled:    return tvqtsdk::ControlMode::Disabled;
	case AppModel::ControlMode_ViewOnly:    return tvqtsdk::ControlMode::ViewOnly;
	case AppModel::ControlMode_FullControl: return tvqtsdk::ControlMode::FullControl;
	}

	return tvqtsdk::ControlMode::Disabled;
}

AppModel::Access toAppAccessValue(tvqtsdk::Access value)
{
	switch (value)
	{
		case tvqtsdk::Access::Allowed:           return AppModel::Access_Allowed;
		case tvqtsdk::Access::AfterConfirmation: return AppModel::Access_AfterConfirmation;
		case tvqtsdk::Access::Denied:            return AppModel::Access_Denied;
	}

	return AppModel::Access_Unknown;
}

AppModel::InstantSupportErrorCode toAppInstantSupportError(tvqtsdk::InstantSupportError errorCode)
{
	switch (errorCode)
	{
		case tvqtsdk::InstantSupportError::InvalidToken:       return AppModel::InvalidToken;
		case tvqtsdk::InstantSupportError::InvalidGroup:       return AppModel::InvalidGroup;
		case tvqtsdk::InstantSupportError::InvalidSessionCode: return AppModel::InvalidSessionCode;
		case tvqtsdk::InstantSupportError::Busy:               return AppModel::Busy;
		case tvqtsdk::InstantSupportError::InternalError:      return AppModel::InternalError;
		case tvqtsdk::InstantSupportError::InvalidEmail:       return AppModel::InvalidEmail;
		case tvqtsdk::InstantSupportError::CloseRequestFailed: return AppModel::CloseRequestFailed;
		case tvqtsdk::InstantSupportError::NotFound:           return AppModel::NotFound;
	}

	return AppModel::InternalError;
}

AppModel::InstantSupportState toAppInstantSupportState(tvqtsdk::InstantSupportState state)
{
	switch (state)
	{
		case tvqtsdk::InstantSupportState::Undefined:   return AppModel::InstantSupportState::Undefined;
		case tvqtsdk::InstantSupportState::Open:        return AppModel::InstantSupportState::Open;
		case tvqtsdk::InstantSupportState::Closed:      return AppModel::InstantSupportState::Closed;
	}

	return AppModel::InstantSupportState::Undefined;
}

tvqtsdk::Access toPluginAccessValue(AppModel::Access value)
{
	switch (value)
	{
		case AppModel::Access_Allowed:           return tvqtsdk::Access::Allowed;
		case AppModel::Access_AfterConfirmation: return tvqtsdk::Access::AfterConfirmation;
		case AppModel::Access_Denied:            return tvqtsdk::Access::Denied;
		case AppModel::Access_Unknown:           break;
	}

	return tvqtsdk::Access::Denied;
}

bool toPluginAccessControlValue(AppModel::AccessControl fromValue, tvqtsdk::AccessControl& toValue)
{
	switch (fromValue)
	{
		case AppModel::AccessControl_FileTransfer:
			toValue = tvqtsdk::AccessControl::FileTransfer;
			return true;
		case AppModel::AccessControl_RemoteView:
			toValue = tvqtsdk::AccessControl::RemoteView;
			return true;
		case AppModel::AccessControl_RemoteControl:
			toValue = tvqtsdk::AccessControl::RemoteControl;
			return true;
		case AppModel::AccessControl_ScreenRecording:
			toValue = tvqtsdk::AccessControl::ScreenRecording;
			return true;
	}

	return false;
}


bool toAppAccessControlValue(tvqtsdk::AccessControl fromValue, AppModel::AccessControl& toValue)
{
	switch (fromValue)
	{
		case tvqtsdk::AccessControl::FileTransfer:
			toValue = AppModel::AccessControl_FileTransfer;
			return true;
		case tvqtsdk::AccessControl::RemoteView:
			toValue = AppModel::AccessControl_RemoteView;
			return true;
		case tvqtsdk::AccessControl::RemoteControl:
			toValue = AppModel::AccessControl_RemoteControl;
			return true;
		case tvqtsdk::AccessControl::ScreenRecording:
			toValue = AppModel::AccessControl_ScreenRecording;
			return true;
	}

	return false;
}

QString SessionCodeToString(uint32_t sessionCode)
{
	return QStringLiteral("s%1-%2-%3")
		.arg(sessionCode / 1000000, 2, 10, QLatin1Char('0'))
		.arg((sessionCode / 1000) % 1000, 3, 10, QLatin1Char('0'))
		.arg((sessionCode % 1000) , 3, 10, QLatin1Char('0'));
}

AppModel::ChatState toAppChatStateValue(tvqtsdk::ChatState chatState)
{
	switch (chatState)
	{
		case tvqtsdk::ChatState::Open:   return AppModel::ChatState::ChatState_Open;
		case tvqtsdk::ChatState::Closed: return AppModel::ChatState::ChatState_Closed;
	}

	return AppModel::ChatState::ChatState_Closed;
}

QJsonObject chatInfoToJsonObject(tvqtsdk::ChatInfo chatInfo)
{
	QJsonObject chatInfoJson;
	chatInfoJson.insert("chatId", QJsonValue::fromVariant(chatInfo.chatId));
	chatInfoJson.insert("title", chatInfo.title);
	chatInfoJson.insert("chatType", QVariant::fromValue(chatInfo.chatType).toJsonValue());

	QString sessionCode = (chatInfo.chatType == tvqtsdk::ChatType::Session) ? SessionCodeToString(chatInfo.chatTypeId) : QString();
	chatInfoJson.insert("chatTypeId", sessionCode);
	chatInfoJson.insert("chatState", toAppChatStateValue(chatInfo.chatState));

	return chatInfoJson;
}

} // namespace

AppModel::AppModel(tvqtsdk::TVQtRCPluginInterface* const plugin, QObject* parent)
	: QObject(parent),
	  m_plugin{plugin},
	  m_timer{new QTimer(this)}
{
	if (m_plugin)
	{
		// NOTE: capturing the this pointer here is safe as the context is given when registering to track the lifetime of this and it gets automatically disconnected if 'this' is destroyed

		m_chat = m_plugin->getChat();

		auto agentCommunicationStatusHandler = [this](bool isConnected)
		{
			if (isConnected)
			{
				// load initial data
				updateAccessMode(AccessControl_FileTransfer);
				updateAccessMode(AccessControl_RemoteView);
				updateAccessMode(AccessControl_RemoteControl);
				updateAccessMode(AccessControl_ScreenRecording);

				m_selectedChatId = obtainChats();
				if (!m_selectedChatId.isNull()
					&& m_chat
					&& m_chat->selectChat(m_selectedChatId))
				{
					m_lastLocalMessageId = 0;

					Q_EMIT chatTitleChanged();
					Q_EMIT chatSelected();
				}

				resendIsListeningForAugmentRCSessionInvite();
			}
			else
			{
				m_selectedChatId = QUuid();
				m_chatTitle = "Chat";
				Q_EMIT chatTitleChanged();
				Q_EMIT chatSelected();
			}
			Q_EMIT tvCommunicationRunningChanged();
		};
		m_plugin->registerAgentCommunicationStatusChanged(agentCommunicationStatusHandler, this);

		auto tvSessionStartedHandler = [this]()
		{
			Q_EMIT tvSessionRunningChanged();
		};
		m_plugin->registerTeamViewerSessionStarted(tvSessionStartedHandler, this);

		auto tvSessionStoppedHandler = [this]()
		{
			Q_EMIT tvSessionRunningChanged();
		};
		m_plugin->registerTeamViewerSessionStopped(tvSessionStoppedHandler, this);

		auto controlModeChangedHandler = [this](tvqtsdk::ControlMode /*controlMode*/)
		{
			Q_EMIT controlModeChanged();
		};
		m_plugin->registerControlModeChanged(controlModeChangedHandler, this);

		auto accessChangedHandler = [this](tvqtsdk::AccessControl feature, tvqtsdk::Access access)
		{
			AccessControl mappedAccess;
			if (toAppAccessControlValue(feature, mappedAccess))
			{
				m_accessModes[mappedAccess] = toAppAccessValue(access);
				Q_EMIT accessModeChanged(mappedAccess);
			}
		};
		m_plugin->registerAccessModeChanged(accessChangedHandler, this);

		auto accessConfirmationRequestedHandler = [this](tvqtsdk::AccessControl feature, uint32_t timeoutMilliseconds)
		{
			AppModel::AccessControl modelFeature;
			if (toAppAccessControlValue(feature, modelFeature))
			{
				Q_EMIT accessControlConfirmationRequested(modelFeature, static_cast<int>(timeoutMilliseconds));
			}
		};
		m_plugin->registerAccessConfirmationRequest(accessConfirmationRequestedHandler, this);

		auto instantSupportErrorNotification = [this](tvqtsdk::InstantSupportError errorCode)
		{
			m_instantSupportErrorCode = toAppInstantSupportError(errorCode);
			Q_EMIT instantSupportRequestStateChanged();
		};

		auto augmentRCSessionInvitationReceivedHandler = [this](QUrl url)
		{
			Q_EMIT augmentRCSessionInvitationReceived(url);
		};
		m_plugin->registerAugmentRCSessionInvitationReceived(augmentRCSessionInvitationReceivedHandler, this);

		m_plugin->registerForInstantSupportErrorNotification(instantSupportErrorNotification, this);

		auto instantSupportModifiedNotification = [this](tvqtsdk::InstantSupportData data)
		{
			m_instantSupportErrorCode = InstantSupportErrorCode::Success;
			m_instantSupportSessionCode = QString::fromUtf8(data.sessionCode.c_str());
			m_instantSupportName =	QString::fromUtf8(data.name.c_str());
			m_instantSupportDescription = QString::fromUtf8(data.description.c_str());
			m_instantSupportSessionState = toAppInstantSupportState(data.state);
			Q_EMIT instantSupportRequestStateChanged();
		};
		m_plugin->registerForInstantSupportModifiedNotification(instantSupportModifiedNotification, this);

		auto instantSupportConfirmationRequestProcessing = [this](tvqtsdk::TVQtRCPluginInterface::ConfirmationResponseFunction responseFunction)
		{
			qInfo() << "[info] AppModel instantSupportConfirmationRequested";

			m_isInstantSupportConfirmationResponseCallback = responseFunction;
			m_isInstantSupportConfirmationRequested = true;
			Q_EMIT instantSupportConfirmationRequestStateChanged();
		};

		m_plugin->registerForInstantSupportConfirmationRequest(instantSupportConfirmationRequestProcessing, this);

		QObject::connect(
			this,
			&AppModel::instantSupportConfirmationReplySig,
			this,
			&AppModel::instantSupportConfirmationReplySlot,
			Qt::QueuedConnection);

		if (m_chat)
		{
			QObject::connect(
				m_chat,
				&tvqtsdk::AbstractChat::chatCreated,
				this,
				&AppModel::chatCreated,
				Qt::QueuedConnection);

			QObject::connect(
				m_chat,
				&tvqtsdk::AbstractChat::chatsRemoved,
				this,
				&AppModel::chatsRemoved,
				Qt::QueuedConnection);

			QObject::connect(
				m_chat,
				&tvqtsdk::AbstractChat::receivedMessages,
				this,
				&AppModel::receivedMessages,
				Qt::QueuedConnection);

			QObject::connect(
				m_chat,
				&tvqtsdk::AbstractChat::messageSent,
				this,
				&AppModel::messageSent,
				Qt::QueuedConnection);

			QObject::connect(
				m_chat,
				&tvqtsdk::AbstractChat::messageNotSent,
				this,
				&AppModel::messageNotSent,
				Qt::QueuedConnection);

			QObject::connect(
				m_chat,
				&tvqtsdk::AbstractChat::loadedMessages,
				this,
				&AppModel::loadedMessages,
				Qt::QueuedConnection);

			QObject::connect(
				m_chat,
				&tvqtsdk::AbstractChat::deletedHistory,
				this,
				&AppModel::deletedHistory,
				Qt::QueuedConnection);

			QObject::connect(
				m_chat,
				&tvqtsdk::AbstractChat::closedChat,
				this,
				&AppModel::closedChat,
				Qt::QueuedConnection);
		}
	}
}

int AppModel::controlMode() const
{
	if (m_plugin)
	{
		return toModelValue(m_plugin->getControlMode());
	}
	else
	{
		return ControlMode_Disabled;
	}
}

void AppModel::requestControlMode(int value)
{
	if (m_plugin)
	{
		const tvqtsdk::ControlMode pluginValue = toPluginValue(static_cast<ControlMode>(value));
		m_plugin->setControlMode(pluginValue);
	}
}

void AppModel::setAccessMode(AppModel::AccessControl accessControl, AppModel::Access whatAccess)
{
	if (!m_plugin)
	{
		return;
	}

	tvqtsdk::AccessControl accessControlSdk;
	const tvqtsdk::Access whatAccessSdk = toPluginAccessValue(whatAccess);

	if (toPluginAccessControlValue(accessControl, accessControlSdk))
	{
		m_plugin->setAccessMode(accessControlSdk, whatAccessSdk);
	}
}

AppModel::Access AppModel::getAccessMode(AccessControl accessControl) const
{
	return m_accessModes.value(accessControl, Access_Unknown);
}

void AppModel::updateAccessMode(AppModel::AccessControl accessControl)
{
	if (!m_plugin)
	{
		return;
	}

	tvqtsdk::Access whatAccessSdk = tvqtsdk::Access::Denied;
	tvqtsdk::AccessControl accessControlSdk;
	if (!toPluginAccessControlValue(accessControl, accessControlSdk))
	{
		return;
	}

	if (!m_plugin->getAccessMode(accessControlSdk, whatAccessSdk))
	{
		return;
	}

	const Access oldValue = m_accessModes[accessControl];
	const Access newValue = toAppAccessValue(whatAccessSdk);
	if (oldValue != newValue)
	{
		m_accessModes[accessControl] = newValue;
		Q_EMIT accessModeChanged(accessControl);
	}
}

bool AppModel::isTVCommunicationRunning() const
{
	return m_plugin && m_plugin->isAgentCommunicationEstablished();
}

bool AppModel::isTVSessionRunning() const
{
	return m_plugin && m_plugin->isTeamViewerSessionRunning();
}

bool AppModel::isChatSelected() const
{
	return !m_selectedChatId.isNull();
}

QString AppModel::chatTitle() const
{
	return m_chatTitle;
}

void AppModel::terminateTeamViewerSession()
{
	if (m_plugin)
	{
		m_plugin->terminateTeamViewerSessions();
	}
}

void AppModel::requestInstantSupport(
	const QString& accessToken,
	const QString& name,
	const QString& group,
	const QString& description,
	const QString& sessionCode,
	const QString& email)
{
	if (m_plugin)
	{
		bool supportRequested = m_plugin->requestInstantSupportV2(accessToken, name, group, description, sessionCode, email);
		if (supportRequested)
		{
			m_instantSupportErrorCode = InstantSupportErrorCode::Processing;
			Q_EMIT instantSupportRequestStateChanged();
		}
	}
}

void AppModel::closeInstantSupportCase(const QString& accessToken, const QString& sessionCode)
{
	bool closeSupportRequested = m_plugin->closeInstantSupportCase(accessToken, sessionCode);
	if (closeSupportRequested)
	{
		m_instantSupportErrorCode = InstantSupportErrorCode::Processing;
		Q_EMIT instantSupportRequestStateChanged();
	}
}

int AppModel::instantSupportState() const
{
	return m_instantSupportSessionState;
}

int AppModel::instantSupportResponseCode() const
{
	return m_instantSupportErrorCode;
}

QString AppModel::instantSupportSessionCode() const
{
	return m_instantSupportSessionCode;
}

QString AppModel::instantSupportDescription() const
{
	return m_instantSupportDescription;
}

QString AppModel::instantSupportName() const
{
	return m_instantSupportName;
}

bool AppModel::isInstantSupportConfirmationRequested() const
{
	return m_isInstantSupportConfirmationRequested;
}

void AppModel::acceptInstantSupportConfirmationRequest()
{
	qInfo() << "[info] AppModel acceptInstantSupportConfirmationRequest";

	if (m_plugin)
	{
		m_isInstantSupportConfirmationRequested = false;
		Q_EMIT instantSupportConfirmationRequestStateChanged();
		Q_EMIT instantSupportConfirmationReplySig(true);
		m_timer->stop();
	}
}

void AppModel::denyInstantSupportConfirmationRequest()
{
	qInfo() << "[info] AppModel denyInstantSupportConfirmationRequest";
	if (m_plugin)
	{
		m_isInstantSupportConfirmationRequested = false;
		Q_EMIT instantSupportConfirmationRequestStateChanged();
		Q_EMIT instantSupportConfirmationReplySig(false);
		m_timer->stop();
	}
}

void AppModel::instantSupportConfirmationReplySlot(bool confirmed)
{
	m_isInstantSupportConfirmationResponseCallback(
		confirmed ? tvqtsdk::ConnectionUserConfirmation::Accepted :
					tvqtsdk::ConnectionUserConfirmation::Denied);
}

void AppModel::replyAccessControlRequest(AccessControl feature, bool confirmed)
{
	if (!m_plugin)
	{
		return;
	}

	tvqtsdk::AccessControl accessControl;

	if (toPluginAccessControlValue(feature, accessControl))
	{
		m_plugin->accessConfirmationReply(accessControl, confirmed);
	}
}

void AppModel::sendChatMessage(QString content)
{
	if (m_chat)
	{
		Q_EMIT newChatMessageAdded(content, QDateTime::currentDateTime().toString(Qt::TextDate), "Me");

		if (m_chat->sendMessage(m_lastLocalMessageId, content))
		{
			++m_lastLocalMessageId;
		}
	}
}

void AppModel::chatCreated(QUuid chatId, QString title, tvqtsdk::ChatType chatType, uint32_t chatTypeId)
{
	tvqtsdk::ChatInfo chatInfo{chatId, title, chatType, chatTypeId, tvqtsdk::ChatState::Open};
	Q_EMIT newContactAdded(chatInfoToJsonObject(chatInfo));

	if (m_selectedChatId.isNull())
	{
		m_selectedChatId = chatId;
		m_chat->selectChat(chatId);

		QString sessionCode = (chatType == tvqtsdk::ChatType::Session) ? SessionCodeToString(chatTypeId) : QString();
		m_chatTitle = title + ' ' + sessionCode;
		Q_EMIT chatTitleChanged();
		Q_EMIT chatSelected();
	}
}

void AppModel::chatsRemoved(QVector<QUuid> chatIds)
{
	for (const auto& chatId: chatIds)
	{
		Q_EMIT chatRemoved(QJsonValue::fromVariant(chatId));
	}
}

void AppModel::receivedMessages(QVector<tvqtsdk::ReceivedMessage> messages)
{
	for (const auto& msg: messages)
	{
		if (m_selectedChatId == msg.chatId)
		{
			Q_EMIT newChatMessageAdded(msg.content, msg.timeStamp.toString(Qt::TextDate), msg.sender);
		}
	}
}

void AppModel::messageSent(uint32_t localId, QUuid messageId, QDateTime timeStamp)
{
	qInfo() << "[info] AppModel messageSent:"
		<< "localId" << localId
		<< "messageId" << messageId.toString()
		<< "timeStamp" << timeStamp.toString();
}

void AppModel::messageNotSent(uint32_t localId)
{
	qInfo() << "[info] AppModel messageNotSent: localId = " << localId;
}

void AppModel::loadedMessages(QVector<tvqtsdk::ReceivedMessage> messages, bool hasMore)
{
	if (!messages.empty())
	{
		QUuid lastMessageId = messages.back().messageId;

		for (const auto& msg : messages)
		{
			if (msg.chatId != m_selectedChatId)
			{
				hasMore = false;
				break;
			}

			Q_EMIT loadedMessagesAdded(
				msg.content,
				msg.timeStamp.toString(Qt::TextDate),
				msg.sender.isEmpty() ? "Me" : msg.sender);
		}

		if (hasMore)
		{
			loadMessages(lastMessageId);
		}
	}
}

void AppModel::deletedHistory(QUuid chatId)
{
	Q_EMIT historyDeleted(QJsonValue::fromVariant(chatId));
}

void AppModel::closedChat(QUuid chatId)
{
	Q_EMIT chatClosed(QJsonValue::fromVariant(chatId));
}

QUuid AppModel::obtainChats()
{
	QUuid firstChatId;
	if (m_chat)
	{
		QVector<tvqtsdk::ChatInfo> chats;
		if (m_chat->obtainChats(chats))
		{
			for (const auto& chat: chats)
			{
				Q_EMIT newContactAdded(chatInfoToJsonObject(chat));
			}

			if (!chats.empty())
			{
				tvqtsdk::ChatInfo firstChat = chats.front();
				QString sessionCode = (firstChat.chatType == tvqtsdk::ChatType::Session) ? SessionCodeToString(firstChat.chatTypeId) : QString();
				firstChatId = firstChat.chatId;
				m_chatTitle = firstChat.title + ' ' + sessionCode;
			}
		}
	}
	return firstChatId;
}

void AppModel::selectChat(QUuid chatId)
{
	if (m_chat && m_chat->selectChat(chatId))
	{
		m_selectedChatId = chatId;

		Q_EMIT chatSelected();
	}
}

void AppModel::loadMessages(QUuid lastMessageId, uint32_t messageCount)
{
	if (m_chat)
	{
		m_chat->loadMessages(messageCount, lastMessageId);
	}
}

void AppModel::deleteHistory()
{
	if (m_chat)
	{
		m_chat->deleteHistory();
	}
}

void AppModel::deleteChat()
{
	if (m_chat)
	{
		m_chat->deleteChat();
	}
}

void AppModel::setIsListeningForAugmentRCSessionInvite(bool isListening)
{
	if (m_isListeningForAugmentRCSessionInvite != isListening)
	{
		m_isListeningForAugmentRCSessionInvite = isListening;
		resendIsListeningForAugmentRCSessionInvite();
		Q_EMIT isListeningForAugmentRCSessionInviteChanged();
	}
}

void AppModel::resendIsListeningForAugmentRCSessionInvite()
{
	if (m_plugin)
	{
		if (m_isListeningForAugmentRCSessionInvite)
		{
			m_plugin->augmentRCSessionStartListening();
		}
		else
		{
			m_plugin->augmentRCSessionStopListening();
		}
	}
}

bool AppModel::isAugmentRCSessionFeatureAvailable() const
{
	return m_plugin->isFeatureAvailable(tvqtsdk::Feature::AugmentRCSession);
}

bool AppModel::isListeningForAugmentRCSessionInvite() const
{
	return m_isListeningForAugmentRCSessionInvite;
}
