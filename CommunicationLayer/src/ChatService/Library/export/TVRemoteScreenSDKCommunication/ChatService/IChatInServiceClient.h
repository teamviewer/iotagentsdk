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

#include "Chat.h"

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/CallStatus.h>
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceClient.h>

#include <vector>

namespace TVRemoteScreenSDKCommunication
{
namespace ChatService
{

class IChatInServiceClient : public IServiceClient
{
public:
	virtual ~IChatInServiceClient() = default;

	struct ObtainChatsResponse : CallStatus
	{
		using CallStatus::CallStatus;

		std::vector<ChatInfo> chats;
	};

	virtual ObtainChatsResponse ObtainChats(const std::string& comId) = 0;

	virtual CallStatus SelectChat(const std::string& comId, std::string chatId) = 0;

	virtual CallStatus SendMessage(const std::string& comId, uint32_t localId, std::string content) = 0;

	virtual CallStatus LoadMessages(const std::string& comId, uint32_t count, std::string lastId) = 0;

	virtual CallStatus DeleteHistory(const std::string& comId) = 0;

	virtual CallStatus DeleteChat(const std::string& comId) = 0;
};

} // namespace ChatService
} // namespace TVRemoteScreenSDKCommunication
