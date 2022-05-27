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

import os
import tvagentapi
from tvagentapi_test.chat import test_chat
from tvagentapi_test.chat import sub_test_chat_send_receive_messages
from tvagentapi_test.chat import sub_test_chat_load_messages
from tvagentapi_test.chat import sub_test_chat_delete_history
from tvagentapi_test.chat import sub_test_chat_delete_chat
from tvagentapi_test.chat import sub_test_chat_error_send_message

# Test cases
test_cases = {
    'test_send_receive': lambda: test_chat(
        behavior=[
            # receive and send first messages to create the chat
            lambda connection: sub_test_chat_send_receive_messages(
                connection,
                expect_chat_room_created=False,
                expected_message_pairs=[("Hello", "Hi there!")]
            ),
            # use first available chat for the rest...
            lambda connection: sub_test_chat_send_receive_messages(
                connection,
                expect_chat_room_created=True,
                expected_message_pairs=[
                    ("Who are you?", "Äugen Öttenburg! 😀"),
                    ("Tell me something.", "Lörem ipsüm dolor sit ämet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.")
                ]
            )
        ]
    ),
    'test_load_messages': lambda: test_chat(
        behavior=[
            lambda connection: sub_test_chat_send_receive_messages(connection, expect_chat_room_created=False, expected_message_pairs=[("Hello", "Hi 😀")]),
            lambda connection: sub_test_chat_load_messages(connection, messages_to_load=2, expected_messages=["Hello", "Hi 😀"])
        ]
    ),
    'test_delete_history': lambda: test_chat(
        behavior=[
            lambda connection: sub_test_chat_send_receive_messages(connection, expect_chat_room_created=False, expected_message_pairs=[("Hello", "Hi 😀")]),
            lambda connection: sub_test_chat_load_messages(connection, messages_to_load=2, expected_messages=["Hello", "Hi 😀"]),
            lambda connection: sub_test_chat_delete_history(connection),
            lambda connection: sub_test_chat_load_messages(connection, messages_to_load=2, expected_messages=[])
        ]
    ),
    'test_delete_chat': lambda: test_chat(
        behavior=[
            lambda connection: sub_test_chat_send_receive_messages(connection, expect_chat_room_created=False, expected_message_pairs=[("Hello", None)]),
            lambda connection: sub_test_chat_delete_chat(connection, number_of_rooms_to_delete=1)
        ]
    ),
    'test_error_cases': lambda: test_chat(
        behavior=[
            lambda connection: sub_test_chat_send_receive_messages(connection, expect_chat_room_created=False, expected_message_pairs=[("Hello", None)]),
            lambda connection: sub_test_chat_error_send_message(connection)
        ]
    )
}

if __name__ == "__main__":
    from tvagentapi_test import run_tests_from_args
    run_tests_from_args(test_cases)
