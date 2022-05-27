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

assert 'TV_ACCESS_TOKEN' in os.environ, "Specify TV_ACCESS_TOKEN environment variable"


def connectionStatusChanged(status, is_module):
    print(f"[IAgentConnection] Status: {status}")
    if status == tvagentapi.AgentConnection.Status.Connected:
        request_data = {
            'accessToken': os.environ['TV_ACCESS_TOKEN'],
            'name': "Help Me Please!",
            'group': "Contacts",
            'description': "I need your support!",
            'email': "supporter@teamviewer.com",
            'sessionCode': os.environ['TV_SESSION_CODE'] if 'TV_SESSION_CODE' in os.environ else ""
        }
        print("\tRequesting instant support... " + str(request_data))
        is_module.requestInstantSupport(request_data)


def instantSupportSessionDataChanged(new_data):
    if new_data['state'] == tvagentapi.InstantSupportModule.SessionState.Undefined:
        output = "no session data"
    else:
        output = str(new_data)
    print(f"[IInstantSupportModule] instantSupportSessionDataChanged: {output}")


def instantSupportRequestError(error_code):
    print(f"[IInstantSupportModule] instantSupportRequestError: {error_code}")


def instantSupportConnectionRequested(is_module):
    c = input("""
Incoming Instant Support request. Supporter wants to connect to your machine.
Available options:
 'a' - Accept connection request
 'r' - Reject connection request
 't' - Timeout connection request
Pick an option (a/r/T): """)

    if c in ('a', 'A'):
        is_module.acceptConnectionRequest()
        print("Instant support request accepted")
    elif c in ('r', 'R'):
        is_module.rejectConnectionRequest()
        print("Instant support request rejected")
    else:
        is_module.timeoutConnectionRequest()
        print("Instant support request timed out")


api = tvagentapi.TVAgentAPI()
connection = api.createAgentConnectionLocal()

instant_support_module = connection.getModule(tvagentapi.ModuleType.InstantSupport)
assert instant_support_module.isSupported(), "Instant Support Module not supported"

connection.setCallbacks(statusChanged=lambda status: connectionStatusChanged(status, instant_support_module))
instant_support_module.setCallbacks(
    sessionDataChangedCallback=instantSupportSessionDataChanged,
    requestErrorCallback=instantSupportRequestError,
    connectionRequestCallback=lambda: instantSupportConnectionRequested(instant_support_module))

print("Connecting to IoT Agent...")
connection.start()

print("Press Ctrl+C to exit")
while True:
    try:
        connection.processEvents(waitForMoreEvents=True, timeoutMs=100)
    except KeyboardInterrupt:
        break

print("Stopping connection to IoT Agent...")

connection.stop()

connection.processEvents()

print("Exiting...")
