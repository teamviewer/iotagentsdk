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
// ==================================
// Generated by TVCMGen. DO NOT EDIT!
// ==================================

#include "RegistrationServiceSocketIOClient.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>

#include "internal/RegistrationServiceFunctions.h"

#include "DiscoverRequest.pb.h"
#include "DiscoverResponse.pb.h"
#include "ExchangeVersionRequest.pb.h"
#include "ExchangeVersionResponse.pb.h"
#include "RegisterRequest.pb.h"
#include "RegisterResponse.pb.h"
#include "ServiceType.pb.h"

namespace TVRemoteScreenSDKCommunication
{

namespace RegistrationService
{

void RegistrationServiceSocketIOClient::StartClient(const std::string& destination)
{
	m_destination = destination;

	m_channel.reset(new Transport::SocketIO::ChannelInterface(m_destination));
}

void RegistrationServiceSocketIOClient::StopClient(bool /*force*/)
{
	m_channel.reset();
}

ServiceType RegistrationServiceSocketIOClient::GetServiceType() const
{
	return ServiceType::Registration;
}

const std::string& RegistrationServiceSocketIOClient::GetDestination() const
{
	return m_destination;
}

// rpc call ExchangeVersion
auto RegistrationServiceSocketIOClient::ExchangeVersion(const std::string& ownVersion) -> ExchangeVersionResponse
{
	ExchangeVersionResponse returnValue{};

	if (m_channel == nullptr)
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_MissingStartClient;
		return returnValue;
	}

	// No input validation

	std::string comId{};

	::tvregistrationservice::ExchangeVersionRequest request{};

	request.set_version(ownVersion);

	::tvregistrationservice::ExchangeVersionResponse response{};

	Transport::SocketIO::Status status = m_channel->Call(comId, Function_ExchangeVersion, request, response);

	if (status.ok())
	{
		returnValue.state = CallState::Ok;
		returnValue.versionNumber = response.version();
	}
	else
	{
		returnValue.errorMessage = status.error_message();
	}
	return returnValue;
}

// rpc call Discover
auto RegistrationServiceSocketIOClient::Discover(const std::string& comId) -> DiscoverResponse
{
	DiscoverResponse returnValue{};

	if (m_channel == nullptr)
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_MissingStartClient;
		return returnValue;
	}

	if (comId.empty())
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_InvalidInputParameter;
		return returnValue;
	}

	::tvregistrationservice::DiscoverRequest request{};

	request.set_communicationversion(comId);

	::tvregistrationservice::DiscoverResponse response{};

	Transport::SocketIO::Status status = m_channel->Call(comId, Function_Discover, request, response);

	if (status.ok())
	{
		returnValue.state = CallState::Ok;
		returnValue.communicationId = response.communicationid();

		std::vector<ServiceInformation> services;

		for (int serviceIndex = 0; serviceIndex < response.serviceinfo_size(); ++serviceIndex)
		{
			ServiceInformation serviceInformation;

			const ::tvregistrationservice::ServiceInformation& serviceInfo = response.serviceinfo(serviceIndex);
			serviceInformation.location = serviceInfo.location();

			const auto serviceType = serviceInfo.type();

			ServiceType serviceTypeEnumValue = ServiceType::Unknown;

			switch (serviceType)
			{
				case ::tvregistrationservice::ServiceType::Unknown:
					serviceTypeEnumValue = ServiceType::Unknown;
					break;
				case ::tvregistrationservice::ServiceType::Registration:
					serviceTypeEnumValue = ServiceType::Registration;
					break;
				case ::tvregistrationservice::ServiceType::Connectivity:
					serviceTypeEnumValue = ServiceType::Connectivity;
					break;
				case ::tvregistrationservice::ServiceType::Image:
					serviceTypeEnumValue = ServiceType::Image;
					break;
				case ::tvregistrationservice::ServiceType::Input:
					serviceTypeEnumValue = ServiceType::Input;
					break;
				case ::tvregistrationservice::ServiceType::SessionControl:
					serviceTypeEnumValue = ServiceType::SessionControl;
					break;
				case ::tvregistrationservice::ServiceType::SessionStatus:
					serviceTypeEnumValue = ServiceType::SessionStatus;
					break;
				case ::tvregistrationservice::ServiceType::ImageNotification:
					serviceTypeEnumValue = ServiceType::ImageNotification;
					break;
				case ::tvregistrationservice::ServiceType::AccessControlIn:
					serviceTypeEnumValue = ServiceType::AccessControlIn;
					break;
				case ::tvregistrationservice::ServiceType::AccessControlOut:
					serviceTypeEnumValue = ServiceType::AccessControlOut;
					break;
				case ::tvregistrationservice::ServiceType::InstantSupport:
					serviceTypeEnumValue = ServiceType::InstantSupport;
					break;
				case ::tvregistrationservice::ServiceType::ViewGeometry:
					serviceTypeEnumValue = ServiceType::ViewGeometry;
					break;
				case ::tvregistrationservice::ServiceType::InstantSupportNotification:
					serviceTypeEnumValue = ServiceType::InstantSupportNotification;
					break;
				case ::tvregistrationservice::ServiceType::ChatIn:
					serviceTypeEnumValue = ServiceType::ChatIn;
					break;
				case ::tvregistrationservice::ServiceType::ChatOut:
					serviceTypeEnumValue = ServiceType::ChatOut;
					break;
				case ::tvregistrationservice::ServiceType::ConnectionConfirmationRequest:
					serviceTypeEnumValue = ServiceType::ConnectionConfirmationRequest;
					break;
				case ::tvregistrationservice::ServiceType::ConnectionConfirmationResponse:
					serviceTypeEnumValue = ServiceType::ConnectionConfirmationResponse;
					break;
				case ::tvregistrationservice::ServiceType::AugmentRCSessionControl:
					serviceTypeEnumValue = ServiceType::AugmentRCSessionControl;
					break;
				case ::tvregistrationservice::ServiceType::AugmentRCSessionConsumer:
					serviceTypeEnumValue = ServiceType::AugmentRCSessionConsumer;
					break;
				default:

					break;
			}

			serviceInformation.type = serviceTypeEnumValue;

			services.push_back(std::move(serviceInformation));
		}

		returnValue.services.swap(services);
	}
	else
	{
		returnValue.errorMessage = status.error_message();
	}
	return returnValue;
}

// rpc call Register
auto RegistrationServiceSocketIOClient::Register(const std::string& comId, ServiceType type, const std::string& location) -> CallStatus
{
	CallStatus returnValue{};

	if (m_channel == nullptr)
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_MissingStartClient;
		return returnValue;
	}

	if (location.empty())
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_InvalidInputParameter;
		return returnValue;
	}

	::tvregistrationservice::RegisterRequest request{};

	::tvregistrationservice::ServiceType typeProtoValue = ::tvregistrationservice::ServiceType::Unknown;

	switch (type)
	{
		case ServiceType::Unknown:
			typeProtoValue = ::tvregistrationservice::ServiceType::Unknown;
			break;
		case ServiceType::Registration:
			typeProtoValue = ::tvregistrationservice::ServiceType::Registration;
			break;
		case ServiceType::Connectivity:
			typeProtoValue = ::tvregistrationservice::ServiceType::Connectivity;
			break;
		case ServiceType::Image:
			typeProtoValue = ::tvregistrationservice::ServiceType::Image;
			break;
		case ServiceType::Input:
			typeProtoValue = ::tvregistrationservice::ServiceType::Input;
			break;
		case ServiceType::SessionControl:
			typeProtoValue = ::tvregistrationservice::ServiceType::SessionControl;
			break;
		case ServiceType::SessionStatus:
			typeProtoValue = ::tvregistrationservice::ServiceType::SessionStatus;
			break;
		case ServiceType::ImageNotification:
			typeProtoValue = ::tvregistrationservice::ServiceType::ImageNotification;
			break;
		case ServiceType::AccessControlIn:
			typeProtoValue = ::tvregistrationservice::ServiceType::AccessControlIn;
			break;
		case ServiceType::AccessControlOut:
			typeProtoValue = ::tvregistrationservice::ServiceType::AccessControlOut;
			break;
		case ServiceType::InstantSupport:
			typeProtoValue = ::tvregistrationservice::ServiceType::InstantSupport;
			break;
		case ServiceType::ViewGeometry:
			typeProtoValue = ::tvregistrationservice::ServiceType::ViewGeometry;
			break;
		case ServiceType::InstantSupportNotification:
			typeProtoValue = ::tvregistrationservice::ServiceType::InstantSupportNotification;
			break;
		case ServiceType::ChatIn:
			typeProtoValue = ::tvregistrationservice::ServiceType::ChatIn;
			break;
		case ServiceType::ChatOut:
			typeProtoValue = ::tvregistrationservice::ServiceType::ChatOut;
			break;
		case ServiceType::ConnectionConfirmationRequest:
			typeProtoValue = ::tvregistrationservice::ServiceType::ConnectionConfirmationRequest;
			break;
		case ServiceType::ConnectionConfirmationResponse:
			typeProtoValue = ::tvregistrationservice::ServiceType::ConnectionConfirmationResponse;
			break;
		case ServiceType::AugmentRCSessionControl:
			typeProtoValue = ::tvregistrationservice::ServiceType::AugmentRCSessionControl;
			break;
		case ServiceType::AugmentRCSessionConsumer:
			typeProtoValue = ::tvregistrationservice::ServiceType::AugmentRCSessionConsumer;
			break;
		default:

			break;
	}

	if (typeProtoValue == ::tvregistrationservice::ServiceType::Unknown)
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_InvalidInputParameter;
		return returnValue;
	}

	request.set_type(typeProtoValue);

	request.set_location(location);

	::tvregistrationservice::RegisterResponse response{};

	Transport::SocketIO::Status status = m_channel->Call(comId, Function_Register, request, response);

	if (status.ok())
	{
		returnValue = CallStatus{CallState::Ok};
	}
	else
	{
		returnValue.errorMessage = status.error_message();
	}

	return returnValue;
}

} // namespace RegistrationService

} // namespace TVRemoteScreenSDKCommunication