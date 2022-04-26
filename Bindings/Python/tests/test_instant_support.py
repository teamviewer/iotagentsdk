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
from tvagentapi_test.instant_support import test_instant_support

access_token = os.environ['TV_ACCESS_TOKEN']
session_code = os.environ['TV_SESSION_CODE'] if 'TV_SESSION_CODE' in os.environ else ""

# Test data
request_data = {
    'accessToken': access_token,
    'name': "Help Me Please!",
    'group': "Contacts",
    'description': "I need your support!",
    'email': "supporter@teamviewer.com",
    'sessionCode': session_code
}

expected_session_data = {
    'state': tvagentapi.InstantSupportModule.SessionState.Open,
    'name': request_data['name'],
    'description': request_data['description'],
    'sessionCode': session_code
}


def test_request_reactivate_service_case():
    # request instant support without session code
    received_session_data = test_instant_support(
            request_data=dict(request_data, sessionCode=""),
            expected_session_data=expected_session_data
        )

    received_session_code=received_session_data['sessionCode']

    # request instant support with previously received session code
    test_instant_support(
            request_data=dict(request_data, sessionCode=received_session_code),
            expected_session_data=dict(expected_session_data, sessionCode=received_session_code)
        )


# Test cases
test_cases = {
    'test_request_created': lambda: test_instant_support(
            request_data=request_data,
            expected_session_data=expected_session_data
        ),
    'test_request_created_without_session_code': lambda: test_instant_support(
            request_data=dict(request_data, sessionCode=""),
            expected_session_data=expected_session_data
        ),
    'test_request_reactivate_service_case': test_request_reactivate_service_case,
    'test_request_bad_access_token': lambda: test_instant_support(
            request_data=dict(request_data, accessToken="bad-access-token"),
            expected_error_code=tvagentapi.InstantSupportModule.RequestErrorCode.InvalidToken
        ),
    'test_request_bad_email': lambda: test_instant_support(
            request_data=dict(request_data, email="bad-email"),
            expected_error_code=tvagentapi.InstantSupportModule.RequestErrorCode.InvalidEmail
        ),
    'test_request_created_without_session_code_change_description': lambda: test_instant_support(
            request_data=dict(request_data, sessionCode=""),
            expected_session_data=dict(expected_session_data, description='No support needed anymore!'),
            amend_support_case_data={'description': "No support needed anymore!"},
        ),
    # Tests requiring TV client interaction
    'test_request_instant_support_response_accept': lambda: test_instant_support(
            request_data=request_data,
            incoming_connection_response=tvagentapi.InstantSupportModule.acceptConnectionRequest
        ),
    'test_request_instant_support_response_reject': lambda: test_instant_support(
            request_data=request_data,
            incoming_connection_response=tvagentapi.InstantSupportModule.rejectConnectionRequest
        ),
    'test_request_instant_support_response_timeout': lambda: test_instant_support(
            request_data=request_data,
            incoming_connection_response=tvagentapi.InstantSupportModule.timeoutConnectionRequest
        )
}

if __name__ == "__main__":
    from tvagentapi_test import run_tests_from_args
    run_tests_from_args(test_cases)
