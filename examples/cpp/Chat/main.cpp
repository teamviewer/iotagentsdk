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
#include <TVAgentAPI/tvagentapi.h>

#include <poll.h>
#include <signal.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{

// Chat UI Command handling
enum class ChatCommandType
{
	Help = 0,
	Exit,
	PrintChats,
	PrintMessages,
	LoadMessages,
	SelectChat,
	DeleteChat,
	DeleteHistory,
	LoadChats,
	SendMessage,
	Unknown
};

const std::array<std::string, 8> commandStrings = {
	"/help",
	"/exit",
	"/chats",
	"/messages",
	"/loadMessages",
	"/selectChat",
	"/deleteChat",
	"/deleteHistory",
	// strings starting with '/' are treated like Unknown commands
	// every other string is treatead like SendMessage command
};

const char* help = R"(
Select the currently active chat room to load and send messages to it.

To issue a command enter:
/<command>

To send a message, enter it directly:
<your message here>

To exit enter /exit command or press Ctrl+C or Ctrl+D.

Supported commands:
    /help                         ... print this help.
    /exit                         ... exit this example app.
    /chats                        ... print out summary of all loaded chat rooms.
    /messages [CHAT_ROOM_ID]      ... print out messages loaded for CHAT_ROOM_ID, or currently selected chat room.
    /loadMessages [NUMBER]        ... load additional NUMBER of messages for currently selected chat room. Default: load 5 messages.
    /selectChat CHAT_ROOM_ID      ... select chat room.
    /deleteChat [CHAT_ROOM_ID]    ... delete chat room with CHAT_ROOM_ID, or currently selected chat room.
    /deleteHistory [CHAT_ROOM_ID] ... delete history for chat room with CHAT_ROOM_ID, or currently selected chat room.

)";

struct ChatCommand
{
	ChatCommandType type = ChatCommandType::Unknown;
	// ChatCommandType::SendMessage has message string inside of payload field
	// All the other types have ChatID string
	std::string payload;
};

std::vector<ChatCommand> s_commands = {{ChatCommandType::Help, {}}};

struct ChatRoomMessage
{
	std::string messageID;
	std::string content;
	std::string sender;
	tvagentapi::IChatModule::TimeStamp timeStamp;
	// for placeholder messages that are not yet delivered
	tvagentapi::IChatModule::RequestID sendRequestID = tvagentapi::IChatModule::InvalidRequestID;
	bool isSendFailed = false;
};

struct ChatRoom
{
	std::string chatID;
	std::string title;
	uint64_t chatEndpointID;
	tvagentapi::IChatModule::ChatEndpointType chatEndpointType;
	tvagentapi::IChatModule::ChatState chatState;
	std::vector<ChatRoomMessage> messages;

	void addMessage(ChatRoomMessage message)
	{
		auto it = std::find_if(messages.begin(), messages.end(),
			[&message](const ChatRoomMessage& m)
			{
				return m.messageID == message.messageID;
			});

		if (it == messages.end())
		{
			messages.push_back(std::move(message));
		}
		else
		{
			*it = std::move(message);
		}

		sortMessagesByTimestamp();
	}

	void sortMessagesByTimestamp()
	{
		std::sort(messages.begin(), messages.end(),
			[](const ChatRoomMessage& lhs, const ChatRoomMessage& rhs)
			{
				return lhs.timeStamp < rhs.timeStamp;
			});
	}
};

// chat rooms (the first is selected)
std::vector<ChatRoom> s_chatRooms;

ChatRoom* getSelectedChatRoom()
{
	return s_chatRooms.empty() ? nullptr : &s_chatRooms.front();
}

ChatRoom* findChatRoomWithID(std::vector<ChatRoom>& chatRooms, const std::string& chatID)
{
	for (ChatRoom& chatRoom : chatRooms)
	{
		if (chatRoom.chatID == chatID)
		{
			return &chatRoom;
		}
	}
	return nullptr;
}

bool s_isInterrupted = false;

void signalHandler(int signo)
{
	if (signo == SIGINT)
	{
		s_isInterrupted = true;
	}
}

std::stringstream s_oss;

std::vector<ChatRoom> loadChatRooms(tvagentapi::IChatModule* chatModule)
{
	std::vector<ChatRoom> chatRooms;

	bool requestResult = chatModule->obtainChats(
	{
		[](
			const char* chatID,
			const char* title,
			tvagentapi::IChatModule::ChatEndpointType chatEndpointType,
			uint64_t chatEndpointID,
			tvagentapi::IChatModule::ChatState chatState,
			void* userdata) -> bool
		{
			auto outVec = static_cast<std::vector<ChatRoom>*>(userdata);
			outVec->push_back({
				chatID,
				title,
				chatEndpointID,
				chatEndpointType,
				chatState,
				{}
			});
			return true;
		},
		&chatRooms
	});

	if (!requestResult)
	{
		s_oss << "Error: failed to load chat rooms. Internal error." << std::endl;
	}

	return chatRooms;
}

void printChatRoomSummary(const ChatRoom& chatRoom)
{
	s_oss << "{ chatID: '" << chatRoom.chatID
		<< "', title: '" << chatRoom.title
		<< "', chatEndpointID: '" << chatRoom.chatEndpointID
		<< "', chatEndpointType: '" << toCString(chatRoom.chatEndpointType)
		<< "', chatState: '" << toCString(chatRoom.chatState)
		<< "' }" << std::endl;
}

void printChatMessages(const std::vector<ChatRoomMessage>& messages)
{
	s_oss << messages.size() << " message(s):\n";
	for (const ChatRoomMessage& message : messages)
	{
		const char* prefix = "    ";
		if (message.isSendFailed)
		{
			prefix = "(!) ";
		}
		else if (message.sendRequestID != tvagentapi::IChatModule::InvalidRequestID)
		{
			prefix = "(?) ";
		}

		s_oss << prefix
			<< (message.sender.size() ? message.sender : "Me")
			<< ": " << message.content << "\n";
	}
	s_oss << std::flush;
}

void printChatRooms(const std::vector<ChatRoom>& chatRooms)
{
	const ChatRoom* selected = getSelectedChatRoom();

	s_oss << chatRooms.size() << " chat room(s):" << std::endl;
	for (const auto& chatRoom : chatRooms)
	{
		const char *indent = selected == &chatRoom ? "(selected) " : "           ";
		s_oss << indent;
		printChatRoomSummary(chatRoom);
	}
}

void connectionStatusChanged(tvagentapi::IAgentConnection::Status status, void* /*userdata*/)
{
	s_oss << "[IAgentConnection] Status: " << tvagentapi::toCString(status) << std::endl;

	if (status == tvagentapi::IAgentConnection::Status::Connected)
	{
		// load new chats when (re)connected to IoT Agent
		s_commands.push_back({ChatCommandType::LoadChats, {}});
	}
	else if (status == tvagentapi::IAgentConnection::Status::ConnectionLost ||
		status == tvagentapi::IAgentConnection::Status::Disconnected)
	{
		// IoT Agent went offline together with the chats.
		// Clear chat rooms to keep example app state in sync with the IoT Agent state
		s_chatRooms.clear();
	}
}

void handleChatCreated(
	const char* chatID,
	const char* title,
	tvagentapi::IChatModule::ChatEndpointType chatEndpointType,
	uint64_t chatEndpointID,
	void* /*userData*/)
{
	ChatRoom newRoom
	{
		chatID,
		title,
		chatEndpointID,
		chatEndpointType,
		tvagentapi::IChatModule::ChatState::Open,
		{}
	};

	s_oss << "[IChatModule] Chat room created: ";
	printChatRoomSummary(newRoom);
	s_oss << std::endl;

	s_chatRooms.push_back(std::move(newRoom));
}

void handleChatsRemoved(
	const char** chatIDs,
	size_t chatIDsCount,
	void* /*userData*/)
{
	s_oss << "[IChatModule] " << chatIDsCount << " chat room(s) removed: " << std::endl;

	s_chatRooms.erase(std::remove_if(s_chatRooms.begin(), s_chatRooms.end(),
		[chatIDs, chatIDsCount](const ChatRoom& chatRoom)
		{
			for (size_t i = 0; i < chatIDsCount; ++i)
			{
				if (chatRoom.chatID == chatIDs[i])
				{
					s_oss << "\t";
					printChatRoomSummary(chatRoom);
					return true;
				}
			}
			return false;
		}), s_chatRooms.end());
}

void handleChatClosed(
	const char* chatID,
	void* /*userData*/)
{
	s_oss << "[IChatModule] chat room closed (" << chatID << ")" << std::endl;

	ChatRoom* chatRoom = findChatRoomWithID(s_chatRooms, chatID);
	if (chatRoom)
	{
		chatRoom->chatState = tvagentapi::IChatModule::ChatState::Closed;
	}
}

void handleNewMessages(
	const tvagentapi::IChatModule::ChatMessage* messages,
	size_t messagesCount,
	void* /*userData*/)
{
	const ChatRoom* selected = getSelectedChatRoom();

	s_oss << "[IChatModule] ";
	for (size_t i = 0; i < messagesCount; ++i)
	{
		auto message = messages[i];

		if (selected && selected->chatID == message.chatID)
		{
			s_oss << "(New message in selected chat) " << std::endl;
		}
		else
		{
			s_oss << "(New message in chat room id " << message.chatID << ") " << std::endl;
		}
		s_oss << message.sender << ": " << message.content << std::endl;

		ChatRoom* chatRoom = findChatRoomWithID(s_chatRooms, message.chatID);
		if (chatRoom)
		{
			chatRoom->addMessage({message.messageID, message.content, message.sender, message.timeStamp});
		}
	}
}

void handleMessageSendFinished(
	tvagentapi::IChatModule::RequestID requestID,
	tvagentapi::IChatModule::SendMessageResult result,
	const char* messageID,
	tvagentapi::IChatModule::TimeStamp timeStamp,
	void* /*userData*/)
{
	switch (result)
	{
		case tvagentapi::IChatModule::SendMessageResult::Succeeded:
			s_oss << "[IChatModule] Message sent (requestID: '" << requestID << "', messageID: '" << messageID << "', timeStamp: " << timeStamp << ")" << std::endl;
			break;
		case tvagentapi::IChatModule::SendMessageResult::Failed:
			s_oss << "[IChatModule] Error: Failed to send message (requestID: " << requestID << ")" << std::endl;
			break;
	}

	for (ChatRoom& chatRoom : s_chatRooms)
	{
		// try to patch placeholder message inside of the ChatRoom
		for (ChatRoomMessage& message : chatRoom.messages)
		{
			if (message.sendRequestID == requestID)
			{
				message.messageID = messageID;
				message.timeStamp = timeStamp;

				// on failure, mark as failed to be delivered so user can retry later (this example app doesn't handle retry logic)
				message.sendRequestID = tvagentapi::IChatModule::InvalidRequestID;
				message.isSendFailed = result == tvagentapi::IChatModule::SendMessageResult::Failed;

				chatRoom.sortMessagesByTimestamp();

				s_oss << "Patched message with send requestID " << requestID << std::endl;
				return;
			}
		}
	}
}

void handleMessagesLoaded(
	const tvagentapi::IChatModule::ChatMessage* messages,
	size_t messagesCount,
	bool hasMore,
	void* /*userData*/)
{
	s_oss << "[IChatModule] loaded " << messagesCount << " message(s). "
		<< (hasMore ? "Can load more." : "Nothing else to load.") << std::endl;

	for (size_t i = 0; i < messagesCount; ++i)
	{
		auto message = messages[i];
		ChatRoom* chatRoom = findChatRoomWithID(s_chatRooms, message.chatID);
		if (chatRoom)
		{
			chatRoom->addMessage({message.messageID, message.content, message.sender, message.timeStamp});
		}
		else
		{
			s_oss << "Error: no chat room with id" << message.chatID << std::endl;
		}
	}
}

void handleHistoryDeleted(
	const char* chatID,
	void* /*userData*/)
{
	s_oss << "[IChatModule] history deleted for chat id " << chatID << std::endl;

	ChatRoom* chatRoom = findChatRoomWithID(s_chatRooms, chatID);
	if (chatRoom)
	{
		chatRoom->messages.clear();
	}
}

ChatCommand parseCommand(const std::string& input)
{
	for (size_t i = 0; i < commandStrings.size(); ++i)
	{
		const std::string& commandString = commandStrings[i];
		if (!input.compare(0, commandString.size(), commandString))
		{
			std::string argument;
			if (input.size() > commandString.size())
			{
				if (input[commandString.size()] != ' ')
				{
					continue; // not a whitespace after /command
				}
				argument = input.substr(commandString.size() + 1);
			}
			return ChatCommand{static_cast<ChatCommandType>(i), argument};
		}
	}

	if (input.empty() || input[0] == '/')
	{
		return {ChatCommandType::Unknown, {}};
	}

	return {ChatCommandType::SendMessage, input};
}

} // namespace

int main()
{
	if (signal(SIGINT, signalHandler) == SIG_ERR)
	{
		std::cerr << "Failed to set up signal handler" << std::endl;
		return EXIT_FAILURE;
	}

	tvagentapi::IAgentAPI* agentAPI = TVGetAgentAPI();
	if (!agentAPI)
	{
		std::cerr << "Failed to create IAgentAPI" << std::endl;
		return EXIT_FAILURE;
	}

	// create file logging
	tvagentapi::ILogging* logging = agentAPI->createFileLogging("example.log");
	if (!logging)
	{
		std::cerr << "Failed to start file logging" << std::endl;
		return EXIT_FAILURE;
	}

	// connect tvagentapi sdk to IoT Agent
	// We pass logging to AgentConnection, but we still manage its lifetime and must pair it with IAgentAPI::destroyLogging()
	// logging object will be used internally and we may only release it (in this case by calling IAgentAPI::destroyLogging()
	// after call to agentAPI->destroyAgentConnection();
	tvagentapi::IAgentConnection* agentConnection = agentAPI->createAgentConnectionLocal(logging);
	if (!agentConnection)
	{
		std::cerr << "Failed to create connection" << std::endl;
		return EXIT_FAILURE;
	}

	tvagentapi::IChatModule* chatModule =
		tvagentapi::getModule<tvagentapi::IChatModule>(agentConnection);
	if (!chatModule)
	{
		std::cerr << "Failed to create ChatModule" << std::endl;
		return EXIT_FAILURE;
	}
	if (!chatModule->isSupported())
	{
		std::cerr << "ChatModule not supported" << std::endl;
		return EXIT_FAILURE;
	}

	chatModule->setCallbacks({
		{handleChatCreated, nullptr},
		{handleChatsRemoved, nullptr},
		{handleChatClosed, nullptr},
		{handleNewMessages, nullptr},
		{handleMessageSendFinished, nullptr},
		{handleMessagesLoaded, nullptr},
		{handleHistoryDeleted, nullptr}
	});

	agentConnection->setStatusChangedCallback({connectionStatusChanged, nullptr});

	// start connection to IoT Agent
	std::cout << "Connecting to IoT Agent..." << std::endl;
	agentConnection->start();

	std::cout << "Simple chat example." << std::endl;

	// main application's event loop
	while (!s_isInterrupted)
	{
		// all the tvagentapi callbacks will be called on the thread calling processEvents()
		agentConnection->processEvents();

		// commands to process
		for (const ChatCommand& command : s_commands)
		{
			ChatRoom* selected = getSelectedChatRoom();

			switch (command.type)
			{
				case ChatCommandType::Unknown:
					s_oss << "Unknown command.\n";
					// fallthrough
				case ChatCommandType::Help:
					s_oss << help << std::flush;
					break;
				case ChatCommandType::Exit:
					s_isInterrupted = true;
					break;
				case ChatCommandType::PrintChats:
					printChatRooms(s_chatRooms);
					break;
				case ChatCommandType::PrintMessages:
					{
						const ChatRoom* chatRoom = command.payload.size()
							? findChatRoomWithID(s_chatRooms, command.payload)
							: selected;

						if (chatRoom)
						{
							printChatMessages(chatRoom->messages);
						}
						else if (command.payload.size())
						{
							s_oss << "Unknown chat id " << command.payload << std::endl;
						}
						else
						{
							s_oss << "No chats available." << std::endl;
						}
						break;
					}
				case ChatCommandType::LoadChats:
					s_oss << "Loading chats..." << std::endl;
					s_chatRooms = loadChatRooms(chatModule);
					printChatRooms(s_chatRooms);
					break;
				case ChatCommandType::LoadMessages:
					if (selected)
					{
						size_t messageCount = 5;
						if (command.payload.size())
						{
							try
							{
								messageCount = std::stoull(command.payload);
							}
							catch (...)
							{
								s_oss << "Error: argument should be a valid number" << std::endl;
								break;
							}
						}

						// find earliest real message id (messages are sorted by ascending time stamp)
						const char* realMessageID = nullptr;
						for (const ChatRoomMessage& message : selected->messages)
						{
							if (message.sendRequestID == tvagentapi::IChatModule::InvalidRequestID)
							{
								realMessageID = message.messageID.c_str();
								break;
							}
						}

						if (chatModule->loadMessages(
							selected->chatID.c_str(),
							messageCount,
							realMessageID))
						{
							s_oss << "Loading additional " << messageCount << " message(s) for chat id " << selected->chatID << "..." << std::endl;
						}
						else
						{
							s_oss << "Error: failed to load messages. Internal error." << std::endl;
						}
					}
					else
					{
						s_oss << "Failed to load messages. No chats available." << std::endl;
					}
					break;
				case ChatCommandType::SelectChat:
					{
						ChatRoom* chatRoom = findChatRoomWithID(s_chatRooms, command.payload);
						if (!chatRoom)
						{
							s_oss << "Error: unknown chat id " << command.payload << std::endl;
							break;
						}

						// move selected chat room at the beginning of a vector
						std::swap(s_chatRooms[0], *chatRoom);

						s_oss << "Selected chat id " << command.payload << std::endl;
						printChatRooms(s_chatRooms);
					}
					break;
				case ChatCommandType::DeleteChat:
					if (selected)
					{
						const char* chatID = command.payload.size() ? command.payload.c_str() : selected->chatID.c_str();
						if (chatModule->deleteChat(chatID))
						{
							s_oss << "Delete chat with id "<< chatID << std::endl;
						}
						else
						{
							s_oss << "Error: failed to delete chat with id " << chatID << ". Internal error." << std::endl;
						}
					}
					else
					{
						s_oss << "Failed to delete chat. No chats available." << std::endl;
					}
					break;
				case ChatCommandType::DeleteHistory:
					if (selected)
					{
						const char* chatID = command.payload.size() ? command.payload.c_str() : selected->chatID.c_str();
						if (chatModule->deleteHistory(chatID))
						{
							s_oss << "Delete history for chat id "<< chatID << std::endl;
						}
						else
						{
							s_oss << "Error: Failed to delete history for chat id " << chatID << ". Internal error." << std::endl;
						}
					}
					else
					{
						s_oss << "Failed to delete history. No chats available." << std::endl;
					}
					break;
				case ChatCommandType::SendMessage:
					if (selected)
					{
						tvagentapi::IChatModule::RequestID requestID =
							chatModule->sendMessage(selected->chatID.c_str(), command.payload.c_str());
						if (requestID != tvagentapi::IChatModule::InvalidRequestID)
						{
							// add placeholder message to chat
							using namespace std::chrono;

							const uint64_t placeholderTimeStamp = duration_cast<milliseconds>(
								system_clock::now().time_since_epoch()).count();

							selected->addMessage({std::to_string(requestID), command.payload, {}, placeholderTimeStamp, requestID});
							s_oss << "Sending message..." << std::endl;
						}
						else
						{
							s_oss << "Error: Failed to send message. Internal error." << std::endl;
						}
					}
					else
					{
						s_oss << "Failed to send message. No chats available." << std::endl;
					}
					break;
			}
		}

		s_commands.clear();

		// print callbacks and commands output to stdout
		if (s_oss.rdbuf()->in_avail())
		{
			std::cout << s_oss.rdbuf();
			std::cout << "\nYour message: " << std::flush; // print input line
		}

		// Non-blocking read of stdin
		// poll for 100ms to see if user has some input in stdin (it'll be there the moment user presses enter)
		struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
		struct pollfd pfds[1] = {pfd};

		if (poll(pfds, 1, 100) > 0)
		{
			std::string input;
			std::getline(std::cin, input);

			if (std::cin.eof())
			{
				s_isInterrupted = true;
			}
			else
			{
				// parse user input and issue a command
				s_commands.push_back(parseCommand(input));
			}
		}
	}

	// stop connection to IoT Agent
	std::cout << "Stopping connection to IoT Agent..." << std::endl;
	agentConnection->stop();

	// we call processEvents() one more time to receive callbacks emitted by stop()
	agentConnection->processEvents();

	std::cout << "Cleaning up..." << std::endl;
	agentAPI->destroyAgentConnection(agentConnection);

	// after destroyAgentConnection() we are sure logging is not used and we can safely destroy it
	agentAPI->destroyLogging(logging);

	std::cout << "Exiting..." << std::endl;

	return EXIT_SUCCESS;
}
