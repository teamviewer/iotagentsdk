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
#include "SessionStatusServiceClient.h"

namespace TVRemoteScreenSDKCommunication
{
namespace SessionStatusService
{

SessionStatusServiceClient::SessionStatusServiceClient()
	: BaseType{ServiceType::SessionStatus}
{
}

CallStatus SessionStatusServiceClient::RemoteControlStarted(
	const std::string& comId,
	const GrabStrategy strategy)
{
	::tvsessionstatusservice::RemoteControlStartedRequest_GrabStrategy wiredStrategy =
		::tvsessionstatusservice::RemoteControlStartedRequest_GrabStrategy_Unknown;
	switch (strategy)
	{
		case GrabStrategy::ChangeNotificationOnly:
			wiredStrategy = ::tvsessionstatusservice::
				RemoteControlStartedRequest_GrabStrategy_ChangeNotificationOnly;
			break;
		case GrabStrategy::EventDrivenByApp:
			wiredStrategy = ::tvsessionstatusservice::
				RemoteControlStartedRequest_GrabStrategy_EventDrivenByApp;
			break;
		case GrabStrategy::NoGrabbing:
			wiredStrategy = ::tvsessionstatusservice::
				RemoteControlStartedRequest_GrabStrategy_NoGrabbing;
			break;
		case GrabStrategy::Unknown:
			break;
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvsessionstatusservice::RemoteControlStartedRequest;
		using Response = ::tvsessionstatusservice::RemoteControlStartedResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			::tvsessionstatusservice::
				RemoteControlStartedRequest_GrabStrategy wiredStrategy)
		{
			request.set_strategy(wiredStrategy);
			return stub.RemoteControlStarted(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, wiredStrategy);
}

CallStatus SessionStatusServiceClient::RemoteControlStopped(const std::string& comId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvsessionstatusservice::RemoteControlStoppedRequest;
		using Response = ::tvsessionstatusservice::RemoteControlStoppedResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response)
		{
			return stub.RemoteControlStopped(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId);
}

CallStatus SessionStatusServiceClient::TVSessionStarted(
	const std::string& comId,
	int32_t tvSessionID,
	int32_t tvSessionsCount)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvsessionstatusservice::TVSessionStartedRequest;
		using Response = ::tvsessionstatusservice::TVSessionStartedResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			int32_t tvSessionID,
			int32_t tvSessionsCount)
		{
			request.set_tvsessionid(tvSessionID);
			request.set_tvsessionscount(tvSessionsCount);
			return stub.TVSessionStarted(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, tvSessionID, tvSessionsCount);
}

CallStatus SessionStatusServiceClient::TVSessionStopped(
	const std::string& comId,
	int32_t tvSessionID,
	int32_t tvSessionsCount)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvsessionstatusservice::TVSessionStoppedRequest;
		using Response = ::tvsessionstatusservice::TVSessionStoppedResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			int32_t tvSessionID,
			int32_t tvSessionsCount)
		{
			request.set_tvsessionid(tvSessionID);
			request.set_tvsessionscount(tvSessionsCount);
			return stub.TVSessionStopped(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, tvSessionID, tvSessionsCount);
}

} // namespace SessionStatusService
} // namespace TVRemoteScreenSDKCommunication
