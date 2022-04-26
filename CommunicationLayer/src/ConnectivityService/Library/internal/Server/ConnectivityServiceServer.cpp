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
#include "ConnectivityServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace ConnectivityService
{

namespace
{

constexpr const char* ErrorMessage_NoProcessingCallback = "No processing callback set.";
constexpr const char* ErrorMessage_NoComId = "No ComId provided.";

} // namespace

ConnectivityServiceServer::ConnectivityServiceServer()
	: BaseType{ServiceType::Connectivity}
{
}

ConnectivityServiceServer::~ConnectivityServiceServer() = default;

void ConnectivityServiceServer::SetIsAvailableCallback(
	const ProcessIsAvailableRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::IsAvailable>(requestProcessing);
}

void ConnectivityServiceServer::SetDisconnectCallback(
	const ProcessDisconnectRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::DisconnectRequest>(requestProcessing);
}

::grpc::Status ConnectivityServiceServer::IsAvailable(
	::grpc::ServerContext* context,
	const ::tvconnectivityservice::HeartbeatRequest* request,
	::tvconnectivityservice::HeartbeatResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::IsAvailable,
		::tvconnectivityservice::HeartbeatRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& /*request*/,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status ConnectivityServiceServer::Disconnect(
	::grpc::ServerContext* context,
	const ::tvconnectivityservice::DisconnectRequest* request,
	::tvconnectivityservice::DisconnectResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::DisconnectRequest,
		::tvconnectivityservice::DisconnectRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& /*request*/,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

} // namespace ConnectivityService
} // namespace TVRemoteScreenSDKCommunication
