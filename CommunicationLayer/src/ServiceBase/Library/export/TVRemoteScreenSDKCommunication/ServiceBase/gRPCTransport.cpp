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
#include "gRPCTransport.h"

namespace TVRemoteScreenSDKCommunication
{

namespace Transport
{

std::shared_ptr<gRPC::ChannelInterface> gRPC::CreateChannel(const std::string& destination)
{
	return ::grpc::CreateChannel(destination, ::grpc::InsecureChannelCredentials());
}

std::unique_ptr<gRPC::Server> gRPC::CreateAndStartSyncServer(
	const std::string& locationUri,
	Service* service,
	int numberOfCompletionQueues,
	int minPollingThreads,
	int maxPollingThreads,
	int completionQueuesTimeout)
{
	::grpc::ServerBuilder builder;

	if (numberOfCompletionQueues > 0)
	{
		builder.SetSyncServerOption(::grpc::ServerBuilder::SyncServerOption::NUM_CQS, numberOfCompletionQueues);
	}

	if (minPollingThreads > 0)
	{
		builder.SetSyncServerOption(::grpc::ServerBuilder::SyncServerOption::MIN_POLLERS, minPollingThreads);
	}

	if (maxPollingThreads > 0)
	{
		builder.SetSyncServerOption(::grpc::ServerBuilder::SyncServerOption::MAX_POLLERS, maxPollingThreads);
	}

	if (completionQueuesTimeout >= 0)
	{
		builder.SetSyncServerOption(::grpc::ServerBuilder::SyncServerOption::CQ_TIMEOUT_MSEC, completionQueuesTimeout);
	}

	builder.AddListeningPort(locationUri, ::grpc::InsecureServerCredentials());
	builder.RegisterService(service);

	return builder.BuildAndStart();
}

} // namespace Transport

} // namespace TVRemoteScreenSDKCommunication