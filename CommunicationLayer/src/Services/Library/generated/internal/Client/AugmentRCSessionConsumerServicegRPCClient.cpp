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

#include "AugmentRCSessionConsumerServicegRPCClient.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>

#include <grpc++/create_channel.h>

namespace TVRemoteScreenSDKCommunication
{

namespace AugmentRCSessionService
{

void AugmentRCSessionConsumerServicegRPCClient::StartClient(const std::string& destination)
{
	m_destination = destination;

	m_channel = ::grpc::CreateChannel(m_destination, ::grpc::InsecureChannelCredentials());
	m_stub = ::tvaugmentrcsessionservice::AugmentRCSessionConsumerService::NewStub(m_channel);
}

void AugmentRCSessionConsumerServicegRPCClient::StopClient(bool /*force*/)
{
	m_stub.reset();
	m_channel.reset();
}

ServiceType AugmentRCSessionConsumerServicegRPCClient::GetServiceType() const
{
	return ServiceType::AugmentRCSessionConsumer;
}

const std::string& AugmentRCSessionConsumerServicegRPCClient::GetDestination() const
{
	return m_destination;
}

// rpc call ReceivedInvitation
auto AugmentRCSessionConsumerServicegRPCClient::ReceivedInvitation(const std::string& comId,
	const std::string& invitationUrl) -> CallStatus
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

	::tvaugmentrcsessionservice::ReceivedInvitationRequest request{};

	request.set_invitationurl(invitationUrl);

	::tvaugmentrcsessionservice::ReceivedInvitationResponse response{};

	::grpc::Status status = m_stub->ReceivedInvitation(&context, request, &response);

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

} // namespace AugmentRCSessionService

} // namespace TVRemoteScreenSDKCommunication