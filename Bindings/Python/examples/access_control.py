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

from argparse import ArgumentParser
import tvagentapi

Feature = tvagentapi.AccessControlModule.Feature
Access = tvagentapi.AccessControlModule.Access

# parse arguments
choices = [a.name for a in Access]
parser = ArgumentParser(description="Access controls management.")
parser.add_argument('-f', '--file-transfer',help='set file transfer access control',
    choices=choices, default=None)
parser.add_argument('-v', '--remote-view', help='set remote view access control',
    choices=choices, default=None)
parser.add_argument('-c', '--remote-control', help='set remote control access control',
    choices=choices, default=None)
args = parser.parse_args()

set_controls = {
    Feature.FileTransfer: getattr(Access, args.file_transfer) if args.file_transfer else None,
    Feature.RemoteView: getattr(Access, args.remote_view) if args.remote_view else None,
    Feature.RemoteControl: getattr(Access, args.remote_control) if args.remote_control else None,
}


def connection_status_changed(status, ac_module):
    print(f"[IAgentConnection] Status: {status}")
    if status == tvagentapi.AgentConnection.Status.Connected:

        print("Feature - Access:")
        for feature in (Feature.FileTransfer, Feature.RemoteView, Feature.RemoteControl):
            print(f"\t{feature.name} - {ac_module.getAccess(feature).name}")

        # set access controls
        # access_changed callback is called only when access control changes access mode
        for feature, access in set_controls.items():
            if access is not None:
                print(f"For feature: {feature.name} setting access: {access.name}...")
                ac_module.setAccess(feature, access)


def access_changed(feature, access):
    print(f"[IAgentConnection] accessChanged: Feature: {feature.name}, Access: {access.name}")


def access_requested(feature, ac_module):
    c = input(f"""
Received Access Control request for {feature.name}.
Confirm Access Control request? (y/N): """)
    if c in ('y', 'Y'):
        ac_module.acceptAccessRequest(feature)
        print("Access control accepted")
    else:
        ac_module.rejectAccessRequest(feature)
        print("Access control rejected")


api = tvagentapi.TVAgentAPI()
connection = api.createAgentConnectionLocal()
access_control_module = connection.getModule(tvagentapi.ModuleType.AccessControl)

assert access_control_module.isSupported(), "AccessControlModule not supported"

connection.setStatusChangedCallback(lambda status: connection_status_changed(status, access_control_module))

access_control_module.setCallbacks({
    'accessChangedCallback': access_changed,
    'accessRequestCallback': lambda feature: access_requested(feature, access_control_module)
})

print("Connecting to IoT Agent...")
connection.start()

print("Press Ctrl+C to exit")
while True:
    try:
        connection.processEvents(waitForMoreEvents=True, timeoutMs=100)
    except KeyboardInterrupt:
        break

print("Stopping connection to IoT Agent...")

connection.stop()

connection.processEvents()

print("Exiting...")
