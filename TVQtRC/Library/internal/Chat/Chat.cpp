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
#include "Chat.h"

namespace tvqtsdk
{
Chat::Chat(const std::shared_ptr<tvqtsdk::CommunicationAdapter>& adapter)
	: m_communicationAdapter(adapter)
{
	if (m_communicationAdapter)
	{
		QObject::connect(
			m_communicationAdapter.get(),
			&CommunicationAdapter::chatCreated,
			this,
			&AbstractChat::chatCreated,
			Qt::QueuedConnection);

		QObject::connect(
			m_communicationAdapter.get(),
			&CommunicationAdapter::chatsRemoved,
			this,
			&AbstractChat::chatsRemoved,
			Qt::QueuedConnection);

		QObject::connect(
			m_communicationAdapter.get(),
			&CommunicationAdapter::receivedMessages,
			this,
			&AbstractChat::receivedMessages,
			Qt::QueuedConnection);

		QObject::connect(
			m_communicationAdapter.get(),
			&CommunicationAdapter::messageSent,
			this,
			&AbstractChat::messageSent,
			Qt::QueuedConnection);

		QObject::connect(
			m_communicationAdapter.get(),
			&CommunicationAdapter::messageNotSent,
			this,
			&AbstractChat::messageNotSent,
			Qt::QueuedConnection);

		QObject::connect(
			m_communicationAdapter.get(),
			&CommunicationAdapter::loadedMessages,
			this,
			&AbstractChat::loadedMessages,
			Qt::QueuedConnection);

		QObject::connect(
			m_communicationAdapter.get(),
			&CommunicationAdapter::deletedHistory,
			this,
			&AbstractChat::deletedHistory,
			Qt::QueuedConnection);

		QObject::connect(
			m_communicationAdapter.get(),
			&CommunicationAdapter::closedChat,
			this,
			&AbstractChat::closedChat,
			Qt::QueuedConnection);
	}
}

bool Chat::obtainChats(QVector<ChatInfo>& chats)
{
	return m_communicationAdapter->sendObtainChatsRequest(chats);
}

bool Chat::selectChat(QUuid chatId)
{
	return m_communicationAdapter->sendSelectChatResult(chatId);
}

bool Chat::sendMessage(uint32_t localId, QString content)
{
	return m_communicationAdapter->sendMessage(localId, content);
}

bool Chat::loadMessages(uint32_t messageCount, QUuid lastMessageId)
{
	return m_communicationAdapter->loadMessages(messageCount, lastMessageId);
}

bool Chat::deleteHistory()
{
	return m_communicationAdapter->deleteHistory();
}

bool Chat::deleteChat()
{
	return m_communicationAdapter->deleteChat();
}

} // namespace tvqtsdk
