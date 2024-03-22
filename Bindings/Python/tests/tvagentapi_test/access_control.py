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

from . import with_connect_urls


@with_connect_urls
def test_file_transfer_access_request(action: str, base_sdk_url=None, agent_api_url=None):
    """
    Test handling an incoming File Transfer access confirmation request

    Argument:
    action  -- 'accept'/'reject'/'timeout' the incoming FT request
    """
    import tvagentapi
    import time

    Feature = tvagentapi.AccessControlModule.Feature
    Access = tvagentapi.AccessControlModule.Access

    access_is_after_confirmation = False
    access_request_received = False

    def connection_status_changed(ac_module, status):
        if status == tvagentapi.AgentConnection.Status.Connected:
            # 2 calls to make sure we get the access_changed() callback
            ac_module.setAccess(Feature.FileTransfer, Access.Denied)
            ac_module.setAccess(Feature.FileTransfer, Access.AfterConfirmation)

    def access_changed(ac_module, feature, access):
        nonlocal access_is_after_confirmation
        if feature == Feature.FileTransfer and access == Access.AfterConfirmation:
            access_is_after_confirmation = True

    def access_request(ac_module, feature):
        nonlocal access_request_received
        if feature == Feature.FileTransfer:
            assert ac_module.getAccess(Feature.FileTransfer) == Access.AfterConfirmation,\
                "FT access not set to AfterConfirmation"
            time.sleep(5)
            access_request_received = True
            print("access_request_received = True")
            if action == 'accept':
                ac_module.acceptAccessRequest(feature)
            elif action == 'reject':
                ac_module.rejectAccessRequest(feature)
            else:
                pass # time out. NB: this will look like 'reject' on client side.

    api = tvagentapi.TVAgentAPI()
    connection = api.createAgentConnection(None)
    if base_sdk_url and agent_api_url:
        connection.setConnectionURLs(base_sdk_url, agent_api_url)

    access_control_module = connection.getModule(tvagentapi.ModuleType.AccessControl)

    connection.setCallbacks(statusChanged=lambda status: connection_status_changed(access_control_module, status))

    access_control_module.setCallbacks(
        accessChangedCallback=lambda feature, access: access_changed(access_control_module, feature, access),
        accessRequestCallback=lambda feature: access_request(access_control_module, feature))

    assert access_control_module.isSupported()

    connection.start()

    while not (access_is_after_confirmation and access_request_received):
        connection.processEvents(True, 100)

    connection.stop()
    connection.processEvents()

    del connection

    reference_error = False
    try:
        access_control_module.isSupported()
    except ReferenceError:
        reference_error = True
    assert reference_error, "Reference error expected"
