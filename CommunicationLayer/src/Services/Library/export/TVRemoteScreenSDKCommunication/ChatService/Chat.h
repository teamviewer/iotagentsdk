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

#include <string>
#include <cstdint>

namespace TVRemoteScreenSDKCommunication
{
namespace ChatService
{

enum class ChatType
{
	Machine = 0,
	Session = 1
};

enum class ChatState
{
	Open = 0,
	Closed = 1
};

struct ChatInfo
{
	std::string chatId;
	std::string title;
	ChatType chatType;
	uint32_t chatTypeId;
	ChatState chatState;
};

struct ReceivedMessage
{
	std::string chatId;
	std::string messageId;
	std::string content;
	uint64_t timeStamp;
	std::string sender;
};

} // namespace ChatService
} // namespace TVRemoteScreenSDKCommunication
