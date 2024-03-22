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

#include "ChannelInterface.h"
#include "Service.h"

#include <type_traits>

namespace TVRemoteScreenSDKCommunication
{

namespace Transport
{

namespace SocketIO
{

struct BaseStub
{
	virtual ~BaseStub() = default;
	std::shared_ptr<Transport::SocketIO::ChannelInterface> channel;
};

template <typename StubType, typename ServiceType>
struct ServiceMeta final
{
	using Stub = StubType;
	using Service = ServiceType;

	static_assert(std::is_base_of<BaseStub, Stub>::value, "Stub must inherit from BaseStub");
	static_assert(std::is_base_of<SocketIO::Service, Service>::value, "Service must inherit from SocketIO::Service");

	static std::unique_ptr<Stub> NewStub(std::shared_ptr<ChannelInterface> channel)
	{
		std::unique_ptr<Stub> newStub{new Stub};
		newStub->channel = std::move(channel);
		return newStub;
	}
};

} // namespace SocketIO

} // namespace Transport

} // namespace TVRemoteScreenSDKCommunication
