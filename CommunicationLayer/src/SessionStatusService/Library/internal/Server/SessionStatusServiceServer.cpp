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
#include "SessionStatusServiceServer.h"

#include "TVRemoteScreenSDKCommunication/SessionStatusService/GrabStrategy.h"

namespace TVRemoteScreenSDKCommunication
{
namespace SessionStatusService
{

SessionStatusServiceServer::SessionStatusServiceServer()
	: BaseType{ServiceType::SessionStatus}
{
}

SessionStatusServiceServer::~SessionStatusServiceServer() = default;

void SessionStatusServiceServer::SetRemoteControlStartedCallback(
	const RemoteControlStartedCallback& requestProcessing)
{
	SetCallback<CBEnum::RemoteControlStarted>(requestProcessing);
}

void SessionStatusServiceServer::SetRemoteControlStoppedCallback(
	const RemoteControlStoppedCallback& requestProcessing)
{
	SetCallback<CBEnum::RemoteControlStopped>(requestProcessing);
}

void SessionStatusServiceServer::SetTVSessionStartedCallback(
	const TVSessionStartedCallback& requestProcessing)
{
	SetCallback<CBEnum::TVSessionStarted>(requestProcessing);
}

void SessionStatusServiceServer::SetTVSessionStoppedCallback(
	const TVSessionStoppedCallback& requestProcessing)
{
	SetCallback<CBEnum::TVSessionStopped>(requestProcessing);
}

::grpc::Status SessionStatusServiceServer::RemoteControlStarted(
	::grpc::ServerContext* context,
	const ::tvsessionstatusservice::RemoteControlStartedRequest* request,
	::tvsessionstatusservice::RemoteControlStartedResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::RemoteControlStarted,
		::tvsessionstatusservice::RemoteControlStartedRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			SessionStatusService::GrabStrategy strategy =
				SessionStatusService::GrabStrategy::Unknown;

			switch (request.strategy())
			{
				case ::tvsessionstatusservice::
						RemoteControlStartedRequest_GrabStrategy_ChangeNotificationOnly:
					strategy = GrabStrategy::ChangeNotificationOnly;
					break;
				case ::tvsessionstatusservice::
						RemoteControlStartedRequest_GrabStrategy_EventDrivenByApp:
					strategy = GrabStrategy::EventDrivenByApp;
					break;
				case ::tvsessionstatusservice::
						RemoteControlStartedRequest_GrabStrategy_NoGrabbing:
					strategy = GrabStrategy::NoGrabbing;
					break;
				case ::tvsessionstatusservice::
						RemoteControlStartedRequest_GrabStrategy_Unknown:
				default:;
			}

			callback(comId, strategy, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status SessionStatusServiceServer::RemoteControlStopped(
	::grpc::ServerContext* context,
	const ::tvsessionstatusservice::RemoteControlStoppedRequest* request,
	::tvsessionstatusservice::RemoteControlStoppedResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::RemoteControlStopped,
		::tvsessionstatusservice::RemoteControlStoppedRequest>
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

grpc::Status SessionStatusServiceServer::TVSessionStarted(
	::grpc::ServerContext* context,
	const ::tvsessionstatusservice::TVSessionStartedRequest* request,
	::tvsessionstatusservice::TVSessionStartedResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::TVSessionStarted,
		::tvsessionstatusservice::TVSessionStartedRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, request.tvsessionid(), request.tvsessionscount(), responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

grpc::Status SessionStatusServiceServer::TVSessionStopped(
	::grpc::ServerContext* context,
	const ::tvsessionstatusservice::TVSessionStoppedRequest* request,
	::tvsessionstatusservice::TVSessionStoppedResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::TVSessionStopped,
		::tvsessionstatusservice::TVSessionStoppedRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, request.tvsessionid(), request.tvsessionscount(), responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

} // namespace SessionStatusService
} // namespace TVRemoteScreenSDKCommunication
