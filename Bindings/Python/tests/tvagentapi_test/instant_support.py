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


def test_instant_support(request_data=None, expected_session_data=None, expected_error_code=None,
                         incoming_connection_response=None, amend_support_case_data=None):
    """
    Instant Support test helper

    Arguments:
    request_data                  -- if not None -> make request. If 'sessionCode' not provided we don't compare it to expected_session_data['sessionCode']
    expected_session_data         -- if not None -> expect sessionDataChangedCallback callback
    expected_error_code           -- if not None -> expect requestErrorCallback callback
    incoming_connection_response  -- if not None -> call response action when supporter connects
    amend_support_case_data       -- if not None -> change support case data after it is created
    """
    import tvagentapi
    from . import get_session, put_session

    session_data = None
    error_code = None
    response_success = None

    def connection_status_changed(status, is_module):
        if status != tvagentapi.AgentConnection.Status.Connected:
            return
        if request_data is not None:
            is_module.requestInstantSupport(request_data)

    def instant_support_session_data_changed(new_session_data):
        nonlocal session_data
        session_data = new_session_data

        # Validate the data validity via WebAPI
        webapi_session_data = get_session(request_data['accessToken'], new_session_data['sessionCode'])
        assert new_session_data['description'] == webapi_session_data['description']
        assert new_session_data['name'] == webapi_session_data['end_customer']['name']

        if amend_support_case_data:
            if 'description' in amend_support_case_data and amend_support_case_data['description'] != \
                    new_session_data['description']:
                put_session(request_data['accessToken'], new_session_data['sessionCode'], **amend_support_case_data)
                # reset data in order to not immediately exit the event loop below
                session_data = None

    def instant_support_request_error(new_error_code):
        nonlocal error_code
        error_code = new_error_code

    def instant_support_connection_requested(is_module):
        nonlocal response_success
        try:
            incoming_connection_response(is_module)
        except:
            response_success = False
            return
        response_success = True

    api = tvagentapi.TVAgentAPI()
    connection = api.createAgentConnectionLocal(None)
    instant_support_module = connection.getModule(tvagentapi.ModuleType.InstantSupport)
    connection.setCallbacks(statusChanged=lambda status: connection_status_changed(status, instant_support_module))
    instant_support_module.setCallbacks(
        sessionDataChangedCallback=instant_support_session_data_changed,
        requestErrorCallback=instant_support_request_error,
        connectionRequestCallback=lambda: instant_support_connection_requested(instant_support_module))

    connection.start()

    # wait for session_data or error_code changes
    # or wait for incoming instant support connection
    while ((session_data is None and error_code is None) or
           (incoming_connection_response is not None and response_success is None)):
        connection.processEvents(True, 100)

    connection.stop()
    connection.processEvents()

    # compare expected
    if expected_session_data is not None:
        if not request_data['sessionCode']:
            # we didn't provide sessionCode for the request -> any received session code is correct
            expected_session_data['sessionCode'] = session_data['sessionCode']

        if session_data != expected_session_data:
            raise RuntimeError(f"session_data != expected_session_data: {session_data} != {expected_session_data}")
    elif expected_error_code is not None and error_code != expected_error_code:
        raise RuntimeError(f"error_code != expected_error_code: {error_code} != {expected_error_code}")

    if incoming_connection_response is not None and not response_success:
        raise RuntimeError(f"incoming connection response failed")

    del connection

    reference_error = False
    try:
        instant_support_module.isSupported()
    except ReferenceError:
        reference_error = True
    assert reference_error, "Reference error expected"

    return session_data
