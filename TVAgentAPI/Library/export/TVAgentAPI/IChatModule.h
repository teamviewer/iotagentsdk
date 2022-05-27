//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2022 TeamViewer Germany GmbH                                     //
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

#include "IModule.h"
#include "Callback.h"

#include <cstdint>
#include <cstdlib>

namespace tvagentapi
{

/**
 * @brief IChatModule provides an interface to the TeamViewer Chat functionality.
 * NOTE:
 * - All const char* can be considered not null and valid during the respective call (function call and callback)
 * - Chat IDs and message IDs are stringified UUIDs ("xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" where 'x' is a hex digit).
 */
class IChatModule : public IModule
{
public:
	// NOTE:
	// - All const char* can be considered not null and valid during the respective call (function call and callback)
	// - Chat IDs and message IDs are stringified UUIDs (xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx where 'x' is a hex digit).

	/// Type value for convenience
	static constexpr Type TypeValue = Type::Chat;

	using RequestID = uint32_t; // internal request identifier to match requests and results
	using TimeStamp = uint64_t; // number of milliseconds since epoch

	static constexpr RequestID InvalidRequestID = 0;

	/// ChatEndpointType describes to what a chat (room) is targetet to.
	enum class ChatEndpointType : int32_t
	{
		/// Chat from/to machines:
		/// corresponding chatEndpointID represents the TeamViewer ID
		Machine = 0,
		/// Chat from/to instant support sessions:
		/// corresponding chatEndpointID represents the session code ID
		InstantSupportSession = 1
	};

	/// ChatState describes variaous states of a chat.
	enum class ChatState : int32_t
	{
		Open = 0,
		Closed = 1
	};

	/// SendMessageResult describes the various results of a request to send a chat message.
	enum class SendMessageResult : int32_t
	{
		Succeeded = 0,
		Failed = 1,
	};

	/// ChatMessage acts as a container for chat messages and their meta information.
	/// NOTE: The lifetime of the members are controlled by the function/callback that provides an instance of this
	/// struct. After the function call all pointers can be considered invalid.
	struct ChatMessage
	{
		const char* chatID;
		const char* messageID;
		const char* content;
		TimeStamp timeStamp;
		const char* sender;
	};

	using ChatCreatedCallback =
		Callback<void(
			const char* chatID,
			const char* title,
			ChatEndpointType chatEndpointType,
			/// additional data per chat type:
			/// - Machine chat has its TeamViewer ID
			/// - Instant Support case has its instant support ID
			uint64_t chatEndpointID,
			void* userData)>;

	using ChatsRemovedCallback =
		Callback<void(
			const char** chatIDs,
			size_t chatIDsCount,
			void* userData)>;

	using ChatClosedCallback =
		Callback<void(
			const char* chatID,
			void* userData)>;

	using ReceivedMessagesCallback =
		Callback<void(
			const ChatMessage* messages,
			size_t messagesCount,
			void* userData)>;

	using SendMessageFinishedCallback =
		Callback<void(
			RequestID requestID,
			SendMessageResult result,
			const char* messageID,
			TimeStamp timeStamp,
			void* userData)>;

	using MessagesLoadedCallback =
		Callback<void(
			const ChatMessage* messages,
			size_t messagesCount,
			bool hasMore,
			void* userData)>;

	using HistoryDeletedCallback =
		Callback<void(
			const char* chatID,
			void* userData)>;

	struct Callbacks
	{
		ChatCreatedCallback chatCreated;
		ChatsRemovedCallback chatsRemoved;
		ChatClosedCallback chatClosed;

		ReceivedMessagesCallback receivedMessages;
		SendMessageFinishedCallback sendMessageFinished;

		MessagesLoadedCallback messagesLoaded;

		HistoryDeletedCallback historyDeleted;
	};

	~IChatModule() override = default;

	/**
	 * @brief Set callbacks to handle various events of this module.
	 * @param callbacks Might be partially or completely omitted, by providing default constructed Callback {}
	 */
	virtual void setCallbacks(const Callbacks& callbacks) = 0;

	/**
	 * @brief Chat enumerator callback.
	 * When the enumerator returns true the iteration continues; on false the iteration stops at this point.
	 */
	using ObtainChatsEnumerator =
		Callback<bool(
			const char* chatID,
			const char* title,
			ChatEndpointType chatEndpointType,
			uint64_t chatEndpointID,
			ChatState chatState,
			void* userData)>;
	/**
	 * @brief obtainChats requests the list of active chats. Should be called initially as soon as the communication
	 * with the TeamViewer IoT Agent bas been established.
	 * @param enumerator given callback to handle each obtained chat.
	 * @return true if this function call is successful, false otherwise
	 */
	virtual bool obtainChats(ObtainChatsEnumerator enumerator) = 0;

	/**
	 * @brief sendMessage requests to send the given content as a message to the given chat.
	 * The returned request ID is referred to in the sendMessageFinished callback.
	 * @param chatID identifier of the chat the message should be sent to
	 * @param content represents the message text to be sent
	 * @return valid request ID on success, InvalidRequestID otherwise
	 */
	virtual RequestID sendMessage(const char* chatID, const char* content) = 0;

	/**
	 * @brief loadMessages requests to load messages from history for the currently selected chat
	 * @param ChatID identifier of the chat messages should be loaded from
	 * @param messageCount maximum number of messages to load
	 * @param messageBefore load messages older than (but not including) message with given message ID
	 * @return true if this function call is successful, false otherwise
	 */
	virtual bool loadMessages(const char* chatID, size_t messageCount, const char* messageBefore = nullptr) = 0;

	/**
	 * @brief deleteHistory requests to delete the selected chat history
	 * @param chatID identifier of the chat to delete the history from
	 * @return true if this function call is successful, false otherwise
	 */
	virtual bool deleteHistory(const char* chatID) = 0;

	/**
	 * @brief deleteChat requests to delete the selected chat
	 * @param chatID identifier of the chat to be deleted
	 * @return true if this function call is successful, false otherwise
	 */
	virtual bool deleteChat(const char* chatID) = 0;
};

} // namespace tvagentapi
