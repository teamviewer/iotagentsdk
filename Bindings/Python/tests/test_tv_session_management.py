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
from tvagentapi_test.tv_session_management import test_tv_session_management


# Test cases
test_cases = {
    'test_request_no_connections': lambda: test_tv_session_management(
        expected_session_counts=[0],
    ),
    # Tests requiring TV client interaction
    'test_request_connect_disconnect': lambda: test_tv_session_management(
        expected_session_counts=[0, 1, 0],
    ),
    'test_request_connect_terminate': lambda: test_tv_session_management(
        expected_session_counts=[0, 1],
        terminate_sessions_after=5
    ),
}

if __name__ == "__main__":
    from tvagentapi_test import run_tests_from_args
    run_tests_from_args(test_cases)
