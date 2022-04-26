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
#include "AccessControlInServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace AccessControlService
{

AccessControlInServiceServer::AccessControlInServiceServer()
	: BaseType{ServiceType::AccessControlIn}
{
}

AccessControlInServiceServer::~AccessControlInServiceServer() = default;

void AccessControlInServiceServer::SetConfirmationReplyCallback(
	const ProcessConfirmationReplyRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::ConfirmationReply>(requestProcessing);
}

void AccessControlInServiceServer::SetGetAccessCallback(
	const ProcessGetAccessRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::GetAccess>(requestProcessing);
}

void AccessControlInServiceServer::SetSetAccessCallback(
	const ProcessSetAccessRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::SetAccess>(requestProcessing);
}

namespace
{

template <CallbacksMeta::Enum Name, typename RequestType>
struct AccessControlMeta:
	AccessControlInServiceServer::DefaultMeta<Name, RequestType>
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

::grpc::Status AccessControlInServiceServer::ConfirmationReply(
	::grpc::ServerContext* context,
	const ::tvaccesscontrolservice::ConfirmationReplyRequest* request,
	::tvaccesscontrolservice::ConfirmationReplyResponse* response)
{
	struct Meta: AccessControlMeta<
		CBEnum::ConfirmationReply,
		::tvaccesscontrolservice::ConfirmationReplyRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			const AccessControl feature = ToAccessControl(request.feature());
			const bool confirmed = request.confirmed();

			callback(comId, feature, confirmed, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status AccessControlInServiceServer::GetAccess(
	::grpc::ServerContext* context,
	const ::tvaccesscontrolservice::GetAccessRequest* request,
	::tvaccesscontrolservice::GetAccessResponse* response)
{
	struct Meta: AccessControlMeta<
		CBEnum::GetAccess,
		::tvaccesscontrolservice::GetAccessRequest>
	{
		using ResponseProcessing =
			std::function<void(const CallStatus& callStatus, Access accessMode)>;

		static ResponseProcessing MakeResponseProcessing(
			::grpc::Status& returnStatus,
			::tvaccesscontrolservice::GetAccessResponse& response)
		{
			return [&returnStatus, &response](
				const CallStatus& callStatus,
				Access accessMode)
			{
				if (callStatus.IsOk())
				{
					returnStatus = ::grpc::Status::OK;

					::tvaccesscontrolservice::Access access =
						::tvaccesscontrolservice::Access::Denied;

					switch (accessMode)
					{
						case Access::Allowed:
							access = ::tvaccesscontrolservice::Access::Allowed;
							break;
						case Access::AfterConfirmation:
							access = ::tvaccesscontrolservice::Access::AfterConfirmation;
							break;
						case Access::Denied:
							access = ::tvaccesscontrolservice::Access::Denied;
							break;
						default:
							returnStatus = ::grpc::Status{
								::grpc::StatusCode::CANCELLED,
								TvServiceBase::ErrorMessage_UnexpectedEnumValue};
							break;
					}

					response.set_access(access);
				}
				else
				{
					returnStatus = ::grpc::Status{
						::grpc::StatusCode::INTERNAL,
						callStatus.errorMessage};
				}
			};
		}

		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			const AccessControl feature = ToAccessControl(request.feature());
			callback(comId, feature, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status AccessControlInServiceServer::SetAccess(
	::grpc::ServerContext* context,
	const ::tvaccesscontrolservice::SetAccessRequest* request,
	::tvaccesscontrolservice::SetAccessResponse* response)
{
	struct Meta: AccessControlMeta<
		CBEnum::SetAccess,
		::tvaccesscontrolservice::SetAccessRequest>
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

} // namespace AccessControlService
} // namespace TVRemoteScreenSDKCommunication
