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

#include "AccessControlInService.grpc.pb.h"

#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlInServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceServer.h>

namespace TVRemoteScreenSDKCommunication
{
namespace AccessControlService
{

struct CallbacksMeta
{
	enum class Enum : size_t
	{
		ConfirmationReply,
		GetAccess,
		SetAccess,

		EnumSize
	};

	using Callbacks = std::tuple<
		IAccessControlInServiceServer::ProcessConfirmationReplyRequestCallback,
		IAccessControlInServiceServer::ProcessGetAccessRequestCallback,
		IAccessControlInServiceServer::ProcessSetAccessRequestCallback>;
};

class AccessControlInServiceServer :
	public ServiceBase::AbstractServiceServer<
		IAccessControlInServiceServer,
		::tvaccesscontrolservice::AccessControlInService,
		CallbacksMeta>
{
public:
	AccessControlInServiceServer();
	~AccessControlInServiceServer() override;

	void SetConfirmationReplyCallback(
		const ProcessConfirmationReplyRequestCallback& requestProcessing) override;
	void SetGetAccessCallback(
		const ProcessGetAccessRequestCallback& requestProcessing) override;
	void SetSetAccessCallback(
		const ProcessSetAccessRequestCallback& requestProcessing) override;

	::grpc::Status ConfirmationReply(
		::grpc::ServerContext* context,
		const ::tvaccesscontrolservice::ConfirmationReplyRequest* request,
		::tvaccesscontrolservice::ConfirmationReplyResponse* response) override;

	::grpc::Status GetAccess(
		::grpc::ServerContext* context,
		const ::tvaccesscontrolservice::GetAccessRequest* request,
		::tvaccesscontrolservice::GetAccessResponse* response) override;

	::grpc::Status SetAccess(
		::grpc::ServerContext* context,
		const ::tvaccesscontrolservice::SetAccessRequest* request,
		::tvaccesscontrolservice::SetAccessResponse* response) override;
};

} // namespace AccessControlService
} // namespace TVRemoteScreenSDKCommunication
