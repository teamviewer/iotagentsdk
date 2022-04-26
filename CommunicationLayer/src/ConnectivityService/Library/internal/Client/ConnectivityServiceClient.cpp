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
#include "ConnectivityServiceClient.h"

namespace TVRemoteScreenSDKCommunication
{
namespace ConnectivityService
{

ConnectivityServiceClient::ConnectivityServiceClient()
	: BaseType{ServiceType::Connectivity}
{
}

CallStatus ConnectivityServiceClient::IsAvailable(const std::string& comId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvconnectivityservice::HeartbeatRequest;
		using Response = ::tvconnectivityservice::HeartbeatResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response)
		{
			return stub.IsAvailable(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId);
}

CallStatus ConnectivityServiceClient::Disconnect(const std::string& comId)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvconnectivityservice::DisconnectRequest;
		using Response = ::tvconnectivityservice::DisconnectResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response)
		{
			return stub.Disconnect(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId);
}

} // namespace ConnectivityService
} // namespace TVRemoteScreenSDKCommunicatio
