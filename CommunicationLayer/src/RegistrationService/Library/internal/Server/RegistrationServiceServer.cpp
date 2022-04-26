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
#include "RegistrationServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace RegistrationService
{

RegistrationServiceServer::RegistrationServiceServer()
	: BaseType{ServiceType::Registration}
{
}

RegistrationServiceServer::~RegistrationServiceServer() = default;

void RegistrationServiceServer::SetExchangeVersionCallback(
	const ProcessExchangeVersionRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::ExchangeVersion>(requestProcessing);
}

void RegistrationServiceServer::SetDiscoverCallback(
	const ProcessDiscoverRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::Discover>(requestProcessing);
}

void RegistrationServiceServer::SetRegisterCallback(
	const ProcessRegisterRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::Register>(requestProcessing);
}

::grpc::Status RegistrationServiceServer::ExchangeVersion(
	::grpc::ServerContext* context,
	const ::tvregistrationservice::ExchangeVersionRequest* request,
	::tvregistrationservice::ExchangeVersionResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::ExchangeVersion,
		::tvregistrationservice::ExchangeVersionRequest>
	{
		using ResponseProcessing =
			std::function<void(
				const std::string& ownVersionNumber,
				const CallStatus& callStatus)>;

		static bool GetComId(
			std::string& /*comId*/,
			const ::grpc::ServerContext& /*context*/)
		{
			return true;
		}

		static ResponseProcessing MakeResponseProcessing(
			::grpc::Status& returnStatus,
			::tvregistrationservice::ExchangeVersionResponse& response)
		{
			return [&returnStatus, &response]
				(const std::string& ownVersionNumber, const CallStatus& callStatus)
			{
				if (callStatus.IsOk())
				{
					response.set_version(ownVersionNumber);
					returnStatus = ::grpc::Status::OK;
				}
				else
				{
					returnStatus = ::grpc::Status{
						::grpc::StatusCode::ABORTED,
						callStatus.errorMessage};
				}
			};
		}

		static void InvokeCallback(
			const Callback& callback,
			const std::string& /*comId*/,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(request.version(), responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status RegistrationServiceServer::Discover(
	::grpc::ServerContext* context,
	const ::tvregistrationservice::DiscoverRequest* request,
	::tvregistrationservice::DiscoverResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::Discover,
		::tvregistrationservice::DiscoverRequest>
	{
		using ResponseProcessing =
		std::function<void(
			const std::string& comId,
			const std::vector<ServiceInformation>& services,
			const CallStatus& callStatus)>;

		static bool GetComId(
			std::string& /*comId*/,
			const ::grpc::ServerContext& /*context*/)
		{
			return true;
		}

		static ResponseProcessing MakeResponseProcessing(
			::grpc::Status& returnStatus,
			::tvregistrationservice::DiscoverResponse& response)
		{
			return [&returnStatus, &response]
				(const std::string& comId,
				 const std::vector<ServiceInformation>& services,
				 const CallStatus& callStatus)
			{
				if (callStatus.IsOk())
				{
					response.set_communicationid(comId);
					for (const ServiceInformation& serviceInfo : services)
					{
						::tvregistrationservice::ServiceInformation* info =
							response.add_serviceinfo();
						::tvregistrationservice::ServiceType communicationServiceType =
							::tvregistrationservice::ServiceType::Unknown;

						switch (serviceInfo.type)
						{
							case ServiceType::Unknown:
								communicationServiceType =
									::tvregistrationservice::ServiceType::Unknown;
								break;
							case ServiceType::Registration:
								communicationServiceType =
									::tvregistrationservice::ServiceType::Registration;
								break;
							case ServiceType::Image:
								communicationServiceType =
									::tvregistrationservice::ServiceType::Image;
								break;
							case ServiceType::ImageNotification:
								communicationServiceType =
									::tvregistrationservice::
									ServiceType::ImageNotification;
								break;
							case ServiceType::Connectivity:
								communicationServiceType =
									::tvregistrationservice::ServiceType::Connectivity;
								break;
							case ServiceType::Input:
								communicationServiceType =
									::tvregistrationservice::ServiceType::Input;
								break;
							case ServiceType::SessionControl:
								communicationServiceType =
									::tvregistrationservice::ServiceType::SessionControl;
								break;
							case ServiceType::SessionStatus:
								communicationServiceType =
									::tvregistrationservice::ServiceType::SessionStatus;
								break;
							case ServiceType::AccessControlIn:
								communicationServiceType =
									::tvregistrationservice::ServiceType::AccessControlIn;
								break;
							case ServiceType::AccessControlOut:
								communicationServiceType =
									::tvregistrationservice::ServiceType::AccessControlOut;
								break;
							case ServiceType::InstantSupport:
								communicationServiceType =
									::tvregistrationservice::ServiceType::InstantSupport;
								break;
							case ServiceType::ViewGeometry:
								communicationServiceType =
									::tvregistrationservice::ServiceType::ViewGeometry;
								break;
							case ServiceType::InstantSupportNotification:
								communicationServiceType =
									::tvregistrationservice::
									ServiceType::InstantSupportNotification;
								break;
							case ServiceType::ChatIn:
								communicationServiceType =
									::tvregistrationservice::ServiceType::ChatIn;
								break;
							case ServiceType::ChatOut:
								communicationServiceType =
									::tvregistrationservice::ServiceType::ChatOut;
								break;
							case ServiceType::ConnectionConfirmationRequest:
								communicationServiceType =
									::tvregistrationservice::
									ServiceType::ConnectionConfirmationRequest;
								break;
							case ServiceType::ConnectionConfirmationResponse:
								communicationServiceType =
									::tvregistrationservice::
									ServiceType::ConnectionConfirmationResponse;
								break;
						}

						info->set_type(communicationServiceType);
						info->set_location(serviceInfo.location);
					}
					returnStatus = ::grpc::Status::OK;
				}
				else
				{
					returnStatus = ::grpc::Status{
						::grpc::StatusCode::ABORTED,
						callStatus.errorMessage};
				}
			};
		}

		static void InvokeCallback(
			const Callback& callback,
			const std::string& /*comId*/,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(request.communicationversion(), responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status RegistrationServiceServer::Register(
	::grpc::ServerContext* context,
	const ::tvregistrationservice::RegisterRequest* request,
	::tvregistrationservice::RegisterResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::Register,
		::tvregistrationservice::RegisterRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			ServiceType serviceType = ServiceType::Unknown;
			switch (request.type())
			{
				case ::tvregistrationservice::ServiceType::Registration:
					serviceType = ServiceType::Registration;
					break;
				case ::tvregistrationservice::ServiceType::Connectivity:
					serviceType = ServiceType::Connectivity;
					break;
				case ::tvregistrationservice::ServiceType::Image:
					serviceType = ServiceType::Image;
					break;
				case ::tvregistrationservice::ServiceType::Input:
					serviceType = ServiceType::Input;
					break;
				case ::tvregistrationservice::ServiceType::SessionControl:
					serviceType = ServiceType::SessionControl;
					break;
				case ::tvregistrationservice::ServiceType::SessionStatus:
					serviceType = ServiceType::SessionStatus;
					break;
				case ::tvregistrationservice::ServiceType::AccessControlIn:
					serviceType = ServiceType::AccessControlIn;
					break;
				case ::tvregistrationservice::ServiceType::AccessControlOut:
					serviceType = ServiceType::AccessControlOut;
					break;
				case ::tvregistrationservice::ServiceType::InstantSupport:
					serviceType = ServiceType::InstantSupport;
					break;
				case ::tvregistrationservice::ServiceType::InstantSupportNotification:
					serviceType = ServiceType::InstantSupportNotification;
					break;
				case ::tvregistrationservice::ServiceType::ChatIn:
					serviceType = ServiceType::ChatIn;
					break;
				case ::tvregistrationservice::ServiceType::ChatOut:
					serviceType = ServiceType::ChatOut;
					break;
				case ::tvregistrationservice::ServiceType::ConnectionConfirmationRequest:
					serviceType = ServiceType::ConnectionConfirmationRequest;
					break;
				case ::tvregistrationservice::ServiceType::ConnectionConfirmationResponse:
					serviceType = ServiceType::ConnectionConfirmationResponse;
					break;
				default:
					break;
			}

			callback(comId, serviceType, request.location(), responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

} // namespace RegistrationService
} // namespace TVRemoteScreenSDKCommunication
