# TeamViewer Agent SDK

## Introduction

On embedded systems, the TeamViewer IoT Agent provides many of the same features you can find on desktop editions of TeamViewer:

* Viewing the remote screen
* Sending keyboard and mouse input
* Accessing the remote file system
* Requesting remote assistance
* Exchanging chat messages, etc.

In addition, the TeamViewer IoT Agent provides the ability to outsource some or all of this functionality to an **external application**.

On your particular embedded system, you may want to:

* Use a more efficient or system-specific way to grab the screen
* Grab only a single window instead of the whole screen
* Supply an arbitrary image feed altogether
* Filter, modify, or log keyboard and mouse input
* Allow or deny incoming connections, attempts to access the file system, etc.

![Agent SDK Deployment Overview](./doc/agent_sdk_deployment_overview.png)

The TeamViewer Agent SDK enables your application to connect to the TeamViewer IoT Agent process running on the same machine, and then provide your own implementations for these features. By loading a dynamic library, your application can talk to the IoT Agent, make requests and receive replies, send picture data, change settings, and set up callbacks for events that interest you. Under the hood, your app talks to the IoT Agent through IPC over Unix domain sockets. If your app is on a different machine, it can talk to the IoT Agent over TCP sockets.

## Ways to use the SDK:

![Agent SDK Common cases](./doc/agent_sdk_common_cases.png)

### **Qt API**

Designed for direct integration into Qt apps running on the device. You can pass native types (`QString`, etc.), use the Qt framework to grab snapshots of your window and simulate input, etc.

* Source: [TVQtRC/](TVQtRC/)
* Build artifact: `libTVQtRC.so`
* A fully featured Qt example application: [examples/Qt/qtsimulate](examples/Qt/qtsimulate)

### **C++ API**

New and modular, this aims to provide a clean, structured interface. In active development; currently offers a limited set of features compared to the Qt API.

* Source: [TVAgentAPI/](TVAgentAPI/)
* Build artifact: `libTVAgentAPI.so`
* Simple command-line example apps: [examples/cpp/](examples/cpp/)

### **Python Bindings**

A Python wrapper around the C++ API is also available, providing a virtually 1:1 correspondence with C++. Users who prefer the convenience of Python can refer to the example scripts provided.

* Source: [Bindings/Python](./Bindings/Python/)
* Build artifact: `tvagentapi.so`
* Example scripts: [examples/py/](examples/py/)

## How does your app communicate with the Agent?

The Agent SDK is designed to be flexible and accommodate a wide variety of hardware setups. Your app can run on the same machine as the Agent, or on a different machine.

### Through Unix sockets (same machine):

The Agent will be listening on multiple Unix sockets, most notably the registration service -- by default at `unix:///tmp/teamviewer-iot-agent-services/remoteScreen/registrationService`.

Your app (through the SDK code it loads) then connects to this socket and requests the full list of sockets the Agent has open, each responsible for a particular piece of functionality.

Your app (through the SDK code it loads) will also open multiple sockets of its own -- by default under `unix:///tmp` -- the Agent will talk to you through these. As soon as your application (via the SDK) opens its own sockets, it will communicate them to the Agent via the 'registrationService' socket above. This happens early in the startup phase.

If your setup requires it, you can customize the location of these sockets using [setRemoteScreenSdkUrls()](./TVQtRC/Library/export/TVQtRC/Interface.h#73).

👉 Make sure the Unix socket paths are writable by the Agent and your application.

### Through TCP sockets (same or different machine):

The Agent will be listening on multiple TCP sockets, most notably the registration service -- by default at `tcp+tv://127.0.0.1` (on port 9221).

Similar to the above, your app (through the SDK code it loads) will talk to this service to discover what TCP sockets the Agent has open, and to announce what TCP sockets of its own it has open.

By default, your app's sockets will be located at `tcp+tv://127.0.0.1` (between ports 9237 and 9326).

You can customize these addresses using [setRemoteScreenSdkUrls()](./TVQtRC/Library/export/TVQtRC/Interface.h#73). NB: port numbers are not customizable.

👉 Make sure the TCP ports between app<->Agent are allowed through the firewall.

## Choosing the socket type

It's up to you whether to use Unix or TCP sockets to communicate with the Agent. The choice is a combination of compilation flags and runtime arguments.

See [setRemoteScreenSdkUrls()](./TVQtRC/Library/export/TVQtRC/Interface.h#73) for more information.

## System Requirements

Currently only Linux-based OSs are supported.

### **Development**

On Debian systems, you will need the following development packages:

* `cmake`
* `build-essential` or `g++`
* `libgrpc++-dev`
* `protobuf-compiler-grpc`
* `libprotobuf-dev`
* `protobuf-compiler`

If you plan to use the Qt5 API:

* `qtbase5-dev`
* `qtdeclarative5-dev`

If you plan to use the Qt6 API:

* `qt6-base-dev`
* `qt6-declarative-dev`
* `qml6-module-qtquick`
* `qml6-module-qtquick-window`
* `qml6-module-qtqml-workerscript`

Depending on what kind of distribution you are using, it could be necessary to install:
* `libgl1-mesa-dev`

To build and use the Python bindings:

* `libpython3-dev`

### **Deployment**

Depending on your build configuration, you may need to install the following libraries on the target system (tested on Debian 10):

* Google Protocol Buffers library (3.0 or newer)
* Google gRPC library (1.16 or newer)
* zlib (needed by gRPC)
* Qt 5 (5.3 or newer) if you have Qt 5.2 or lower, screen capture can still work with limited functionality. Please contact https://www.teamviewer.com/en/global/contact-sales/ to start a discussion about your project.

### System Requirements

* For supported platforms see https://www.teamviewer.com/link/?url=150157
* At least 200 MB free disk space
* Systemd software suite
* Glibc-utils package (version 2.17 or higher)
* Linux 2.6.27 kernel

### IoT Agent version

To use the full functionality set of the C++ API, you need IoT Agent version 2.21.13 or higher. Prior versions of the Agent do not fully support the Session Management feature (session start/stop notifications, enumerating running sessions).

The functionality set of the Qt API is not affected.

## Building and Setup

### Steps to build

```bash
# create and navigate to a build directory
mkdir build
cd build

# run CMake configure step
cmake ../source

# run make
make
```

Building the package will produce the following binaries (depending on the configure options used):

Libraries:
* `libTVAgentApi.so` : The C++ API shared library
* `libTVQtRC.so` : The Qt API shared library (optional, ON by default)
* `CommunicationLayer/src/<Xyz>/Library/lib<Xyz>.a` : Individual IPC-level components
* `Bindings/Python/tvagentapi.so` : The Python library (optional, OFF by default)

Example Applications:
* `examples/example_<Abc>` : The C++ API example apps (command line)
* `qt_simulate` : The Qt API example app (GUI)

### Options for the CMake configure step

Disable building the Qt API + example app:

```bash
cmake -DENABLE_QT_PLUGIN=OFF <path-to-source>/
```

Configure for a Qt6 environment:

```bash
cmake -DQT_MAJOR_VERSION=6 <path-to-source>/
```

Enable building the Python bindings:

```bash
cmake -DENABLE_PYTHON_MODULE=ON <path-to-source>/
```

### Installation for development

You can install the SDK for use in a development environment:

```bash
# default installation
make install
```
This will install the `.so` and `.a` files (by default to `/usr/local`), along with the public header files needed to use them (`/usr/local/include/*.h`).

To install the SDK to a custom location, override `CMAKE_INSTALL_PREFIX` before building:

```bash
# configure to install into custom prefix
cmake -DCMAKE_INSTALL_PREFIX=<custom-install-path> <path-to-sources-root>
```

### Configuration

👉 At installation time, the IoT Agent creates the `tv_api` group. To communicate with the Agent, your application must run as a user that is part of this group:

```bash
# add current user to the 'tv_api' group
usermod -a -G tv_api $USER
# important: to ensure this takes effect, remember to logout/login (or open a new console/SSH session)
```

If your application wants to handle screen grabbing and receive keyboard/mouse input, the IoT Agent needs to be configured accordingly (this functonality in the Agent is off by default):

```bash
teamviewer-iot-agent configure set EnableRemoteScreen 1
```

The Agent can grab the screen by itself, or rely on your external app to provide screen updates. This is controlled by the `RemoteScreenChannels` property.

### Agent built-in grabbing

Learn about the various channel configurations available: https://www.teamviewer.com/link/?url=710439

### External app-assisted grabbing

#### External app grabs screen data and sends to Agent:

```bash
teamviewer-iot-agent configure set RemoteScreenChannels 'EAP'
```

#### External app notifies Agent when screen has changed; Agent then grabs screen:

```bash
teamviewer-iot-agent configure set RemoteScreenChannels 'EAP:FBPush'
```

#### External app is not involved; Agent grabs screen:

```bash
teamviewer-iot-agent configure set RemoteScreenChannels 'EAP:FBPull'
```

👉 The IoT Agent will default to the first Linux framebuffer device, usually `/dev/fb0`. You can specify a custom device with `FBPush` or `FBPull`:

```bash
teamviewer-iot-agent configure set RemoteScreenChannels 'EAP:FBPush:/dev/fb42'
```

#### [Wayland-specific] External app displays running session indicator; Agent grabs screen

```bash
teamviewer-iot-agent configure set RemoteScreenChannels 'EAP:WL:/run/user/1000/wayland-0'
```

## Qt-Plugin Runtime

During runtime the Qt-Plugin's behavior can be influenced in several ways. Besides the environment variables from Qt and the QGuiApplications flags there are ways to influence the performance, CPU-load etc.

### Environment Variables

```bash
TV_SDK_QT_FAVOUR_QTQUICK_GRAB = 1
```
By setting TV_SDK_QT_FAVOUR_QTQUICK_GRAB in the process' environment the QQuickWindow::grabWindow() can be requested explicitly for grabbing window contents on platforms other than EGLFS.

Background:
QQuickWindow::grabWindow() imposes a huge performance penalty for the rendering thread. This penalty could render a QtQuick based UI pretty much unusable - especially if there are animations. On the EGLFS platform QQuickWindow::grabWindow is although favoured as there a QQuickWindow cannot be grabbed using QScreen::grabWindow.

```bash
TV_SDK_QT_FORCE_INTERVAL_GRAB = 1
```
By setting TV_SDK_QT_FORCE_INTERVAL_GRAB in the process' environment even if event-based window grabbing is possible it will be ignored and its fallback (interval-based window grabbing) is chosen.

```bash
TV_SDK_QT_GRABS_PER_SECOND = 10
```
By setting TV_SDK_QT_GRABS_PER_SECOND in the process' environment the number of window grabs per second can be adjusted (default is 25).

With a lower setting the CPU-load might be a lot less but the remote control performance will be decreased. It is recommended to choose a lower number for use cases where animations etc. are not important and CPU-time is limited.

## Creating Access Tokens for Instant Support

In order to request Instant Support, your application will need an access token (such as `"12345678-LgxKf0bybuAESdNIelrY"`) which uniquely identifies the remote supporter (Note: not a TeamViewer ID). A supporter will create such tokens under their account and communicate them to you.

As a supporter:

* Navigate to https://login.teamviewer.com/
* Log in to your account
* In the upper right corner, click the dropdown box with your name and select "Edit profile"
* In the "Profile settings" list, select "Apps"
* In the section that opens, click "Create script token"
* In the menu that opens, enter a name for your token (e.g., "My Instant Support Token") and an optional description
* Open the "Session management" dropdown and check all boxes
* Click "Create"
* Open the newly created script token, copy the string value, and communicate it to your users or embed it into your application.

## Links & References

By using the Agent SDK you also accept the TeamViewer IoT EULA. Please find the full version of the IoT EULA here => http://www.teamviewer.com/link/?url=653670

You can also check our documentation space for further information => https://www.teamviewer.com/link/?url=362507

## License

For licensing information see [LICENSE](./LICENSE).

## Author

TeamViewer Germany GmbH https://www.teamviewer.com/en/support/contact
