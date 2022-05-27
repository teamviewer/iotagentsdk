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

import tvagentapi


def print_running_sessions(tvsm_module):
    running_sessions = tvsm_module.getRunningSessions()
    print(f"[TVSessionManagement]: Running sessions {running_sessions}")


def connection_status_changed(status, tvsm_module):
    print(f"[AgentConnection] Status: {status}")
    if status == tvagentapi.AgentConnection.Status.Connected:
        print_running_sessions(tvsm_module)


def session_state_changed(tvsm_module, started, session_id, sessions_count):
    print(f"[TVSessionManagement] Session {'started' if started else 'stopped'}")
    print(f"[TVSessionManagement] Session ID: {session_id}, sessions count: {sessions_count}")
    print_running_sessions(tvsm_module)


api = tvagentapi.TVAgentAPI()
connection = api.createAgentConnectionLocal()

tv_session_management = connection.getModule(tvagentapi.ModuleType.TVSessionManagement)
assert tv_session_management.isSupported(), "TV Session Management Module not supported"

connection.setCallbacks(statusChanged=lambda status: connection_status_changed(status, tv_session_management))
tv_session_management.setCallbacks(
    sessionStartedCallback=lambda sid, s_cnt: session_state_changed(tv_session_management, True, sid, s_cnt),
    sessionStoppedCallback=lambda sid, s_cnt: session_state_changed(tv_session_management, False, sid, s_cnt))

print("Connecting to IoT Agent...")
connection.start()

print("Monitoring incoming sessions... Press Ctrl+C to exit")
try:
    while True:
        connection.processEvents(waitForMoreEvents=True, timeoutMs=100)
except KeyboardInterrupt:
    pass

print("Stopping connection to IoT Agent...")

# not mandatory at the end of execution:
tv_session_management.terminateTeamViewerSessions()

# ...but can always be used to forcefully terminate all active sessions;
# in this case, you might want to wait until they are gone:
while tv_session_management.getRunningSessions():
    connection.processEvents(waitForMoreEvents=True, timeoutMs=100)

connection.stop()

connection.processEvents()

print("Exiting...")
