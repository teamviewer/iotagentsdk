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
#include "AccessControlOutServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace AccessControlService
{

AccessControlOutServiceServer::AccessControlOutServiceServer()
	: BaseType{ServiceType::AccessControlOut}
{
}

AccessControlOutServiceServer::~AccessControlOutServiceServer() = default;

void AccessControlOutServiceServer::SetProcessConfirmationRequestCallback(
	const ProcessConfirmationRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::Confirmation>(requestProcessing);
}

void AccessControlOutServiceServer::SetProcessNotifyChangeRequestCallback(
	const ProcessNotifyChangeRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::NotifyChange>(requestProcessing);
}

namespace
{

template<CallbacksMeta::Enum Name, typename RequestType>
struct AccessControlMeta :
	AccessControlOutServiceServer::DefaultMeta<Name, RequestType>
{
	using BaseMeta = AccessControlMeta;

	static bool ValidateAccessControl(::tvaccesscontrolservice::AccessControl feature)
	{
		return
			feature == ::tvaccesscontrolservice::AccessControl::FileTransfer ||
			feature == ::tvaccesscontrolservice::AccessControl::AllowPartnerViewDesktop ||
			feature == ::tvaccesscontrolservice::AccessControl::RemoteControl;
	}

	static AccessControl ToAccessControl(
		::tvaccesscontrolservice::AccessControl accessControl)
	{
		switch (accessControl)
		{
			case ::tvaccesscontrolservice::AccessControl::AllowPartnerViewDesktop:
				return AccessControl::AllowPartnerViewDesktop;
			case ::tvaccesscontrolservice::AccessControl::RemoteControl:
				return AccessControl::RemoteControl;
			case ::tvaccesscontrolservice::AccessControl::FileTransfer:
			default:
				return AccessControl::FileTransfer;
		}
	}

	static bool ValidateAccess(::tvaccesscontrolservice::Access access)
	{
		return
			access == ::tvaccesscontrolservice::Access::Allowed ||
			access == ::tvaccesscontrolservice::Access::AfterConfirmation ||
			access == ::tvaccesscontrolservice::Access::Denied;
	}

	static Access ToAccess(::tvaccesscontrolservice::Access access)
	{
		switch (access)
		{
			case ::tvaccesscontrolservice::Access::Allowed:
				return Access::Allowed;
			case ::tvaccesscontrolservice::Access::AfterConfirmation:
				return Access::AfterConfirmation;
				break;
			case ::tvaccesscontrolservice::Access::Denied:
			default:
				return Access::Denied;
		}
	}


	static ::grpc::Status ValidateRequest(const RequestType& request)
	{
		if (!ValidateAccessControl(request.feature()))
		{
			return ::grpc::Status{
				::grpc::StatusCode::CANCELLED,
				TvServiceBase::ErrorMessage_UnexpectedEnumValue};
		}

		return ::grpc::Status::OK;
	}
};

} // namespace

::grpc::Status AccessControlOutServiceServer::AskForConfirmation(
	::grpc::ServerContext* context,
	const ::tvaccesscontrolservice::AskForConfirmationRequest* request,
	::tvaccesscontrolservice::AskForConfirmationResponse* response)
{
	struct Meta: AccessControlMeta<
		CBEnum::Confirmation,
		::tvaccesscontrolservice::AskForConfirmationRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			const AccessControl feature = ToAccessControl(request.feature());

			callback(comId, feature, request.timeout(), responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status AccessControlOutServiceServer::NotifyChange(
	::grpc::ServerContext* context,
	const ::tvaccesscontrolservice::NotifyChangeRequest* request,
	::tvaccesscontrolservice::NotifyChangeResponse* response)
{
	struct Meta: AccessControlMeta<
		CBEnum::NotifyChange,
		::tvaccesscontrolservice::NotifyChangeRequest>
	{
		static ::grpc::Status ValidateRequest(const Request& request)
		{
			::grpc::Status result = BaseMeta::ValidateRequest(request);
			if (!result.ok())
			{
				return result;
			}

			const ::tvaccesscontrolservice::Access access = request.access();
			if (!ValidateAccess(access))
			{
				return ::grpc::Status{
					::grpc::StatusCode::CANCELLED,
					TvServiceBase::ErrorMessage_UnexpectedEnumValue};
			}
			return ::grpc::Status::OK;
		}

		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			const AccessControl feature = ToAccessControl(request.feature());
			const Access access = ToAccess(request.access());

			callback(comId, feature, access, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}
} // namespace AccessControlOutService
} // namespace TVRemoteScreenSDKCommunication
