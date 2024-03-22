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

#include "TVQtRC/AccessControl.h"
#include "TVQtRC/BaseUrlParseResultCode.h"
#include "TVQtRC/ChatInfo.h"
#include "TVQtRC/ConnectionData.h"
#include "TVQtRC/ControlMode.h"
#include "TVQtRC/InstantSupportData.h"
#include "TVQtRC/InstantSupportError.h"

#include "TVAgentAPIPrivate/CommunicationChannel.h"
#include "TVAgentAPIPrivate/ILoggingPrivate.h"

#include "internal/Communication/ScreenGrabStrategy.h"
#include "internal/Communication/VirtualDesktop.h"
#include "internal/Logging/ILogging.h"
#include "internal/Grabbing/Screen/ColorFormat.h"

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QUrl>
#include <QtCore/QUuid>
#include <QtCore/QDateTime>

#include <memory>
#include <string>

namespace tvagentapi
{
	class CommunicationChannel;
} // namespace tvagentapi

namespace tvqtsdk
{

class ScreenGrabResult;
class SimulateKeyCommand;
class SimulateMouseCommand;

enum class ViewGeometrySendResult
{
	Ok = 0,
	NoService,
	CallError,
	Rejected
};

class CommunicationAdapter : public QObject
{
	Q_OBJECT
public:
	static std::shared_ptr<CommunicationAdapter> Create(
		const std::shared_ptr<tvqtsdk::ILogging>& logging,
		const std::shared_ptr<tvagentapi::ILoggingPrivate>& loggingPvt,
		QObject* parent = nullptr);
	~CommunicationAdapter() override = default;

	void setRemoteScreenSdkBaseUrl(QUrl remoteScreenSdkBaseUrl);
	BaseUrlParseResultCode setRemoteScreenSdkBaseUrlChecked(QUrl remoteScreenSdkBaseUrl);

	BaseUrlParseResultCode setRemoteScreenSdkUrls(
		QUrl baseServerUrl,
		QUrl agentRegistrationServiceUrl);

public Q_SLOTS:
	void startup();
	void shutdown();

	bool requestInstantSupport(
		QString accessToken,
		QString name,
		QString group,
		QString description,
		QString sessionCode,
		QString email);

	bool confirmConnectionRequest(tvqtsdk::ConnectionType connectionType, tvqtsdk::ConnectionUserConfirmation confirmation);

	void sendStop();

	void sendControlMode(ControlMode mode);

	bool sendGetAccessMode(AccessControl feature, Access& access);
	bool sendSetAccessMode(AccessControl feature, Access access);
	bool sendAccessConfirmationReply(AccessControl feature, bool confirmed);

	void sendScreenGrabResult(const tvqtsdk::ScreenGrabResult& grabResult) const;
	void sendImageDefinitionForGrabResult(
		const QString& title,
		QSize size,
		double dpi,
		ColorFormat colorFormat) const;

	void sendGrabRequest(const QRect& rectOfInterest) const;
	void sendImageDefinitionForGrabRequest(
		const QString& title,
		QSize size) const;

	ViewGeometrySendResult sendVirtualDesktopGeometry(
		const VirtualDesktop& virtualDesktop) const;
	ViewGeometrySendResult sendAreaOfInterest(const QRect& areaOfInterest) const;

	bool sendObtainChatsRequest(QVector<ChatInfo>& chats);
	bool sendSelectChatResult(QUuid chatId);
	bool sendMessage(uint32_t localId, QString content);
	bool loadMessages(uint32_t count, QUuid lastId);
	bool deleteHistory();
	bool deleteChat();

Q_SIGNALS:
	void accessConfirmationRequested(const tvqtsdk::AccessControl feature, uint32_t timeout);
	void accessModeChangeNotified(const tvqtsdk::AccessControl feature, const tvqtsdk::Access accessMode);
	void agentCommunicationEstablished();
	void agentCommunicationLost();
	void instantSupportErrorNotification(InstantSupportError errorCode);
	void instantSupportModifiedNotification(InstantSupportData data);
	void instantSupportConnectionConfirmationRequested();
	void tvSessionStarted(const tvqtsdk::ScreenGrabStrategy strategy);
	void tvSessionStopped();
	void simulateKeyInputRequested(const std::shared_ptr<tvqtsdk::SimulateKeyCommand>& command);
	void simulateMouseInputRequested(const std::shared_ptr<tvqtsdk::SimulateMouseCommand>& command);
	void chatCreated(QUuid chatId, QString title, tvqtsdk::ChatType chatType, uint32_t chatTypeId);
	void chatsRemoved(QVector<QUuid> chatIds);
	void receivedMessages(QVector<ReceivedMessage> messages);
	void messageSent(uint32_t localId, QUuid messageId, QDateTime timeStamp);
	void messageNotSent(uint32_t localId);
	void loadedMessages(QVector<ReceivedMessage> messages, bool hasMore);
	void deletedHistory(QUuid chatId);
	void closedChat(QUuid chatId);

private:
	CommunicationAdapter(
		const std::shared_ptr<tvqtsdk::ILogging>& logging,
		const std::shared_ptr<tvagentapi::ILoggingPrivate>& loggingPvt,
		QObject* parent = nullptr);
	void setup();

	std::shared_ptr<tvqtsdk::ILogging> m_logging;
	std::shared_ptr<tvagentapi::CommunicationChannel> m_communicationChannel;

	std::weak_ptr<CommunicationAdapter> m_weakThis;

	std::vector<tvagentapi::ObserverConnection> m_observerConnections;
};

} // namespace tvqtsdk

Q_DECLARE_METATYPE(std::shared_ptr<tvqtsdk::SimulateKeyCommand>)
Q_DECLARE_METATYPE(std::shared_ptr<tvqtsdk::SimulateMouseCommand>)
