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

#include "AugmentRCSessionInvitationControlServicegRPCServer.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/ServiceErrorMessage.h>

#include <grpc++/grpc++.h>

namespace TVRemoteScreenSDKCommunication
{

namespace AugmentRCSessionInvitationService
{

bool AugmentRCSessionInvitationControlServicegRPCServer::StartServer(const std::string& location)
{
	m_location = location;

	m_server = TransportFW::CreateAndStartSyncServer(m_location,
		this); // Register this "service" as the instance through which we'll communicate with clients.

	return m_server != nullptr;
}

void AugmentRCSessionInvitationControlServicegRPCServer::StopServer(bool /*force*/)
{
	if (m_server)
	{
		m_server->Shutdown();
	}
	m_server.reset();
}

ServiceType AugmentRCSessionInvitationControlServicegRPCServer::GetServiceType() const
{
	return ServiceType::AugmentRCSessionInvitationControl;
}

const std::string& AugmentRCSessionInvitationControlServicegRPCServer::GetLocation() const
{
	return m_location;
}

void AugmentRCSessionInvitationControlServicegRPCServer::SetStartListeningCallback(
	const ProcessStartListeningRequestCallback& requestProcessing)
{
	m_startListeningProcessing = requestProcessing;
}

void AugmentRCSessionInvitationControlServicegRPCServer::SetStopListeningCallback(
	const ProcessStopListeningRequestCallback& requestProcessing)
{
	m_stopListeningProcessing = requestProcessing;
}

void AugmentRCSessionInvitationControlServicegRPCServer::SetCancelReceivedInvitationCallback(
	const ProcessCancelReceivedInvitationRequestCallback& requestProcessing)
{
	m_cancelReceivedInvitationProcessing = requestProcessing;
}

::grpc::Status AugmentRCSessionInvitationControlServicegRPCServer::StartListening(::grpc::ServerContext* context,
	const ::tvaugmentrcsessioninvitationservice::StartListeningRequest* requestPtr,
	::tvaugmentrcsessioninvitationservice::StartListeningResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_startListeningProcessing)
	{
		return ::grpc::Status(::grpc::StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback);
	}
	auto& request = *requestPtr;
	(void)request;

	auto& response = *responsePtr;
	(void)response;

	std::string comId;

	const auto foundComId = context->client_metadata().find(ServiceBase::CommunicationIdToken);
	if (foundComId == context->client_metadata().end())
	{
		return ::grpc::Status(::grpc::StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId);
	}
	comId = std::string((foundComId->second).data(), (foundComId->second).length());

	::grpc::Status returnStatus =
		::grpc::Status(::grpc::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled);

	auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
	{
		if (callStatus.IsOk())
		{
			returnStatus = ::grpc::Status::OK;
		}
		else
		{
			returnStatus = ::grpc::Status(::grpc::StatusCode::ABORTED, callStatus.errorMessage);
		}
	};

	m_startListeningProcessing(comId, responseProcessing);

	return returnStatus;
}

::grpc::Status AugmentRCSessionInvitationControlServicegRPCServer::StopListening(::grpc::ServerContext* context,
	const ::tvaugmentrcsessioninvitationservice::StopListeningRequest* requestPtr,
	::tvaugmentrcsessioninvitationservice::StopListeningResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_stopListeningProcessing)
	{
		return ::grpc::Status(::grpc::StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback);
	}
	auto& request = *requestPtr;
	(void)request;

	auto& response = *responsePtr;
	(void)response;

	std::string comId;

	const auto foundComId = context->client_metadata().find(ServiceBase::CommunicationIdToken);
	if (foundComId == context->client_metadata().end())
	{
		return ::grpc::Status(::grpc::StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId);
	}
	comId = std::string((foundComId->second).data(), (foundComId->second).length());

	::grpc::Status returnStatus =
		::grpc::Status(::grpc::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled);

	auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
	{
		if (callStatus.IsOk())
		{
			returnStatus = ::grpc::Status::OK;
		}
		else
		{
			returnStatus = ::grpc::Status(::grpc::StatusCode::ABORTED, callStatus.errorMessage);
		}
	};

	m_stopListeningProcessing(comId, responseProcessing);

	return returnStatus;
}

::grpc::Status AugmentRCSessionInvitationControlServicegRPCServer::CancelReceivedInvitation(::grpc::ServerContext* context,
	const ::tvaugmentrcsessioninvitationservice::CancelReceivedInvitationRequest* requestPtr,
	::tvaugmentrcsessioninvitationservice::CancelReceivedInvitationResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_cancelReceivedInvitationProcessing)
	{
		return ::grpc::Status(::grpc::StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback);
	}
	auto& request = *requestPtr;
	(void)request;

	auto& response = *responsePtr;
	(void)response;

	std::string comId;

	const auto foundComId = context->client_metadata().find(ServiceBase::CommunicationIdToken);
	if (foundComId == context->client_metadata().end())
	{
		return ::grpc::Status(::grpc::StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId);
	}
	comId = std::string((foundComId->second).data(), (foundComId->second).length());

	::grpc::Status returnStatus =
		::grpc::Status(::grpc::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled);

	auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
	{
		if (callStatus.IsOk())
		{
			returnStatus = ::grpc::Status::OK;
		}
		else
		{
			returnStatus = ::grpc::Status(::grpc::StatusCode::ABORTED, callStatus.errorMessage);
		}
	};

	const auto cancellationReason = request.cancellationreason();

	CancellationReason cancellationReasonEnumValue = CancellationReason::Invalid;

	switch (cancellationReason)
	{
		case ::tvaugmentrcsessioninvitationservice::CancellationReason::Invalid:
			cancellationReasonEnumValue = CancellationReason::Invalid;
			break;
		case ::tvaugmentrcsessioninvitationservice::CancellationReason::ClosedDialog:
			cancellationReasonEnumValue = CancellationReason::ClosedDialog;
			break;
		case ::tvaugmentrcsessioninvitationservice::CancellationReason::PressedCancelButton:
			cancellationReasonEnumValue = CancellationReason::PressedCancelButton;
			break;
		default:

			break;
	}

	m_cancelReceivedInvitationProcessing(comId,
		cancellationReasonEnumValue,

		responseProcessing);

	return returnStatus;
}

} // namespace AugmentRCSessionInvitationService

} // namespace TVRemoteScreenSDKCommunication