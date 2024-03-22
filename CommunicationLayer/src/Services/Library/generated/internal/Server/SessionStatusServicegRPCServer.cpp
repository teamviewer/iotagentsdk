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

#include "SessionStatusServicegRPCServer.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/ServiceErrorMessage.h>

#include <grpc++/grpc++.h>

namespace TVRemoteScreenSDKCommunication
{

namespace SessionStatusService
{

bool SessionStatusServicegRPCServer::StartServer(const std::string& location)
{
	m_location = location;

	m_server = TransportFW::CreateAndStartSyncServer(m_location,
		this); // Register this "service" as the instance through which we'll communicate with clients.

	return m_server != nullptr;
}

void SessionStatusServicegRPCServer::StopServer(bool /*force*/)
{
	if (m_server)
	{
		m_server->Shutdown();
	}
	m_server.reset();
}

ServiceType SessionStatusServicegRPCServer::GetServiceType() const
{
	return ServiceType::SessionStatus;
}

const std::string& SessionStatusServicegRPCServer::GetLocation() const
{
	return m_location;
}

void SessionStatusServicegRPCServer::SetRemoteControlStartedCallback(const ProcessRemoteControlStartedRequestCallback& requestProcessing)
{
	m_remoteControlStartedProcessing = requestProcessing;
}

void SessionStatusServicegRPCServer::SetRemoteControlStoppedCallback(const ProcessRemoteControlStoppedRequestCallback& requestProcessing)
{
	m_remoteControlStoppedProcessing = requestProcessing;
}

void SessionStatusServicegRPCServer::SetTVSessionStartedCallback(const ProcessTVSessionStartedRequestCallback& requestProcessing)
{
	m_tVSessionStartedProcessing = requestProcessing;
}

void SessionStatusServicegRPCServer::SetTVSessionStoppedCallback(const ProcessTVSessionStoppedRequestCallback& requestProcessing)
{
	m_tVSessionStoppedProcessing = requestProcessing;
}

::grpc::Status SessionStatusServicegRPCServer::RemoteControlStarted(::grpc::ServerContext* context,
	const ::tvsessionstatusservice::RemoteControlStartedRequest* requestPtr,
	::tvsessionstatusservice::RemoteControlStartedResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_remoteControlStartedProcessing)
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

	const auto strategy = request.strategy();

	GrabStrategy strategyEnumValue = GrabStrategy::Unknown;

	switch (strategy)
	{
		case ::tvsessionstatusservice::RemoteControlStartedRequest_GrabStrategy_Unknown:
			strategyEnumValue = GrabStrategy::Unknown;
			break;
		case ::tvsessionstatusservice::RemoteControlStartedRequest_GrabStrategy_NoGrabbing:
			strategyEnumValue = GrabStrategy::NoGrabbing;
			break;
		case ::tvsessionstatusservice::RemoteControlStartedRequest_GrabStrategy_EventDrivenByApp:
			strategyEnumValue = GrabStrategy::EventDrivenByApp;
			break;
		case ::tvsessionstatusservice::RemoteControlStartedRequest_GrabStrategy_ChangeNotificationOnly:
			strategyEnumValue = GrabStrategy::ChangeNotificationOnly;
			break;
		default:

			break;
	}

	m_remoteControlStartedProcessing(comId,
		strategyEnumValue,

		responseProcessing);

	return returnStatus;
}

::grpc::Status SessionStatusServicegRPCServer::RemoteControlStopped(::grpc::ServerContext* context,
	const ::tvsessionstatusservice::RemoteControlStoppedRequest* requestPtr,
	::tvsessionstatusservice::RemoteControlStoppedResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_remoteControlStoppedProcessing)
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

	m_remoteControlStoppedProcessing(comId, responseProcessing);

	return returnStatus;
}

::grpc::Status SessionStatusServicegRPCServer::TVSessionStarted(::grpc::ServerContext* context,
	const ::tvsessionstatusservice::TVSessionStartedRequest* requestPtr,
	::tvsessionstatusservice::TVSessionStartedResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_tVSessionStartedProcessing)
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

	m_tVSessionStartedProcessing(comId,
		request.tvsessionid(),

		request.tvsessionscount(),

		responseProcessing);

	return returnStatus;
}

::grpc::Status SessionStatusServicegRPCServer::TVSessionStopped(::grpc::ServerContext* context,
	const ::tvsessionstatusservice::TVSessionStoppedRequest* requestPtr,
	::tvsessionstatusservice::TVSessionStoppedResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_tVSessionStoppedProcessing)
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

	m_tVSessionStoppedProcessing(comId,
		request.tvsessionid(),

		request.tvsessionscount(),

		responseProcessing);

	return returnStatus;
}

} // namespace SessionStatusService

} // namespace TVRemoteScreenSDKCommunication