//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2024 TeamViewer Germany GmbH                                     //
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

#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlInServiceClient.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlInServiceServer.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/InServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlOutServiceClient.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlOutServiceServer.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/OutServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ChatService/IChatInServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ChatService/IChatInServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ChatService/InServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ChatService/IChatOutServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ChatService/IChatOutServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ChatService/OutServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/ParseUrl.h>

#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationRequestServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationRequestServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationResponseServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationResponseServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/RequestServiceFactory.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/ResponseServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ConnectivityService/IConnectivityServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ConnectivityService/IConnectivityServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ConnectivityService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ImageService/IImageServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ImageService/IImageServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ImageService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ImageNotificationService/IImageNotificationServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ImageNotificationService/IImageNotificationServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ImageNotificationService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/InputService/IInputServiceClient.h>
#include <TVRemoteScreenSDKCommunication/InputService/IInputServiceServer.h>
#include <TVRemoteScreenSDKCommunication/InputService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportNotificationServiceClient.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportNotificationServiceServer.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportServiceClient.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportServiceServer.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/NotificationServiceFactory.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/RegistrationService/IRegistrationServiceClient.h>
#include <TVRemoteScreenSDKCommunication/RegistrationService/IRegistrationServiceServer.h>
#include <TVRemoteScreenSDKCommunication/RegistrationService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/SessionControlService/ISessionControlServiceClient.h>
#include <TVRemoteScreenSDKCommunication/SessionControlService/ISessionControlServiceServer.h>
#include <TVRemoteScreenSDKCommunication/SessionControlService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/SessionStatusService/ISessionStatusServiceClient.h>
#include <TVRemoteScreenSDKCommunication/SessionStatusService/ISessionStatusServiceServer.h>
#include <TVRemoteScreenSDKCommunication/SessionStatusService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ViewGeometryService/IViewGeometryServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ViewGeometryService/IViewGeometryServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ViewGeometryService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/AugmentRCSessionService/IAugmentRCSessionControlServiceClient.h>
#include <TVRemoteScreenSDKCommunication/AugmentRCSessionService/IAugmentRCSessionControlServiceServer.h>
#include <TVRemoteScreenSDKCommunication/AugmentRCSessionService/ControlServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/AugmentRCSessionService/IAugmentRCSessionConsumerServiceClient.h>
#include <TVRemoteScreenSDKCommunication/AugmentRCSessionService/IAugmentRCSessionConsumerServiceServer.h>
#include <TVRemoteScreenSDKCommunication/AugmentRCSessionService/ConsumerServiceFactory.h>

#include <algorithm>
#include <mutex>
#include <tuple>
#include <vector>

namespace tvagentapi
{

namespace Details
{

template <TVRemoteScreenSDKCommunication::ServiceType Type>
struct FactoryMeta;

template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::AccessControlIn>
{
	using Factory = TVRemoteScreenSDKCommunication::AccessControlService::InServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::AccessControlOut>
{
	using Factory = TVRemoteScreenSDKCommunication::AccessControlService::OutServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::AugmentRCSessionConsumer>
{
	using Factory = TVRemoteScreenSDKCommunication::AugmentRCSessionService::ConsumerServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::AugmentRCSessionControl>
{
	using Factory = TVRemoteScreenSDKCommunication::AugmentRCSessionService::ControlServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::ChatIn>
{
	using Factory = TVRemoteScreenSDKCommunication::ChatService::InServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::ChatOut>
{
	using Factory = TVRemoteScreenSDKCommunication::ChatService::OutServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::ConnectionConfirmationRequest>
{
	using Factory = TVRemoteScreenSDKCommunication::ConnectionConfirmationService::RequestServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::ConnectionConfirmationResponse>
{
	using Factory = TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ResponseServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::Connectivity>
{
	using Factory = TVRemoteScreenSDKCommunication::ConnectivityService::ServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::Image>
{
	using Factory = TVRemoteScreenSDKCommunication::ImageService::ServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::ImageNotification>
{
	using Factory = TVRemoteScreenSDKCommunication::ImageNotificationService::ServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::Input>
{
	using Factory = TVRemoteScreenSDKCommunication::InputService::ServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::InstantSupport>
{
	using Factory = TVRemoteScreenSDKCommunication::InstantSupportService::ServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::InstantSupportNotification>
{
	using Factory = TVRemoteScreenSDKCommunication::InstantSupportService::NotificationServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::Registration>
{
	using Factory = TVRemoteScreenSDKCommunication::RegistrationService::ServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::SessionControl>
{
	using Factory = TVRemoteScreenSDKCommunication::SessionControlService::ServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::SessionStatus>
{
	using Factory = TVRemoteScreenSDKCommunication::SessionStatusService::ServiceFactory;
};
template<> struct FactoryMeta<TVRemoteScreenSDKCommunication::ServiceType::ViewGeometry>
{
	using Factory = TVRemoteScreenSDKCommunication::ViewGeometryService::ServiceFactory;
};

template <TVRemoteScreenSDKCommunication::ServiceType Type>
using ServiceFactory = typename FactoryMeta<Type>::Factory;

using TF = TVRemoteScreenSDKCommunication::TransportFramework;
using ST = TVRemoteScreenSDKCommunication::ServiceType;

static_assert(ST::Unknown < ST::LastServiceType, "Update LastServiceType");

template <ST Type, TF Framework = TF::gRPCTransport>
using ServerInterface =
	typename std::result_of<decltype(Details::ServiceFactory<Type>::template CreateServer<Framework>)&()>::type;

template <ST Type, TF Framework = TF::gRPCTransport>
using ClientInterface =
	typename std::result_of<decltype(Details::ServiceFactory<Type>::template CreateClient<Framework>)&()>::type;

template <ST Type, bool IsServer>
struct GenericStorage :
	GenericStorage<static_cast<ST>(static_cast<typename std::underlying_type<ST>::type>(Type) - 1), IsServer>
{
	static_assert(Type <= ST::LastServiceType, "Update LastServiceType");
	using StoredType = typename std::conditional<IsServer, ServerInterface<Type>, ClientInterface<Type>>::type;

	mutable std::mutex mutex;
	StoredType target;
};

template <bool IsServer>
struct GenericStorage<ST::Unknown, IsServer>
{
};

template <typename ActorUniquePtr>
struct SafeActor
{
	std::unique_lock<std::mutex> lock;
	ActorUniquePtr& target;
	explicit operator bool() const
	{
		return !!target;
	}

	typename std::add_const<typename ActorUniquePtr::element_type>::type* operator->() const
	{
		return target.get();
	}

	typename ActorUniquePtr::pointer operator->()
	{
		return target.get();
	}
};

} // namespace Details

class ServicesMediator final
{
public:
	using ServicesInformation = std::vector<TVRemoteScreenSDKCommunication::RegistrationService::ServiceInformation>;

	template <Details::ST Type>
	using SafeClient = Details::SafeActor<Details::ClientInterface<Type>>;

	template <Details::ST Type>
	using SafeServer = Details::SafeActor<Details::ServerInterface<Type>>;

	ServicesMediator(
		TVRemoteScreenSDKCommunication::UrlComponents serverUrlComponents,
		Details::TF framework,
		std::string registrationServiceUrl);

	template <Details::TF Framework>
	static const char* ServerLocation(Details::ST serviceType);

	std::string FullServerLocation(Details::ST serviceType) const;

	template <Details::ST Type>
	SafeServer<Type> CreateAndStartServer()
	{
		static_assert(
			std::is_same<
				Details::ServerInterface<Type, Details::TF::gRPCTransport>,
				Details::ServerInterface<Type, Details::TF::TCPSocketTransport>>::value,
			"Server interfaces are not the same");
		Details::ServerInterface<Type> server{};

		switch (m_framework)
		{
			case Details::TF::gRPCTransport:
#if TV_COMM_ENABLE_GRPC
				server = Details::ServiceFactory<Type>::template CreateServer<Details::TF::gRPCTransport>();
#endif
				break;
			case Details::TF::TCPSocketTransport:
#ifdef TV_COMM_ENABLE_PLAIN_SOCKET
				server = Details::ServiceFactory<Type>::template CreateServer<Details::TF::TCPSocketTransport>();
#endif
				break;
			case Details::TF::UnknownTransport:
				break;
		}

		const std::string fullServerLocation = FullServerLocation(Type);

		auto safeServer = AcquireServer<Type>();
		safeServer.target = std::move(server);

		if (safeServer && !safeServer->StartServer(fullServerLocation))
		{
			safeServer.target.reset();
		}

		return safeServer;
	}

	std::string AgentServiceLocation(Details::ST serviceType) const
	{
		if (serviceType == Details::ST::Registration)
		{
			return m_registrationServiceUrl;
		}

		const auto foundInfo = std::find_if(
			m_servicesInfo.cbegin(),
			m_servicesInfo.cend(),
			[serviceType](const TVRemoteScreenSDKCommunication::RegistrationService::ServiceInformation& serviceInfo)
			{
				return serviceInfo.type == serviceType;
			});
		if (foundInfo == m_servicesInfo.cend())
		{
			return {};
		}
		return foundInfo->location;
	}

	template <Details::ST Type>
	SafeClient<Type> SetupClient()
	{
		static_assert(
			std::is_same<
				Details::ClientInterface<Type, Details::TF::gRPCTransport>,
				Details::ClientInterface<Type, Details::TF::TCPSocketTransport>>::value,
			"Client interfaces are not the same");

		Details::ClientInterface<Type> client{};

		switch (m_framework)
		{
			case Details::TF::gRPCTransport:
#if TV_COMM_ENABLE_GRPC
				client = Details::ServiceFactory<Type>::template CreateClient<Details::TF::gRPCTransport>();
#endif
				break;
			case Details::TF::TCPSocketTransport:
#if TV_COMM_ENABLE_PLAIN_SOCKET
				client = Details::ServiceFactory<Type>::template CreateClient<Details::TF::TCPSocketTransport>();
#endif
				break;
			case Details::TF::UnknownTransport:
				break;
		}

		const std::string location = AgentServiceLocation(Type);

		auto safeClient = AcquireClient<Type>();
		safeClient.target = std::move(client);

		if (!location.empty() && safeClient)
		{
			safeClient->StartClient(location);
		}
		else
		{
			safeClient.target.reset();
		}

		return safeClient;
	}

	template<Details::ST Type>
	SafeClient<Type> AcquireClient()
	{
		auto& row = static_cast<Details::GenericStorage<Type, /*IsServer=*/false>&>(m_clients);
		return {std::unique_lock<std::mutex>{row.mutex}, row.target};
	}

	template<Details::ST Type>
	SafeServer<Type> AcquireServer()
	{
		auto& row = static_cast<Details::GenericStorage<Type, /*IsServer=*/true>&>(m_servers);
		return {std::unique_lock<std::mutex>{row.mutex}, row.target};
	}

	void SetServicesInformation(ServicesInformation servicesInfo)
	{
		m_servicesInfo = std::move(servicesInfo);
	}

	void SetCommunicationId(const std::string& comId)
	{
		m_comId = comId;
	}

	template<Details::ST Type>
	void TeardownClient()
	{
		if (auto safeClient = AcquireClient<Type>())
		{
			safeClient->StopClient();
			safeClient.target.reset();
		}
	}

	template<Details::ST Type>
	void TeardownServer()
	{
		if (auto safeServer = AcquireServer<Type>())
		{
			safeServer->StopServer();
			safeServer.target.reset();
		}
	}

	uint64_t GetRunningServicesBitmask();

private:
	template <Details::ST Type>
	typename std::enable_if<Type != Details::ST::Unknown, uint64_t>::type GetRunningServiceFlagsRec()
	{
		constexpr auto serviceTypeValue =
			static_cast<typename std::underlying_type<Details::ST>::type>(Type);
		constexpr auto serviceTypeLastValue =
			static_cast<typename std::underlying_type<Details::ST>::type>(Details::ST::LastServiceType);

		static_assert(serviceTypeValue < 64, "Index exceeds bitmask size");
		static_assert(serviceTypeValue <= serviceTypeLastValue, "Index exceeds last value");

		const bool serviceActive = (!!AcquireClient<Type>().target || !!AcquireServer<Type>().target);
		return (serviceActive ? (1ULL << serviceTypeValue) : 0)
			| GetRunningServiceFlagsRec<static_cast<Details::ST>(serviceTypeValue - 1)>();
	}

	template <Details::ST Type>
	typename std::enable_if<Type == Details::ST::Unknown, uint64_t>::type GetRunningServiceFlagsRec()
	{
		return 0;
	}

private:
	const TVRemoteScreenSDKCommunication::UrlComponents m_serverUrlComponents;
	const Details::TF m_framework;
	const std::string m_registrationServiceUrl;

	ServicesInformation m_servicesInfo;
	std::string m_comId;

	Details::GenericStorage<Details::ST::LastServiceType, /*IsServer=*/false> m_clients;
	Details::GenericStorage<Details::ST::LastServiceType, /*IsServer=*/true> m_servers;
};

} // namespace tvagentapi
