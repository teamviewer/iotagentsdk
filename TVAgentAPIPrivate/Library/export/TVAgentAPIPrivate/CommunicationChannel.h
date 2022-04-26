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

#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/ConnectionData.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/AccessControl.h>
#include <TVRemoteScreenSDKCommunication/ImageService/ColorFormat.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/InstantSupportData.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/InstantSupportError.h>
#include <TVRemoteScreenSDKCommunication/InputService/MouseButton.h>
#include <TVRemoteScreenSDKCommunication/InputService/KeyState.h>
#include <TVRemoteScreenSDKCommunication/SessionControlService/ControlMode.h>
#include <TVRemoteScreenSDKCommunication/SessionStatusService/GrabStrategy.h>
#include <TVRemoteScreenSDKCommunication/ViewGeometryService/VirtualDesktop.h>
#include <TVRemoteScreenSDKCommunication/ChatService/Chat.h>
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/ServiceType.h>

#include "Observer.h"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace TVRemoteScreenSDKCommunication
{
namespace ConnectivityService
{
class IConnectivityServiceServer;
class IConnectivityServiceClient;
} // namespace ConnectivityService

namespace ImageService
{
class IImageServiceClient;
} // namespace ImageService

namespace ImageNotificationService
{
class IImageNotificationServiceClient;
} // ImageNotificationService

namespace InputService
{
class IInputServiceServer;
} // namespace InputService

namespace RegistrationService
{
class IRegistrationServiceClient;
struct ServiceInformation;
} // namespace RegistrationService

namespace SessionControlService
{
class ISessionControlServiceClient;
} // namespace SessionControlService

namespace SessionStatusService
{
class ISessionStatusServiceServer;
} // namespace SessionStatusService

namespace InstantSupportService
{
class IInstantSupportServiceClient;
class IInstantSupportNotificationServiceServer;
} // namespace InstantSupportService

namespace ConnectionConfirmationService
{
class IConnectionConfirmationResponseServiceClient;
class IConnectionConfirmationRequestServiceServer;
} //ConnectionConfirmationService

namespace AccessControlService
{
class IAccessControlInServiceClient;
class IAccessControlOutServiceServer;
} // namespace AccessControlService

namespace ViewGeometryService
{
class IViewGeometryServiceClient;
} // namespace ViewGeometryService

namespace ChatService
{
class IChatInServiceClient;
class IChatOutServiceServer;
} // namespace ChatService

} // namespace TVRemoteScreenSDKCommunication

namespace tvagentapi
{

class ILoggingPrivate;

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
	SchemaNotValid
};

class CommunicationChannel final
{
public:
	static std::shared_ptr<CommunicationChannel> Create(
		std::shared_ptr<ILoggingPrivate> logging,
		std::string registrationServiceLocation);
	~CommunicationChannel();

	bool setRemoteScreenSdkBaseUrl(const std::string& url);
	BaseUrlParseResultCode setRemoteScreenSdkBaseUrlChecked(const std::string& url);

	void startup();
	void shutdown();

	bool requestInstantSupport(
		const std::string& accessToken,
		const std::string& name,
		const std::string& group,
		const std::string& description,
		const std::string& sessionCode,
		const std::string& email);

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
		const std::string& pictureData) const;
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

private:
	CommunicationChannel(std::shared_ptr<ILoggingPrivate> logging, std::string registrationServiceLocation);

	void shutdownInternal();

	std::string getServerLocation(const TVRemoteScreenSDKCommunication::ServiceType serviceType) const;
	bool establishConnection();
	bool setupClientAndServer();

	bool setupAccessControlOutService();
	bool setupConnectivityService();
	bool setupInputService();
	bool setupSessionStatusService();
	bool setupInstantSupportNotificationService();
	bool setupConnectionConfirmationRequestService();
	bool setupChatOutService();

	bool setupConnectivityClient();
	bool setupImageClient();
	bool setupImageNotificationClient();
	bool setupSessionControlClient();
	bool setupChatInClient();
	bool setupInstantSupportClient();
	bool setupConnectionConfirmationResponseClient();
	bool setupAccessControlInClient();

	bool setupViewGeometryClient();

	void sendDisconnect() const;
	void startPing();

	void tearDown();

	struct Condition
	{
		std::condition_variable condition;
		std::mutex mutex;
	};

	const std::shared_ptr<ILoggingPrivate> m_logging;
	const std::string m_registrationServiceLocation;
	const std::unique_ptr<Condition> m_disconnectCondition;
	const std::unique_ptr<Condition> m_shutdownCondition;

	std::string m_socketPrefix;			// e.g. "unix://"
	std::string m_serviceFolderPath;	// e.g. "/tmp/" (NB: always trailing '/')

	std::mutex m_inputServiceServerMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::InputService::IInputServiceServer> m_inputServiceServer;

	std::mutex m_connectivityServiceServerMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::ConnectivityService::IConnectivityServiceServer> m_connectivityServiceServer;

	std::mutex m_sessionStatusServiceServerMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::SessionStatusService::ISessionStatusServiceServer> m_sessionStatusServiceServer;

	std::mutex m_accessControlOutServiceServerMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::AccessControlService::IAccessControlOutServiceServer> m_accessControlOutServiceServer;

	std::mutex m_instantSupportNotificationServiceServerMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::InstantSupportService::IInstantSupportNotificationServiceServer> m_instantSupportNotificationServiceServer;

	std::mutex m_connectionConfirmationRequestServiceServerMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::ConnectionConfirmationService::IConnectionConfirmationRequestServiceServer> m_connectionConfirmationRequestServiceServer;

	std::mutex m_chatOutServiceServerMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::ChatService::IChatOutServiceServer> m_chatOutServiceServer;

	mutable std::mutex m_imageServiceClientMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::ImageService::IImageServiceClient> m_imageServiceClient;

	mutable std::mutex m_connectivityServiceClientMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::ConnectivityService::IConnectivityServiceClient> m_connectivityServiceClient;

	mutable std::mutex m_registrationServiceClientMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::RegistrationService::IRegistrationServiceClient> m_registrationServiceClient;

	std::mutex m_sessionControlServiceClientMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::SessionControlService::ISessionControlServiceClient> m_sessionControlServiceClient;

	mutable std::mutex m_imageNotificationServiceClientMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::ImageNotificationService::IImageNotificationServiceClient> m_imageNotificationServiceClient;

	std::mutex m_instantSupportServiceClientMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::InstantSupportService::IInstantSupportServiceClient> m_instantSupportServiceClient;

	std::mutex m_connectionConfirmationResponseServiceClientMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::ConnectionConfirmationService::IConnectionConfirmationResponseServiceClient> m_connectionConfirmationResponseServiceClient;

	mutable std::mutex m_accessControlInServiceClientMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::AccessControlService::IAccessControlInServiceClient> m_accessControlInServiceClient;

	mutable std::mutex m_viewGeometryServiceClientMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::ViewGeometryService::IViewGeometryServiceClient> m_viewGeometryServiceClient;

	mutable std::mutex m_chatInServiceClientMutex;
	std::unique_ptr<TVRemoteScreenSDKCommunication::ChatService::IChatInServiceClient> m_chatInServiceClient;

	std::string m_communicationId;
	std::vector<TVRemoteScreenSDKCommunication::RegistrationService::ServiceInformation> m_serviceInformations;

	std::thread m_thread;

	std::atomic_bool m_isRunning{false};

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
		TVRemoteScreenSDKCommunication::InputService::KeyState keyState,
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
	auto& accessConfirmationRequested() { return m_accessConfirmationRequested; }
	auto& accessModeChangeNotified() { return m_accessModeChangeNotified; }
	auto& agentCommunicationEstablished() { return m_agentCommunicationEstablished; }
	auto& agentCommunicationLost() { return m_agentCommunicationLost; }
	auto& instantSupportErrorNotification() { return m_instantSupportErrorNotification; }
	auto& instantSupportModifiedNotification() { return m_instantSupportModifiedNotification; }
	auto& instantSupportConnectionConfirmationRequested() { return m_instantSupportConnectionConfirmationRequested; }
	auto& rcSessionStarted() { return m_rcSessionStarted; }
	auto& rcSessionStopped() { return m_rcSessionStopped; }
	auto& tvSessionStarted() { return m_tvSessionStarted; }
	auto& tvSessionStopped() { return m_tvSessionStopped; }
	auto& simulateKeyInputRequested() { return m_simulateKeyInputRequested; }
	auto& simulateMouseMoveRequested() { return m_simulateMouseMoveRequested; }
	auto& simulateMousePressReleaseRequested() { return m_simulateMousePressReleaseRequested; }
	auto& simulateMouseWheelRequested() { return m_simulateMouseWheelRequested; }
	auto& chatCreated() { return m_chatCreated; }
	auto& chatsRemoved() { return m_chatsRemoved; }
	auto& receivedMessages() { return m_receivedMessages; }
	auto& messageSent() { return m_messageSent; }
	auto& messageNotSent() { return m_messageNotSent; }
	auto& loadedMessages() { return m_loadedMessages; }
	auto& deletedHistory() { return m_deletedHistory; }
	auto& closedChat() { return m_closedChat; }
};

} // namespace tvagentapi
