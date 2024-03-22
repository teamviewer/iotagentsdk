# Python Bindings examples

## **Running the examples**

Follow the configuration and usage steps from [the Python Bindings README file](../../Bindings/Python/Readme.md).

Once the `tvagentapi.so` binary has been made accessible via the paths listed in the `sys.path`, the example scripts can be run.

The example Python scripts provided showcase the functionality of the API:

>`agent_connection.py`  
`tv_session_management.py`  
`access_control.py`&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;(accepts command-line arguments; run with `-h` for details)  
`instant_support.py`&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;(accepts command-line arguments; run with `-h` for details)  
`chat.py`

Continue reading for an architectural breakdown of each example and the meaning of their command-line arguments.

## **Basic example: Agent Connection**

This example shows the steps for creating and maintaining a connection to the TeamViewer IoT Agent process running on your local machine.

See source: [agent_connection.py](agent_connection.py)

👉 **Important**: There are two connection types possible between SDK and Agent: Unix sockets (gRPC), or TCP sockets.
Which one will be used is up to the developer, see `tvagentapi.AgentConnection.setConnectionURLs` documentation.
In case of the examples, the parameters for `setConnectionURLs` are injected via 2 environment variables:
**TV_BASE_SDK_URL** and **TV_AGENT_API_URL**. Setting them is optional but required if you use
a non-standard configuration (e.g. the IoT Agent is located on a different device reachable over TCP).
The same environment variables are used for running the Python tests.

## Step 1: Get the API object

```python
api = tvagentapi.TVAgentAPI()
```

## Step 2: Create a logging object (optional)

```python
logging = api.createFileLogging(log_filename)
```

As shown above, you can ask the Agent API to create a default logging object for you. This will log to a file, which is adequate for most cases.

## Step 3: Get a connection to your local TeamViewer IoT Agent

```python
connection = api.createAgentConnectionLocal(logging)
```

As mentioned, providing the `logging` object is optional but useful for debugging.
Note: at this stage the connection is created, but not yet active.

## Step 4: Set the connection callback

```python
connection.setCallbacks(statusChanged=connectionStatusChanged)
```

You can ask the connection object to notify you of changes: a successful connection to the IoT Agent, connection lost, etc.

👉 Your app (through the Agent API) makes requests to the IoT Agent process by pushing packets to its Unix sockets. In turn, the IoT Agent may asynchronously reply by pushing packets to *our* Unix sockets. (In this sense, your app is both a "client" and a "server".) Packets are serialized using [Protocol Buffers](https://developers.google.com/protocol-buffers) and sent using [GRPC](https://grpc.io/).

Most objects let you set callbacks for important events that happen to them. You will see examples of setting callbacks below.

## Step 5: Start the connection

```python
connection.start()
```

This launches a background thread that:
* initializes the various client and server sockets on the API side
* handshakes with the IoT Agent process, and
* runs a keepalive ping loop.

You are now connected to the IoT Agent. You can initiate actions and react to events.

## Step 6: Run the event loop

```python
print("Monitoring connection status changes... Press Ctrl+C to exit")
try:
    while True:
        connection.processEvents(waitForMoreEvents=True, timeoutMs=100)
except KeyboardInterrupt:
    pass
```

The first event you should get is a call to your `connectionStatusChanged()`, with the status `Status.Connecting`, followed by another with `Status.Connected` if all goes well.

In our basic example app, this is all that happens (aside from an unfortunate `Status.ConnectionLost`). You can stop the event loop at any time. Here, we break on `Ctrl-C`.

## Step 7: Shutdown and cleanup

```python
connection.stop()
```

This sends the IoT Agent process a "disconnect" request and terminates the background API thread.

It's a good idea to handle any leftover events that `stop()` itself may have generated:

```python
connection.processEvents()
```

To more meaningfully use the Agent API to interact with the IoT Agent, we need to use API modules.

## **Module example #1: Session Management**

See source: [tv_session_management.py](tv_session_management.py)

👉 A "session" is any incoming connection to your local IoT Agent. You can use the Session Management module to monitor these connections.

Create the Session Management module:

```python
tv_session_management = connection.getModule(tvagentapi.ModuleType.TVSessionManagement)
```

It is a good idea to check whether the module was successfully created and is supported:

```python
assert tv_session_management.isSupported(), "TV Session Management Module not supported"
```

👉 In rare cases, you may find yourself running on a system with a version of the IoT Agent that doesn't support a particular module (or the IoT Agent may have been downgraded while your app is running). You will still be able to create and interact with the module, but most methods &ndash; other than `isSupported()` &ndash; will be no-ops.

Set the module-specific callbacks:

```python
tv_session_management.setCallbacks(
    sessionStartedCallback=lambda sid, s_cnt: session_state_changed(tv_session_management, True, sid, s_cnt),
    sessionStoppedCallback=lambda sid, s_cnt: session_state_changed(tv_session_management, False, sid, s_cnt))
```

👉 Unlike the `agentConnection` object which has only one callback, modules typically have several. Look at the documentation of `<YourModule>.setCallbacks()` using `pydoc3 tvagentapi`to see what callbacks it expects, and in what order.

(You can provide `None` for any callback you are not interested in.)

The event loop then runs as usual:

```python
while ...:
    connection.processEvents(waitForMoreEvents=True, timeoutMs=100)
```

You will now receive callbacks when an incoming session to your IoT Agent is established or a session ends:

```python
def session_state_changed(tvsm_module, started, session_id, sessions_count):
    print(f"[TVSessionManagement] Session {'started' if started else 'stopped'}")
    print(f"[TVSessionManagement] Session ID: {session_id}, sessions count: {sessions_count}")
    print_running_sessions(tvsm_module)
    ...
```

* `session_id` : a numerical identifier of the newly connected or disconnected session. Note: this is **not** a TeamViewer ID. If the same remote user connects, disconnects, then connects again, you will see different `session_id`s.
* `sessions_count` : the number of currently connected sessions, including this one. Typically 1, but can conceivably be any number. Goes to 0 when the last session ends.

## **Module example #2: Access Control**

See source: [access_control.py](access_control.py)

👉 Some features provided by the IoT Agent (`FileTransfer`, `RemoteView`, `RemoteControl`) have permissions associated with them:
* `Allowed` : a remote TeamViewer client connected to this IoT Agent can use the feature.
* `Denied` : the remote client cannot use the feature.
* `AfterConfirmation` : the remote client can use the feature **after** asking for permission. Your app will get a callback from the Agent API, at which point it can show a UI prompt, programmatically make the decision, etc.

You can use the Access Control module to manage these permissions on a per-feature basis, as well as handle any incoming access requests.

(Note: Permissions are only taken into account once, when a new session starts. Changing permissions during an active session will only affect future sessions.)

Create the Access Control module:

```python
access_control_module = connection.getModule(tvagentapi.ModuleType.AccessControl)
```

As usual, it's good to do some sanity checks:

```python
assert access_control_module.isSupported(), "AccessControlModule not supported"
```

Set the callbacks:

```python
access_control_module.setCallbacks(
    accessChangedCallback=access_changed,
    accessRequestCallback=lambda feature: access_requested(feature, access_control_module))
```

* `access_changed()` : Someone (possibly your app) has changed an access permission on the IoT Agent for a particular feature; for example, `FileTransfer` was set to `Allowed`. (Note: only called when the value has actually changed.)
* `access_requested()` : A remotely connected TeamViewer client wants to use a feature which is currently set to `AfterConfirmation`. Your app must take the steps to allow or deny the request (by prompting the user, deciding programmatically, etc.). If you do nothing, the request will time out on the remote client's side after a few minutes.

Set and get access permissions for specific features:

```python
Feature = tvagentapi.AccessControlModule.Feature
Access = tvagentapi.AccessControlModule.Access
access_control_module.setAccess(Feature.RemoteControl, Access.AfterConfirmation)
```

```python
Feature = tvagentapi.AccessControlModule.Feature
for feature in (Feature.FileTransfer, Feature.RemoteView, Feature.RemoteControl):
    print(f"\t{feature.name} - {access_control_module.getAccess(feature).name}")
```

When a particular feature is set to `AfterConfirmation`, you can decide whether to allow a remote party to use it:

```python
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
```

## **Module example #3: Instant Support**

See source: [instant_support.py](instant_support.py)

👉 TeamViewer users have the ability to request remote assistance from other users. This shows up in the supporter's TeamViewer client GUI as a separate entry in the Computers & Contacts list. The supporter can then request to connect to the supportee's machine, send and receive chat messages, etc. This functionality is of course also available in the TeamViewer IoT Agent, and reflected in the Agent API.

Create the Instant Support module and set the callbacks:

```python
instant_support_module = connection.getModule(tvagentapi.ModuleType.InstantSupport)
assert instant_support_module.isSupported(), "Instant Support Module not supported"
...

instant_support_module.setCallbacks(
    sessionDataChangedCallback=instantSupportSessionDataChanged,
    requestErrorCallback=instantSupportRequestError,
    connectionRequestCallback=lambda: instantSupportConnectionRequested(instant_support_module))
```

Once connected to the IoT Agent, your app can make an Instant Support request:

```python
def connectionStatusChanged(status, is_module):
    if status == tvagentapi.AgentConnection.Status.Connected:
        request_data = {
            'accessToken': "12345678-LgxKf0bybuAESdNIelrY", # see below
            'name': "Help Me Please!",
            'group': "Contacts",
            'description': "I need your support!",
            'email': "supporter@teamviewer.com",
            'sessionCode': "s01-234-567" # see below
        }
        is_module.requestInstantSupport(request_data)
```

- `accessToken` : an alphanumeric string uniquely identifying the remote supporter (**not** a TeamViewer ID). You will typically already have this on hand for your application to use. Supporters can refer to **Creating Access Tokens for Instant Support** in [the main README file](../../README.md).
- `sessionCode` : a session identifier used to join or rejoin an existing instant support session. This is a long-running ID your supporter uses to keep track of your case, until they manually close it. You can think of it as a "service case number". Can be `""` on your first request.

👉 To request a new instant support session, you only need a valid `accessToken`. You will be called back in `instantSupportSessionDataChanged()` with the new `sessionCode`.

👉 To join an existing session (either created by you or the remote supporter), you need a valid `accessToken` and the correct `sessionCode`.

Once you are in an instant support session, the remote supporter may try to connect to you. This will prompt a call to your `instantSupportConnectionRequested()`, where you can allow or deny the incoming connection.

Note: a current limitation of the API is that an application can only make one Instant Support request.

## **Module example #4: Chat**

See source: [chat.py](chat.py)

TeamViewer users also have the ability to exchange chat messages. To initiate a conversation, users right click an entry in their Computers & Contacts list, and select "Send chat message".

👉 As a security precaution, the IoT Agent cannot initiate conversations, or look up conversations from past runs. You have to wait for an initial message from the outside.

**First things first: wait for an incoming message**

For the sake of demonstration, go ahead and send a friendly message from your TeamViewer desktop client to the machine running your IoT Agent...

![Send message from Windows client](../../doc/windows_client_send_message.png)

If the Chat example app is running, you should see this (assuming the sender's name is indeed Chatty McChatterson):

```
[IChatModule] Chat room created: 25666256-4fcf-41c0-9971-7343680d96dc, Chatty McChatterson, tvagentapi.ChatModule.ChatEndpointType.Machine, 2032226623

[IChatModule] received 1 message(s):
New message in selected chat room 25666256-4fcf-41c0-9971-7343680d96dc:
Chatty McChatterson: Hello there!
```

Now stop the Chat example app (`Ctrl-C`) and run it again. Send a second message from your desktop client...

```
[IChatModule] received 1 message(s):
New message in chat room 25666256-4fcf-41c0-9971-7343680d96dc:
Chatty McChatterson: Hey, where did you go?
```

Notice how the chat is not created this time: the ID is the same. The conversation is still active on the IoT Agent, and you can start sending replies right away.

👉 Each conversation with a remote party falls under a separate chat room (or simply "chat"). Chats are identified by string UUIDs, e.g. `"25666256-4fcf-41c0-9971-7343680d96dc"`.

The example program aims to be a complete chat application and is therefore quite extensive, but as you are already generally familiar with our SDK API, we'll just look at a few highlights. We create the Chat module and set the callbacks:

```python
chat = connection.getModule(tvagentapi.ModuleType.Chat)
assert chat.isSupported()
...

chat.setCallbacks(
    chatCreatedCallback=chat_created,
    chatsRemovedCallback=chats_removed,
    chatClosedCallback=chat_closed,
    receivedMessagesCallback=received_messages,
    sendMessageFinishedCallback=send_message_finished,
    messagesLoadedCallback=messages_loaded,
    historyDeletedCallback=history_deleted)
```

The most relevant is `ReceivedMessagesCallback`: incoming messages are delivered here. As soon as you get the first one, you can chat back:

```python
request_id = chat.sendMessage(chat_id, message)
```

`sendMessage()` returns an immediate numerical ID for your message; if the message is sent successfully, you will be called back asynchronously in `SendMessageFinishedCallback` with the same numerical ID, as well as a permanent string UUID for the message.

👉 Each individual message (both sent and received) is identified by a string UUID.

For completeness, the other Chat API calls are listed below. They interact only with the chat cache on your local IoT Agent: chats saved in the TeamViewer cloud are not affected. Refer to the example app for their usage.

```python
obtainChats()    // get a list of available chats
loadMessages()   // load message history
deleteHistory()  // delete message history in a given chat
deleteChat()     // delete a chat altogether
```