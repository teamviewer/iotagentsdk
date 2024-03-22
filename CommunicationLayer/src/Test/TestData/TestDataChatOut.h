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

#include <TVRemoteScreenSDKCommunication/ChatService/Chat.h>

#include <cstdint>

namespace TestChatOutService
{
template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
struct TestData;

template<>
struct TestData<TVRemoteScreenSDKCommunication::gRPCTransport>
{
	static constexpr const char* Socket = "unix:///tmp/chatOut";
	static constexpr const char* ComId = "tokengRPC";
	static constexpr const char* ChatId = "01234567-89ab-cdef-0123-456789abcdef";
	static constexpr const char* ChatTitle = "title" ;
	static constexpr TVRemoteScreenSDKCommunication::ChatService::ChatType ChatType = TVRemoteScreenSDKCommunication::ChatService::ChatType::Machine;
	static constexpr const uint32_t ChatTypeId = 1234567890;
	static constexpr const uint32_t LocalId = 1234567890;
	static constexpr const char* MessageId = "f1234567-89ab-cdef-0123-456789abcde0";
	static constexpr const char* Content = "content";
	static constexpr const uint64_t TimeStamp = 0x123456789abcdef0;
	static constexpr const char* Sender = "Message Sender";
};

template<>
struct TestData<TVRemoteScreenSDKCommunication::TCPSocketTransport>
{
	static constexpr const char* Socket = "tv+tcp://127.0.0.1:9003";
	static constexpr const char* ComId = "tokenSocketIO";
	static constexpr const char* ChatId = "01234567-89ab-cdef-0123-456789defabc";
	static constexpr const char* ChatTitle = "Title" ;
	static constexpr TVRemoteScreenSDKCommunication::ChatService::ChatType ChatType = TVRemoteScreenSDKCommunication::ChatService::ChatType::Machine;
	static constexpr const uint32_t ChatTypeId = 123456789;
	static constexpr const uint32_t LocalId = 123456789;
	static constexpr const char* MessageId = "f1234567-89ab-cdef-0123-456789deabc0";
	static constexpr const char* Content = "Content";
	static constexpr const uint64_t TimeStamp = 0x123456789abcdef;
	static constexpr const char* Sender = "message sender";
};
} // namespace TestChatOutService
