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

#include "InServiceFactory.h"

#ifdef TV_COMM_ENABLE_GRPC
#include "internal/Server/AccessControlInServicegRPCServer.h"
#include "internal/Client/AccessControlInServicegRPCClient.h"
#endif

#ifdef TV_COMM_ENABLE_PLAIN_SOCKET
#include "internal/Server/AccessControlInServiceSocketIOServer.h"
#include "internal/Client/AccessControlInServiceSocketIOClient.h"
#endif

namespace TVRemoteScreenSDKCommunication
{

namespace AccessControlService
{

#ifdef TV_COMM_ENABLE_GRPC
template<>
std::unique_ptr<IAccessControlInServiceServer> InServiceFactory::CreateServer<gRPCTransport>()
{
	return std::unique_ptr<IAccessControlInServiceServer>{new AccessControlInServicegRPCServer};
}

template<>
std::unique_ptr<IAccessControlInServiceClient> InServiceFactory::CreateClient<gRPCTransport>()
{
	return std::unique_ptr<IAccessControlInServiceClient>{new AccessControlInServicegRPCClient};
}
#endif

#ifdef TV_COMM_ENABLE_PLAIN_SOCKET
template<>
std::unique_ptr<IAccessControlInServiceServer> InServiceFactory::CreateServer<TCPSocketTransport>()
{
	return std::unique_ptr<IAccessControlInServiceServer>{new AccessControlInServiceSocketIOServer};
}

template<>
std::unique_ptr<IAccessControlInServiceClient> InServiceFactory::CreateClient<TCPSocketTransport>()
{
	return std::unique_ptr<IAccessControlInServiceClient>{new AccessControlInServiceSocketIOClient};
}
#endif

} // namespace AccessControlService

} // namespace TVRemoteScreenSDKCommunication