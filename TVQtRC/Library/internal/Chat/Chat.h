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

#include "TVQtRC/AbstractChat.h"

#include "internal/Communication/CommunicationAdapter.h"

namespace tvqtsdk
{

class Chat final : public AbstractChat
{
	Q_OBJECT
public:
	explicit Chat(const std::shared_ptr<tvqtsdk::CommunicationAdapter>& adapter);

	~Chat() override = default;

	bool obtainChats(QVector<ChatInfo>& chats) override;

	bool selectChat(QUuid chatId) override;

	bool sendMessage(uint32_t localId, QString content) override;

	bool loadMessages(uint32_t messageCount, QUuid lastMessageId) override;

	bool deleteHistory() override;

	bool deleteChat() override;

private:
	const std::shared_ptr<tvqtsdk::CommunicationAdapter> m_communicationAdapter;

};
} // namespace tvqtsdk
