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

class IChatOutServiceServer : public IServiceServer
{
public:
	virtual ~IChatOutServiceServer() = default;

	using ChatCreatedResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessChatCreatedRequestCallback = std::function<void(const std::string& comId, std::string chatId, std::string title, ChatType chatType, uint32_t chatTypeId, const ChatCreatedResponseCallback& response)>;
	virtual void SetProcessChatCreatedRequestCallback(const ProcessChatCreatedRequestCallback& requestProcessing) = 0;

	using ChatsRemovedResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessChatsRemovedRequestCallback = std::function<void(const std::string& comId, std::vector<std::string> chatIds, const ChatsRemovedResponseCallback& response)>;
	virtual void SetProcessChatsRemovedRequestCallback(const ProcessChatsRemovedRequestCallback& requestProcessing) = 0;

	using ReceivedMessagesResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessReceivedMessagesRequestCallback = std::function<void(const std::string& comId, std::vector<ReceivedMessage> messages, const ReceivedMessagesResponseCallback& response)>;
	virtual void SetProcessReceivedMessagesRequestCallback(const ProcessReceivedMessagesRequestCallback& requestProcessing) = 0;

	using MessageSentResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessMessageSentRequestCallback = std::function<void(const std::string& comId, uint32_t localId, std::string messageId, uint64_t timeStamp, const MessageSentResponseCallback& response)>;
	virtual void SetProcessMessageSentRequestCallback(const ProcessMessageSentRequestCallback& requestProcessing) = 0;

	using MessageNotSentResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessMessageNotSentRequestCallback = std::function<void(const std::string& comId, uint32_t localId, const MessageNotSentResponseCallback& response)>;
	virtual void SetProcessMessageNotSentRequestCallback(const ProcessMessageNotSentRequestCallback& requestProcessing) = 0;

	using LoadedMessagesResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessLoadedMessagesRequestCallback = std::function<void(const std::string& comId, std::vector<ReceivedMessage> messages, bool hasMore, const ReceivedMessagesResponseCallback& response)>;
	virtual void SetProcessLoadedMessagesRequestCallback(const ProcessLoadedMessagesRequestCallback& requestProcessing) = 0;

	using DeletedHistoryResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessDeletedHistoryRequestCallback = std::function<void(const std::string& comId, std::string chatId, const ReceivedMessagesResponseCallback& response)>;
	virtual void SetProcessDeletedHistoryRequestCallback(const ProcessDeletedHistoryRequestCallback& requestProcessing) = 0;

	using ClosedChatResponseCallback = std::function<void(const CallStatus& callStatus)>;
	using ProcessClosedChatRequestCallback = std::function<void(const std::string& comId, std::string chatId, const ReceivedMessagesResponseCallback& response)>;
	virtual void SetProcessClosedChatRequestCallback(const ProcessClosedChatRequestCallback& requestProcessing) = 0;
};

} // namespace ChatOutService
} // namespace TVRemoteScreenSDKCommunication
