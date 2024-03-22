//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2022 TeamViewer Germany GmbH                                     //
//                                                                                //
// Permission is hereby granted, free of charge, to any person obtaining a copy   //
// of this software and associated documentation files (the "Software"), to deal  //
// in the Software without restriction, including without limitation the rights   //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      //
// copies of the Software, and to permit persons to whom the Software is          //
// furnished to do so, subject to the following conditions:                       //
//                                                                                //
// The above copyright notice and this permission notice shall be included in all //
// copies or substantial portions of the Software.                                //
//                                                                                //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  //
// SOFTWARE.                                                                      //
//********************************************************************************//
#pragma once

#include <TVAgentAPI/IModule.h>
#include <TVAgentAPI/Callback.h>

#include <cstdint>

namespace tvagentapi
{

class IAgentConnection
{
public:
	enum class Status : int32_t
	{
		Disconnected = 0,	// initial state - no connection to the IoT Agent
		Disconnecting,		// connection to the IoT Agent is shutting down
		Connecting,			// trying to establish the connection to the IoT Agent
		Connected,			// connection to the IoT Agent is successfully established
		ConnectionLost,		// connectivity check with the IoT Agent timed out
	};

	enum class SetConnectionURLsResult : int32_t
	{
		Success = 0,					// no error
		CharacterLimitForPathExceeded,	// resulting url for services is too long
		SchemaNotValid,					// given URL format is not supported or mismatched
		ConnectionIsInUse,				// you cannot change URLs in the active connection. stop() it first.
	};

	virtual ~IAgentConnection() = default;

	/**
	 * @brief setConnectionURLs sets a custom base URL where the plugin will host its server sockets
	 * and agent URL which locates the IoT Agent API entry point.
	 * If never called, by default @p baseSdkUrl is @c unix:///tmp or @c tcp+tv://127.0.0.1
	 * and @p agentApiUrl is @c unix:///tmp/teamviewer-iot-agent-services/remoteScreen/registrationService or @c tcp+tv://127.0.0.1:9221
	 * depending on which flags the SDK has been built with.
	 * Check TV_COMM_ENABLE_GRPC and TV_COMM_ENABLE_PLAIN_SOCKET CMake options.
	 * The gRPC version is always preferable over TCP once if compiled with both options enabled (by default both are ON).
	 * If you call this method, you should do so once early on after the plugin has been loaded,
	 * before any [register*] methods.
	 * Affects all subsequently initiated communication sessions.
	 * This functions checks if the base URL in combination with the created sockets names, will exceed the limit for socket names.
	 * If the limit is reached, the function will return a corresponding error code and the SDK will not use the provided path.
	 * In that case, please provide a shorter path to this function.
	 * Apart from that it checks consistency and validity of the URLs given (e.g. if the schemes differ, rejects the arguments).
	 * @param baseSdkUrl the new base URL
	 * @param agentApiUrl path to agent API entry point
	 * @return result of URLs' change as enum value, see @p SetConnectionURLsResult
	 */
	virtual SetConnectionURLsResult setConnectionURLs(const char* baseSdkUrl, const char* agentApiUrl) = 0;

	/**
	 * @brief start tries to establish a connection to a running TeamViewer IoT Agent and maintains it until stop() is called.
	 * The connection is re-established automatically if it was lost.
	 *
	 * To be notified about the connectivity set the status callback below.
	 */
	virtual void start() = 0;

	/**
	 * @brief stop shuts down all communication with the TeamViewer IoT Agent and changes its state after that to Disconnected.
	 */
	virtual void stop() = 0;

	/**
	 * @brief getStatus returns the current status of the connection
	 */
	virtual Status getStatus() const = 0;

	using StatusChangedCallback = Callback<void(Status newStatus, void* userdata) noexcept>;

	/**
	 * @brief setStatusChangedCallback can be used to set a single status callback
	 * every status change calls the callback
	 */
	virtual void setStatusChangedCallback(StatusChangedCallback callback) = 0;

	/**
	 * @brief processEvents is responsible to execute all actions/events/etc. (or simply: "events") that are pending in order they are issued:
	 * - triggers user's callbacks
	 * - updates internal states
	 * - notifies modules
	 * - handles modules' requests
	 * The method can optionally wait for more events if no events queued.
	 * @param waitForMoreEvents if true and no queued events, wait for events to be queued,
	 * otherwise process the queued events immediately.
	 * @param timeoutMs wait timeout in milliseconds. If zero, wait infinitely.
	 * @return true if at least one action has been processed
	 */
	virtual bool processEvents(bool waitForMoreEvents = false, uint32_t timeoutMs = 0) = 0;

	/**
	 * @brief getModule returns an abstract module if possible for the given type.
	 * @param moduleType type of a module to create.
	 * NOTE: the module's lifetime is bound to IAgentConnection lifetime.
	 * @return a pointer to the module or nullptr in case of an error.
	 */
	virtual IModule* getModule(IModule::Type moduleType) = 0;
};

/**
 * @brief getModule provides a convenient way to return a pointer to a specific kind of module type from the given connection.
 * @param conn pointer to an IAgentConnection's implementation.
 * @return pointer to the module if available, nullptr otherwise
 */
template<class ModuleT>
ModuleT* getModule(IAgentConnection* conn)
{
	return conn ? static_cast<ModuleT*>(conn->getModule(ModuleT::TypeValue)) : nullptr;
}

} // namespace tvagentapi
