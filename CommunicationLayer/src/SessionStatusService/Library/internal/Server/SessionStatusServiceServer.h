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
#pragma once

#include "SessionStatusService.grpc.pb.h"

#include <TVRemoteScreenSDKCommunication/SessionStatusService/ISessionStatusServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceServer.h>

namespace TVRemoteScreenSDKCommunication
{
namespace SessionStatusService
{

struct CallbacksMeta
{
	enum class Enum : size_t
	{
		RemoteControlStarted,
		RemoteControlStopped,
		TVSessionStarted,
		TVSessionStopped,

		EnumSize
	};

	using Callbacks = std::tuple<
		ISessionStatusServiceServer::RemoteControlStartedCallback,
		ISessionStatusServiceServer::RemoteControlStoppedCallback,
		ISessionStatusServiceServer::TVSessionStartedCallback,
		ISessionStatusServiceServer::TVSessionStoppedCallback>;
};

class SessionStatusServiceServer :
	public ServiceBase::AbstractServiceServer<
		ISessionStatusServiceServer,
		tvsessionstatusservice::SessionStatusService,
		CallbacksMeta>
{
public:
	SessionStatusServiceServer();
	~SessionStatusServiceServer() override;

	void SetRemoteControlStartedCallback(
		const RemoteControlStartedCallback& requestProcessing) override;

	void SetRemoteControlStoppedCallback(
		const RemoteControlStoppedCallback& requestProcessing) override;

	void SetTVSessionStartedCallback(
		const TVSessionStartedCallback& requestProcessing) override;

	void SetTVSessionStoppedCallback(
		const TVSessionStoppedCallback& requestProcessing) override;

	::grpc::Status RemoteControlStarted(
		::grpc::ServerContext* context,
		const ::tvsessionstatusservice::RemoteControlStartedRequest* request,
		::tvsessionstatusservice::RemoteControlStartedResponse* response) override;

	::grpc::Status RemoteControlStopped(
		::grpc::ServerContext* context,
		const ::tvsessionstatusservice::RemoteControlStoppedRequest* request,
		::tvsessionstatusservice::RemoteControlStoppedResponse* response) override;

	::grpc::Status TVSessionStarted(
		::grpc::ServerContext* context,
		const ::tvsessionstatusservice::TVSessionStartedRequest* request,
		::tvsessionstatusservice::TVSessionStartedResponse* response) override;

	::grpc::Status TVSessionStopped(
		::grpc::ServerContext* context,
		const ::tvsessionstatusservice::TVSessionStoppedRequest* request,
		::tvsessionstatusservice::TVSessionStoppedResponse* response) override;
};

} // namespace SessionStatusService
} // namespace TVRemoteScreenSDKCommunication
