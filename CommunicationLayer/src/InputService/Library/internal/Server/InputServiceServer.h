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

#include "InputService.grpc.pb.h"

#include <TVRemoteScreenSDKCommunication/InputService/IInputServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceServer.h>

namespace TVRemoteScreenSDKCommunication
{
namespace InputService
{

struct CallbacksMeta
{
	enum class Enum : size_t
	{
		KeyRequest,
		MouseMove,
		MousePressRelease,
		MouseWheel,

		EnumSize
	};

	using Callbacks = std::tuple<
		IInputServiceServer::ProcessKeyRequestCallback,
		IInputServiceServer::ProcessMouseMoveRequestCallback,
		IInputServiceServer::ProcessMousePressReleaseRequestCallback,
		IInputServiceServer::ProcessMouseWheelRequestCallback>;
};


class InputServiceServer :
	public ServiceBase::AbstractServiceServer<
		IInputServiceServer,
		::tvinputservice::InputService,
		CallbacksMeta>
{
public:
	InputServiceServer();
	~InputServiceServer() override;

	void SetReceivedKeyCallback(
		const ProcessKeyRequestCallback& requestProcessing) override;

	void SetMouseMoveCallback(
		const ProcessMouseMoveRequestCallback& requestProcessing) override;

	void SetMousePressReleaseCallback(
		const ProcessMousePressReleaseRequestCallback& requestProcessing) override;

	void SetMouseWheelCallback(
		const ProcessMouseWheelRequestCallback& requestProcessing) override;

	::grpc::Status SimulateKey(
		::grpc::ServerContext* context,
		const ::tvinputservice::KeyRequest* request,
		::tvinputservice::KeyResponse* response) override;

	::grpc::Status SimulateMouseMove(
		::grpc::ServerContext* context,
		const ::tvinputservice::MouseMoveRequest* request,
		::tvinputservice::MouseMoveResponse* response) override;

	::grpc::Status SimulateMousePressRelease(
		::grpc::ServerContext* context,
		const ::tvinputservice::MousePressReleaseRequest* request,
		::tvinputservice::MousePressReleaseResponse* response) override;

	::grpc::Status SimulateMouseWheel(
		::grpc::ServerContext* context,
		const ::tvinputservice::MouseWheelRequest* request,
		::tvinputservice::MouseWheelResponse* response) override;
};

} // namespace InputService
} // namespace TVRemoteScreenSDKCommunication
