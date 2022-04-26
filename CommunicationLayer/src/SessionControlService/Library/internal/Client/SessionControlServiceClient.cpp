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
#include "SessionControlServiceClient.h"

namespace TVRemoteScreenSDKCommunication
{
namespace SessionControlService
{

SessionControlServiceClient::SessionControlServiceClient()
	: BaseType{ServiceType::SessionControl}
{
}

CallStatus SessionControlServiceClient::ChangeControlMode(
	const std::string& comId,
	ControlMode controlMode)
{
	::tvsessioncontrolservice::ChangeControlModeRequest_ControlMode strategy =
		::tvsessioncontrolservice::ChangeControlModeRequest_ControlMode_Unknown;
	switch (controlMode)
	{
		case ControlMode::FullControl:
			strategy = ::tvsessioncontrolservice::
				ChangeControlModeRequest_ControlMode_FullControl;
			break;
		case ControlMode::ScreenSharing:
			strategy = ::tvsessioncontrolservice::
				ChangeControlModeRequest_ControlMode_ScreenSharing;
			break;
		case ControlMode::Disable:
			strategy = ::tvsessioncontrolservice::
				ChangeControlModeRequest_ControlMode_Disable;
			break;
		case ControlMode::Unknown:
			strategy = ::tvsessioncontrolservice::
				ChangeControlModeRequest_ControlMode_Unknown;
			break;
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvsessioncontrolservice::ChangeControlModeRequest;
		using Response = ::tvsessioncontrolservice::ChangeControlModeResponse;

		static bool ValidateInput(
			const std::string& comId,
			::tvsessioncontrolservice::ChangeControlModeRequest_ControlMode strategy)
		{
			return DefaultMeta::ValidateInput(comId) &&
				strategy != ::tvsessioncontrolservice::
					ChangeControlModeRequest_ControlMode_Unknown;
		}

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			::tvsessioncontrolservice::ChangeControlModeRequest_ControlMode strategy)
		{
			request.set_controlmode(strategy);

			return stub.ChangeControlMode(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, strategy);
}

CallStatus SessionControlServiceClient::CloseRc(const std::string& comId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvsessioncontrolservice::CloseRcRequest;
		using Response = ::tvsessioncontrolservice::CloseRcResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response)
		{
			return stub.CloseRc(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId);
}

ISessionControlServiceClient::TVSessionsResult SessionControlServiceClient::GetRunningTVSessions(
	const std::string& comId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvsessioncontrolservice::RunningSessionsRequest;
		using Response = ::tvsessioncontrolservice::RunningSessionsResponse;
		using Return = ISessionControlServiceClient::TVSessionsResult;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response)
		{
			return stub.GetRunningTVSessions(&context, request, &response);
		}

		static Return HandleResponse(
			const ::grpc::Status& status,
			Response& response)
		{
			Return result;
			if (status.ok())
			{
				result.state = CallState::Ok;
				result.tvSessionIDs.reserve(response.tvsessioninfos_size());

				for (int i = 0; i < response.tvsessioninfos_size(); ++i)
				{
					result.tvSessionIDs.push_back(response.tvsessioninfos(i).sessionid());
				}
			}
			else
			{
				result.errorMessage = status.error_message();
			}
			return result;
		}
	};

	return ClientCall<Meta>(comId);
}

} // namespace SessionControlService
} // namespace TVRemoteScreenSDKCommunication
