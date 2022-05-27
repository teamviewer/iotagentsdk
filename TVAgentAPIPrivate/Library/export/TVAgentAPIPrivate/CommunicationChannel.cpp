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

#include "CommunicationChannel.h"

#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlInServiceClient.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/InServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlOutServiceServer.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/OutServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ChatService/IChatInServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ChatService/InServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ChatService/IChatOutServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ChatService/OutServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationRequestServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationResponseServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ConnectivityService/IConnectivityServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ConnectivityService/IConnectivityServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ConnectivityService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ImageService/IImageServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ImageService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ImageNotificationService/IImageNotificationServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ImageNotificationService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/InputService/IInputServiceServer.h>
#include <TVRemoteScreenSDKCommunication/InputService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportNotificationServiceServer.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportServiceClient.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/RegistrationService/IRegistrationServiceClient.h>
#include <TVRemoteScreenSDKCommunication/RegistrationService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/SessionControlService/ISessionControlServiceClient.h>
#include <TVRemoteScreenSDKCommunication/SessionControlService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/SessionStatusService/ISessionStatusServiceServer.h>
#include <TVRemoteScreenSDKCommunication/SessionStatusService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ViewGeometryService/IViewGeometryServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ViewGeometryService/ServiceFactory.h>

#include "ILoggingPrivate.h"

#include <sys/stat.h>

namespace tvagentapi
{

namespace
{
constexpr const char* ClientVersion = "1.0";
constexpr const char* AccessControlOutLocation = "TVQtRC/aCOut/";
constexpr const char* ChatOutLocation = "TVQtRC/chatOut/";
constexpr const char* ConnectivityLocation = "TVQtRC/conn/";
constexpr const char* InputLocation = "TVQtRC/input/";
constexpr const char* SessionStatusLocation = "TVQtRC/sesStat/";
constexpr const char* InstantSupportNotificationLocation = "TVQtRC/insSupE/";
constexpr const char* ConnectionConfirmationRequestLocation = "TVQtRC/ConConRe/";
// when adding a new service, also add it to the checks inside the setRemoteScreenSdkBaseUrlChecked function

constexpr uint32_t MaxSizeOfSocketPath = 107; // Socket paths under linux have a limit of around 100 characters. GRPC itself has a hard limit on 107 character.
constexpr uint32_t UuidSize = 32;

template<typename FactoryFuncT, class ClientT>
bool setupClient(const std::string& serviceLocation, FactoryFuncT factory, typename std::unique_ptr<ClientT>& client, std::mutex& mutex)
{
	if (serviceLocation.empty())
	{
		return false;
	}

	std::lock_guard<std::mutex> clientLock(mutex);
	client = factory();
	client->StartClient(serviceLocation);

	return true;
}

template<class ClientT>
void teardownClient(typename std::unique_ptr<ClientT>& client, std::mutex& mutex)
{
	std::lock_guard<std::mutex> clientLock(mutex);
	if (client)
	{
		client->StopClient();
		client.reset();
	}
}

template<class ServerT>
void teardownServer(typename std::unique_ptr<ServerT>& client, std::mutex& mutex)
{
	std::lock_guard<std::mutex> clientLock(mutex);
	if (client)
	{
		client->StopServer();
		client.reset();
	}
}

bool createDirsForPath(const std::string& path)
{
	size_t pos = path.find_first_not_of('/', 0);
	while (pos != std::string::npos)
	{
		pos = path.find('/', pos);
		pos = path.find_first_not_of('/', pos);
		std::string subPath = path.substr(0, pos);
		if (::mkdir(subPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0 && errno != EEXIST)
		{
			return false;
		}
	}
	return true;
}

bool setupDir(const std::string& serviceFolderPath)
{
	constexpr const char* locations[] = {
		AccessControlOutLocation,
		ChatOutLocation,
		ConnectivityLocation,
		InputLocation,
		SessionStatusLocation,
		InstantSupportNotificationLocation,
		ConnectionConfirmationRequestLocation
	};

	for (const char* location : locations)
	{
		if (!createDirsForPath(serviceFolderPath + location))
		{
			return false;
		}
	}

	return true;
}

bool splitRemoteScreenSdkBaseUrl(const std::string& url, std::string& socketPrefix, std::string& serviceFolderPath)
{
	const std::string UrlSplitter = "://";

	size_t schemePos = url.find(UrlSplitter);
	if (schemePos != std::string::npos)
	{
		socketPrefix = url.substr(0, schemePos + UrlSplitter.length());
		serviceFolderPath = url.substr(schemePos + UrlSplitter.length()) + "/";
		return true;
	}
	return false;
}

} // namespace

std::shared_ptr<CommunicationChannel> CommunicationChannel::Create(
	std::shared_ptr<ILoggingPrivate> logging,
	std::string registrationServiceLocation)
{
	const std::shared_ptr<CommunicationChannel> communicationChannel(
		new CommunicationChannel(
			std::move(logging),
			std::move(registrationServiceLocation)));

	communicationChannel->m_weakThis = communicationChannel;
	return communicationChannel;
}

CommunicationChannel::CommunicationChannel(
	std::shared_ptr<ILoggingPrivate> logging,
	std::string registrationServiceLocation)
	: m_logging(std::move(logging))
	, m_registrationServiceLocation(std::move(registrationServiceLocation))
	, m_disconnectCondition(std::make_unique<Condition>())
	, m_shutdownCondition(std::make_unique<Condition>())
{
	setRemoteScreenSdkBaseUrl("unix:///tmp");
}

CommunicationChannel::~CommunicationChannel()
{
	shutdownInternal();
}

bool CommunicationChannel::setRemoteScreenSdkBaseUrl(const std::string& url)
{
	return splitRemoteScreenSdkBaseUrl(url, m_socketPrefix, m_serviceFolderPath);
}

BaseUrlParseResultCode CommunicationChannel::setRemoteScreenSdkBaseUrlChecked(const std::string& url)
{
	std::string socketPrefix;
	std::string serviceFolderPath;
	if (!splitRemoteScreenSdkBaseUrl(url, socketPrefix, serviceFolderPath))
	{
		return BaseUrlParseResultCode::SchemaNotValid;
	}

	const std::vector<std::string> socketLocations = {
		AccessControlOutLocation,
		ChatOutLocation,
		ConnectivityLocation,
		InputLocation,
		SessionStatusLocation,
		InstantSupportNotificationLocation,
		ConnectionConfirmationRequestLocation
	};

	const size_t basicSize = socketPrefix.size() + serviceFolderPath.size() + UuidSize;
	for (const std::string& socketLocation : socketLocations)
	{
		if ((basicSize + socketLocation.size()) >= MaxSizeOfSocketPath)
		{
			// no temporary string allocated, no respective clang-tidy warning
			std::string testPath = socketPrefix;
			testPath.append(serviceFolderPath).append(socketLocation);

			m_logging->logError(
				"[Communication Channel] setRemoteScreenSdkBaseUrl(): "
				"socket path \"" + testPath + "\" exceeds supported length " +
				std::to_string(MaxSizeOfSocketPath) + ".");
			return BaseUrlParseResultCode::CharacterLimitForPathExceeded;
		}
	}

	m_socketPrefix = std::move(socketPrefix);
	m_serviceFolderPath = std::move(serviceFolderPath);

	return BaseUrlParseResultCode::Success;
}

void CommunicationChannel::startup()
{
	std::lock_guard<std::mutex> shutdownLock(m_shutdownCondition->mutex);
	if (m_isRunning)
	{
		return;
	}

	if (m_socketPrefix == "unix://" && !setupDir(m_serviceFolderPath))
	{
		m_logging->logInfo("[Communication Channel] Directory setup failed.");
		return;
	}

	m_isRunning = true;

	m_thread = std::thread([this]()
	{
		constexpr uint8_t ShutdownRetryTime = 1; //seconds

		while (m_isRunning)
		{
			if (establishConnection())
			{
				if (setupClientAndServer())
				{
					m_logging->logInfo("[Communication Channel] Setup successful.");
					agentCommunicationEstablished().notifyAll();
					startPing();
					agentCommunicationLost().notifyAll();
				}
			}

			sendDisconnect();
			tearDown();

			std::unique_lock<std::mutex> shutdownLockLoop(m_shutdownCondition->mutex);
			m_shutdownCondition->condition.wait_for(
				 shutdownLockLoop, std::chrono::seconds(ShutdownRetryTime));
		}
	});
}

void CommunicationChannel::shutdown()
{
	shutdownInternal();
}

void CommunicationChannel::shutdownInternal()
{
	sendDisconnect();

	m_isRunning = false;

	{
		std::lock_guard<std::mutex> lock(m_shutdownCondition->mutex);
		m_shutdownCondition->condition.notify_all();
	}

	if (m_thread.joinable())
	{
		m_thread.join();
	}

	tearDown();
}

bool CommunicationChannel::requestInstantSupport(
	const std::string& accessToken,
	const std::string& name,
	const std::string& group,
	const std::string& description,
	const std::string& sessionCode,
	const std::string& email)
{
	using namespace TVRemoteScreenSDKCommunication::InstantSupportService;

	std::lock_guard<std::mutex> lock(m_instantSupportServiceClientMutex);
	if (m_instantSupportServiceClient)
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			m_instantSupportServiceClient->RequestInstantSupport(m_communicationId, accessToken, name, group, description, sessionCode, email);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Request instant support failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] Instant support client not available.");
		return false;
	}

	return true;
}

bool CommunicationChannel::confirmConnectionRequest(
	TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType connectionType,
	TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionUserConfirmation confirmation)
{
	using namespace TVRemoteScreenSDKCommunication::ConnectionConfirmationService;

	std::lock_guard<std::mutex> lock(m_connectionConfirmationResponseServiceClientMutex);
	if (m_connectionConfirmationResponseServiceClient)
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			m_connectionConfirmationResponseServiceClient->ConfirmConnectionRequest(m_communicationId, connectionType, confirmation);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Instant Support request confirmation failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] Instant support client not available.");
		return false;
	}

	return true;
}

bool CommunicationChannel::sendStop()
{
	std::lock_guard<std::mutex> lock(m_sessionControlServiceClientMutex);
	if (!m_sessionControlServiceClient)
	{
		m_logging->logError("[CommunicationChannel] Session control service client not available.");
		return false;
	}

	const TVRemoteScreenSDKCommunication::CallStatus status =
		m_sessionControlServiceClient->CloseRc(m_communicationId);

	if (!status.IsOk())
	{
		const std::string errorMsg =
			"[Communication Channel] Error in closing the remote control. Reason: " + status.errorMessage;
		m_logging->logError(errorMsg);
		return false;
	}

	return true;
}

bool CommunicationChannel::sendGetRunningTVSessions(std::vector<int32_t>& tvSessionIDs)
{
	using namespace TVRemoteScreenSDKCommunication::SessionControlService;

	std::lock_guard<std::mutex> lock(m_sessionControlServiceClientMutex);
	if (!m_sessionControlServiceClient)
	{
		m_logging->logError("[CommunicationChannel] Session control service client not available.");
		return false;
	}

	const ISessionControlServiceClient::TVSessionsResult result =
		m_sessionControlServiceClient->GetRunningTVSessions(
			m_communicationId);

	if (!result.IsOk())
	{
		const std::string errorMsg =
			"[CommunicationChannel] Failed to request running TVSessions. Reason: " + result.errorMessage;
		m_logging->logError(errorMsg);
		return false;
	}

	tvSessionIDs = std::move(result.tvSessionIDs);
	return true;
}

void CommunicationChannel::sendControlMode(TVRemoteScreenSDKCommunication::SessionControlService::ControlMode mode)
{
	std::lock_guard<std::mutex> lock(m_sessionControlServiceClientMutex);
	if (m_sessionControlServiceClient)
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			m_sessionControlServiceClient->ChangeControlMode(
				m_communicationId,
				mode);

		if (status.IsOk() == false)
		{
			const std::string errorMsg =
				"[CommunicationChannel] Disabling of remote control failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
		}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] Session control service client not available.");
	}
}

bool CommunicationChannel::sendGetAccessMode(
	TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
	TVRemoteScreenSDKCommunication::AccessControlService::Access& access)
{
	using namespace TVRemoteScreenSDKCommunication::AccessControlService;

	std::lock_guard<std::mutex> lock(m_accessControlInServiceClientMutex);
	if (m_accessControlInServiceClient)
	{
		const IAccessControlInServiceClient::GetAccessResponse responseGetAccess =
			m_accessControlInServiceClient->GetAccess(
				m_communicationId,
				feature);

		access = responseGetAccess.access;

		if (!responseGetAccess.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Getting access type failed. Reason: " + responseGetAccess.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] Access control input service client not available.");
		return false;
	}

	return true;
}

bool CommunicationChannel::sendSetAccessMode(
	TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
	TVRemoteScreenSDKCommunication::AccessControlService::Access access)
{
	std::lock_guard<std::mutex> lock(m_accessControlInServiceClientMutex);
	if (m_accessControlInServiceClient)
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			m_accessControlInServiceClient->SetAccess(
				m_communicationId,
				feature,
				access);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Setting access type failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] Access control input service client not available.");
		return false;
	}
	return true;
}

bool CommunicationChannel::sendAccessConfirmationReply(
	TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
	bool confirmed)
{
	std::lock_guard<std::mutex> lock(m_accessControlInServiceClientMutex);
	if (m_accessControlInServiceClient)
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			m_accessControlInServiceClient->ConfirmationReply(
				m_communicationId,
				feature,
				confirmed);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Sending confirmation reply failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] Access control input service client not available.");
		return false;
	}
	return true;
}

void CommunicationChannel::sendScreenGrabResult(
	int32_t x,
	int32_t y,
	int32_t width,
	int32_t height,
	const std::string& pictureData) const
{
	TVRemoteScreenSDKCommunication::CallStatus callStatus;

	std::lock_guard<std::mutex> lock(m_imageServiceClientMutex);
	if (m_imageServiceClient)
	{
		callStatus = m_imageServiceClient->UpdateImage(
			m_communicationId,
			x,
			y,
			width,
			height,
			pictureData);

		if (!callStatus.IsOk())
		{
			const std::string errorMessage = "[Communication Channel] Image update failed: " + callStatus.errorMessage;
			m_logging->logError(errorMessage);
		}
	}
	else
	{
		m_logging->logError("[Communication Channel] Client not available for image service");
	}
}

void CommunicationChannel::sendImageDefinitionForGrabResult(
	const std::string& imageSourceTitle,
	int32_t width,
	int32_t height,
	TVRemoteScreenSDKCommunication::ImageService::ColorFormat format,
	double dpi) const
{
	std::lock_guard<std::mutex> lock(m_imageServiceClientMutex);
	if (m_imageServiceClient)
	{
		const TVRemoteScreenSDKCommunication::CallStatus response =
			m_imageServiceClient->UpdateImageDefinition(
				m_communicationId,
				imageSourceTitle,
				width,
				height,
				format,
				dpi);

		if (response.IsOk() == false)
		{
			const std::string errorMsg =
				"[CommunicationChannel] Sending image definition failed. Reason: " + response.errorMessage;
			m_logging->logError(errorMsg);
		}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] Image service client not available.");
	}
}

void CommunicationChannel::sendGrabRequest(
	int32_t x,
	int32_t y,
	int32_t width,
	int32_t height) const
{
	std::lock_guard<std::mutex> clientLock(m_imageNotificationServiceClientMutex);

	if (m_imageNotificationServiceClient)
	{
		TVRemoteScreenSDKCommunication::CallStatus response =
			m_imageNotificationServiceClient->NotifyImageChanged(
				m_communicationId,
				x,
				y,
				width,
				height);

		if (!response.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Sending grab request failed. Reason: " + response.errorMessage;
			m_logging->logError(errorMsg);
		}
	}
}

void CommunicationChannel::sendImageDefinitionForGrabRequest(
	const std::string& imageSourceTitle,
	int32_t width,
	int32_t height) const
{
	std::lock_guard<std::mutex> clientLock(m_imageNotificationServiceClientMutex);

	if (m_imageNotificationServiceClient)
	{
		TVRemoteScreenSDKCommunication::CallStatus response =
			m_imageNotificationServiceClient->UpdateImageDefinition(
				m_communicationId,
				imageSourceTitle,
				width,
				height);

		if (!response.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Sending image definition failed. Reason: " + response.errorMessage;
			m_logging->logError(errorMsg);
		}
	}
}

ViewGeometrySendResult CommunicationChannel::sendVirtualDesktopGeometry(
	const TVRemoteScreenSDKCommunication::ViewGeometryService::VirtualDesktop& geometry) const
{
	std::lock_guard<std::mutex> clientLock{m_viewGeometryServiceClientMutex};

	if (!m_viewGeometryServiceClient)
	{
		return ViewGeometrySendResult::NoService;
	}

	TVRemoteScreenSDKCommunication::ViewGeometryService::IViewGeometryServiceClient::UpdateResult result =
		m_viewGeometryServiceClient->UpdateVirtualDesktop(m_communicationId, geometry);

	if (!result.IsOk())
	{
		const std::string errorMsg =
			"[CommunicationChannel] Sending virtual desktop geometry failed. Reason: " + result.errorMessage;
		m_logging->logError(errorMsg);
		return ViewGeometrySendResult::CallError;
	}

	if (result.result != TVRemoteScreenSDKCommunication::ViewGeometryService::UpdateGeometryResult::Ok)
	{
		m_logging->logError("Virtual desktop geometry verification failed");
		return ViewGeometrySendResult::Rejected;
	}

	return ViewGeometrySendResult::Ok;
}

ViewGeometrySendResult CommunicationChannel::sendAreaOfInterest(
	const TVRemoteScreenSDKCommunication::ViewGeometryService::Rect& rect) const
{
	std::lock_guard<std::mutex> clientLock{m_viewGeometryServiceClientMutex};

	if (!m_viewGeometryServiceClient)
	{
		return ViewGeometrySendResult::NoService;
	}

	TVRemoteScreenSDKCommunication::ViewGeometryService::IViewGeometryServiceClient::UpdateResult result =
		m_viewGeometryServiceClient->UpdateAreaOfInterest(m_communicationId, rect);

	if (!result.IsOk())
	{
		const std::string errorMsg =
			"[CommunicationChannel] Sending area of interest failed. Reason: " + result.errorMessage;
		m_logging->logError(errorMsg);
		return ViewGeometrySendResult::CallError;
	}

	if (result.result != TVRemoteScreenSDKCommunication::ViewGeometryService::UpdateGeometryResult::Ok)
	{
		m_logging->logError("Failed to update area of interest");
		return ViewGeometrySendResult::Rejected;
	}

	return ViewGeometrySendResult::Ok;
}

bool CommunicationChannel::sendObtainChatsRequest(
	std::vector<TVRemoteScreenSDKCommunication::ChatService::ChatInfo>& chats)
{
	using namespace TVRemoteScreenSDKCommunication::ChatService;

	bool returnValue = false;

	std::lock_guard<std::mutex> lock(m_chatInServiceClientMutex);
	if (m_chatInServiceClient)
	{
		const IChatInServiceClient::ObtainChatsResponse obtainChatsReturnValue =
			m_chatInServiceClient->ObtainChats(m_communicationId);

		if (!obtainChatsReturnValue.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Obtaining chats failed. Reason: " + obtainChatsReturnValue.errorMessage;
			m_logging->logError(errorMsg);
		}
		else
		{
			chats = std::move(obtainChatsReturnValue.chats);
			returnValue = true;
		}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] ChatIn service client not available.");
	}

	return returnValue;
}

bool CommunicationChannel::sendSelectChatResult(std::string chatId)
{
	std::lock_guard<std::mutex> lock(m_chatInServiceClientMutex);
	if (m_chatInServiceClient)
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			m_chatInServiceClient->SelectChat(
				m_communicationId,
				chatId);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Selecting chat failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] ChatIn service client not available.");
		return false;
	}

	return true;
}

bool CommunicationChannel::sendMessage(uint32_t localId, std::string content)
{
	std::lock_guard<std::mutex> lock(m_chatInServiceClientMutex);
	if (m_chatInServiceClient)
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			m_chatInServiceClient->SendMessage(
				m_communicationId,
				localId,
				content);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Sending message failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
			}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] ChatIn service client not available.");
		return false;
	}

	return true;
}

bool CommunicationChannel::loadMessages(uint32_t count, std::string lastId)
{
	if (lastId.empty())
	{
		// agent doesn't accept empty string
		lastId = "00000000-0000-0000-0000-000000000000";
	}

	std::lock_guard<std::mutex> lock(m_chatInServiceClientMutex);
	if (m_chatInServiceClient)
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			m_chatInServiceClient->LoadMessages(
				m_communicationId,
				count,
				lastId);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Load messages failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] ChatIn service client not available.");
		return false;
	}

	return true;
}

bool CommunicationChannel::deleteHistory()
{
	std::lock_guard<std::mutex> lock(m_chatInServiceClientMutex);
	if (m_chatInServiceClient)
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			m_chatInServiceClient->DeleteHistory(m_communicationId);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Deleting chat history failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] ChatIn service client not available.");
		return false;
	}

	return true;
}

bool CommunicationChannel::deleteChat()
{
	std::lock_guard<std::mutex> lock(m_chatInServiceClientMutex);
	if (m_chatInServiceClient)
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			m_chatInServiceClient->DeleteChat(m_communicationId);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Deleting chat failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
	}
	else
	{
		m_logging->logError("[CommunicationChannel] ChatIn service client not available.");
		return false;
	}

	return true;
}

std::string CommunicationChannel::getServerLocation(const TVRemoteScreenSDKCommunication::ServiceType serviceType) const
{
	for (const TVRemoteScreenSDKCommunication::RegistrationService::ServiceInformation& serviceInformation : m_serviceInformations)
	{
		if (serviceInformation.type == serviceType)
		{
			return serviceInformation.location;
		}
	}

	return std::string();
}

bool CommunicationChannel::establishConnection()
{
	using namespace TVRemoteScreenSDKCommunication::RegistrationService;

	std::lock_guard<std::mutex> lock(m_registrationServiceClientMutex);
	m_registrationServiceClient = TVRemoteScreenSDKCommunication::RegistrationService::ServiceFactory::CreateClient();
	m_registrationServiceClient->StartClient(m_socketPrefix + m_serviceFolderPath + m_registrationServiceLocation);

	const IRegistrationServiceClient::ExchangeVersionResponse exchangeResponse =
		m_registrationServiceClient->ExchangeVersion(ClientVersion);

	if (exchangeResponse.IsOk() == false)
	{
		const std::string errorMsg =
			"[CommunicationChannel] Exchange of version failed. Reason: " + exchangeResponse.errorMessage;
		m_logging->logError(errorMsg);
		return false;
	}

	const std::string ServerVersion = exchangeResponse.versionNumber;
	std::string version = ServerVersion;

	if (std::strtof(ClientVersion, nullptr) < std::strtof(ServerVersion.c_str(), nullptr))
	{
		version = ClientVersion;
	}

	const IRegistrationServiceClient::DiscoverResponse discoverResponse = m_registrationServiceClient->Discover(version);

	if (!discoverResponse.IsOk())
	{
		const std::string errorMsg =
			"[CommunicationChannel] Version Handshake failed. Reason: " + exchangeResponse.errorMessage;
		m_logging->logError(errorMsg);
		return false;
	}

	m_communicationId = discoverResponse.communicationId;
	m_serviceInformations = discoverResponse.services;

	return true;
}


bool CommunicationChannel::setupClientAndServer()
{
	using ThisType = CommunicationChannel;
	struct Setup
	{
		const char* name;
		bool (ThisType::*setup)();
	};

	constexpr Setup mandatorySetups[] =
	{
		{"Connectivity client" , &ThisType::setupConnectivityClient },
		{"Connectivity service", &ThisType::setupConnectivityService},
	};

	constexpr Setup optionalSetups[] =
	{
		{"Session control client"                         , &ThisType::setupSessionControlClient                     },
		{"Session status service"                         , &ThisType::setupSessionStatusService                     },
		{"Image client"                                   , &ThisType::setupImageClient                              },
		{"Image notification client"                      , &ThisType::setupImageNotificationClient                  },
		{"Instant support client"                         , &ThisType::setupInstantSupportClient                     },
		{"Access control-in client"                       , &ThisType::setupAccessControlInClient                    },
		{"Input service"                                  , &ThisType::setupInputService                             },
		{"Access control-out service"                     , &ThisType::setupAccessControlOutService                  },
		{"View geometry client"                           , &ThisType::setupViewGeometryClient                       },
		{"Instant support notification service"           , &ThisType::setupInstantSupportNotificationService        },
		{"Chat-in client"                                 , &ThisType::setupChatInClient                             },
		{"Chat-out service"                               , &ThisType::setupChatOutService                           },
		{"Instant support confirmation request service"   , &ThisType::setupConnectionConfirmationRequestService     },
		{"Instant support confirmation response client"   , &ThisType::setupConnectionConfirmationResponseClient     },
	};

	for (const auto setup: mandatorySetups)
	{
		const bool setupResult = (this->*setup.setup)();
		if (!setupResult)
		{
			m_logging->logError(std::string("Mandatory setup failed for: ") + setup.name);
			return false;
		}
	}

	for (const auto setup: optionalSetups)
	{
		const bool setupResult = (this->*setup.setup)();
		if (!setupResult)
		{
			m_logging->logError(std::string("Optional setup failed for: ") + setup.name);
		}
	}

	return true;
}

bool CommunicationChannel::setupConnectivityService()
{
	using namespace TVRemoteScreenSDKCommunication::ConnectivityService;

	std::lock_guard<std::mutex> lock(m_connectivityServiceServerMutex);
	m_connectivityServiceServer = ServiceFactory::CreateServer();

	if (m_connectivityServiceServer->StartServer(m_socketPrefix + m_serviceFolderPath + ConnectivityLocation + m_communicationId) == false)
	{
		return false;
	}

	auto availableCallback = [communicationId{m_communicationId}](
		const std::string& comId, const IConnectivityServiceServer::IsAvailableResponseCallback& response)
	{
		if (comId == communicationId)
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_connectivityServiceServer->SetIsAvailableCallback(availableCallback);

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;
	auto disconnectCallback = [weakThis](
		const std::string& comId, const IConnectivityServiceServer::IsAvailableResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));

			const std::lock_guard<std::mutex> disconnectLock(communicationChannel->m_disconnectCondition->mutex);
			communicationChannel->m_disconnectCondition->condition.notify_all();
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_connectivityServiceServer->SetDisconnectCallback(disconnectCallback);

	return true;
}

bool CommunicationChannel::setupInputService()
{
	std::unique_lock<std::mutex> lock{m_inputServiceServerMutex};
	m_inputServiceServer = TVRemoteScreenSDKCommunication::InputService::ServiceFactory::CreateServer();

	if (!m_inputServiceServer->StartServer(m_socketPrefix + m_serviceFolderPath + InputLocation + m_communicationId))
	{
		return false;
	}

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;

	auto receivedKeyCallback = [weakThis](
		const std::string& comId,
		TVRemoteScreenSDKCommunication::InputService::KeyState keyState,
		uint32_t xkbSymbol,
		uint32_t unicodeCharacter,
		uint32_t xkbModifiers,
		const TVRemoteScreenSDKCommunication::InputService::IInputServiceServer::KeyResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId)
			&& keyState != TVRemoteScreenSDKCommunication::InputService::KeyState::Unknown)
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->simulateKeyInputRequested().notifyAll(keyState, xkbSymbol, unicodeCharacter, xkbModifiers);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_inputServiceServer->SetReceivedKeyCallback(receivedKeyCallback);

	auto mouseMoveCallback = [weakThis](
		const std::string& comId,
		int32_t posX,
		int32_t posY,
		const TVRemoteScreenSDKCommunication::InputService::IInputServiceServer::MouseMoveResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->simulateMouseMoveRequested().notifyAll(posX, posY);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_inputServiceServer->SetMouseMoveCallback(mouseMoveCallback);

	auto mousePressReleaseCallback = [weakThis](
		const std::string& comId,
		TVRemoteScreenSDKCommunication::InputService::KeyState keyState,
		int32_t posX,
		int32_t posY,
		TVRemoteScreenSDKCommunication::InputService::MouseButton button,
		const TVRemoteScreenSDKCommunication::InputService::IInputServiceServer::MousePressReleaseResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId)
			&& keyState != TVRemoteScreenSDKCommunication::InputService::KeyState::Unknown
			&& button != TVRemoteScreenSDKCommunication::InputService::MouseButton::Unknown)
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->simulateMousePressReleaseRequested().notifyAll(keyState, posX, posY, button);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_inputServiceServer->SetMousePressReleaseCallback(mousePressReleaseCallback);

	auto mouseWheelCallback = [weakThis](
		const std::string& comId,
		int32_t posX,
		int32_t posY,
		int32_t angle,
		const TVRemoteScreenSDKCommunication::InputService::IInputServiceServer::MouseWheelResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->simulateMouseWheelRequested().notifyAll(posX, posY, angle);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_inputServiceServer->SetMouseWheelCallback(mouseWheelCallback);
	lock.unlock();

	std::lock_guard<std::mutex> guard{m_registrationServiceClientMutex};

	TVRemoteScreenSDKCommunication::CallStatus status = m_registrationServiceClient->RegisterService(
		m_communicationId,
		TVRemoteScreenSDKCommunication::ServiceType::Input,
		m_socketPrefix + m_serviceFolderPath + InputLocation + m_communicationId);

	return status.IsOk();
}

bool CommunicationChannel::setupSessionStatusService()
{
	using namespace TVRemoteScreenSDKCommunication::SessionStatusService;

	std::unique_lock<std::mutex> lock{m_sessionStatusServiceServerMutex};
	m_sessionStatusServiceServer = ServiceFactory::CreateServer();

	if (m_sessionStatusServiceServer->StartServer(m_socketPrefix + m_serviceFolderPath + SessionStatusLocation + m_communicationId) == false)
	{
		return false;
	}

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;
	auto remoteControlStarted = [weakThis](
		const std::string& comId,
		const GrabStrategy strategy,
		const ISessionStatusServiceServer::RemoteControllStartedResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->rcSessionStarted().notifyAll(strategy);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_sessionStatusServiceServer->SetRemoteControlStartedCallback(remoteControlStarted);

	auto remoteControlStopped = [weakThis](
		const std::string& comId,
		const ISessionStatusServiceServer::RemoteControllStartedResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->rcSessionStopped().notifyAll();
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_sessionStatusServiceServer->SetRemoteControlStoppedCallback(remoteControlStopped);

	auto tvSessionStarted = [weakThis](
		const std::string& comId,
		const int32_t tvSessionID,
		const int32_t tvSessionsCount,
		const ISessionStatusServiceServer::RemoteControllStartedResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->tvSessionStarted().notifyAll(tvSessionID, tvSessionsCount);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_sessionStatusServiceServer->SetTVSessionStartedCallback(tvSessionStarted);

	auto tvSessionStopped = [weakThis](
		const std::string& comId,
		const int32_t tvSessionID,
		const int32_t tvSessionsCount,
		const ISessionStatusServiceServer::RemoteControllStartedResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->tvSessionStopped().notifyAll(tvSessionID, tvSessionsCount);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_sessionStatusServiceServer->SetTVSessionStoppedCallback(tvSessionStopped);

	lock.unlock();

	std::lock_guard<std::mutex> guard{m_registrationServiceClientMutex};

	TVRemoteScreenSDKCommunication::CallStatus status = m_registrationServiceClient->RegisterService(
		m_communicationId,
		TVRemoteScreenSDKCommunication::ServiceType::SessionStatus,
		m_socketPrefix + m_serviceFolderPath + SessionStatusLocation + m_communicationId);

	return status.IsOk();
}

bool CommunicationChannel::setupAccessControlOutService()
{
	using namespace TVRemoteScreenSDKCommunication::AccessControlService;

	std::unique_lock<std::mutex> lock{m_accessControlOutServiceServerMutex};
	m_accessControlOutServiceServer = OutServiceFactory::CreateServer();

	if (m_accessControlOutServiceServer->StartServer(m_socketPrefix + m_serviceFolderPath + AccessControlOutLocation + m_communicationId) == false)
	{
		return false;
	}

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;
	auto confirmationRequest = [weakThis](
		const std::string& comId,
		TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
		uint32_t timeout,
		const IAccessControlOutServiceServer::ConfirmationResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->accessConfirmationRequested().notifyAll(feature, timeout);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_accessControlOutServiceServer->SetProcessConfirmationRequestCallback(confirmationRequest);

	auto notifyAccessModeChange = [weakThis](
		const std::string& comId,
		TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
		TVRemoteScreenSDKCommunication::AccessControlService::Access accessMode,
		const IAccessControlOutServiceServer::NotifyChangeResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->accessModeChangeNotified().notifyAll(feature, accessMode);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_accessControlOutServiceServer->SetProcessNotifyChangeRequestCallback(notifyAccessModeChange);
	lock.unlock();

	std::lock_guard<std::mutex> guard{m_registrationServiceClientMutex};

	TVRemoteScreenSDKCommunication::CallStatus status = m_registrationServiceClient->RegisterService(
		m_communicationId,
		TVRemoteScreenSDKCommunication::ServiceType::AccessControlOut,
		m_socketPrefix + m_serviceFolderPath + AccessControlOutLocation + m_communicationId);

	return status.IsOk();
}

bool CommunicationChannel::setupInstantSupportNotificationService()
{
	using namespace TVRemoteScreenSDKCommunication::InstantSupportService;

	std::unique_lock<std::mutex> lock{m_instantSupportNotificationServiceServerMutex};
	m_instantSupportNotificationServiceServer = ServiceFactory::CreateInstantSupportNotificationServiceServer();

	if (!m_instantSupportNotificationServiceServer->StartServer(m_socketPrefix + m_serviceFolderPath + InstantSupportNotificationLocation + m_communicationId))
	{
		return false;
	}

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;
	auto errorHandling = [weakThis](
		const std::string& comId,
		TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError responseError,
		const IInstantSupportNotificationServiceServer::ResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
/*
			InstantSupportError sdkResponseError = getQtSdkInstantSupportError(responseError);
*/
			(void)responseError;
			communicationChannel->instantSupportErrorNotification().notifyAll(/*sdkResponseError*/responseError);
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};

	m_instantSupportNotificationServiceServer->SetInstantSupportErrorNotificationCallback(errorHandling);

	auto modifiedHandling = [weakThis](
		const std::string& comId,
		InstantSupportData data,
		const IInstantSupportNotificationServiceServer::ResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
/*
			InstantSupportData sdkResponseData = getQtSdkInstantSupportData(data);
*/
			communicationChannel->instantSupportModifiedNotification().notifyAll(data);
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};

	m_instantSupportNotificationServiceServer->SetInstantSupportModifiedNotificationCallback(modifiedHandling);

	lock.unlock();

	std::lock_guard<std::mutex> guard{m_registrationServiceClientMutex};

	TVRemoteScreenSDKCommunication::CallStatus status = m_registrationServiceClient->RegisterService(
		m_communicationId,
		TVRemoteScreenSDKCommunication::ServiceType::InstantSupportNotification,
		m_socketPrefix + m_serviceFolderPath + InstantSupportNotificationLocation + m_communicationId);

	return status.IsOk();
}

bool CommunicationChannel::setupConnectionConfirmationRequestService()
{
	using namespace TVRemoteScreenSDKCommunication::ConnectionConfirmationService;

	std::unique_lock<std::mutex> lock{m_connectionConfirmationRequestServiceServerMutex};
	m_connectionConfirmationRequestServiceServer = ServiceFactory::CreateConnectionConfirmationRequestServiceServer();

	if (!m_connectionConfirmationRequestServiceServer->StartServer(m_socketPrefix + m_serviceFolderPath + ConnectionConfirmationRequestLocation + m_communicationId))
	{
		return false;
	}

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;
	const auto connectionConfirmationRequestCallback = [weakThis](
		const std::string& comId,
		TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType connectionType,
		const IConnectionConfirmationRequestServiceServer::ConnectionConfirmationRequestResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			switch (connectionType) {
				case TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType::InstantSupport:
					communicationChannel->instantSupportConnectionConfirmationRequested().notifyAll();
					response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
					break;
				case TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType::Undefined:
				default:
					response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
					break;
			}
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};

	m_connectionConfirmationRequestServiceServer->SetRequestConnectionConfirmationCallback(connectionConfirmationRequestCallback);

	lock.unlock();

	std::lock_guard<std::mutex> guard{m_registrationServiceClientMutex};

	TVRemoteScreenSDKCommunication::CallStatus status = m_registrationServiceClient->RegisterService(
		m_communicationId,
		TVRemoteScreenSDKCommunication::ServiceType::ConnectionConfirmationRequest,
		m_socketPrefix + m_serviceFolderPath + ConnectionConfirmationRequestLocation + m_communicationId);

	return status.IsOk();
}

bool CommunicationChannel::setupChatOutService()
{
	using namespace TVRemoteScreenSDKCommunication::ChatService;

	std::unique_lock<std::mutex> lock{m_chatOutServiceServerMutex};
	m_chatOutServiceServer = OutServiceFactory::CreateServer();

	if (m_chatOutServiceServer->StartServer(m_socketPrefix + m_serviceFolderPath + ChatOutLocation + m_communicationId) == false)
	{
		return false;
	}

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;
	auto chatCreatedRequest = [weakThis](
		const std::string& comId,
		std::string chatId,
		std::string title,
		TVRemoteScreenSDKCommunication::ChatService::ChatType sdkchatType,
		uint32_t chatTypeId,
		const IChatOutServiceServer::ChatCreatedResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->chatCreated().notifyAll(chatId, title, sdkchatType, chatTypeId);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_chatOutServiceServer->SetProcessChatCreatedRequestCallback(chatCreatedRequest);

	auto chatsRemovedRequest = [weakThis](
		const std::string& comId,
		std::vector<std::string> chatIds,
		const IChatOutServiceServer::ChatsRemovedResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->chatsRemoved().notifyAll(chatIds);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_chatOutServiceServer->SetProcessChatsRemovedRequestCallback(chatsRemovedRequest);

	auto receivedMessagesRequest = [weakThis](
		const std::string& comId,
		std::vector<TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage> messages,
		const IChatOutServiceServer::ReceivedMessagesResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->receivedMessages().notifyAll(messages);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_chatOutServiceServer->SetProcessReceivedMessagesRequestCallback(receivedMessagesRequest);

	auto messageSentRequest = [weakThis](
		const std::string& comId,
		uint32_t localId,
		std::string messageId,
		uint64_t timeStamp,
		const IChatOutServiceServer::MessageSentResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->messageSent().notifyAll(localId, messageId, timeStamp);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_chatOutServiceServer->SetProcessMessageSentRequestCallback(messageSentRequest);

	auto messageNotSentRequest = [weakThis](
		const std::string& comId,
		uint32_t localId,
		const IChatOutServiceServer::MessageNotSentResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->messageNotSent().notifyAll(localId);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_chatOutServiceServer->SetProcessMessageNotSentRequestCallback(messageNotSentRequest);

	auto loadedMessagesRequest = [weakThis](
		const std::string& comId,
		std::vector<TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage> messages,
		bool hasMore,
		const IChatOutServiceServer::ReceivedMessagesResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->loadedMessages().notifyAll(messages, hasMore);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_chatOutServiceServer->SetProcessLoadedMessagesRequestCallback(loadedMessagesRequest);

	auto deletedHistoryRequest = [weakThis](
		const std::string& comId,
		std::string chatId,
		const IChatOutServiceServer::DeletedHistoryResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->deletedHistory().notifyAll(chatId);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_chatOutServiceServer->SetProcessDeletedHistoryRequestCallback(deletedHistoryRequest);

	auto closedChatRequest = [weakThis](
		const std::string& comId,
		std::string chatId,
		const IChatOutServiceServer::ClosedChatResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Ok));
			communicationChannel->closedChat().notifyAll(chatId);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus(TVRemoteScreenSDKCommunication::CallState::Failed));
		}
	};
	m_chatOutServiceServer->SetProcessClosedChatRequestCallback(closedChatRequest);

	lock.unlock();

	std::lock_guard<std::mutex> guard{m_registrationServiceClientMutex};

	TVRemoteScreenSDKCommunication::CallStatus status = m_registrationServiceClient->RegisterService(
		m_communicationId,
		TVRemoteScreenSDKCommunication::ServiceType::ChatOut,
		m_socketPrefix + m_serviceFolderPath + ChatOutLocation + m_communicationId);

	return status.IsOk();
}

bool CommunicationChannel::setupConnectivityClient()
{
	const std::string ServiceLocation = getServerLocation(TVRemoteScreenSDKCommunication::ServiceType::Connectivity);
	if (!setupClient(
		ServiceLocation,
		TVRemoteScreenSDKCommunication::ConnectivityService::ServiceFactory::CreateClient,
		m_connectivityServiceClient,
		m_connectivityServiceClientMutex))
	{
		return false;
	}

	std::lock_guard<std::mutex> guard{m_registrationServiceClientMutex};

	TVRemoteScreenSDKCommunication::CallStatus status = m_registrationServiceClient->RegisterService(
		m_communicationId,
		TVRemoteScreenSDKCommunication::ServiceType::Connectivity,
		m_socketPrefix + m_serviceFolderPath + ConnectivityLocation + m_communicationId);

	return status.IsOk();
}

bool CommunicationChannel::setupImageClient()
{
	const std::string ServiceLocation = getServerLocation(TVRemoteScreenSDKCommunication::ServiceType::Image);
	return setupClient(
		ServiceLocation,
		TVRemoteScreenSDKCommunication::ImageService::ServiceFactory::CreateClient,
		m_imageServiceClient,
		m_imageServiceClientMutex);
}

bool CommunicationChannel::setupImageNotificationClient()
{
	const std::string ServiceLocation = getServerLocation(TVRemoteScreenSDKCommunication::ServiceType::ImageNotification);
	return setupClient(
		ServiceLocation,
		TVRemoteScreenSDKCommunication::ImageNotificationService::ServiceFactory::CreateClient,
		m_imageNotificationServiceClient,
		m_imageNotificationServiceClientMutex);
}

bool CommunicationChannel::setupSessionControlClient()
{
	const std::string ServiceLocation = getServerLocation(TVRemoteScreenSDKCommunication::ServiceType::SessionControl);
	return setupClient(
		ServiceLocation,
		TVRemoteScreenSDKCommunication::SessionControlService::ServiceFactory::CreateClient,
		m_sessionControlServiceClient,
		m_sessionControlServiceClientMutex);
}

bool CommunicationChannel::setupInstantSupportClient()
{
	const std::string ServiceLocation = getServerLocation(TVRemoteScreenSDKCommunication::ServiceType::InstantSupport);
	return setupClient(
		ServiceLocation,
		TVRemoteScreenSDKCommunication::InstantSupportService::ServiceFactory::CreateClient,
		m_instantSupportServiceClient,
		m_instantSupportServiceClientMutex);
}

bool CommunicationChannel::setupConnectionConfirmationResponseClient()
{
	const std::string ServiceLocation = getServerLocation(TVRemoteScreenSDKCommunication::ServiceType::ConnectionConfirmationResponse);
	return setupClient(
		ServiceLocation,
		TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ServiceFactory::CreateConnectionConfirmationResponseServiceClient,
		m_connectionConfirmationResponseServiceClient,
		m_connectionConfirmationResponseServiceClientMutex);
}

bool CommunicationChannel::setupAccessControlInClient()
{
	const std::string ServiceLocation = getServerLocation(TVRemoteScreenSDKCommunication::ServiceType::AccessControlIn);
	return setupClient(
		ServiceLocation,
		TVRemoteScreenSDKCommunication::AccessControlService::InServiceFactory::CreateClient,
		m_accessControlInServiceClient,
		m_accessControlInServiceClientMutex);
}

bool CommunicationChannel::setupViewGeometryClient()
{
	const std::string serviceLocation =
		getServerLocation(TVRemoteScreenSDKCommunication::ServiceType::ViewGeometry);

	return setupClient(
		serviceLocation,
		TVRemoteScreenSDKCommunication::ViewGeometryService::ServiceFactory::CreateClient,
		m_viewGeometryServiceClient,
		m_viewGeometryServiceClientMutex);
}

bool CommunicationChannel::setupChatInClient()
{
	const std::string ServiceLocation = getServerLocation(TVRemoteScreenSDKCommunication::ServiceType::ChatIn);
	return setupClient(
		ServiceLocation,
		TVRemoteScreenSDKCommunication::ChatService::InServiceFactory::CreateClient,
		m_chatInServiceClient,
		m_chatInServiceClientMutex);
}

void CommunicationChannel::sendDisconnect() const
{
	{
		std::lock_guard<std::mutex> connectivityServiceClientLock(m_connectivityServiceClientMutex);
		if (m_connectivityServiceClient)
		{
			m_connectivityServiceClient->Disconnect(m_communicationId);
		}
	}

	{
		std::lock_guard<std::mutex> disconnectConditionLock(m_disconnectCondition->mutex);
		m_disconnectCondition->condition.notify_all();
	}
}

void CommunicationChannel::startPing()
{
	constexpr uint8_t PingTimeout = 5; //seconds

	while (m_isRunning)
	{
		{
			std::unique_lock<std::mutex> lock(m_disconnectCondition->mutex);
			if (m_disconnectCondition->condition.wait_for(
					lock,
					std::chrono::seconds(PingTimeout)) != std::cv_status::timeout)
			{
				return;
			}
		}

		{
			const std::lock_guard<std::mutex> lock(m_connectivityServiceClientMutex);
			if (!m_connectivityServiceClient ||
				!m_connectivityServiceClient->IsAvailable(m_communicationId).IsOk())
			{
				return;
			}
		}
	}
}

void CommunicationChannel::tearDown()
{
	teardownServer(m_connectivityServiceServer,                      m_connectivityServiceServerMutex);
	teardownServer(m_inputServiceServer,                             m_inputServiceServerMutex);
	teardownServer(m_sessionStatusServiceServer,                     m_sessionStatusServiceServerMutex);
	teardownServer(m_accessControlOutServiceServer,                  m_accessControlOutServiceServerMutex);
	teardownServer(m_instantSupportNotificationServiceServer,        m_instantSupportNotificationServiceServerMutex);
	teardownServer(m_chatOutServiceServer,                           m_chatOutServiceServerMutex);
	teardownServer(m_connectionConfirmationRequestServiceServer,     m_connectionConfirmationRequestServiceServerMutex);

	teardownClient(m_connectivityServiceClient,                      m_connectivityServiceClientMutex);
	teardownClient(m_imageNotificationServiceClient,                 m_imageNotificationServiceClientMutex);
	teardownClient(m_imageServiceClient,                             m_imageServiceClientMutex);
	teardownClient(m_registrationServiceClient,                      m_registrationServiceClientMutex);
	teardownClient(m_sessionControlServiceClient,                    m_sessionControlServiceClientMutex);
	teardownClient(m_instantSupportServiceClient,                    m_instantSupportServiceClientMutex);
	teardownClient(m_accessControlInServiceClient,                   m_accessControlInServiceClientMutex);
	teardownClient(m_viewGeometryServiceClient,                      m_viewGeometryServiceClientMutex);
	teardownClient(m_chatInServiceClient,                            m_chatInServiceClientMutex);
	teardownClient(m_connectionConfirmationResponseServiceClient,    m_connectionConfirmationResponseServiceClientMutex);
}

} // namespace tvagentapi
