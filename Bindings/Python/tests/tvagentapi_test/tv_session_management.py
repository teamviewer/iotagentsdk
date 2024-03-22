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


@with_connect_urls
def test_tv_session_management(expected_session_counts, terminate_sessions_after=None,
                               base_sdk_url=None, agent_api_url=None):
    """
    TV Session Management test helper

    Arguments:
    expected_session_counts     -- A list of sequential session counts based on the running scenario
    terminate_sessions_after    -- If a number, wait N seconds then forcefully terminate any remaining open sessions
    """
    import tvagentapi

    assert isinstance(expected_session_counts, list), "expected_session_counts is not a list"
    assert expected_session_counts, "expected_session_counts is empty"

    current_sessions_count = 0
    current_session_counts = []
    current_session_ids = set()

    def connection_status_changed(status, tvsm_module):
        if status == tvagentapi.AgentConnection.Status.Connected:
            current_session_ids.update(tvsm_module.getRunningSessions())
            assert len(current_session_ids) == expected_session_counts[0]
            current_session_counts.append(expected_session_counts[0])

    def session_state_changed(tvsm_module, started, session_id, sessions_count):
        nonlocal current_sessions_count
        if started:
            assert session_id not in current_session_ids, "new connection id already in list"
            assert sessions_count == current_sessions_count + 1, "sessions count mismatch"
            current_session_ids.add(session_id)
            current_sessions_count += 1
        else:
            assert session_id in current_session_ids, "ended connection id not in list"
            assert sessions_count == current_sessions_count - 1, "sessions count mismatch"
            current_session_ids.remove(session_id)
            current_sessions_count -= 1
        current_session_counts.append(sessions_count)

    api = tvagentapi.TVAgentAPI()
    connection = api.createAgentConnection(None)
    if base_sdk_url and agent_api_url:
        connection.setConnectionURLs(base_sdk_url, agent_api_url)
    tv_session_management = connection.getModule(tvagentapi.ModuleType.TVSessionManagement)
    connection.setCallbacks(statusChanged=lambda status: connection_status_changed(status, tv_session_management))
    tv_session_management.setCallbacks(
        sessionStartedCallback=lambda sid, s_cnt: session_state_changed(tv_session_management, True, sid, s_cnt),
        sessionStoppedCallback=lambda sid, s_cnt: session_state_changed(tv_session_management, False, sid, s_cnt))

    assert tv_session_management.isSupported()

    connection.start()

    while current_session_counts != expected_session_counts:
        connection.processEvents(True, 100)

    if isinstance(terminate_sessions_after, int):
        wait_end = datetime.now() + timedelta(seconds=terminate_sessions_after)
        while datetime.now() < wait_end:
            connection.processEvents(True, 100)
        tv_session_management.terminateTeamViewerSessions()
        while tv_session_management.getRunningSessions():
            connection.processEvents(True, 100)

    connection.stop()
    connection.processEvents()

    del connection

    reference_error = False
    try:
        tv_session_management.isSupported()
    except ReferenceError:
        reference_error = True
    assert reference_error, "Reference error expected"
