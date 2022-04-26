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
#include "SessionControlServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace SessionControlService
{

SessionControlServiceServer::SessionControlServiceServer()
	: BaseType{ServiceType::SessionControl}
{
}

SessionControlServiceServer::~SessionControlServiceServer() = default;

void SessionControlServiceServer::SetChangeControlModeCallback(
	const ProcessChangeControlModeRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::ChangeControlMode>(requestProcessing);
}

void SessionControlServiceServer::SetCloseRcCallback(
	const ProcessCloseRcRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::CloseRc>(requestProcessing);
}

void SessionControlServiceServer::SetGetRunningTVSessionsCallback(
	const ProcessGetRunningTVSessionsRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::GetRunningTVSessions>(requestProcessing);
}

::grpc::Status SessionControlServiceServer::ChangeControlMode(
	::grpc::ServerContext* context,
	const ::tvsessioncontrolservice::ChangeControlModeRequest* request,
	::tvsessioncontrolservice::ChangeControlModeResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::ChangeControlMode,
		::tvsessioncontrolservice::ChangeControlModeRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			ControlMode controlMode = ControlMode::Unknown;
			switch (request.controlmode())
			{
				case ::tvsessioncontrolservice::ChangeControlModeRequest::ControlMode::
						ChangeControlModeRequest_ControlMode_FullControl:
					controlMode = ControlMode::FullControl;
					break;
				case ::tvsessioncontrolservice::ChangeControlModeRequest::ControlMode::
						ChangeControlModeRequest_ControlMode_ScreenSharing:
					controlMode = ControlMode::ScreenSharing;
					break;
				case ::tvsessioncontrolservice::ChangeControlModeRequest::ControlMode::
						ChangeControlModeRequest_ControlMode_Disable:
					controlMode = ControlMode::Disable;
					break;
				case ::tvsessioncontrolservice::ChangeControlModeRequest::ControlMode::
						ChangeControlModeRequest_ControlMode_Unknown:
				default:;
			}

			callback(comId, controlMode, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status SessionControlServiceServer::CloseRc(
	::grpc::ServerContext* context,
	const ::tvsessioncontrolservice::CloseRcRequest* request,
	::tvsessioncontrolservice::CloseRcResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::CloseRc,
		::tvsessioncontrolservice::CloseRcRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& /*request*/,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

grpc::Status SessionControlServiceServer::GetRunningTVSessions(
	grpc::ServerContext* context,
	const ::tvsessioncontrolservice::RunningSessionsRequest* request,
	::tvsessioncontrolservice::RunningSessionsResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::GetRunningTVSessions,
		::tvsessioncontrolservice::RunningSessionsRequest>
	{
		using ResponseProcessing =
			ISessionControlServiceServer::GetRunningTVSessionsResponseCallback;

		static ResponseProcessing MakeResponseProcessing(
			::grpc::Status& returnStatus,
			::tvsessioncontrolservice::RunningSessionsResponse& response)
		{
			return [&returnStatus, &response](const CallStatus& callStatus, const std::vector<int32_t>& tvSessionIDs)
			{
				if (callStatus.IsOk())
				{
					for (int32_t sessionID : tvSessionIDs)
					{
						auto sessionInfo = response.add_tvsessioninfos();
						sessionInfo->set_sessionid(sessionID);
					}

					returnStatus = ::grpc::Status::OK;
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
			const Request& /*request*/,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

} // namespace SessionControlService
} // namespace TVRemoteScreenSDKCommunication
