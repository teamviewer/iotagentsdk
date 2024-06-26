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
#pragma once

#include <TVRemoteScreenSDKCommunication/InputService/IInputServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/gRPCTransport.h>

#include "InputService.grpc.pb.h"

#include <grpc++/server.h>

#include <memory>
#include <string>

namespace TVRemoteScreenSDKCommunication
{

namespace InputService
{

class InputServicegRPCServer
	: public ::tvinputservice::InputService::Service
	, public IInputServiceServer
{
public:
	using TransportFW = Transport::gRPC;

	InputServicegRPCServer() = default;
	~InputServicegRPCServer() override = default;

	bool StartServer(const std::string& location) override;
	void StopServer(bool force) override;

	ServiceType GetServiceType() const override;
	const std::string& GetLocation() const override;

	// public interface impl
	void SetSimulateKeyCallback(const ProcessSimulateKeyRequestCallback& requestProcessing) override;

	void SetSimulateMouseMoveCallback(const ProcessSimulateMouseMoveRequestCallback& requestProcessing) override;

	void SetSimulateMousePressReleaseCallback(const ProcessSimulateMousePressReleaseRequestCallback& requestProcessing) override;

	void SetSimulateMouseWheelCallback(const ProcessSimulateMouseWheelRequestCallback& requestProcessing) override;

	// grpc service impl
	::grpc::Status SimulateKey(::grpc::ServerContext* context,
		const ::tvinputservice::KeyRequest* request,
		::tvinputservice::KeyResponse* response) override;

	::grpc::Status SimulateMouseMove(::grpc::ServerContext* context,
		const ::tvinputservice::MouseMoveRequest* request,
		::tvinputservice::MouseMoveResponse* response) override;

	::grpc::Status SimulateMousePressRelease(::grpc::ServerContext* context,
		const ::tvinputservice::MousePressReleaseRequest* request,
		::tvinputservice::MousePressReleaseResponse* response) override;

	::grpc::Status SimulateMouseWheel(::grpc::ServerContext* context,
		const ::tvinputservice::MouseWheelRequest* request,
		::tvinputservice::MouseWheelResponse* response) override;

private:
	std::string m_location;
	std::unique_ptr<::grpc::Server> m_server;

	ProcessSimulateKeyRequestCallback m_simulateKeyProcessing;
	ProcessSimulateMouseMoveRequestCallback m_simulateMouseMoveProcessing;
	ProcessSimulateMousePressReleaseRequestCallback m_simulateMousePressReleaseProcessing;
	ProcessSimulateMouseWheelRequestCallback m_simulateMouseWheelProcessing;
};

} // namespace InputService

} // namespace TVRemoteScreenSDKCommunication