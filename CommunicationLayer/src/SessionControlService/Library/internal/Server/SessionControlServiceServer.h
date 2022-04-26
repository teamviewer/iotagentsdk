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

#include "SessionControlService.grpc.pb.h"

#include <TVRemoteScreenSDKCommunication/SessionControlService/ISessionControlServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceServer.h>

namespace TVRemoteScreenSDKCommunication
{
namespace SessionControlService
{

struct CallbacksMeta
{
	enum class Enum : size_t
	{
		ChangeControlMode,
		CloseRc,
		GetRunningTVSessions,

		EnumSize
	};

	using Callbacks = std::tuple<
		ISessionControlServiceServer::ProcessChangeControlModeRequestCallback,
		ISessionControlServiceServer::ProcessCloseRcRequestCallback,
		ISessionControlServiceServer::ProcessGetRunningTVSessionsRequestCallback>;
};

class SessionControlServiceServer :
	public ServiceBase::AbstractServiceServer<
		ISessionControlServiceServer,
		::tvsessioncontrolservice::SessionControlService,
		CallbacksMeta>
{
public:
	SessionControlServiceServer();
	~SessionControlServiceServer() override;

	void SetChangeControlModeCallback(
		const ProcessChangeControlModeRequestCallback& requestProcessing) override;

	void SetCloseRcCallback(
		const ProcessCloseRcRequestCallback& requestProcessing) override;

	void SetGetRunningTVSessionsCallback(
		const ProcessGetRunningTVSessionsRequestCallback& requestProcessing) override;

	::grpc::Status ChangeControlMode(
		::grpc::ServerContext* context,
		const ::tvsessioncontrolservice::ChangeControlModeRequest* request,
		::tvsessioncontrolservice::ChangeControlModeResponse* response) override;

	::grpc::Status CloseRc(
		::grpc::ServerContext* context,
		const ::tvsessioncontrolservice::CloseRcRequest* request,
		::tvsessioncontrolservice::CloseRcResponse* response) override;

	::grpc::Status GetRunningTVSessions(
		::grpc::ServerContext* context,
		const ::tvsessioncontrolservice::RunningSessionsRequest* request,
		::tvsessioncontrolservice::RunningSessionsResponse* response) override;
};

} // namespace SessionControlService
} // namespace TVRemoteScreenSDKCommunication
