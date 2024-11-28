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

#include "ILoggingPrivate.h"

#include <TVRemoteScreenSDKCommunication/AccessControlService/AccessControl.h>
#include <TVRemoteScreenSDKCommunication/ChatService/Chat.h>
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/ParseUrl.h>
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/ServiceType.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/ConnectionData.h>
#include <TVRemoteScreenSDKCommunication/ImageService/ColorFormat.h>
#include <TVRemoteScreenSDKCommunication/InputService/KeyState.h>
#include <TVRemoteScreenSDKCommunication/InputService/MouseButton.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/InstantSupportData.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/InstantSupportError.h>
#include <TVRemoteScreenSDKCommunication/SessionControlService/ControlMode.h>
#include <TVRemoteScreenSDKCommunication/SessionStatusService/GrabStrategy.h>
#include <TVRemoteScreenSDKCommunication/ViewGeometryService/VirtualDesktop.h>

#include "Observer.h"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace TVRemoteScreenSDKCommunication
{
namespace RegistrationService
{
class IRegistrationServiceClient;
} // namespace RegistrationService
} // namespace TVRemoteScreenSDKCommunication

namespace tvagentapi
{

class ILoggingPrivate;
class ServicesMediator;

enum class ViewGeometrySendResult
{
	Ok = 0,
	NoService,
	CallError,
	Rejected
};

enum BaseUrlParseResultCode
{
	Success,
	CharacterLimitForPathExceeded,
	SchemaNotValid,
	ConnectionIsInUse,
};

class CommunicationChannel final
{
public:
	static std::shared_ptr<CommunicationChannel> Create(
		std::shared_ptr<ILoggingPrivate> logging);
	~CommunicationChannel();

	bool setRemoteScreenSdkBaseUrl(const std::string& url);
	BaseUrlParseResultCode setRemoteScreenSdkBaseUrlChecked(const std::string& url);

	BaseUrlParseResultCode setUrls(const std::string& baseServerUrl, const std::string& agentApiUrl);

	void startup();
	void shutdown();

	bool requestInstantSupport(
		const std::string& accessToken,
		const std::string& name,
		const std::string& group,
		const std::string& description,
		const std::string& sessionCode,
		const std::string& email);

	bool closeInstantSupportCase(
		const std::string& accessToken,
		const std::string& sessionCode);

	bool confirmConnectionRequest(
		TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType connectionType,
		TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionUserConfirmation confirmation);

	bool sendStop();

	bool sendGetRunningTVSessions(std::vector<int32_t>& tvSessionIDs);

	void sendControlMode(TVRemoteScreenSDKCommunication::SessionControlService::ControlMode mode);

	bool sendGetAccessMode(
		TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
		TVRemoteScreenSDKCommunication::AccessControlService::Access& access);
	bool sendSetAccessMode(
		TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
		TVRemoteScreenSDKCommunication::AccessControlService::Access access);
	bool sendAccessConfirmationReply(
		TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
		bool confirmed);

	void sendScreenGrabResult(
		int32_t x,
		int32_t y,
		int32_t width,
		int32_t height,
		std::string pictureData);
	void sendImageDefinitionForGrabResult(
		const std::string& imageSourceTitle,
		int32_t width,
		int32_t height,
		TVRemoteScreenSDKCommunication::ImageService::ColorFormat format,
		double dpi) const;

	void sendGrabRequest(
		int32_t x,
		int32_t y,
		int32_t width,
		int32_t height) const;
	void sendImageDefinitionForGrabRequest(
		const std::string& imageSourceTitle,
		int32_t width,
		int32_t height) const;

	ViewGeometrySendResult sendVirtualDesktopGeometry(
		const TVRemoteScreenSDKCommunication::ViewGeometryService::VirtualDesktop& geometry) const;
	virtual ViewGeometrySendResult sendAreaOfInterest(
		const TVRemoteScreenSDKCommunication::ViewGeometryService::Rect& rect) const;

	bool sendObtainChatsRequest(
		std::vector<TVRemoteScreenSDKCommunication::ChatService::ChatInfo>& chats);
	bool sendSelectChatResult(std::string chatId);
	bool sendMessage(uint32_t localId, std::string content);
	bool loadMessages(uint32_t count, std::string lastId);
	bool deleteHistory();
	bool deleteChat();
	bool sendAugmentRCSessionStartListening();
	bool sendAugmentRCSessionStopListening();
	uint64_t getRunningServicesBitmask() const;

private:
	struct GrabResult
	{
		int32_t x;
		int32_t y;
		int32_t width;
		int32_t height;
		std::string pictureData;
	};

	explicit CommunicationChannel(std::shared_ptr<ILoggingPrivate> logging);

	void shutdownInternal();

	bool establishConnection();
	bool setupClientAndServer();

	bool setupAccessControlOutService();
	bool setupAugmentRCSessionConsumerService();
	bool setupConnectivityService();
	bool setupInputService();
	bool setupSessionStatusService();
	bool setupInstantSupportNotificationService();
	bool setupConnectionConfirmationRequestService();
	bool setupChatOutService();

	bool setupConnectivityClient();

	void sendDisconnect() const;
	void startPing();

	void tearDown();

	void startScreenGrabResultWorker();
	void sendScreenGrabResultBuffer(GrabResult& sendBuffer);

	struct Condition
	{
		std::condition_variable condition;
		std::mutex mutex;
	};

	template <TVRemoteScreenSDKCommunication::ServiceType Type>
	bool setupClient();

	bool registerService(TVRemoteScreenSDKCommunication::ServiceType type);

	const std::shared_ptr<ILoggingPrivate> m_logging;
	const std::unique_ptr<Condition> m_disconnectCondition;
	const std::unique_ptr<Condition> m_shutdownCondition;

	TVRemoteScreenSDKCommunication::UrlComponents m_serverUrlComponents;
	TVRemoteScreenSDKCommunication::TransportFramework m_transportFramework =
		TVRemoteScreenSDKCommunication::UnknownTransport;

	std::string m_communicationId;
	std::unique_ptr<ServicesMediator> m_servicesMediator;

	std::thread m_communicationThread;

	std::atomic_bool m_isRunning{false};

	std::atomic_bool m_processGrabResult{false};
	const std::unique_ptr<Condition> m_grabResultCondition;
	GrabResult m_grabResultBuffer;
	std::thread m_grabResultThread;

	std::weak_ptr<CommunicationChannel> m_weakThis;

private:
	Observer<void(
		TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
		uint32_t timeout)>
			m_accessConfirmationRequested;

	Observer<void(
		TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
		TVRemoteScreenSDKCommunication::AccessControlService::Access accessMode)>
			m_accessModeChangeNotified;

	Observer<void()>
			m_agentCommunicationEstablished;

	Observer<void()>
			m_agentCommunicationLost;

	Observer<void(
		std::string url)>
			m_augmentRCSessionInvitationReceived;

	Observer<void(
		TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError)>
			m_instantSupportErrorNotification;

	Observer<void(
		TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportData)>
			m_instantSupportModifiedNotification;

	Observer<void()>
			m_instantSupportConnectionConfirmationRequested;

	Observer<void(
		TVRemoteScreenSDKCommunication::SessionStatusService::GrabStrategy)>
			m_rcSessionStarted;

	Observer<void()>
			m_rcSessionStopped;

	Observer<void(
		int32_t tvSessionID,
		int32_t tvSessionsCount)>
			m_tvSessionStarted;

	Observer<void(
		int32_t tvSessionID,
		int32_t tvSessionsCount)>
			m_tvSessionStopped;

	Observer<void(
		TVRemoteScreenSDKCommunication::InputService::KeyState keyState,
		uint32_t xkbSymbol,
		uint32_t unicodeCharacter,
		uint32_t xkbModifiers)>
			m_simulateKeyInputRequested;

	Observer<void(
		int32_t posX,
		int32_t posY)>
			m_simulateMouseMoveRequested;

	Observer<void(
		TVRemoteScreenSDKCommunication::InputService::MouseButtonState buttonState,
		int32_t posX,
		int32_t posY,
		TVRemoteScreenSDKCommunication::InputService::MouseButton button)>
			m_simulateMousePressReleaseRequested;

	Observer<void(
		int32_t posX,
		int32_t posY,
		int32_t angle)>
			m_simulateMouseWheelRequested;

	Observer<void(
		std::string chatId,
		std::string title,
		TVRemoteScreenSDKCommunication::ChatService::ChatType sdkchatType,
		uint32_t chatTypeId)>
			m_chatCreated;

	Observer<void(
		std::vector<std::string> chatIds)>
			m_chatsRemoved;

	Observer<void(
		std::vector<TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage> messages)>
			m_receivedMessages;

	Observer<void(
		uint32_t localId,
		std::string messageId,
		uint64_t timeStamp )>
			m_messageSent;

	Observer<void(
		uint32_t localId)>
			m_messageNotSent;

	Observer<void(
		std::vector<TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage> messages,
		bool hasMore)>
			m_loadedMessages;

	Observer<void(
		std::string chatId)>
			m_deletedHistory;

	Observer<void(
		std::string chatId)>
			m_closedChat;

public:
	auto accessConfirmationRequested() -> decltype(m_accessConfirmationRequested)& { return m_accessConfirmationRequested; }
	auto accessModeChangeNotified() -> decltype(m_accessModeChangeNotified)& { return m_accessModeChangeNotified; }
	auto agentCommunicationEstablished() -> decltype(m_agentCommunicationEstablished)& { return m_agentCommunicationEstablished; }
	auto agentCommunicationLost() -> decltype(m_agentCommunicationLost)& { return m_agentCommunicationLost; }
	auto augmentRCSessionInvitationReceived() -> decltype(m_augmentRCSessionInvitationReceived)& { return m_augmentRCSessionInvitationReceived; }
	auto instantSupportErrorNotification() -> decltype(m_instantSupportErrorNotification)& { return m_instantSupportErrorNotification; }
	auto instantSupportModifiedNotification() -> decltype(m_instantSupportModifiedNotification)& { return m_instantSupportModifiedNotification; }
	auto instantSupportConnectionConfirmationRequested() -> decltype(m_instantSupportConnectionConfirmationRequested)& { return m_instantSupportConnectionConfirmationRequested; }
	auto rcSessionStarted() -> decltype(m_rcSessionStarted)& { return m_rcSessionStarted; }
	auto rcSessionStopped() -> decltype(m_rcSessionStopped)& { return m_rcSessionStopped; }
	auto tvSessionStarted() -> decltype(m_tvSessionStarted)& { return m_tvSessionStarted; }
	auto tvSessionStopped() -> decltype(m_tvSessionStopped)& { return m_tvSessionStopped; }
	auto simulateKeyInputRequested() -> decltype(m_simulateKeyInputRequested)& { return m_simulateKeyInputRequested; }
	auto simulateMouseMoveRequested() -> decltype(m_simulateMouseMoveRequested)& { return m_simulateMouseMoveRequested; }
	auto simulateMousePressReleaseRequested() -> decltype(m_simulateMousePressReleaseRequested)& { return m_simulateMousePressReleaseRequested; }
	auto simulateMouseWheelRequested() -> decltype(m_simulateMouseWheelRequested)& { return m_simulateMouseWheelRequested; }
	auto chatCreated() -> decltype(m_chatCreated)& { return m_chatCreated; }
	auto chatsRemoved() -> decltype(m_chatsRemoved)& { return m_chatsRemoved; }
	auto receivedMessages() -> decltype(m_receivedMessages)& { return m_receivedMessages; }
	auto messageSent() -> decltype(m_messageSent)& { return m_messageSent; }
	auto messageNotSent() -> decltype(m_messageNotSent)& { return m_messageNotSent; }
	auto loadedMessages() -> decltype(m_loadedMessages)& { return m_loadedMessages; }
	auto deletedHistory() -> decltype(m_deletedHistory)& { return m_deletedHistory; }
	auto closedChat() -> decltype(m_closedChat)& { return m_closedChat; }
};

} // namespace tvagentapi
