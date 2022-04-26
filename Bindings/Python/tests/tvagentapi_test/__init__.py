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

import multiprocessing
import argparse
import warnings


def run_test_process(test_name="test", target=None, timeout=40):
    """
    Run test as a separate process.

    Arguments:
    test_name   -- a meaningful name for the test (default "test")
    target      -- target to run (default None)
    timeout     -- timeout in seconds after which the test fails (default 40)
    """
    print(f" 🐍 Test '{test_name}'...")
    p = multiprocessing.Process(target=target)
    p.start()
    p.join(timeout)

    if p.is_alive():
        p.terminate()
        p.join(5)
        p.kill()
        p.join()
        raise RuntimeError(f" ❌ Test '{test_name}' timed out after {timeout} seconds")

    if p.exitcode != 0:
        raise RuntimeError(f" ❌ Test '{test_name}' failed with exitcode {p.exitcode}")

    print(f" ✅ Test '{test_name}' passed\n")


def parse_args(test_names=None):
    """
    Parse test case arguments.

    Arguments:
    test_names -- list of test case names (default None)
    """
    test_names = [] if test_names is None else test_names

    test_names_string='\n'.join(f"    {n}" for n in test_names)

    parser = argparse.ArgumentParser(
        description=f"Available tests:\n{test_names_string}",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument(
        '-t', '--tests',
        help='space separated list of tests to run',
        nargs='+',
        metavar='test_case', # to make help more readable
        choices=test_names,
        default=test_names
    )

    return parser.parse_args()


def run_tests_from_args(test_cases=None):
    """
    Parse test case arguments and run each test case in a separate process

    Arguments:
    test_cases -- dictionary of test case names to test functions (default None)
    """

    args = parse_args(test_cases.keys())

    for n in args.tests:
        if not n in test_cases:
            warnings.warn(f"No test '{n}' in test_cases")
        else:
            run_test_process(test_name=n, target=test_cases[n])


def tvwebapicall(method):
    """
    See https://webapi.teamviewer.com/api/v1/docs/index
    """
    def wrapped_with_method(func):
        def wrapped(api_token, *args, **kwargs):
            import json
            import requests

            base_url = 'https://webapi.teamviewer.com/api/v1/'

            url, parameters = func(*args, **kwargs)

            headers = {'Authorization': "Bearer " + api_token}

            response = requests.request(method, base_url + url, data=parameters, headers=headers)

            if not response:
                raise RuntimeError("No response")

            if not response.ok:
                raise RuntimeError("Request failed")
            if response.text:
                obj = json.loads(response.text)
                return obj
        return wrapped
    return wrapped_with_method


@tvwebapicall('get')
def get_sessions(*args, **kwargs):
    return 'sessions', kwargs


@tvwebapicall('get')
def get_session(session_code, *args, **kwargs):
    return f'sessions/{session_code}', kwargs


@tvwebapicall('put')
def put_session(session_code, *args, **kwargs):
    params = dict(**kwargs)
    params['end_customer'] = {}
    if 'name' in kwargs:
        params['end_customer']['name'] = params['name']
        del params['name']
    if 'email' in kwargs:
        params['end_customer']['email'] = params['email']
        del params['email']
    return f'sessions/{session_code}', params
