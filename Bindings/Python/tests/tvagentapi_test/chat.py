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

from datetime import datetime, timedelta
from . import with_connect_urls


def sub_test_chat_send_receive_messages(connection, expected_message_pairs, expect_chat_room_created,
                                        ):
    """
    Chat test helper for sending/receiving messages

    Arguments:
    connection                  -- agent connection to use
    expected_message_pairs      -- a list of message pairs based on the running scenario. First value is the expected message paired with a reply for it
    expect_chat_room_created    -- boolean argument to check for one chat room to be created and used for sending/receiving
    """
    import tvagentapi

    assert isinstance(expected_message_pairs, list), "expected_message_pairs is not a list"
    assert expected_message_pairs, "expected_message_pairs is empty"

    chat = connection.getModule(tvagentapi.ModuleType.Chat)
    assert chat.isSupported()

    expected_chat_id = None
    message_pairs_to_process = expected_message_pairs
    messages_to_send = []

    if expect_chat_room_created:
        chat_rooms = chat.obtainChats()
        assert isinstance(chat_rooms, list), "chat_rooms is not a list"
        assert chat_rooms, "chat_rooms is empty"

        expected_chat_id = chat_rooms[0]['chatID'];

    def handle_received_messages(messages_list):
        for message in messages_list:
            expected_pair = message_pairs_to_process.pop(0)
            expected_content, reply_content = expected_pair

            received_chat_id = message["chatID"]
            received_content = message["content"]

            if expected_chat_id:
                assert expected_chat_id == received_chat_id, "unexpected chat ID loaded: \"%s\", expected \"%s\"" % (received_chat_id, expected_chat_id)

            assert received_content == expected_content, "unexpected message received: \"%s\", expected \"%s\"" % (received_content, expected_content)

            if reply_content:
                request_id = chat.sendMessage(received_chat_id, reply_content);
                messages_to_send.append(request_id)

    def handle_send_message_finished(request_id, send_result, messaged_id, time_stamp):
        messages_to_send.remove(request_id)
        assert send_result == tvagentapi.ChatModule.SendMessageResult.Succeeded, "chat message was not send successfully %s" % send_result

    chat.setCallbacks(
        receivedMessagesCallback=handle_received_messages,
        sendMessageFinishedCallback=handle_send_message_finished)

    while message_pairs_to_process or messages_to_send:
        connection.processEvents(True, 100)

    # reset callbacks
    chat.setCallbacks()

def sub_test_chat_load_messages(connection, expected_messages, messages_to_load):
    """
    Chat test helper for loading messages for the first chat room

    Arguments:
    connection          -- agent connection to use
    expected_messages   -- a list of messages to expect when loading
    messages_to_load    -- number of messages to load
    """
    import tvagentapi

    assert isinstance(expected_messages, list), "expected_messages is not a list"

    chat = connection.getModule(tvagentapi.ModuleType.Chat)
    assert chat.isSupported()

    chat_rooms = chat.obtainChats()
    assert isinstance(chat_rooms, list), "chat_rooms is not a list"
    assert chat_rooms, "chat_rooms is empty"

    expected_chat_id = chat_rooms[0]['chatID']

    messages_loaded = []

    def handle_loaded_messages(messages_list, has_more):
        assert len(messages_list) == 0 or len(messages_loaded) < len(expected_messages), "No more messages expected, but loaded more (%s)" % len(messages_list)

        for message in messages_list:
            messages_loaded.append(message)

        if has_more and len(messages_loaded) < len(expected_messages):
            next_messages_to_load = len(expected_messages) - len(messages_loaded)

            if len(messages_list) > 0:
                last_message_id = messages_list[-1]['messageID']
                chat.loadMessages(
                    expected_chat_id,
                    next_messages_to_load,
                    last_message_id)
            else:
                chat.loadMessages(
                    expected_chat_id,
                    next_messages_to_load)

    chat.setCallbacks(
        messagesLoadedCallback=handle_loaded_messages)

    chat.loadMessages(expected_chat_id, messages_to_load)

    while len(messages_loaded) < len(expected_messages):
        connection.processEvents(True, 100)

    connection.processEvents()

    messages_loaded.sort(key=lambda entry: entry["timeStamp"])

    for message in messages_loaded:
        received_chat_id = message["chatID"]
        received_content = message["content"]

        expected_content = expected_messages.pop(0)

        assert expected_content == received_content, "unexpected message loaded: \"%s\", expected \"%s\"" % (received_chat_id, expected_chat_id)
        assert expected_chat_id == received_chat_id, "unexpected chat ID loaded: \"%s\", expected \"%s\"" % (received_chat_id, expected_chat_id)


    # reset callbacks
    chat.setCallbacks()

def sub_test_chat_delete_history(connection):
    """
    Chat test helper for deleting the history of the first chat

    Arguments:
    connection              -- agent connection to use
    """
    import tvagentapi

    chat = connection.getModule(tvagentapi.ModuleType.Chat)
    assert chat.isSupported()

    chat_rooms = chat.obtainChats()
    assert isinstance(chat_rooms, list), "chat_rooms is not a list"
    assert chat_rooms, "chat_rooms is empty"

    expected_chat_id = chat_rooms[0]['chatID'];

    def handle_loaded_messages(messages_list, has_more):
        assert len(message_list) == 0, "No more messages expected, but loaded more"

    def handle_history_deleted(chat_id):
        assert expected_chat_id == chat_id, "unexpected chat id: \"%s\", expected \"%s\"" % (chat_id, expected_chat_id)

    chat.setCallbacks(
        messagesLoadedCallback=handle_loaded_messages,
        historyDeletedCallback=handle_history_deleted)

    chat.deleteHistory(expected_chat_id)
    chat.loadMessages(expected_chat_id, 1)

    connection.processEvents(True, 1000)

    # reset callbacks
    chat.setCallbacks()

def sub_test_chat_delete_chat(connection, number_of_rooms_to_delete):
    """
    Chat test helper for deleting the first N chat rooms

    Arguments:
    connection                  -- agent connection to use
    number_of_rooms_to_delete   -- number of chat rooms to delete
    """
    import tvagentapi

    chat = connection.getModule(tvagentapi.ModuleType.Chat)
    assert chat.isSupported()

    initial_chat_rooms = chat.obtainChats()
    assert isinstance(initial_chat_rooms, list), "chat_rooms is not a list"
    assert initial_chat_rooms, "chat_rooms is empty"

    rooms_to_delete = []
    for i in range(number_of_rooms_to_delete):
        chat_id = initial_chat_rooms[i]['chatID']
        rooms_to_delete.append(chat_id);

    def handle_chats_removed(chat_ids):
        for chat_id in chat_ids:
            rooms_to_delete.remove(chat_id)

    chat.setCallbacks(
        chatsRemovedCallback=handle_chats_removed)

    for chat_id in rooms_to_delete:
        chat.deleteChat(chat_id)

    while rooms_to_delete:
        connection.processEvents(True, 100)

    remaining_chat_rooms = chat.obtainChats()
    actual_deleted_rooms = len(initial_chat_rooms) - len(remaining_chat_rooms)
    assert actual_deleted_rooms == number_of_rooms_to_delete, "Unexpected number of deleted chat rooms %s, expected %s" % (actual_deleted_rooms, number_of_rooms_to_delete)

    # reset callbacks
    chat.setCallbacks()

def sub_test_chat_error_send_message(connection):
    """
    Chat test helper for checking wrong usage of sendMessage

    Arguments:
    connection  -- agent connection to use
    """
    import tvagentapi

    chat = connection.getModule(tvagentapi.ModuleType.Chat)
    assert chat.isSupported()

    chat_rooms = chat.obtainChats()
    assert isinstance(chat_rooms, list), "chat_rooms is not a list"
    assert chat_rooms, "chat_rooms is empty"

    expected_chat_id = chat_rooms[0]['chatID'];

    try:
        request_id = chat.sendMessage("__INVALID_ID__", "TestMessage")
        assert False, "Tried to send to an invalid chat id. We should never get here."
    except RuntimeError as err:
        pass


@with_connect_urls
def test_chat(behavior, base_sdk_url=None, agent_api_url=None):
    """
    Chat test helper with injectable behavior

    Arguments:
    behavior     -- A list of pairs for the behavior to execute on the running connection with chat module.
    """
    import tvagentapi

    assert isinstance(behavior, list), "behavior is not a list"
    assert behavior, "behavior is empty"

    api = tvagentapi.TVAgentAPI()
    logging = api.createFileLogging("./test.log")
    connection = api.createAgentConnection(logging)
    if base_sdk_url and agent_api_url:
        connection.setConnectionURLs(base_sdk_url, agent_api_url)
    chat = connection.getModule(tvagentapi.ModuleType.Chat)
    assert chat.isSupported()

    connection.start()

    for action in behavior:
        action(connection);

    chat_rooms = chat.obtainChats()
    for chat_room in chat_rooms:
        chat.deleteHistory(chat_room['chatID'])
        chat.deleteChat(chat_room['chatID'])

    connection.stop()
    connection.processEvents()

    del connection

    reference_error = False
    try:
        chat.isSupported()
    except ReferenceError:
        reference_error = True
    assert reference_error, "Reference error expected"
