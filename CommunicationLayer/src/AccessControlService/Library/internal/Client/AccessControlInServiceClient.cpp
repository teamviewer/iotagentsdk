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
#include "AccessControlInServiceClient.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h>

namespace TVRemoteScreenSDKCommunication
{
namespace AccessControlService
{

AccessControlInServiceClient::AccessControlInServiceClient()
	: BaseType{ServiceType::AccessControlIn}
{
}

CallStatus AccessControlInServiceClient::ConfirmationReply(
	const std::string& comId,
	AccessControl feature,
	bool confirmed)
{
	::tvaccesscontrolservice::AccessControl requestedFeature =
		::tvaccesscontrolservice::AccessControl::FileTransfer;
	switch (feature)
	{
		case AccessControl::FileTransfer:
			requestedFeature = ::tvaccesscontrolservice::AccessControl::FileTransfer;
			break;
		case AccessControl::AllowPartnerViewDesktop:
			requestedFeature = ::tvaccesscontrolservice::AccessControl::AllowPartnerViewDesktop;
			break;
		case AccessControl::RemoteControl:
			requestedFeature = ::tvaccesscontrolservice::AccessControl::RemoteControl;
			break;
		default:
			return CallStatus{
				CallState::Failed,
				TvServiceBase::ErrorMessage_InvalidInputParameter};
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvaccesscontrolservice::ConfirmationReplyRequest;
		using Response = ::tvaccesscontrolservice::ConfirmationReplyResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			::tvaccesscontrolservice::AccessControl requestedFeature,
			bool confirmed)
		{
			request.set_feature(requestedFeature);
			request.set_confirmed(confirmed);

			return stub.ConfirmationReply(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, requestedFeature, confirmed);
}

IAccessControlInServiceClient::GetAccessResponse AccessControlInServiceClient::GetAccess(
	const std::string& comId,
	AccessControl feature)
{
	::tvaccesscontrolservice::AccessControl requestedFeature =
		::tvaccesscontrolservice::AccessControl::FileTransfer;
	switch (feature)
	{
		case AccessControl::FileTransfer:
			requestedFeature = ::tvaccesscontrolservice::AccessControl::FileTransfer;
			break;
		case AccessControl::AllowPartnerViewDesktop:
			requestedFeature = ::tvaccesscontrolservice::AccessControl::AllowPartnerViewDesktop;
			break;
		case AccessControl::RemoteControl:
			requestedFeature = ::tvaccesscontrolservice::AccessControl::RemoteControl;
			break;
		default:
			GetAccessResponse returnValue;
			returnValue.errorMessage = TvServiceBase::ErrorMessage_InvalidInputParameter;
			return returnValue;
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvaccesscontrolservice::GetAccessRequest;
		using Response = ::tvaccesscontrolservice::GetAccessResponse;
		using Return = GetAccessResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			::tvaccesscontrolservice::AccessControl requestedFeature)
		{
			request.set_feature(requestedFeature);

			return stub.GetAccess(&context, request, &response);
		}

		static Return HandleResponse(
			const ::grpc::Status& status,
			Response& response,
			::tvaccesscontrolservice::AccessControl /*requestedFeature*/)
		{
			Return returnValue;
			if (status.ok())
			{
				::tvaccesscontrolservice::Access access = response.access();
				switch (access)
				{
					case ::tvaccesscontrolservice::Access::Denied:
						returnValue.access = Access::Denied;
						break;
					case ::tvaccesscontrolservice::Access::Allowed:
						returnValue.access = Access::Allowed;
						break;
					case ::tvaccesscontrolservice::Access::AfterConfirmation:
						returnValue.access = Access::AfterConfirmation;
						break;
					default:
						returnValue.errorMessage =
							TvServiceBase::ErrorMessage_InvalidResponseValue;
						return returnValue;
				}
				returnValue.state = CallState::Ok;
			}
			else
			{
				returnValue.errorMessage = status.error_message();
			}
			return returnValue;
		}
	};

	return ClientCall<Meta>(comId, requestedFeature);
}

CallStatus AccessControlInServiceClient::SetAccess(
	const std::string& comId,
	AccessControl feature,
	Access access)
{
	::tvaccesscontrolservice::AccessControl requestedFeature = ::tvaccesscontrolservice::AccessControl::FileTransfer;
	switch (feature)
	{
		case AccessControl::FileTransfer:
			requestedFeature = ::tvaccesscontrolservice::AccessControl::FileTransfer;
			break;
		case AccessControl::AllowPartnerViewDesktop:
			requestedFeature = ::tvaccesscontrolservice::AccessControl::AllowPartnerViewDesktop;
			break;
		case AccessControl::RemoteControl:
			requestedFeature = ::tvaccesscontrolservice::AccessControl::RemoteControl;
			break;
		default:
			return CallStatus{
				CallState::Failed,
				TvServiceBase::ErrorMessage_InvalidInputParameter};
	}

	::tvaccesscontrolservice::Access requestedAccess = ::tvaccesscontrolservice::Access::Denied;
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
		using Request = ::tvaccesscontrolservice::SetAccessRequest;
		using Response = ::tvaccesscontrolservice::SetAccessResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			::tvaccesscontrolservice::AccessControl requestedFeature,
			::tvaccesscontrolservice::Access requestedAccess)
		{
			request.set_feature(requestedFeature);
			request.set_access(requestedAccess);

			return stub.SetAccess(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, requestedFeature, requestedAccess);
}

} // namespace AccessControlService
} // namespace TVRemoteScreenSDKCommunication
