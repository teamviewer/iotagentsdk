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

def test_agent_connection(wait_for_more_events=True, more_events_timeout_ms=100, process_events_expect_error=False):
    import tvagentapi

    ConnStatus = tvagentapi.AgentConnection.Status

    statuses = []
    connected_obtained = False

    def connection_status_changed(status):
        statuses.append(status)
        nonlocal connected_obtained
        if status == ConnStatus.Connected:
            connected_obtained = True

    api = tvagentapi.TVAgentAPI()
    connection = api.createAgentConnectionLocal(None)
    connection.setCallbacks(statusChanged=connection_status_changed)
    connection.start()
    while not connected_obtained:
        events_processed = False
        try:
            events_processed = connection.processEvents(
                waitForMoreEvents=wait_for_more_events,
                timeoutMs=more_events_timeout_ms)
        except (TypeError, ValueError, OverflowError) as err:
            if process_events_expect_error:
                break
            else:
                raise err
        if connected_obtained and not events_processed:
            raise RuntimeError("procesEvents claims to have nothing processed even though connection to the agent has been obtained")

    connection.stop()

    connection.processEvents()

    if process_events_expect_error:
        return

    assert not connection.processEvents()

    expected_statuses = [ConnStatus.Connecting, ConnStatus.Connected,
        ConnStatus.Disconnecting, ConnStatus.Disconnected]

    if statuses != expected_statuses:
        raise RuntimeError(f"statuses not as expected. {statuses} != {expected_statuses}")
