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
#include "AccessControlOutServiceClient.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h>

namespace TVRemoteScreenSDKCommunication
{
namespace AccessControlService
{

AccessControlOutServiceClient::AccessControlOutServiceClient()
	: BaseType{ServiceType::AccessControlOut}
{
}

CallStatus AccessControlOutServiceClient::AskForConfirmation(
	const std::string& comId,
	AccessControl feature,
	uint32_t timeout)
{
	::tvaccesscontrolservice::AccessControl requestedAccess =
		::tvaccesscontrolservice::AccessControl::FileTransfer;
	switch (feature)
	{
		case AccessControl::FileTransfer:
			requestedAccess = ::tvaccesscontrolservice::AccessControl::FileTransfer;
			break;
		case AccessControl::AllowPartnerViewDesktop:
			requestedAccess = ::tvaccesscontrolservice::AccessControl::AllowPartnerViewDesktop;
			break;
		case AccessControl::RemoteControl:
			requestedAccess = ::tvaccesscontrolservice::AccessControl::RemoteControl;
			break;
		default:
			return CallStatus{
				CallState::Failed,
				TvServiceBase::ErrorMessage_InvalidInputParameter};
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvaccesscontrolservice::AskForConfirmationRequest;
		using Response = ::tvaccesscontrolservice::AskForConfirmationResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			::tvaccesscontrolservice::AccessControl requestedAccess,
			uint32_t timeout)
		{
			request.set_timeout(timeout);
			request.set_feature(requestedAccess);

			return stub.AskForConfirmation(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, requestedAccess, timeout);
}

CallStatus AccessControlOutServiceClient::NotifyChange(
	const std::string& comId,
	AccessControl feature,
	Access access)
{
	::tvaccesscontrolservice::AccessControl requestedAccessControl =
		::tvaccesscontrolservice::AccessControl::FileTransfer;
	switch (feature)
	{
		case AccessControl::FileTransfer:
			requestedAccessControl = ::tvaccesscontrolservice::AccessControl::FileTransfer;
			break;
		case AccessControl::AllowPartnerViewDesktop:
			requestedAccessControl = ::tvaccesscontrolservice::AccessControl::AllowPartnerViewDesktop;
			break;
		case AccessControl::RemoteControl:
			requestedAccessControl = ::tvaccesscontrolservice::AccessControl::RemoteControl;
			break;
		default:
			return CallStatus{
				CallState::Failed,
				TvServiceBase::ErrorMessage_InvalidInputParameter};
	}

	::tvaccesscontrolservice::Access requestedAccess =
		::tvaccesscontrolservice::Access::Denied;
	switch (access)
	{
		case Access::Allowed:
			requestedAccess = ::tvaccesscontrolservice::Access::Allowed;
			break;
		case Access::AfterConfirmation:
			requestedAccess = ::tvaccesscontrolservice::Access::AfterConfirmation;
			break;
		case Access::Denied:
			requestedAccess = ::tvaccesscontrolservice::Access::Denied;
			break;
		default:
			return CallStatus{
				CallState::Failed,
				TvServiceBase::ErrorMessage_InvalidInputParameter};
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvaccesscontrolservice::NotifyChangeRequest;
		using Response = ::tvaccesscontrolservice::NotifyChangeResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			::tvaccesscontrolservice::AccessControl requestedAccessControl,
			::tvaccesscontrolservice::Access requestedAccess)
		{
			request.set_feature(requestedAccessControl);
			request.set_access(requestedAccess);

			return stub.NotifyChange(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, requestedAccessControl, requestedAccess);
}

} // namespace AccessControlService
} // namespace TVRemoteScreenSDKCommunication
