#!/usr/bin/env python3

__author__ = "TeamViewer Germany GmbH"
__copyright__ = """
Copyright (c) 2022 TeamViewer Germany GmbH

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
"""
__license__ = "MIT License"

import select
import sys
import time

import tvagentapi

help = """
Select the currently active chat room to load and send messages to it.

To issue a command enter:
/<command>

To send a message, enter it directly:
<your message here>

To exit enter /exit command or press Ctrl+C.

Supported commands:
    /help                         ... print this help.
    /exit                         ... exit this example app.
    /chats                        ... print out summary of all loaded chat rooms.
    /messages [CHAT_ROOM_ID]      ... print out messages loaded for CHAT_ROOM_ID, or currently selected chat room.
    /loadMessages [NUMBER]        ... load additional NUMBER of messages for currently selected chat room. Default: load 5 messages.
    /selectChat CHAT_ROOM_ID      ... select chat room.
    /deleteChat [CHAT_ROOM_ID]    ... delete chat room with CHAT_ROOM_ID, or currently selected chat room.
    /deleteHistory [CHAT_ROOM_ID] ... delete history for chat room with CHAT_ROOM_ID, or currently selected chat room.

"""

# commands
COMMAND_TYPE_HELP           = '/help'
COMMAND_TYPE_EXIT           = '/exit'
COMMAND_TYPE_CHATS          = '/chats'
COMMAND_TYPE_MESSAGES       = '/messages'
COMMAND_TYPE_LOAD_CHATS     = '/loadChats'
COMMAND_TYPE_LOAD_MESSAGES  = '/loadMessages'
COMMAND_TYPE_SELECT_CHAT    = '/selectChat'
COMMAND_TYPE_DELETE_CHAT    = '/deleteChat'
COMMAND_TYPE_DELETE_HISTORY = '/deleteHistory'
COMMAND_TYPE_SEND_MESSAGE   = 'non-parsable-send'
COMMAND_TYPE_UNKNOWN        = 'non-parsable-unknown'

PARSABLE_COMMANDS=[
    COMMAND_TYPE_HELP,
    COMMAND_TYPE_EXIT,
    COMMAND_TYPE_CHATS,
    COMMAND_TYPE_MESSAGES,
    COMMAND_TYPE_LOAD_MESSAGES,
    COMMAND_TYPE_SELECT_CHAT,
    COMMAND_TYPE_DELETE_CHAT,
    COMMAND_TYPE_DELETE_HISTORY,
    # other strings starting with '/' are treated as COMMAND_TYPE_UNKNOWN
    # every other string is treatead as COMMAND_TYPE_SEND_MESSAGE
]

# Command is a tuple (command_type, payload)
commands = [(COMMAND_TYPE_HELP, None)]


def parse_command(input):
    for command in PARSABLE_COMMANDS:
        if input.startswith(command):
            # everything after space character is a payload string
            split = input.split(" ", 1)
            payload = split[-1] if len(split) > 1 else None
            return (command, payload)

    if not input or input[0] == '/':
        return (COMMAND_TYPE_UNKNOWN, None)

    return (COMMAND_TYPE_SEND_MESSAGE, input)


class ChatRoomMessage():
    def __init__(self, message_id, content, sender, time_stamp, send_request_id=None, is_send_failed=False):
        self.id = message_id
        self.content = content
        self.sender = sender
        self.time_stamp = time_stamp
        self.send_request_id = send_request_id
        self.is_send_failed = is_send_failed


class ChatRoom():
    def __init__(self, chat_id, title, chat_endpoint_id, chat_endpoint_type, chat_state, messages=None):
        self.id = chat_id
        self.title = title
        self.endpoint_id = chat_endpoint_id
        self.endpoint_type = chat_endpoint_type
        self.state = chat_state
        self.messages = messages or [] # [ChatRoomMessages]

    def add_message(self, message):
        found = next((m for m in self.messages if m.id == message.id), None)

        if found:
            self.messages[self.messages.index(found)] = message
        else:
            self.messages.append(message)

        self.sort_messages_by_timestamp()

    def sort_messages_by_timestamp(self):
        self.messages.sort(key=lambda m: m.time_stamp)


# [ChatRoom] the first is selected
chat_rooms = []


def selected_chat_room(rooms):
    return (rooms or [None])[0]


def find_chat_room_with_id(rooms, chat_id):
    return next((r for r in rooms if r.id == chat_id), None)


def print_chat_room_summary(chat_room):
    print(f"{chat_room.id}, {chat_room.title}, {chat_room.endpoint_id}, {chat_room.endpoint_type}, {chat_room.state}")


def print_chat_rooms(rooms):
    print(f"{len(rooms)} chat room(s):")

    selected = selected_chat_room(rooms)
    for chat_room in rooms:
        prefix = '(selected) ' if chat_room == selected else '           '
        print(prefix, end='')
        print_chat_room_summary(chat_room)


def print_chat_room_messages(chat_room):
    print(f"{len(chat_room.messages)} message(s):")

    for message in chat_room.messages:
        prefix = '    '

        if message.is_send_failed:
            # send failed
            prefix = '(!) '
        elif message.send_request_id:
            # sending
            prefix = '(?) '

        print(f"{prefix}{message.sender or 'Me'}: {message.content}")


# callbacks
def connection_status_chaged(status):
    print(f"[IAgentConnection] Status: {status}")

    if status == tvagentapi.AgentConnection.Status.Connected:
        commands.append((COMMAND_TYPE_LOAD_CHATS, None))
    elif status in (tvagentapi.AgentConnection.Status.ConnectionLost,
                    tvagentapi.AgentConnection.Status.Disconnected):
        chat_rooms.clear()


def chat_created(chat_id, title, endpoint_type, endpoint_id):
    print(f"[IChatModule] Chat room created: {chat_id}, {title}, {endpoint_type}, {endpoint_id}")

    chat_rooms.append(ChatRoom(
        chat_id,
        title,
        endpoint_type,
        endpoint_id,
        tvagentapi.ChatModule.ChatState.Open))

    print_chat_rooms(chat_rooms)


def chats_removed(chat_ids):
    print(f"[IChatModule] chat room(s) removed: {chat_ids}")

    global chat_rooms
    chat_rooms = [r for r in chat_rooms if r.id not in chat_ids]

    print_chat_rooms(chat_rooms)


def chat_closed(chat_id):
    print(f"[IChatModule] chat room closed {chat_id}")

    chat_room = find_chat_room_with_id(chat_rooms, chat_id)

    if chat_room:
        chat_room.chat_state = tvagentapi.ChatModule.ChatState.Closed


def received_messages(messages):
    print(f"[IChatModule] received {len(messages)} message(s):")

    selected = selected_chat_room(chat_rooms)

    for message in messages:
        if selected and selected.id == message['chatID']:
            print("New message in selected chat room:")
        else:
            print(f"New message in chat room {message['chatID']}:")

        print(f"{message['sender']}: {message['content']}")

        chat_room = find_chat_room_with_id(chat_rooms, message['chatID'])
        if chat_room:
            chat_room.add_message(
                ChatRoomMessage(
                    message['messageID'],
                    message['content'],
                    message['sender'],
                    message['timeStamp']))


def send_message_finished(request_id, send_result, message_id, time_stamp):
    if send_result:
        print(f"[IChatModule] message sent (request_id: {request_id}, message_id: {message_id}, time stamp: {time_stamp}")
    else:
        print(f"[IChatModule] failed to send message (request_id: {request_id})")

    for chat_room in chat_rooms:
        for message in chat_room.messages:
            if message.send_request_id == request_id:
                message.id = message_id
                message.time_stamp = time_stamp

                # on failure, mark as failed to be delivered so user can retry later
                # (this example app doesn't handle retry logic)
                message.send_request_id = None
                message.is_send_failed = not send_result

                chat_room.sort_messages_by_timestamp()
                print(f"Patched message with send requestID {request_id}")
                return


def messages_loaded(messages, has_more):
    print(f"[IChatModule] loaded messages {messages}. Can load more: {has_more}")

    for message in messages:
        chat_room = find_chat_room_with_id(chat_rooms, message['chatID'])
        if chat_room:
            chat_room.add_message(
                ChatRoomMessage(
                    message['messageID'],
                    message['content'],
                    message['sender'],
                    message['timeStamp']))


def history_deleted(chat_id):
    print(f"[IChatModule] history deleted: {chat_id}")

    chat_room = find_chat_room_with_id(chat_rooms, chat_id)
    if chat_room:
        chat_room.messages = []


api = tvagentapi.TVAgentAPI()
connection = api.createAgentConnectionLocal()
connection.setCallbacks(statusChanged=connection_status_chaged)

chat = connection.getModule(tvagentapi.ModuleType.Chat)
assert chat.isSupported(), "Chat Module not supported"


chat.setCallbacks(
    chatCreatedCallback=chat_created,
    chatsRemovedCallback=chats_removed,
    chatClosedCallback=chat_closed,
    receivedMessagesCallback=received_messages,
    sendMessageFinishedCallback=send_message_finished,
    messagesLoadedCallback=messages_loaded,
    historyDeletedCallback=history_deleted)


print("Connecting to IoT Agent...")
connection.start()

print("Simple chat example.")
try:
    while True:
        connection.processEvents(waitForMoreEvents=True, timeoutMs=100)

        selected = selected_chat_room(chat_rooms)

        for command in commands:
            command_type, payload = command

            if command_type == COMMAND_TYPE_UNKNOWN:
                print("Unknown command")
                print(help)
            elif command_type == COMMAND_TYPE_HELP:
                print(help)
            elif command_type == COMMAND_TYPE_EXIT:
                raise KeyboardInterrupt
            elif command_type == COMMAND_TYPE_CHATS:
                print_chat_rooms(chat_rooms)
            elif command_type == COMMAND_TYPE_MESSAGES:
                chat_room = find_chat_room_with_id(chat_rooms, payload) if payload else selected

                if chat_room:
                    print_chat_room_messages(chat_room)
                elif payload:
                    print(f"Unknown chat id {payload}")
                else:
                    print("No chats available.")
            elif command_type == COMMAND_TYPE_LOAD_CHATS:
                print("Loading chats...")
                chats = chat.obtainChats()

                chat_rooms = []
                for c in chats:
                    chat_rooms.append(
                        ChatRoom(
                            c['chatID'],
                            c['title'],
                            c['chatEndpointID'],
                            c['chatEndpointType'],
                            c['chatState'],
                            []))

                print_chat_rooms(chat_rooms)
            elif command_type == COMMAND_TYPE_LOAD_MESSAGES:
                if selected:
                    count = 5
                    try:
                        count = int(payload)
                    except:
                        pass

                    # find earliest real message id (messages are sorted by ascending time stamp)
                    real_message = next((m for m in selected.messages if not m.send_request_id), None)

                    chat.loadMessages(selected.id, count, real_message.id if real_message else None)
                    print(f"Loading additional {count} message(s) for chat id {selected.id}")
                else:
                    print("Failed to load messages. No chats available.")
            elif command_type == COMMAND_TYPE_SELECT_CHAT:
                chat_room = find_chat_room_with_id(chat_rooms, payload)
                if chat_room:
                    # move chat room at the beginning of a chat room list
                    chat_rooms.insert(0, chat_rooms.pop(chat_rooms.index(chat_room)))
                    print_chat_rooms(chat_rooms)
                else:
                    print(f"Unknown chat id {payload}")
            elif command_type == COMMAND_TYPE_DELETE_CHAT:
                if selected:
                    chat_id = payload if payload else selected.id
                    try:
                        chat.deleteChat(chat_id)
                        print(f"Deleted chat with id {chat_id}")
                    except RuntimeError:
                        print(f"Error: failed to delete chat with id {chat_id}")
                else:
                    print("Failed to delete chat. No chats available.")
            elif command_type == COMMAND_TYPE_DELETE_HISTORY:
                if selected:
                    chat.deleteHistory(payload if payload else selected.id)
                else:
                    print("Failed to delete history. No chats available.")
            elif command_type == COMMAND_TYPE_SEND_MESSAGE:
                if selected:
                    request_id = chat.sendMessage(selected.id, payload)

                    # add placeholder message to selected chat room
                    time_stamp = time.time_ns() // 1_000_000
                    selected.add_message(ChatRoomMessage(str(request_id), payload, "", time_stamp, request_id))

                    print("Sending message...")
                else:
                    print("Failed to send message. No chats available.")

        commands.clear()

        # Non-blocking read of stdin
        # poll for 100ms to see if user has some input in stdin (it'll be there the moment user presses enter)
        ins, _, _ = select.select([sys.stdin.fileno()], [], [], 0.1)

        if len(ins):
            for input in sys.stdin.readline().splitlines():
                commands.append(parse_command(input))
except KeyboardInterrupt:
    pass

print("Stopping connection to IoT Agent...")

connection.stop()

connection.processEvents()

print("Cleaning up...")

print("Exiting...")
