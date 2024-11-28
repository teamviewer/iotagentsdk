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

#include "ILoggingPrivate.h"

#include "internal/ServicesMediator.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/VersionNumber.h>

#include <sys/stat.h>

namespace tvagentapi
{

using TVRemoteScreenSDKCommunication::ServiceType;
using TVRemoteScreenSDKCommunication::TransportFramework;
using TVRemoteScreenSDKCommunication::UrlComponents;
using TVRemoteScreenSDKCommunication::VersionNumber;
using TVRemoteScreenSDKCommunication::VersionNumberFromString;
using TVRemoteScreenSDKCommunication::VersionNumberToString;

namespace
{
constexpr VersionNumber ClientVersion = {1, 0}; // our SDK version

constexpr uint32_t MaxSizeOfSocketPath = 107; // Socket paths under linux have a limit of around 100 characters. GRPC itself has a hard limit on 107 character.
constexpr uint32_t UuidSize = 32;

constexpr const char* DefaultgRPCRegServicePath = "teamviewer-iot-agent-services/remoteScreen/registrationService";
constexpr const char* DefaultTCPRegServiceSocket = "9221";

#if defined(TV_COMM_ENABLE_GRPC)
const std::string DefaultBaseServerUrl = "unix:///tmp";
const std::string DefaultAgentRegistrationServiceUrl = DefaultBaseServerUrl + '/' + DefaultgRPCRegServicePath;
#elif defined(TV_COMM_ENABLE_PLAIN_SOCKET)
const std::string DefaultBaseServerUrl = "tcp+tv://127.0.0.1";
const std::string DefaultAgentRegistrationServiceUrl = DefaultBaseServerUrl + ':' + DefaultTCPRegServiceSocket;
#endif

bool CreateDirsForPath(const std::string& path)
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

bool SetupDir(const std::string& serviceFolderPath)
{
	auto ServerLocation = &ServicesMediator::ServerLocation<TransportFramework::gRPCTransport>;
	const char* locations[] = {
		ServerLocation(ServiceType::AccessControlOut),
		ServerLocation(ServiceType::ChatOut),
		ServerLocation(ServiceType::Connectivity),
		ServerLocation(ServiceType::Input),
		ServerLocation(ServiceType::SessionStatus),
		ServerLocation(ServiceType::InstantSupportNotification),
		ServerLocation(ServiceType::ConnectionConfirmationRequest),
		ServerLocation(ServiceType::AugmentRCSessionConsumer)
	};

	for (const char* location : locations)
	{
		if (!CreateDirsForPath(serviceFolderPath + '/' + location))
		{
			return false;
		}
	}

	return true;
}

bool ParseUrlIntoUrlComponentsAndFramework(
	const std::string& url,
	UrlComponents& components,
	TransportFramework& transportFramework)
{
	if (!TVRemoteScreenSDKCommunication::ParseUrl(url, components))
	{
		return false;
	}
	transportFramework = TVRemoteScreenSDKCommunication::GetTransportFramework(components.scheme);
	return true;
}

} // namespace

std::shared_ptr<CommunicationChannel> CommunicationChannel::Create(std::shared_ptr<ILoggingPrivate> logging)
{
	std::shared_ptr<CommunicationChannel> communicationChannel(new CommunicationChannel(std::move(logging)));

	communicationChannel->m_weakThis = communicationChannel;
	return communicationChannel;
}

CommunicationChannel::CommunicationChannel(
	std::shared_ptr<ILoggingPrivate> logging)
	: m_logging(std::move(logging))
	, m_disconnectCondition(new Condition())
	, m_shutdownCondition(new Condition())
	, m_grabResultCondition(new Condition())
{
	setUrls(DefaultBaseServerUrl, DefaultAgentRegistrationServiceUrl);
}

CommunicationChannel::~CommunicationChannel()
{
	shutdownInternal();
}

bool CommunicationChannel::setRemoteScreenSdkBaseUrl(const std::string& url)
{
	return setRemoteScreenSdkBaseUrlChecked(url) == BaseUrlParseResultCode::Success;
}

BaseUrlParseResultCode CommunicationChannel::setRemoteScreenSdkBaseUrlChecked(const std::string& url)
{
	UrlComponents components{};
	TransportFramework framework = TransportFramework::UnknownTransport;
	if (!ParseUrlIntoUrlComponentsAndFramework(url, components, framework))
	{
		return BaseUrlParseResultCode::SchemaNotValid;
	}

	std::string agentRegistrationServiceUrl = components.scheme;
	agentRegistrationServiceUrl.append("://");
	switch (framework)
	{
		case TransportFramework::gRPCTransport:
			agentRegistrationServiceUrl.append(components.path).append("/").append(DefaultgRPCRegServicePath);
			break;
		case TransportFramework::TCPSocketTransport:
			agentRegistrationServiceUrl.append(components.host).append(":").append(DefaultTCPRegServiceSocket);
			break;
		case TransportFramework::UnknownTransport:
			return BaseUrlParseResultCode::SchemaNotValid;
	}

	return setUrls(url, agentRegistrationServiceUrl);
}

BaseUrlParseResultCode CommunicationChannel::setUrls(const std::string& baseServerUrl, const std::string& agentApiUrl)
{
	if (m_isRunning)
	{
		m_logging->logError(
			"[Communication Channel] setUrls(): "
			"the connection is currently in use");
		return BaseUrlParseResultCode::ConnectionIsInUse;
	}
	UrlComponents components{};
	TransportFramework framework = TransportFramework::UnknownTransport;
	if (!ParseUrlIntoUrlComponentsAndFramework(baseServerUrl, components, framework))
	{
		return BaseUrlParseResultCode::SchemaNotValid;
	}

	UrlComponents agentApiUrlComponents{};
	TransportFramework agentFramework = TransportFramework::UnknownTransport;
	if (!ParseUrlIntoUrlComponentsAndFramework(agentApiUrl, agentApiUrlComponents, agentFramework))
	{
		return BaseUrlParseResultCode::SchemaNotValid;
	}

	if (agentFramework != framework)
	{
		m_logging->logError(
			"[Communication Channel] setUrls(): "
			"base and agent URLs must use the same URL scheme");
		return BaseUrlParseResultCode::SchemaNotValid;
	}

	switch (framework)
	{
		case TransportFramework::gRPCTransport:
			{
				if (!components.host.empty())
				{
					m_logging->logError(
						"[Communication Channel] setUrls(): "
						"Base gRPC socket url \"" + baseServerUrl + "\" must contain no host component.");
					return BaseUrlParseResultCode::SchemaNotValid;
				}
				if (components.port != 0)
				{
					m_logging->logError(
						"[Communication Channel] setUrls(): "
						"Base gRPC socket url \"" + baseServerUrl + "\" must contain no port component.");
					return BaseUrlParseResultCode::SchemaNotValid;
				}
				if (!agentApiUrlComponents.host.empty())
				{
					m_logging->logError(
						"[Communication Channel] setUrls(): "
						"Agent gRPC socket url \"" + baseServerUrl + "\" must contain no host component.");
					return BaseUrlParseResultCode::SchemaNotValid;
				}
				if (agentApiUrlComponents.port != 0)
				{
					m_logging->logError(
						"[Communication Channel] setUrls(): "
						"Agent gRPC socket url \"" + baseServerUrl + "\" must contain no port component.");
					return BaseUrlParseResultCode::SchemaNotValid;
				}

				auto ServerLocation = &ServicesMediator::ServerLocation<TransportFramework::gRPCTransport>;
				const std::vector<std::string> socketLocations = {
					ServerLocation(ServiceType::AccessControlOut),
					ServerLocation(ServiceType::ChatOut),
					ServerLocation(ServiceType::Connectivity),
					ServerLocation(ServiceType::Input),
					ServerLocation(ServiceType::SessionStatus),
					ServerLocation(ServiceType::InstantSupportNotification),
					ServerLocation(ServiceType::ConnectionConfirmationRequest)
				};

				const size_t basicSize = baseServerUrl.size() + UuidSize;
				for (const std::string& socketLocation: socketLocations)
				{
					if ((basicSize + socketLocation.size()) >= MaxSizeOfSocketPath)
					{
						// no temporary string allocated, no respective clang-tidy warning
						std::string testPath = components.scheme;
						testPath.append("://").append(components.path).append(socketLocation);

						m_logging->logError(
							"[Communication Channel] setUrls(): "
							"socket path \"" + testPath + "\" exceeds supported length " +
							std::to_string(MaxSizeOfSocketPath) + ".");
						return BaseUrlParseResultCode::CharacterLimitForPathExceeded;
					}
				}
			}
			break;
		case TransportFramework::TCPSocketTransport:
			{
				if (!components.path.empty())
				{
					m_logging->logError(
						"[Communication Channel] setUrls(): "
						"Base TCP socket url \"" + baseServerUrl + "\" must contain no path component.");
					return BaseUrlParseResultCode::SchemaNotValid;
				}

				if (components.port != 0)
				{
					m_logging->logError(
						"[Communication Channel] setUrls(): "
						"Base TCP socket url \"" + baseServerUrl + "\" must contain no port component.");
					return BaseUrlParseResultCode::SchemaNotValid;
				}

				if (!agentApiUrlComponents.path.empty())
				{
					m_logging->logError(
						"[Communication Channel] setUrls(): "
						"Agent TCP socket url \"" + baseServerUrl + "\" must contain no path component.");
					return BaseUrlParseResultCode::SchemaNotValid;
				}

				if (agentApiUrlComponents.port == 0)
				{
					m_logging->logError(
						"[Communication Channel] setUrls(): "
						"Agent TCP socket url \"" + baseServerUrl + "\" must contain port component.");
					return BaseUrlParseResultCode::SchemaNotValid;
				}
			}
			break;
		case TransportFramework::UnknownTransport:
			return BaseUrlParseResultCode::SchemaNotValid;
	}

	m_serverUrlComponents = std::move(components);
	m_transportFramework = framework;

	m_servicesMediator.reset(new ServicesMediator(m_serverUrlComponents, m_transportFramework, agentApiUrl));

	return BaseUrlParseResultCode::Success;
}

void CommunicationChannel::startup()
{
	std::lock_guard<std::mutex> shutdownLock(m_shutdownCondition->mutex);
	if (m_isRunning)
	{
		return;
	}

	if (m_serverUrlComponents.scheme == "unix" && !SetupDir(m_serverUrlComponents.path))
	{
		m_logging->logInfo("[Communication Channel] Directory setup failed.");
		return;
	}

	m_isRunning = true;

	bool requireScreenGrabResultWorker = false;
	{
		std::unique_lock<std::mutex> sendLock(m_grabResultCondition->mutex);
		requireScreenGrabResultWorker = !m_grabResultBuffer.pictureData.empty();
	}

	if (requireScreenGrabResultWorker)
	{
		startScreenGrabResultWorker();
	}

	m_communicationThread = std::thread([this]()
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
				shutdownLockLoop,
				std::chrono::seconds(ShutdownRetryTime));
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
	m_processGrabResult = false;

	{
		std::lock_guard<std::mutex> lock(m_grabResultCondition->mutex);
		m_grabResultCondition->condition.notify_all();
	}

	{
		std::lock_guard<std::mutex> lock(m_shutdownCondition->mutex);
		m_shutdownCondition->condition.notify_all();
	}

	if (m_grabResultThread.joinable())
	{
		m_grabResultThread.join();
	}

	if (m_communicationThread.joinable())
	{
		m_communicationThread.join();
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

	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::InstantSupport>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			safeClient->RequestInstantSupport(m_communicationId, accessToken, name, group, description, sessionCode, email);

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

bool CommunicationChannel::closeInstantSupportCase(
	const std::string& accessToken,
	const std::string& sessionCode)
{
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::InstantSupport>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			safeClient->CloseInstantSupportCase(m_communicationId, accessToken, sessionCode);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Close instant support request failed. Reason: " + status.errorMessage;
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

	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::ConnectionConfirmationResponse>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			safeClient->ConfirmConnectionRequest(m_communicationId, connectionType, confirmation);

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
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::SessionControl>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status = safeClient->CloseRc(m_communicationId);
		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[Communication Channel] Error in closing the remote control. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
		return true;
	}

	m_logging->logError("[CommunicationChannel] Session control service client not available.");
	return false;
}

bool CommunicationChannel::sendGetRunningTVSessions(std::vector<int32_t>& tvSessionIDs)
{
	using namespace TVRemoteScreenSDKCommunication::SessionControlService;

	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::SessionControl>())
	{
		ISessionControlServiceClient::TVSessionsResult result =
			safeClient->GetRunningTVSessions(m_communicationId);

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

	m_logging->logError("[CommunicationChannel] Session control service client not available.");
	return false;
}

void CommunicationChannel::sendControlMode(TVRemoteScreenSDKCommunication::SessionControlService::ControlMode mode)
{
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::SessionControl>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			safeClient->ChangeControlMode(m_communicationId, mode);

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

	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::AccessControlIn>())
	{
		const IAccessControlInServiceClient::GetAccessResponse responseGetAccess =
			safeClient->GetAccess(m_communicationId, feature);

		access = responseGetAccess.access;

		if (!responseGetAccess.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Getting access type failed. Reason: " + responseGetAccess.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
		return true;
	}

	m_logging->logError("[CommunicationChannel] Access control input service client not available.");
	return false;
}

bool CommunicationChannel::sendSetAccessMode(
	TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
	TVRemoteScreenSDKCommunication::AccessControlService::Access access)
{
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::AccessControlIn>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			safeClient->SetAccess(m_communicationId, feature, access);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Setting access type failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
		return true;
	}
	m_logging->logError("[CommunicationChannel] Access control input service client not available.");
	return false;
}

bool CommunicationChannel::sendAccessConfirmationReply(
	TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
	bool confirmed)
{
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::AccessControlIn>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			safeClient->ConfirmationReply(m_communicationId, feature, confirmed);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Sending confirmation reply failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
		return true;
	}
	m_logging->logError("[CommunicationChannel] Access control input service client not available.");
	return false;
}

void CommunicationChannel::sendScreenGrabResult(
	int32_t x,
	int32_t y,
	int32_t width,
	int32_t height,
	std::string pictureData)
{
	if (pictureData.empty())
	{
		return;
	}

	{
		std::lock_guard<std::mutex> lock(m_grabResultCondition->mutex);

		m_grabResultBuffer.x = x;
		m_grabResultBuffer.y = y;
		m_grabResultBuffer.width = width;
		m_grabResultBuffer.height = height;
		m_grabResultBuffer.pictureData.swap(pictureData);

		m_grabResultCondition->condition.notify_all();
	}
	startScreenGrabResultWorker();
}

void CommunicationChannel::startScreenGrabResultWorker()
{
	bool expected = false;
	const bool desired = true;
	if (!m_processGrabResult.compare_exchange_strong(expected, desired))
	{
		return;
	}

	m_grabResultThread = std::thread([this]()
	{
		constexpr std::chrono::seconds ShutdownRetryTime{2};

		while(m_processGrabResult)
		{
			GrabResult sendBuffer;
			{
				std::unique_lock<std::mutex> sendLock(m_grabResultCondition->mutex);
				m_grabResultCondition->condition.wait_for(
					sendLock,
					ShutdownRetryTime);

				std::swap(sendBuffer, m_grabResultBuffer);
			}

			if (!sendBuffer.pictureData.empty())
			{
				sendScreenGrabResultBuffer(sendBuffer);
			}
		}
	});
}
void CommunicationChannel::sendScreenGrabResultBuffer(CommunicationChannel::GrabResult& sendBuffer)
{
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::Image>())
	{
		TVRemoteScreenSDKCommunication::CallStatus callStatus = safeClient->UpdateImage(
			m_communicationId,
			sendBuffer.x,
			sendBuffer.y,
			sendBuffer.width,
			sendBuffer.height,
			sendBuffer.pictureData);

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
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::Image>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus response =
			safeClient->UpdateImageDefinition(
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
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::ImageNotification>())
	{
		TVRemoteScreenSDKCommunication::CallStatus response =
			safeClient->NotifiyImageChanged(
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
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::ImageNotification>())
	{
		TVRemoteScreenSDKCommunication::CallStatus response =
			safeClient->UpdateImageDefinition(
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
	auto safeClient = m_servicesMediator->AcquireClient<ServiceType::ViewGeometry>();

	if (!safeClient)
	{
		return ViewGeometrySendResult::NoService;
	}

	TVRemoteScreenSDKCommunication::ViewGeometryService::IViewGeometryServiceClient::UpdateResult result =
		safeClient->UpdateVirtualDesktop(m_communicationId, geometry);

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
	auto safeClient = m_servicesMediator->AcquireClient<ServiceType::ViewGeometry>();
	if (!safeClient)
	{
		return ViewGeometrySendResult::NoService;
	}

	TVRemoteScreenSDKCommunication::ViewGeometryService::IViewGeometryServiceClient::UpdateResult result =
		safeClient->UpdateAreaOfInterest(m_communicationId, rect);

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

	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::ChatIn>())
	{
		IChatInServiceClient::ObtainChatsResponse obtainChatsReturnValue =
			safeClient->ObtainChats(m_communicationId);

		if (!obtainChatsReturnValue.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Obtaining chats failed. Reason: " + obtainChatsReturnValue.errorMessage;
			m_logging->logError(errorMsg);
		}
		else
		{
			chats = std::move(obtainChatsReturnValue.chats);
			return true;
		}
	}
	m_logging->logError("[CommunicationChannel] ChatIn service client not available.");
	return false;
}

bool CommunicationChannel::sendSelectChatResult(std::string chatId)
{
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::ChatIn>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			safeClient->SelectChat(m_communicationId, chatId);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Selecting chat failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
		return true;
	}
	m_logging->logError("[CommunicationChannel] ChatIn service client not available.");
	return false;
}

bool CommunicationChannel::sendMessage(uint32_t localId, std::string content)
{
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::ChatIn>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			safeClient->SendMessage(m_communicationId, localId, content);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Sending message failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
		return true;
	}
	m_logging->logError("[CommunicationChannel] ChatIn service client not available.");
	return false;
}

bool CommunicationChannel::loadMessages(uint32_t count, std::string lastId)
{
	if (lastId.empty())
	{
		// agent doesn't accept empty string
		lastId = "00000000-0000-0000-0000-000000000000";
	}

	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::ChatIn>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			safeClient->LoadMessages(m_communicationId, count, lastId);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Load messages failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
		return true;
	}
	m_logging->logError("[CommunicationChannel] ChatIn service client not available.");
	return false;
}

bool CommunicationChannel::deleteHistory()
{
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::ChatIn>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status = safeClient->DeleteHistory(m_communicationId);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Deleting chat history failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
		return true;
	}
	m_logging->logError("[CommunicationChannel] ChatIn service client not available.");
	return false;
}

bool CommunicationChannel::deleteChat()
{
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::ChatIn>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status =
			safeClient->DeleteChat(m_communicationId);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] Deleting chat failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
		return true;
	}
	m_logging->logError("[CommunicationChannel] ChatIn service client not available.");
	return false;
}

bool CommunicationChannel::sendAugmentRCSessionStartListening()
{
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::AugmentRCSessionControl>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status = safeClient->StartListening(m_communicationId);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] sendAugmentRCSessionStartListening failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
		return true;
	}
	m_logging->logError("[CommunicationChannel] AugmentRCSessionControl service client not available.");
	return false;
}

bool CommunicationChannel::sendAugmentRCSessionStopListening()
{
	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::AugmentRCSessionControl>())
	{
		const TVRemoteScreenSDKCommunication::CallStatus status = safeClient->StopListening(m_communicationId);

		if (!status.IsOk())
		{
			const std::string errorMsg =
				"[CommunicationChannel] sendAugmentRCSessionStopListening failed. Reason: " + status.errorMessage;
			m_logging->logError(errorMsg);
			return false;
		}
		return true;
	}
	m_logging->logError("[CommunicationChannel] AugmentRCSessionControl service client not available.");
	return false;
}

uint64_t CommunicationChannel::getRunningServicesBitmask() const
{
	return m_servicesMediator->GetRunningServicesBitmask();
}

bool CommunicationChannel::establishConnection()
{
	using namespace TVRemoteScreenSDKCommunication::RegistrationService;

	auto safeClient = m_servicesMediator->SetupClient<ServiceType::Registration>();
	if (!safeClient)
	{
		return false;
	}

	const IRegistrationServiceClient::ExchangeVersionResponse exchangeResponse =
		safeClient->ExchangeVersion(VersionNumberToString(ClientVersion));

	if (exchangeResponse.IsOk() == false)
	{
		const std::string errorMsg =
			"[CommunicationChannel] Exchange of version failed. Reason: " + exchangeResponse.errorMessage;
		m_logging->logError(errorMsg);
		return false;
	}

	VersionNumber serverVersion; // their Agent version
	const bool versionConversionSuccess = VersionNumberFromString(exchangeResponse.versionNumber.c_str(), serverVersion);
	if (!versionConversionSuccess)
	{
		m_logging->logError("[CommunicationChannel] ExchangeVersionResponse bad version '" + exchangeResponse.versionNumber + "'");
	}

	VersionNumber minVersion = ClientVersion < serverVersion ? ClientVersion : serverVersion;
	m_logging->logInfo("[CommunicationChannel] minimum version '" + VersionNumberToString(minVersion) + "'");

	IRegistrationServiceClient::DiscoverResponse discoverResponse = safeClient->Discover(VersionNumberToString(minVersion));

	if (!discoverResponse.IsOk())
	{
		const std::string errorMsg =
			"[CommunicationChannel] Version Handshake failed. Reason: " + discoverResponse.errorMessage;
		m_logging->logError(errorMsg);
		return false;
	}

	m_communicationId = discoverResponse.communicationId;
	m_servicesMediator->SetServicesInformation(std::move(discoverResponse.services));
	m_servicesMediator->SetCommunicationId(m_communicationId);

	return true;
}

template <TVRemoteScreenSDKCommunication::ServiceType Type>
bool CommunicationChannel::setupClient()
{
	return !!m_servicesMediator->SetupClient<Type>().target;
}

bool CommunicationChannel::registerService(ServiceType serviceType)
{
	if (auto safeClient = m_servicesMediator->SetupClient<ServiceType::Registration>())
	{
		TVRemoteScreenSDKCommunication::CallStatus status = safeClient->Register(
			m_communicationId,
			serviceType,
			m_servicesMediator->FullServerLocation(serviceType));
		return status.IsOk();
	}

	return false;
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

	using ST = ServiceType;

	constexpr Setup optionalSetups[] =
	{
		{"Session control client"                      , &ThisType::setupClient<ST::SessionControl>                },
		{"Session status service"                      , &ThisType::setupSessionStatusService                      },
		{"Image client"                                , &ThisType::setupClient<ST::Image>                         },
		{"Image notification client"                   , &ThisType::setupClient<ST::ImageNotification>             },
		{"Instant support client"                      , &ThisType::setupClient<ST::InstantSupport>                },
		{"Access control-in client"                    , &ThisType::setupClient<ST::AccessControlIn>               },
		{"Input service"                               , &ThisType::setupInputService                              },
		{"Access control-out service"                  , &ThisType::setupAccessControlOutService                   },
		{"View geometry client"                        , &ThisType::setupClient<ST::ViewGeometry>                  },
		{"Instant support notification service"        , &ThisType::setupInstantSupportNotificationService         },
		{"Chat-in client"                              , &ThisType::setupClient<ST::ChatIn>                        },
		{"Chat-out service"                            , &ThisType::setupChatOutService                            },
		{"Instant support confirmation request service", &ThisType::setupConnectionConfirmationRequestService      },
		{"Instant support confirmation response client", &ThisType::setupClient<ST::ConnectionConfirmationResponse>},
		{"AugmentRCSession consumer service"           , &ThisType::setupAugmentRCSessionConsumerService           },
		{"AugmentRCSession control client"             , &ThisType::setupClient<ST::AugmentRCSessionControl>       },
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

	auto safeServer = m_servicesMediator->CreateAndStartServer<ServiceType::Connectivity>();

	if (!safeServer)
	{
		return false;
	}

	const auto communicationId = m_communicationId;
	auto availableCallback = [communicationId](
		const std::string& comId, const IConnectivityServiceServer::IsAvailableResponseCallback& response)
	{
		if (comId == communicationId)
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetIsAvailableCallback(availableCallback);

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;
	auto disconnectCallback = [weakThis](
		const std::string& comId, const IConnectivityServiceServer::IsAvailableResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);

			const std::lock_guard<std::mutex> disconnectLock(communicationChannel->m_disconnectCondition->mutex);
			communicationChannel->m_disconnectCondition->condition.notify_all();
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetDisconnectCallback(disconnectCallback);

	return true;
}

bool CommunicationChannel::setupInputService()
{
	auto safeServer = m_servicesMediator->CreateAndStartServer<ServiceType::Input>();

	if (!safeServer)
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
		const TVRemoteScreenSDKCommunication::InputService::IInputServiceServer::SimulateKeyResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId)
			&& keyState != TVRemoteScreenSDKCommunication::InputService::KeyState::Unknown)
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->simulateKeyInputRequested().notifyAll(keyState, xkbSymbol, unicodeCharacter, xkbModifiers);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetSimulateKeyCallback(receivedKeyCallback);

	auto mouseMoveCallback = [weakThis](
		const std::string& comId,
		int32_t posX,
		int32_t posY,
		const TVRemoteScreenSDKCommunication::InputService::IInputServiceServer::SimulateMouseMoveResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->simulateMouseMoveRequested().notifyAll(posX, posY);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetSimulateMouseMoveCallback(mouseMoveCallback);

	auto mousePressReleaseCallback = [weakThis](
		const std::string& comId,
		TVRemoteScreenSDKCommunication::InputService::MouseButtonState buttonState,
		int32_t posX,
		int32_t posY,
		TVRemoteScreenSDKCommunication::InputService::MouseButton button,
		const TVRemoteScreenSDKCommunication::InputService::IInputServiceServer::SimulateMousePressReleaseResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId)
			&& buttonState != TVRemoteScreenSDKCommunication::InputService::MouseButtonState::Unknown
			&& button != TVRemoteScreenSDKCommunication::InputService::MouseButton::Unknown)
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->simulateMousePressReleaseRequested().notifyAll(buttonState, posX, posY, button);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetSimulateMousePressReleaseCallback(mousePressReleaseCallback);

	auto mouseWheelCallback = [weakThis](
		const std::string& comId,
		int32_t posX,
		int32_t posY,
		int32_t angle,
		const TVRemoteScreenSDKCommunication::InputService::IInputServiceServer::SimulateMouseWheelResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->simulateMouseWheelRequested().notifyAll(posX, posY, angle);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetSimulateMouseWheelCallback(mouseWheelCallback);
	safeServer.lock.unlock();

	return registerService(ServiceType::Input);
}

bool CommunicationChannel::setupSessionStatusService()
{
	using namespace TVRemoteScreenSDKCommunication::SessionStatusService;

	auto safeServer = m_servicesMediator->CreateAndStartServer<ServiceType::SessionStatus>();

	if (!safeServer)
	{
		return false;
	}

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;
	auto remoteControlStarted = [weakThis](
		const std::string& comId,
		const GrabStrategy strategy,
		const ISessionStatusServiceServer::RemoteControlStartedResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->rcSessionStarted().notifyAll(strategy);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetRemoteControlStartedCallback(remoteControlStarted);

	auto remoteControlStopped = [weakThis](
		const std::string& comId,
		const ISessionStatusServiceServer::RemoteControlStartedResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->rcSessionStopped().notifyAll();
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetRemoteControlStoppedCallback(remoteControlStopped);

	auto tvSessionStarted = [weakThis](
		const std::string& comId,
		const int32_t tvSessionID,
		const int32_t tvSessionsCount,
		const ISessionStatusServiceServer::RemoteControlStartedResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->tvSessionStarted().notifyAll(tvSessionID, tvSessionsCount);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetTVSessionStartedCallback(tvSessionStarted);

	auto tvSessionStopped = [weakThis](
		const std::string& comId,
		const int32_t tvSessionID,
		const int32_t tvSessionsCount,
		const ISessionStatusServiceServer::RemoteControlStartedResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->tvSessionStopped().notifyAll(tvSessionID, tvSessionsCount);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetTVSessionStoppedCallback(tvSessionStopped);
	safeServer.lock.unlock();

	return registerService(ServiceType::SessionStatus);
}

bool CommunicationChannel::setupAccessControlOutService()
{
	using namespace TVRemoteScreenSDKCommunication::AccessControlService;

	auto safeServer = m_servicesMediator->CreateAndStartServer<ServiceType::AccessControlOut>();

	if (!safeServer)
	{
		return false;
	}

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;
	auto confirmationRequest = [weakThis](
		const std::string& comId,
		TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
		uint32_t timeout,
		const IAccessControlOutServiceServer::AskForConfirmationResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->accessConfirmationRequested().notifyAll(feature, timeout);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetAskForConfirmationCallback(confirmationRequest);

	auto notifyAccessModeChange = [weakThis](
		const std::string& comId,
		TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
		TVRemoteScreenSDKCommunication::AccessControlService::Access accessMode,
		const IAccessControlOutServiceServer::NotifyChangeResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->accessModeChangeNotified().notifyAll(feature, accessMode);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetNotifyChangeCallback(notifyAccessModeChange);
	safeServer.lock.unlock();

	return registerService(ServiceType::AccessControlOut);
}

bool CommunicationChannel::setupAugmentRCSessionConsumerService()
{
	using namespace TVRemoteScreenSDKCommunication::AugmentRCSessionService;

	auto safeServer = m_servicesMediator->CreateAndStartServer<ServiceType::AugmentRCSessionConsumer>();

	if (!safeServer)
	{
		return false;
	}

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;
	auto invitationReceived = [weakThis](
		const std::string& comId,
		const std::string url,
		const IAugmentRCSessionConsumerServiceServer::ReceivedInvitationResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->augmentRCSessionInvitationReceived().notifyAll(url);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetReceivedInvitationCallback(invitationReceived);
	safeServer.lock.unlock();

	return registerService(ServiceType::AugmentRCSessionConsumer);
}

bool CommunicationChannel::setupInstantSupportNotificationService()
{
	using namespace TVRemoteScreenSDKCommunication::InstantSupportService;

	auto safeServer = m_servicesMediator->CreateAndStartServer<ServiceType::InstantSupportNotification>();

	if (!safeServer)
	{
		return false;
	}

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;
	auto errorHandling = [weakThis](
		const std::string& comId,
		TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError responseError,
		const IInstantSupportNotificationServiceServer::NotifyInstantSupportErrorResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
/*
			InstantSupportError sdkResponseError = getQtSdkInstantSupportError(responseError);
*/
			(void)responseError;
			communicationChannel->instantSupportErrorNotification().notifyAll(/*sdkResponseError*/responseError);
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};

	safeServer->SetNotifyInstantSupportErrorCallback(errorHandling);

	auto modifiedHandling = [weakThis](
		const std::string& comId,
		InstantSupportData data,
		const IInstantSupportNotificationServiceServer::NotifyInstantSupportModifiedResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
/*
			InstantSupportData sdkResponseData = getQtSdkInstantSupportData(data);
*/
			communicationChannel->instantSupportModifiedNotification().notifyAll(data);
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};

	safeServer->SetNotifyInstantSupportModifiedCallback(modifiedHandling);
	safeServer.lock.unlock();

	return registerService(ServiceType::InstantSupportNotification);
}

bool CommunicationChannel::setupConnectionConfirmationRequestService()
{
	using namespace TVRemoteScreenSDKCommunication::ConnectionConfirmationService;

	auto safeServer = m_servicesMediator->CreateAndStartServer<ServiceType::ConnectionConfirmationRequest>();

	if (!safeServer)
	{
		return false;
	}

	std::weak_ptr<CommunicationChannel> weakThis = m_weakThis;
	const auto connectionConfirmationRequestCallback = [weakThis](
		const std::string& comId,
		TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType connectionType,
		const IConnectionConfirmationRequestServiceServer::RequestConnectionConfirmationResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			switch (connectionType) {
				case TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType::InstantSupport:
					communicationChannel->instantSupportConnectionConfirmationRequested().notifyAll();
					response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
					break;
				case TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType::Undefined:
				default:
					response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
					break;
			}
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};

	safeServer->SetRequestConnectionConfirmationCallback(connectionConfirmationRequestCallback);
	safeServer.lock.unlock();

	return registerService(ServiceType::ConnectionConfirmationRequest);
}

bool CommunicationChannel::setupChatOutService()
{
	using namespace TVRemoteScreenSDKCommunication::ChatService;

	auto safeServer = m_servicesMediator->CreateAndStartServer<ServiceType::ChatOut>();

	if (!safeServer)
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
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->chatCreated().notifyAll(chatId, title, sdkchatType, chatTypeId);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetChatCreatedCallback(chatCreatedRequest);

	auto chatsRemovedRequest = [weakThis](
		const std::string& comId,
		std::vector<std::string> chatIds,
		const IChatOutServiceServer::ChatsRemovedResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->chatsRemoved().notifyAll(chatIds);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetChatsRemovedCallback(chatsRemovedRequest);

	auto receivedMessagesRequest = [weakThis](
		const std::string& comId,
		std::vector<TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage> messages,
		const IChatOutServiceServer::ReceivedMessagesResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->receivedMessages().notifyAll(messages);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetReceivedMessagesCallback(receivedMessagesRequest);

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
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->messageSent().notifyAll(localId, messageId, timeStamp);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetMessageSentCallback(messageSentRequest);

	auto messageNotSentRequest = [weakThis](
		const std::string& comId,
		uint32_t localId,
		const IChatOutServiceServer::MessageNotSentResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->messageNotSent().notifyAll(localId);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetMessageNotSentCallback(messageNotSentRequest);

	auto loadedMessagesRequest = [weakThis](
		const std::string& comId,
		std::vector<TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage> messages,
		bool hasMore,
		const IChatOutServiceServer::ReceivedMessagesResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->loadedMessages().notifyAll(messages, hasMore);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetLoadedMessagesCallback(loadedMessagesRequest);

	auto deletedHistoryRequest = [weakThis](
		const std::string& comId,
		std::string chatId,
		const IChatOutServiceServer::DeletedHistoryResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->deletedHistory().notifyAll(chatId);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetDeletedHistoryCallback(deletedHistoryRequest);

	auto closedChatRequest = [weakThis](
		const std::string& comId,
		std::string chatId,
		const IChatOutServiceServer::ClosedChatResponseCallback& response)
	{
		const std::shared_ptr<CommunicationChannel> communicationChannel = weakThis.lock();
		if (communicationChannel && (communicationChannel->m_communicationId == comId))
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Ok);
			communicationChannel->closedChat().notifyAll(chatId);
		}
		else
		{
			response(TVRemoteScreenSDKCommunication::CallStatus::Failed);
		}
	};
	safeServer->SetClosedChatCallback(closedChatRequest);
	safeServer.lock.unlock();

	return registerService(ServiceType::ChatOut);
}

bool CommunicationChannel::setupConnectivityClient()
{
	if (!m_servicesMediator->SetupClient<ServiceType::Connectivity>())
	{
		return false;
	}

	return registerService(ServiceType::Connectivity);
}

void CommunicationChannel::sendDisconnect() const
{

	if (auto safeClient = m_servicesMediator->AcquireClient<ServiceType::Connectivity>())
	{
		safeClient->Disconnect(m_communicationId);
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
			auto safeClient = m_servicesMediator->AcquireClient<ServiceType::Connectivity>();
			if (!safeClient || !safeClient->IsAvailable(m_communicationId).IsOk())
			{
				return;
			}
		}
	}
}

void CommunicationChannel::tearDown()
{
	m_servicesMediator->TeardownServer<ServiceType::Connectivity>();
	m_servicesMediator->TeardownServer<ServiceType::Input>();
	m_servicesMediator->TeardownServer<ServiceType::SessionStatus>();
	m_servicesMediator->TeardownServer<ServiceType::AccessControlOut>();
	m_servicesMediator->TeardownServer<ServiceType::InstantSupportNotification>();
	m_servicesMediator->TeardownServer<ServiceType::ChatOut>();
	m_servicesMediator->TeardownServer<ServiceType::ConnectionConfirmationRequest>();

	m_servicesMediator->TeardownClient<ServiceType::Connectivity>();
	m_servicesMediator->TeardownClient<ServiceType::ImageNotification>();
	m_servicesMediator->TeardownClient<ServiceType::Image>();
	m_servicesMediator->TeardownClient<ServiceType::Registration>();
	m_servicesMediator->TeardownClient<ServiceType::SessionControl>();
	m_servicesMediator->TeardownClient<ServiceType::InstantSupport>();
	m_servicesMediator->TeardownClient<ServiceType::AccessControlIn>();
	m_servicesMediator->TeardownClient<ServiceType::ViewGeometry>();
	m_servicesMediator->TeardownClient<ServiceType::ChatIn>();
	m_servicesMediator->TeardownClient<ServiceType::ConnectionConfirmationResponse>();
}

} // namespace tvagentapi
