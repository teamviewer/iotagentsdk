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

#include <QtCore/QJsonObject>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtCore/QUrl>

#include <TVQtRC/AbstractChat.h>
#include <TVQtRC/ConnectionData.h>

namespace tvqtsdk
{
class TVQtRCPluginInterface;
} // namespace tvqtsdk

class AppModel : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int controlMode                                 READ controlMode NOTIFY controlModeChanged)
	Q_PROPERTY(bool isTVCommunicationRunning                   READ isTVCommunicationRunning NOTIFY tvCommunicationRunningChanged)
	Q_PROPERTY(bool isTVSessionRunning                         READ isTVSessionRunning NOTIFY tvSessionRunningChanged)
	Q_PROPERTY(bool isChatSelected                             READ isChatSelected NOTIFY chatSelected)
	Q_PROPERTY(QString chatTitle                               READ chatTitle NOTIFY chatTitleChanged)

	Q_PROPERTY(int controlMode_Disabled    MEMBER ControlMode_Disabled CONSTANT)
	Q_PROPERTY(int controlMode_ViewOnly    MEMBER ControlMode_ViewOnly CONSTANT)
	Q_PROPERTY(int controlMode_FullControl MEMBER ControlMode_FullControl CONSTANT)

	Q_PROPERTY(int instantSupportResponseCode    READ instantSupportResponseCode NOTIFY instantSupportRequestStateChanged)
	Q_PROPERTY(QString instantSupportSessionCode READ instantSupportSessionCode NOTIFY instantSupportRequestStateChanged)
	Q_PROPERTY(int instantSupportState           READ instantSupportState NOTIFY instantSupportRequestStateChanged)
	Q_PROPERTY(QString instantSupportName        READ instantSupportName NOTIFY instantSupportRequestStateChanged)
	Q_PROPERTY(QString instantSupportDescription READ instantSupportDescription NOTIFY instantSupportRequestStateChanged)
	Q_PROPERTY(bool isInstantSupportConfirmationRequested READ isInstantSupportConfirmationRequested NOTIFY instantSupportConfirmationRequestStateChanged)

	Q_PROPERTY(int NotInitialized     MEMBER NotInitialized CONSTANT)
	Q_PROPERTY(int Success            MEMBER Success CONSTANT)
	Q_PROPERTY(int Processing         MEMBER Processing CONSTANT)
	Q_PROPERTY(int InternalError      MEMBER InternalError CONSTANT)
	Q_PROPERTY(int InvalidToken       MEMBER InvalidToken CONSTANT)
	Q_PROPERTY(int InvalidSessionCode MEMBER InvalidSessionCode CONSTANT)
	Q_PROPERTY(int InvalidGroup       MEMBER InvalidGroup CONSTANT)
	Q_PROPERTY(int Busy               MEMBER Busy CONSTANT)
	Q_PROPERTY(int InvalidEmail       MEMBER InvalidEmail CONSTANT)
	Q_PROPERTY(int CloseRequestFailed MEMBER CloseRequestFailed CONSTANT)
	Q_PROPERTY(int NotFound           MEMBER NotFound CONSTANT)

	Q_PROPERTY(InstantSupportState Open   MEMBER Open CONSTANT)
	Q_PROPERTY(InstantSupportState Closed MEMBER Closed CONSTANT)

	Q_PROPERTY(Access access_Allowed           MEMBER Access_Allowed CONSTANT)
	Q_PROPERTY(Access access_AfterConfirmation MEMBER Access_AfterConfirmation CONSTANT)
	Q_PROPERTY(Access access_Denied            MEMBER Access_Denied CONSTANT)

	Q_PROPERTY(AccessControl accessControl_FileTransfer   MEMBER AccessControl_FileTransfer    CONSTANT)
	Q_PROPERTY(AccessControl accessControl_RemoteView     MEMBER AccessControl_RemoteView      CONSTANT)
	Q_PROPERTY(AccessControl accessControl_RemoteControl  MEMBER AccessControl_RemoteControl   CONSTANT)
	Q_PROPERTY(AccessControl accessControl_ScreenRecoring MEMBER AccessControl_ScreenRecording CONSTANT)

	Q_PROPERTY(bool isAugmentRCSessionFeatureAvailable READ isAugmentRCSessionFeatureAvailable NOTIFY tvCommunicationRunningChanged)
	Q_PROPERTY(bool isListeningForAugmentRCSessionInvite  READ isListeningForAugmentRCSessionInvite WRITE setIsListeningForAugmentRCSessionInvite NOTIFY isListeningForAugmentRCSessionInviteChanged)

public:
	enum ControlMode
	{
		ControlMode_Disabled = 0,
		ControlMode_ViewOnly = 1,
		ControlMode_FullControl = 2,
	};

	enum InstantSupportErrorCode
	{
		NotInitialized = 0,
		Processing = 1,
		InternalError = 2,
		InvalidToken = 3,
		InvalidGroup = 4,
		InvalidSessionCode = 5,
		Busy = 6,
		Success = 7,
		InvalidEmail = 8,
		CloseRequestFailed = 9,
		NotFound = 10,
	};

	enum InstantSupportState
	{
		Undefined,
		Open,
		Closed,
	};

	enum InstantSupportUserConfirmation
	{
		NoResponse = 0,
		Accepted = 1,
		Denied = 2
	};

	Q_ENUM(InstantSupportState);
	Q_ENUM(InstantSupportErrorCode);
	Q_ENUM(InstantSupportUserConfirmation);

	enum AccessControl
	{
		AccessControl_FileTransfer = 0,
		AccessControl_RemoteView = 1,
		AccessControl_RemoteControl = 2,
		AccessControl_ScreenRecording = 3,
	};

	enum Access
	{
		Access_Unknown = -1,
		Access_Allowed = 0,
		Access_AfterConfirmation = 1,
		Access_Denied = 2,
	};

	Q_ENUM(Access);
	Q_ENUM(AccessControl);

	enum ChatState
	{
		ChatState_Open = 0,
		ChatState_Closed = 1
	};

	Q_ENUM(ChatState);

	AppModel(tvqtsdk::TVQtRCPluginInterface* const plugin, QObject* parent = nullptr);
	~AppModel() override = default;

	// remote control
	int controlMode() const;
	Q_SIGNAL void controlModeChanged();

	bool isTVCommunicationRunning() const;
	Q_SIGNAL void tvCommunicationRunningChanged();

	bool isTVSessionRunning() const;
	Q_SIGNAL void tvSessionRunningChanged();

	Q_SLOT void requestControlMode(int value);

	Q_SLOT void terminateTeamViewerSession();

	// instant support
	int instantSupportResponseCode() const;
	QString instantSupportName() const;
	QString instantSupportSessionCode() const;
	QString instantSupportDescription() const;
	int instantSupportState() const;
	Q_SIGNAL void instantSupportRequestStateChanged();
	Q_SLOT void requestInstantSupport(
		const QString& accessToken,
		const QString& name,
		const QString& group,
		const QString& description,
		const QString& sessionCode,
		const QString& email);
	Q_SLOT void closeInstantSupportCase(
		const QString& accessToken,
		const QString& sessionCode);

	bool isInstantSupportConfirmationRequested() const;
	Q_SLOT void acceptInstantSupportConfirmationRequest();
	Q_SLOT void denyInstantSupportConfirmationRequest();
	Q_SIGNAL void instantSupportConfirmationRequestStateChanged();
	Q_SIGNAL void instantSupportConfirmationReplySig(bool confirmed);
	Q_SLOT void instantSupportConfirmationReplySlot(bool confirmed);

	// access controls
	Q_SLOT void setAccessMode(AppModel::AccessControl accessControl, AppModel::Access whatAccess);
	Q_INVOKABLE AppModel::Access getAccessMode(AppModel::AccessControl accessControl) const;
	Q_SLOT void updateAccessMode(AppModel::AccessControl accessControl);
	Q_SIGNAL void accessModeChanged(AppModel::AccessControl accessControl);

	Q_SIGNAL void accessControlConfirmationRequested(AppModel::AccessControl accessControl, int timeOutMilliseconds);
	Q_SLOT void replyAccessControlRequest(AppModel::AccessControl feature, bool confirmed);

	// chat
	QString chatTitle() const;
	bool isChatSelected() const;

	Q_SIGNAL void chatTitleChanged();
	Q_SIGNAL void chatSelected();

	Q_SIGNAL void newChatMessageAdded(QString message, QString timestamp, QString sender);
	Q_SIGNAL void loadedMessagesAdded(QString message, QString timestamp, QString sender);
	Q_SIGNAL void newContactAdded(QJsonObject chatInfo);
	Q_SIGNAL void chatRemoved(const QJsonValue chatId);
	Q_SIGNAL void historyDeleted(const QJsonValue chatId);
	Q_SIGNAL void chatClosed(const QJsonValue chatId);

	Q_SLOT void selectChat(QUuid chatId);
	Q_SLOT void sendChatMessage(QString content);
	Q_SLOT void loadMessages(QUuid lastMessageId = QUuid(), uint32_t messageCount = 10);
	Q_SLOT void deleteHistory();
	Q_SLOT void deleteChat();

	// Augment RC session
	bool isAugmentRCSessionFeatureAvailable() const;
	bool isListeningForAugmentRCSessionInvite() const;
	void setIsListeningForAugmentRCSessionInvite(bool isListening);
	Q_SIGNAL void isListeningForAugmentRCSessionInviteChanged();
	void resendIsListeningForAugmentRCSessionInvite();

	Q_SIGNAL void augmentRCSessionInvitationReceived(const QUrl url);

private:
	Q_SLOT void chatCreated(QUuid chatId, QString title, tvqtsdk::ChatType chatType, uint32_t chatTypeId);
	Q_SLOT void chatsRemoved(QVector<QUuid> chatIds);
	Q_SLOT void receivedMessages(QVector<tvqtsdk::ReceivedMessage> messages);
	Q_SLOT void messageSent(uint32_t localId, QUuid messageId, QDateTime timeStamp);
	Q_SLOT void messageNotSent(uint32_t localId);
	Q_SLOT void loadedMessages(QVector<tvqtsdk::ReceivedMessage> messages, bool hasMore);
	Q_SLOT void deletedHistory(QUuid chatId);
	Q_SLOT void closedChat(QUuid chatId);

	QUuid obtainChats();

	tvqtsdk::TVQtRCPluginInterface* const m_plugin = nullptr;

	QPointer<tvqtsdk::AbstractChat> m_chat = nullptr;

	InstantSupportState m_instantSupportSessionState = InstantSupportState::Undefined;
	InstantSupportErrorCode m_instantSupportErrorCode = InstantSupportErrorCode::NotInitialized;
	QString m_instantSupportSessionCode;
	QString m_instantSupportName;
	QString m_instantSupportDescription;
	bool m_isInstantSupportConfirmationRequested = false;
	bool m_isListeningForAugmentRCSessionInvite = false;
	std::function<void(tvqtsdk::ConnectionUserConfirmation confirmation)> m_isInstantSupportConfirmationResponseCallback;

	QUuid m_selectedChatId;
	uint32_t m_lastLocalMessageId = 0;
	QString m_chatTitle = "Chat";

	QMap<AccessControl, Access> m_accessModes;

	QTimer* m_timer;
};
