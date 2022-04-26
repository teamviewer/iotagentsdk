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

#include "AccessControlOutService.grpc.pb.h"

#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlOutServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceServer.h>

namespace TVRemoteScreenSDKCommunication
{
namespace AccessControlService
{

struct CallbacksMeta
{
	enum class Enum : size_t
	{
		Confirmation,
		NotifyChange,

		EnumSize
	};

	using Callbacks = std::tuple<
		IAccessControlOutServiceServer::ProcessConfirmationRequestCallback,
		IAccessControlOutServiceServer::ProcessNotifyChangeRequestCallback>;
};

class AccessControlOutServiceServer :
	public ServiceBase::AbstractServiceServer<
		IAccessControlOutServiceServer,
		::tvaccesscontrolservice::AccessControlOutService,
		CallbacksMeta>
{
public:
	AccessControlOutServiceServer();
	~AccessControlOutServiceServer() override;

	void SetProcessConfirmationRequestCallback(
		const ProcessConfirmationRequestCallback& requestProcessing) override;

	void SetProcessNotifyChangeRequestCallback(
		const ProcessNotifyChangeRequestCallback& requestProcessing) override;

	::grpc::Status AskForConfirmation(
		::grpc::ServerContext* context,
		const ::tvaccesscontrolservice::AskForConfirmationRequest* request, 
		::tvaccesscontrolservice::AskForConfirmationResponse* response) override;
		
	::grpc::Status NotifyChange(
		::grpc::ServerContext* context,
		const ::tvaccesscontrolservice::NotifyChangeRequest* request, 
		::tvaccesscontrolservice::NotifyChangeResponse* response) override;
};

} // namespace AccessControlService
} // namespace TVRemoteScreenSDKCommunication
