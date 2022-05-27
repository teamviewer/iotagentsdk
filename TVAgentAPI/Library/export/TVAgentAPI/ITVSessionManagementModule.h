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

#include "IModule.h"
#include <TVAgentAPI/Callback.h>
#include <cstdlib>

namespace tvagentapi
{

class ITVSessionManagementModule : public IModule
{
public:
	/// Type value for convenience
	static constexpr Type TypeValue = Type::TVSessionManagement;

	using TVSessionID = int32_t;

	using SessionCallback = Callback<void(TVSessionID tvSessionID, int32_t tvSessionsCount, void* userdata) noexcept>;

	struct Callbacks
	{
		SessionCallback sessionStartedCallback;
		SessionCallback sessionStoppedCallback;
	};

	~ITVSessionManagementModule() override = default;

	/**
	 * @brief setCallbacks sets callbacks to signal when an incoming TeamViewer session has started or stopped.
	 * @param callbacks Might be partially or completely omitted, by providing default constructed Callback {}
	 */
	virtual void setCallbacks(const Callbacks& callbacks) = 0;

	/**
	 * @brief Session ID enumerator callback.
	 * When the enumerator returns true the iteration continues on false the iteration stops at this point.
	 */
	using SessionEnumerator = Callback<bool(TVSessionID tvSessionID, void* userdata) noexcept>;
	/**
	 * @brief enumerateRunningSessions iterates over the currently running TeamViewer sessions.
	 * This function blocks until the enumeration is done and calls the given enumerator synchronously during this
	 * process.
	 * When the enumerator returns true the iteration continues on false the iteration stops at this point.
	 * @param enumerator given callback to handle each running session.
	 * @return true if the enumeration finished without any internal error, false otherwise
	 */
	virtual bool enumerateRunningSessions(SessionEnumerator enumerator) const = 0;

	/**
	 * @brief terminateTeamViewerSessions requests to terminate all incoming TeamViewer remote screen sessions
	 * @return true if the request finished without any internal error, false otherwise
	 */
	virtual bool terminateTeamViewerSessions() = 0;
};

} // namespace tvagentapi
