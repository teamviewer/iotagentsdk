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
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceServer.h>

#include <functional>
#include <vector>

namespace TVRemoteScreenSDKCommunication
{
namespace ChatService
{

class IChatInServiceServer : public IServiceServer
{
public:
	virtual ~IChatInServiceServer() = default;

	using ObtainChatsResponseCallback = std::function<void(const std::vector<ChatInfo>& chats, const CallStatus& callStatus)>;
	using ProcessObtainChatsRequestCallback = std::function<void(const std::string& comId, const ObtainChatsResponseCallback& response)>;
	virtual void SetProcessObtainChatsCallback(const ProcessObtainChatsRequestCallback& requestProcessing) = 0;

	using SelectChatResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessSelectChatRequestCallback = std::function<void(const std::string& comId, std::string chatId, const SelectChatResponseCallback& response)>;
	virtual void SetProcessSelectChatCallback(const ProcessSelectChatRequestCallback& requestProcessing) = 0;

	using SendMessageResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessSendMessageRequestCallback = std::function<void(const std::string& comId, uint32_t localId, std::string content, const SendMessageResponseCallback& response)>;
	virtual void SetProcessSendMessageCallback(const ProcessSendMessageRequestCallback& requestProcessing) = 0;

	using LoadMessagesResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessLoadMessagesRequestCallback = std::function<void(const std::string& comId, uint32_t count, std::string lastId, const LoadMessagesResponseCallback& response)>;
	virtual void SetProcessLoadMessagesCallback(const ProcessLoadMessagesRequestCallback& requestProcessing) = 0;

	using DeleteHistoryResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessDeleteHistoryRequestCallback = std::function<void(const std::string& comId, const DeleteHistoryResponseCallback& response)>;
	virtual void SetProcessDeleteHistoryCallback(const ProcessDeleteHistoryRequestCallback& requestProcessing) = 0;

	using DeleteChatResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessDeleteChatRequestCallback = std::function<void(const std::string& comId, const DeleteChatResponseCallback& response)>;
	virtual void SetProcessDeleteChatCallback(const ProcessDeleteChatRequestCallback& requestProcessing) = 0;
};

} // namespace ChatService
} // namespace TVRemoteScreenSDKCommunication
