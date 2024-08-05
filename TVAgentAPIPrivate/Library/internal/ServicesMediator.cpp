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
#include "ServicesMediator.h"

using TVRemoteScreenSDKCommunication::ServiceType;
using TVRemoteScreenSDKCommunication::TransportFramework;
using TVRemoteScreenSDKCommunication::UrlComponents;

namespace tvagentapi
{

ServicesMediator::ServicesMediator(
	TVRemoteScreenSDKCommunication::UrlComponents serverUrlComponents,
	TVRemoteScreenSDKCommunication::TransportFramework framework,
	std::string registrationServiceUrl)
	: m_serverUrlComponents{std::move(serverUrlComponents)}
	, m_framework{framework}
	, m_registrationServiceUrl{std::move(registrationServiceUrl)}
{
}

template<>
const char* ServicesMediator::ServerLocation<TransportFramework::gRPCTransport>(
	TVRemoteScreenSDKCommunication::ServiceType serviceType)
{
	switch (serviceType)
	{
		case ServiceType::AccessControlOut:
			return "TVQtRC/aCOut/";
		case ServiceType::ChatOut:
			return "TVQtRC/chatOut/";
		case ServiceType::Connectivity:
			return "TVQtRC/conn/";
		case ServiceType::Input:
			return "TVQtRC/input/";
		case ServiceType::SessionStatus:
			return "TVQtRC/sesStat/";
		case ServiceType::InstantSupportNotification:
			return "TVQtRC/insSupE/";
		case ServiceType::ConnectionConfirmationRequest:
			return "TVQtRC/ConConRe/";
		case ServiceType::AugmentRCSessionConsumer:
			return "TVQtRC/augSessCons/";
		default:
			return "";
	}
}

template<>
const char* ServicesMediator::ServerLocation<TransportFramework::TCPSocketTransport>(
	TVRemoteScreenSDKCommunication::ServiceType serviceType)
{
	switch (serviceType)
	{
		case ServiceType::AccessControlOut:
			return "9237";
		case ServiceType::ChatOut:
			return "9238";
		case ServiceType::Connectivity:
			return "9322";
		case ServiceType::Input:
			return "9323";
		case ServiceType::SessionStatus:
			return "9324";
		case ServiceType::InstantSupportNotification:
			return "9325";
		case ServiceType::ConnectionConfirmationRequest:
			return "9326";
		case ServiceType::AugmentRCSessionConsumer:
			return "9327";
		default:
			return "";
	}
}

std::string ServicesMediator::FullServerLocation(ServiceType serviceType) const
{
	std::string fullServerLocation = m_serverUrlComponents.scheme;
	fullServerLocation.append("://");

	switch (m_framework)
	{
		case TransportFramework::gRPCTransport:
			fullServerLocation.append(m_serverUrlComponents.path).append("/")
				.append(ServerLocation<TransportFramework::gRPCTransport>(serviceType)).append(m_comId);
			break;
		case TransportFramework::TCPSocketTransport:
			fullServerLocation.append(m_serverUrlComponents.path).append(m_serverUrlComponents.host)
				.append(":").append(ServerLocation<TransportFramework::TCPSocketTransport>(serviceType));
			break;
		case TransportFramework::UnknownTransport:
			fullServerLocation.clear();
			break;
	}
	return fullServerLocation;
}

uint64_t ServicesMediator::GetRunningServicesBitmask()
{
	return GetRunningServiceFlagsRec<Details::ST::LastServiceType>();
}

} // namespace tvagentapi
