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

namespace TestChatInService
{
namespace TestData
{

constexpr const char* Socket = "unix:///tmp/chatIn";
constexpr const char* ComId = "token";
constexpr const char* ChatId = "01234567-89ab-cdef-0123-456789abcdef";
constexpr const char* ChatTitle = "title" ;
const TVRemoteScreenSDKCommunication::ChatService::ChatType ChatType = TVRemoteScreenSDKCommunication::ChatService::ChatType::Machine;
constexpr const uint32_t ChatTypeId = 1234567890;
const TVRemoteScreenSDKCommunication::ChatService::ChatState ChatState = TVRemoteScreenSDKCommunication::ChatService::ChatState::Open;
constexpr const uint32_t LocalId = 1234567890;
constexpr const char* Content = "content";
constexpr const uint32_t Count = 1;
constexpr const char* MessageId = "f1234567-89ab-cdef-0123-456789abcde0";

} // namespace TestData
} // namespace TestChatInService
