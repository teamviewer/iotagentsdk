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

#include "ChatInfo.h"

#include <functional>

#include <QtCore/QObject>

namespace tvqtsdk
{

class AbstractChat : public QObject
{
	Q_OBJECT
public:
	explicit AbstractChat(QObject* parent = nullptr)
		: QObject(parent) {}

	~AbstractChat() override = default;

	/**
	 * @brief obtainChats requests the list of active chats. Should be called when the communication between the agent and the plugin is established
	 * @param chats chat information of active chats filled by this function
	 * @return true if this function call is successfull, false otherwise
	 */
	virtual bool obtainChats(QVector<ChatInfo>& chats) = 0;

	/**
	 * @brief selectChat selects chat for communication
	 * @param chatId chat id of the selected chat
	 * @return true if this function call is successfull, false otherwise
	 */
	virtual bool selectChat(QUuid chatId) = 0;

	/**
	 * @brief sendMessage sends message to the partner
	 * @param localId locally generated Id for the message
	 * @param content message text to be sent to the partner
	 * @return true if this function call is successfull, false otherwise
	 */
	virtual bool sendMessage(uint32_t localId, QString content) = 0;

	/**
	 * @brief LoadMessages send request to load messages from history
	 * @param messageCount Maximum number of messages to load
	 * @param lastMessageId load messages older then message with this ID
	 * @return true if this function call is successfull, false otherwise
	 */
	virtual bool loadMessages(uint32_t messageCount, QUuid lastMessageId = QUuid() ) = 0;

	/**
	 * @brief deleteHistory send request to delete selected chat history
	 * @return true if this function call is successfull, false otherwise
	 */
	virtual bool deleteHistory() = 0;

	/**
	 * @brief deleteChat send request to delete selected chat
	 * @return true if this function call is successfull, false otherwise
	 */
	virtual bool deleteChat() = 0;

Q_SIGNALS:

	/**
	 * @brief chatCreated signal to connect with the user defined slot to handle creation of a chat
	 * @param chatId chat id of the created chat
	 * @param title title of the created chat
	 * @param chatType chat type of the created chat
	 * @param chatTypeId chat type id of the created chat
	 */
	void chatCreated(QUuid chatId, QString title, ChatType chatType, uint32_t chatTypeId);

	/**
	 * @brief chatsRemoved signal to connect with the user defined slot to handle chats removal
	 * @param chatIds container of removed chat ids
	 */
	void chatsRemoved(QVector<QUuid> chatIds);

	/**
	 * @brief receivedMessages signal to connect with the user defined slot to handle received messages
	 * @param messages container of received messages
	 */
	void receivedMessages(QVector<ReceivedMessage> messages);

	/**
	 * @brief messageSent signal to connect with the user defined slot to handle sent message
	 * @param localId local id of the sent message
	 * @param messageId message id of the sent message
	 * @param timeStamp timestamp of the sent message
	 */
	void messageSent(uint32_t localId, QUuid messageId, QDateTime timeStamp);

	/**
	 * @brief messageNotSent signal to connect with the user defined slot to handle not sent message
	 * @param localId local id of the not sent message
	 */
	void messageNotSent(uint32_t localId);

	/**
	 * @brief loadedMessages signal to connect with the user defined slot to handle loaded messages
	 * @param messages container of loaded messages
	 * @param hasMore has more messages to be loaded
	 */
	void loadedMessages(QVector<ReceivedMessage> messages, bool hasMore);

	/**
	 * @brief deletedHistory signal to connect with the user defined slot to handle chat history deletion
	 * @param chatId chat Id of the deleted chat history
	 */
	void deletedHistory(QUuid chatId);

	/**
	 * @brief loadedMessages signal to connect with the user defined slot to handle chat deletion
	 * @param chatId chat Id of the deleted chat
	 */
	void closedChat(QUuid chatid);
};

} // namespace tvqtsdk
