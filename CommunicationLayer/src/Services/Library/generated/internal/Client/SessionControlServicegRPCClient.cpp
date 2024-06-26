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

#include "SessionControlServicegRPCClient.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>

#include <grpc++/create_channel.h>

namespace TVRemoteScreenSDKCommunication
{

namespace SessionControlService
{

void SessionControlServicegRPCClient::StartClient(const std::string& destination)
{
	m_destination = destination;

	m_channel = ::grpc::CreateChannel(m_destination, ::grpc::InsecureChannelCredentials());
	m_stub = ::tvsessioncontrolservice::SessionControlService::NewStub(m_channel);
}

void SessionControlServicegRPCClient::StopClient(bool /*force*/)
{
	m_stub.reset();
	m_channel.reset();
}

ServiceType SessionControlServicegRPCClient::GetServiceType() const
{
	return ServiceType::SessionControl;
}

const std::string& SessionControlServicegRPCClient::GetDestination() const
{
	return m_destination;
}

// rpc call ChangeControlMode
auto SessionControlServicegRPCClient::ChangeControlMode(const std::string& comId, ControlMode controlMode) -> CallStatus
{
	CallStatus returnValue{};

	if (m_channel == nullptr || m_stub == nullptr)
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_MissingStartClient;
		return returnValue;
	}

	if (comId.empty())
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_InvalidInputParameter;
		return returnValue;
	}

	::grpc::ClientContext context{};

	context.AddMetadata(ServiceBase::CommunicationIdToken, comId);

	::tvsessioncontrolservice::ChangeControlModeRequest request{};

	::tvsessioncontrolservice::ChangeControlModeRequest_ControlMode controlModeProtoValue =
		::tvsessioncontrolservice::ChangeControlModeRequest_ControlMode_Unknown;

	switch (controlMode)
	{
		case ControlMode::Unknown:
			controlModeProtoValue = ::tvsessioncontrolservice::ChangeControlModeRequest_ControlMode_Unknown;
			break;
		case ControlMode::FullControl:
			controlModeProtoValue = ::tvsessioncontrolservice::ChangeControlModeRequest_ControlMode_FullControl;
			break;
		case ControlMode::ScreenSharing:
			controlModeProtoValue = ::tvsessioncontrolservice::ChangeControlModeRequest_ControlMode_ScreenSharing;
			break;
		case ControlMode::Disable:
			controlModeProtoValue = ::tvsessioncontrolservice::ChangeControlModeRequest_ControlMode_Disable;
			break;
		default:

			break;
	}

	if (controlModeProtoValue == ::tvsessioncontrolservice::ChangeControlModeRequest_ControlMode_Unknown)
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_InvalidInputParameter;
		return returnValue;
	}

	request.set_controlmode(controlModeProtoValue);

	::tvsessioncontrolservice::ChangeControlModeResponse response{};

	::grpc::Status status = m_stub->ChangeControlMode(&context, request, &response);

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

// rpc call CloseRc
auto SessionControlServicegRPCClient::CloseRc(const std::string& comId) -> CallStatus
{
	CallStatus returnValue{};

	if (m_channel == nullptr || m_stub == nullptr)
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_MissingStartClient;
		return returnValue;
	}

	if (comId.empty())
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_InvalidInputParameter;
		return returnValue;
	}

	::grpc::ClientContext context{};

	context.AddMetadata(ServiceBase::CommunicationIdToken, comId);

	::tvsessioncontrolservice::CloseRcRequest request{};

	::tvsessioncontrolservice::CloseRcResponse response{};

	::grpc::Status status = m_stub->CloseRc(&context, request, &response);

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

// rpc call GetRunningTVSessions
auto SessionControlServicegRPCClient::GetRunningTVSessions(const std::string& comId) -> TVSessionsResult
{
	TVSessionsResult returnValue{};

	if (m_channel == nullptr || m_stub == nullptr)
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_MissingStartClient;
		return returnValue;
	}

	if (comId.empty())
	{
		returnValue.errorMessage = TvServiceBase::ErrorMessage_InvalidInputParameter;
		return returnValue;
	}

	::grpc::ClientContext context{};

	context.AddMetadata(ServiceBase::CommunicationIdToken, comId);

	::tvsessioncontrolservice::RunningSessionsRequest request{};

	::tvsessioncontrolservice::RunningSessionsResponse response{};

	::grpc::Status status = m_stub->GetRunningTVSessions(&context, request, &response);

	if (status.ok())
	{
		returnValue.state = CallState::Ok;
		returnValue.tvSessionIDs.reserve(response.tvsessioninfos_size());

		for (int i = 0; i < response.tvsessioninfos_size(); ++i)
		{
			returnValue.tvSessionIDs.push_back(response.tvsessioninfos(i).sessionid());
		}
	}
	else
	{
		returnValue.errorMessage = status.error_message();
	}

	return returnValue;
}

} // namespace SessionControlService

} // namespace TVRemoteScreenSDKCommunication