//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2023 TeamViewer Germany GmbH                                     //
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

#include <grpc++/grpc++.h>

#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/server.h>

#include "ServerParams.h"

namespace TVRemoteScreenSDKCommunication
{

namespace Transport
{

struct gRPC final
{
	using ChannelInterface   = ::grpc::ChannelInterface;
	using ClientContext      = ::grpc::ClientContext;
	using Server             = ::grpc::Server;
	using ServerContext      = ::grpc::ServerContext;
	using Service            = ::grpc::Service;
	using Status             = ::grpc::Status;
	using StatusCode         = ::grpc::StatusCode;

	static std::shared_ptr<ChannelInterface> CreateChannel(const std::string& destination);

	/**
	 * @brief CreateAndStartSyncServer creates and starts a synchronous gRPC server with the given parameters.
	 * @param locationUri location for the server (see gRPC documentation for supported schemes)
	 * @param service pointer to service instance to handle incoming calls (the provided instance needs to
	 * @param numberOfCompletionQueues number of completion queues, 0 .. do not set, use gRPC default
	 * @param minPollingThreads minimum number of polling threads, 0 .. do not set, use gRPC default
	 * @param maxPollingThreads maximum number of polling threads, 0 .. do not set, use gRPC default
	 * @param completionQueuesTimeout timeout in milliseconds for completion queues, -1 .. do not set, use gRPC default
	 * @return server instance if creating and starting was successful
	 */
	static std::unique_ptr<Server> CreateAndStartSyncServer(
		const std::string& locationUri,
		Service* service,
		int numberOfCompletionQueues = DefaultNumberOfCompletionQueues,
		int minPollingThreads = DefaultMinPollingThreads,
		int maxPollingThreads = DefaultMaxPollingThreads,
		int completionQueuesTimeout = DefaultCompletionQueueTimeout);
};

} // namespace Transport

} // namespace TVRemoteScreenSDKCommunication
