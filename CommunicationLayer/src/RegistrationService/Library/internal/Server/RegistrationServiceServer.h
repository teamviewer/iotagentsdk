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

#include "RegistrationService.grpc.pb.h"

#include <TVRemoteScreenSDKCommunication/RegistrationService/IRegistrationServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceServer.h>

namespace TVRemoteScreenSDKCommunication
{
namespace RegistrationService
{

struct CallbacksMeta
{
	enum class Enum : size_t
	{
		ExchangeVersion,
		Discover,
		Register,

		EnumSize
	};

	using Callbacks = std::tuple<
		IRegistrationServiceServer::ProcessExchangeVersionRequestCallback,
		IRegistrationServiceServer::ProcessDiscoverRequestCallback,
		IRegistrationServiceServer::ProcessRegisterRequestCallback>;
};

class RegistrationServiceServer :
	public ServiceBase::AbstractServiceServer<
		IRegistrationServiceServer,
		::tvregistrationservice::RegistrationService,
		CallbacksMeta>
{
public:
	RegistrationServiceServer();
	~RegistrationServiceServer() override;

	void SetExchangeVersionCallback(
		const ProcessExchangeVersionRequestCallback& requestProcessing) override;
	void SetDiscoverCallback(
		const ProcessDiscoverRequestCallback& requestProcessing) override;
	void SetRegisterCallback(
		const ProcessRegisterRequestCallback& requestProcessing) override;

	::grpc::Status ExchangeVersion(
		::grpc::ServerContext* context,
		const ::tvregistrationservice::ExchangeVersionRequest* request,
		::tvregistrationservice::ExchangeVersionResponse* response) override;

	::grpc::Status Discover(
		::grpc::ServerContext* context,
		const ::tvregistrationservice::DiscoverRequest* request,
		::tvregistrationservice::DiscoverResponse* response) override;

	::grpc::Status Register(
		::grpc::ServerContext* context,
		const ::tvregistrationservice::RegisterRequest* request,
		::tvregistrationservice::RegisterResponse* response) override;
};

} // namespace RegistrationService
} // namespace TVRemoteScreenSDKCommunication
