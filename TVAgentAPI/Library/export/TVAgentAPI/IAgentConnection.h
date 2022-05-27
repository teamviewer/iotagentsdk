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
		Disconnecting,		// connection to the IoT Agent is shuting down
		Connecting,			// trying to establish the connection to the IoT Agent
		Connected,			// connection to the IoT Agent is sucessfully established
		ConnectionLost,		// connectivity check with the IoT Agent timed out
	};

	virtual ~IAgentConnection() = default;

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
	 * @brief processEvents is responsible to execute all actions/events/etc. (or simply: "events") that are pendning in order they are issued:
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
