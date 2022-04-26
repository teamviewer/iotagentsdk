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

#include "IDispatcher.h"

#include <condition_variable>
#include <deque>
#include <mutex>

namespace tvagentapi
{

class LazyDispatcher final : public IDispatcher
{
public:
	template<typename ActionType>
	void post(ActionType action)
	{
		static_assert(noexcept(action()), "Action must be noexcept");
		post(Action{std::move(action)});
	}

	/**
	 * @brief process all queued actions.
	 * Optionally wait for actions if no actions queued.
	 * @param waitForMoreEvents if true and no queued actions, wait for actions to be queued,
	 * otherwise process the queued actions immediately.
	 * @param timeoutMs wait timeout in milliseconds. If zero, wait infinitely.
	 * @return true if at least one action has been processed
	 */
	bool processActions(bool waitForMoreEvents = false, uint32_t timeoutMs = 0);

private:
	using Actions = std::deque<Action>;

	void post(Action action) override;

	std::mutex m_mutex;
	std::condition_variable m_hasActions;
	Actions m_actions;
};

} // namespace tvagentapi
