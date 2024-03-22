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

log_filename = 'example.log'
api = tvagentapi.TVAgentAPI()

# internal tvagentapi logging will be appended to file the file
logging = api.createFileLogging(log_filename)
connection = api.createAgentConnection(logging)
if 'TV_BASE_SDK_URL' in os.environ and 'TV_AGENT_API_URL' in os.environ:
    connection.setConnectionURLs(os.environ['TV_BASE_SDK_URL'], os.environ['TV_AGENT_API_URL'])

# you can also use logging for your own needs
logging.logInfo("log some information")
logging.logError("log an error")

connection.start()

print(f"Logging to '{log_filename}' Press Ctrl+C to exit")
while True:
    try:
        connection.processEvents(waitForMoreEvents=True, timeoutMs=100)
    except KeyboardInterrupt:
        break

connection.stop()
