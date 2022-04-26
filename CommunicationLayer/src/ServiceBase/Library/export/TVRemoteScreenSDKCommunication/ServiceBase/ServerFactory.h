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

#include "ServerParams.h"

#include <grpc++/grpc++.h>
#include <memory>
#include <string>

namespace TVRemoteScreenSDKCommunication
{

/**
 * @brief CreateAndStartSyncServer creates and starts a sychronous gRPC server with the given parameters.
 * @param locationUri location for the server (see gRPC documentation for supported schemes)
 * @param credentials pointer to server credentials to use for incoming connections
 * @param service pointer to service instance to handle incoming calls (the provided instance needs to
 * @param numberOfCompletionQueues number of completion queues, 0 .. do not set, use gRPC default
 * @param minPollingThreads minimum number of polling threads, 0 .. do not set, use gRPC default
 * @param maxPollingThreads maximum number of polling threads, 0 .. do not set, use gRPC default
 * @param completionQueuesTimeout timeout in milliseconds for completion queues, -1 .. do not set, use gRPC default
 * @return server instance if creatihg and starting was successful
 */
std::unique_ptr<::grpc::Server> CreateAndStartSyncServer(
	const std::string& locationUri,
	const std::shared_ptr<::grpc::ServerCredentials>& credentials,
	::grpc::Service* service,
	int numberOfCompletionQueues = DefaultNumberOfCompletionQueues,
	int minPollingThreads = DefaultMinPollingThreads,
	int maxPollingThreads = DefaultMaxPollingThreads,
	int completionQueuesTimeout = DefaultCompletionQueueTimeout);

} // namespace TVRemoteScreenSDKCommunication
