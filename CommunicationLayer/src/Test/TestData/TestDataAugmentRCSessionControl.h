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
#pragma once

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/TransportFramework.h>

#include <TVRemoteScreenSDKCommunication/SessionControlService/ControlMode.h>

#include <vector>

namespace TestAugmentRCSessionControlService
{
template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
struct TestData;

template<>
struct TestData<TVRemoteScreenSDKCommunication::gRPCTransport>
{
	static constexpr const char *Socket = "unix:///tmp/augmentRCSessionControl";
	static constexpr const char *ComId = "tokengRPC";
};

template<>
struct TestData<TVRemoteScreenSDKCommunication::TCPSocketTransport>
{
	static constexpr const char *Socket = "tv+tcp://127.0.0.1:9003";
	static constexpr const char *ComId = "tokenSocketIO";
};
} // namespace TestSessionControlService
