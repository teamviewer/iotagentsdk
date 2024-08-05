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

from . import with_connect_urls


@with_connect_urls
def test_augment_rc_session(base_sdk_url=None, agent_api_url=None):
    """
    Test handling an incoming AugmentRCSession Invitation.
    Setup: Have running agent, start this test and establish a RC to the agent.
    In the RC session window the 'Augment Session' button should appear.
    Clicking this button should send the invitation to the sdk and the test should succeed.
    """
    import tvagentapi
    import time

    invitation_received = False

    def connection_status_changed(status):
        print(f"[IAgentConnection] Status: {status}")
        if status != tvagentapi.AgentConnection.Status.Connected:
            return

        print("Starting listening for AugmentRCSession invitations")
        augment_rc_session_module.startListening()


    def augment_rc_session_invitation_received(ac_module, url):
        nonlocal invitation_received
        invitation_received = True
        print(f"AugmentRCSession Invitation received: {url}")

    api = tvagentapi.TVAgentAPI()
    connection = api.createAgentConnection(None)
    if base_sdk_url and agent_api_url:
        connection.setConnectionURLs(base_sdk_url, agent_api_url)

    augment_rc_session_module = connection.getModule(tvagentapi.ModuleType.AugmentRCSession)

    augment_rc_session_module.setCallbacks(
        augmentRCSessionInvitationReceivedCallback=lambda url: augment_rc_session_invitation_received(
            augment_rc_session_module, url))

    connection.setCallbacks(statusChanged=lambda status: connection_status_changed(status))

    assert augment_rc_session_module.isSupported()

    connection.start()

    print("Waiting for AugmentRCSession Invitation...")
    while not (invitation_received):
        connection.processEvents(True, 100)

    augment_rc_session_module.stopListening()

    connection.stop()
    connection.processEvents()

    del connection

    reference_error = False
    try:
        augment_rc_session_module.isSupported()
    except ReferenceError:
        reference_error = True
    assert reference_error, "Reference error expected"
