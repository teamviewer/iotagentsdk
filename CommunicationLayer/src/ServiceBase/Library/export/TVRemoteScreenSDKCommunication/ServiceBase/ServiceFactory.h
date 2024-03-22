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

#ifdef TV_COMM_ENABLE_GRPC
#include "gRPCTransport.h"
#endif

#ifdef TV_COMM_ENABLE_PLAIN_SOCKET
#include "SocketIOTransport.h"
#endif

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/TransportFramework.h>

#include <memory>
#include <type_traits>

namespace TVRemoteScreenSDKCommunication
{

namespace Detail
{

template<typename Service, typename = void>
struct ServiceFramework
{
	using type = void;
};

#ifdef TV_COMM_ENABLE_GRPC
template<typename ServiceType>
struct ServiceFramework<
	ServiceType,
	typename std::enable_if<std::is_base_of<::grpc::Service, typename ServiceType::Service>::value, void>::type>
{
	using type = Transport::gRPC;
};
#endif

#ifdef TV_COMM_ENABLE_PLAIN_SOCKET
template<typename ServiceType>
struct ServiceFramework<
	ServiceType,
	typename std::enable_if<std::is_base_of<Transport::SocketIO::Service, typename ServiceType::Service>::value, void>::type>
{
	using type = Transport::TCPSocket;
};
#endif

} // namespace Detail

template<
	typename ServiceType,
	template<typename, typename> class ServiceInterface>
auto MakeServiceActor() ->
	std::unique_ptr<ServiceInterface<
		typename Detail::ServiceFramework<ServiceType>::type, ServiceType>>
{
	using EffectiveFramework = typename Detail::ServiceFramework<ServiceType>::type;
	static_assert(!std::is_same<void, EffectiveFramework>::value, "This framework is not supported");
	return std::unique_ptr<ServiceInterface<EffectiveFramework, ServiceType>>{
		new ServiceInterface<EffectiveFramework, ServiceType>};
}

} // namespace TVRemoteScreenSDKCommunication
