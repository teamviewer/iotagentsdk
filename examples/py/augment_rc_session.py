#!/usr/bin/env python3

__author__ = "TeamViewer Germany GmbH"
__copyright__ = """
Copyright (c) 2024 TeamViewer Germany GmbH

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

from argparse import ArgumentParser
import os
import tvagentapi

def connection_status_changed(status):
	print(f"[IAgentConnection] Status: {status}")
	if status == tvagentapi.AgentConnection.Status.Connected:
		print(f"[IAgentConnection] Status: {status}")

		print("Starting listening for AugmentRCSession invitations")
		augment_rc_session_module.startListening()

def invitation_received(url, as_module):
	print(f"[IAgentConnection] Augment RC Session Invitation received: {url}")


api = tvagentapi.TVAgentAPI()
connection = api.createAgentConnection()
if 'TV_BASE_SDK_URL' in os.environ and 'TV_AGENT_API_URL' in os.environ:
	connection.setConnectionURLs(os.environ['TV_BASE_SDK_URL'], os.environ['TV_AGENT_API_URL'])

augment_rc_session_module = connection.getModule(tvagentapi.ModuleType.AugmentRCSession)
assert augment_rc_session_module.isSupported(), "Augment RC Session Invitation Module not supported"

connection.setCallbacks(statusChanged=lambda status: connection_status_changed(status))

augment_rc_session_module.setCallbacks(
	augmentRCSessionInvitationReceivedCallback=lambda url: invitation_received(url, augment_rc_session_module))

print("Connecting to IoT Agent...")
connection.start()

print("Press Ctrl+C to exit")
while True:
	try:
		connection.processEvents(waitForMoreEvents=True, timeoutMs=100)
	except KeyboardInterrupt:
		break

print("Stopping listening for AugmentRCSession invitations")
augment_rc_session_module.stopListening()

print("Stopping connection to IoT Agent...")
connection.stop()

connection.processEvents()

print("Exiting...")
