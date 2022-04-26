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
#include "RegistrationServiceClient.h"

namespace TVRemoteScreenSDKCommunication
{
namespace RegistrationService
{

RegistrationServiceClient::RegistrationServiceClient()
	: BaseType{ServiceType::Registration}
{
}

IRegistrationServiceClient::ExchangeVersionResponse
	RegistrationServiceClient::ExchangeVersion(
		const std::string& ownVersion)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvregistrationservice::ExchangeVersionRequest;
		using Response = ::tvregistrationservice::ExchangeVersionResponse;
		using Return = ExchangeVersionResponse;

		static bool ValidateInput(const std::string&, ...)
		{
			return true;
		}

		static void PrepareContext(::grpc::ClientContext&, ...)
		{
		}

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::string& comId)
		{
			request.set_version(comId);

			return stub.ExchangeVersion(&context, request, &response);
		}

		static Return HandleResponse(
			const ::grpc::Status& status,
			const Response& response,
			...)
		{
			Return returnValue;
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
	};

	return ClientCall<Meta>({}, ownVersion);
}

IRegistrationServiceClient::DiscoverResponse RegistrationServiceClient::Discover(
	const std::string& comId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvregistrationservice::DiscoverRequest;
		using Response = ::tvregistrationservice::DiscoverResponse;
		using Return = DiscoverResponse;

		static void PrepareContext(::grpc::ClientContext&, ...)
		{
		}

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::string& comId)
		{
			request.set_communicationversion(comId);
			return stub.Discover(&context, request, &response);
		}

		static Return HandleResponse(
			const ::grpc::Status& status,
			const Response& response,
			...)
		{
			Return returnValue;

			if (status.ok())
			{
				returnValue.state = CallState::Ok;
				returnValue.communicationId = response.communicationid();

				std::vector<ServiceInformation> services;

				for (int serviceIndex = 0; serviceIndex < response.serviceinfo_size(); ++serviceIndex)
				{
					ServiceInformation serviceInformation;
					serviceInformation.type = ServiceType::Unknown;

					const ::tvregistrationservice::ServiceInformation& serviceInfo = response.serviceinfo(serviceIndex);

					serviceInformation.location = serviceInfo.location();
					switch (serviceInfo.type())
					{
						case ::tvregistrationservice::ServiceType::Unknown:
							serviceInformation.type = ServiceType::Unknown;
							break;
						case ::tvregistrationservice::ServiceType::Registration:
							serviceInformation.type = ServiceType::Registration;
							break;
						case ::tvregistrationservice::ServiceType::Connectivity:
							serviceInformation.type = ServiceType::Connectivity;
							break;
						case ::tvregistrationservice::ServiceType::Image:
							serviceInformation.type = ServiceType::Image;
							break;
						case ::tvregistrationservice::ServiceType::ImageNotification:
							serviceInformation.type = ServiceType::ImageNotification;
							break;
						case ::tvregistrationservice::ServiceType::Input:
							serviceInformation.type = ServiceType::Input;
							break;
						case ::tvregistrationservice::ServiceType::SessionControl:
							serviceInformation.type = ServiceType::SessionControl;
							break;
						case ::tvregistrationservice::ServiceType::SessionStatus:
							serviceInformation.type = ServiceType::SessionStatus;
							break;
						case ::tvregistrationservice::ServiceType::AccessControlIn:
							serviceInformation.type = ServiceType::AccessControlIn;
							break;
						case ::tvregistrationservice::ServiceType::AccessControlOut:
							serviceInformation.type = ServiceType::AccessControlOut;
							break;
						case ::tvregistrationservice::ServiceType::InstantSupport:
							serviceInformation.type = ServiceType::InstantSupport;
							break;
						case ::tvregistrationservice::ServiceType::InstantSupportNotification:
							serviceInformation.type = ServiceType::InstantSupportNotification;
							break;
						case ::tvregistrationservice::ServiceType::ChatIn:
							serviceInformation.type = ServiceType::ChatIn;
							break;
						case ::tvregistrationservice::ServiceType::ChatOut:
							serviceInformation.type = ServiceType::ChatOut;
							break;
						case ::tvregistrationservice::ServiceType::ConnectionConfirmationRequest:
							serviceInformation.type = ServiceType::ConnectionConfirmationRequest;
							break;
						case ::tvregistrationservice::ServiceType::ConnectionConfirmationResponse:
							serviceInformation.type = ServiceType::ConnectionConfirmationResponse;
							break;
						default:
							break;
					}

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
	};

	return ClientCall<Meta>(comId, comId);
}

CallStatus RegistrationServiceClient::RegisterService(
	const std::string& comId,
	const ServiceType serviceType,
	const std::string& serviceLocation)
{
	::tvregistrationservice::ServiceType wireFormatServiceType =
		::tvregistrationservice::ServiceType::Unknown;

	switch (serviceType)
	{
		case ServiceType::Unknown:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::Unknown;
			break;
		case ServiceType::Registration:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::Registration;
			break;
		case ServiceType::Connectivity:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::Connectivity;
			break;
		case ServiceType::Image:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::Image;
			break;
		case ServiceType::ImageNotification:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::ImageNotification;
			break;
		case ServiceType::Input:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::Input;
			break;
		case ServiceType::SessionControl:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::SessionControl;
			break;
		case ServiceType::SessionStatus:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::SessionStatus;
			break;
		case ServiceType::AccessControlIn:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::AccessControlIn;
			break;
		case ServiceType::AccessControlOut:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::AccessControlOut;
			break;
		case ServiceType::InstantSupport:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::InstantSupport;
			break;
		case ServiceType::ViewGeometry:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::ViewGeometry;
			break;
		case ServiceType::InstantSupportNotification:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::
				InstantSupportNotification;
			break;
		case ServiceType::ChatIn:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::ChatIn;
			break;
		case ServiceType::ChatOut:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::ChatOut;
			break;
		case ServiceType::ConnectionConfirmationRequest:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::
				ConnectionConfirmationRequest;
			break;
		case ServiceType::ConnectionConfirmationResponse:
			wireFormatServiceType = ::tvregistrationservice::ServiceType::
				ConnectionConfirmationResponse;
			break;
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvregistrationservice::RegisterRequest;
		using Response = ::tvregistrationservice::RegisterResponse;

		static bool ValidateInput(
			const std::string& comId,
			::tvregistrationservice::ServiceType wireFormatServiceType,
			const std::string& serviceLocation)
		{
			return DefaultMeta::ValidateInput(comId) &&
				wireFormatServiceType != ::tvregistrationservice::ServiceType::Unknown &&
				!serviceLocation.empty();
		}

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			::tvregistrationservice::ServiceType wireFormatServiceType,
			const std::string& serviceLocation)
		{
			request.set_type(wireFormatServiceType);
			request.set_location(serviceLocation);

			return stub.Register(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, wireFormatServiceType, serviceLocation);
}

} // namespace RegistrationService
} // namespace TVRemoteScreenSDKCommunication
