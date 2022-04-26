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
#include "LazyDispatcher.h"

#include <chrono>

namespace tvagentapi
{

void LazyDispatcher::post(Action action)
{
	std::lock_guard<std::mutex> lock{m_mutex};
	m_actions.emplace_back(std::move(action));
	m_hasActions.notify_one();
}

bool LazyDispatcher::processActions(bool waitForMoreEvents, uint32_t timeoutMs)
{
	Actions actions{};
	{
		std::unique_lock<std::mutex> lock{m_mutex};
		if (waitForMoreEvents && m_actions.empty())
		{
			if (timeoutMs == 0)
			{
				m_hasActions.wait(lock);
			}
			else
			{
				const std::cv_status status =
					m_hasActions.wait_for(lock, std::chrono::milliseconds{timeoutMs});
				if (status == std::cv_status::timeout)
				{
					return false;
				}
			}
		}
		std::swap(m_actions, actions);
	}
	for (const auto& action : actions)
	{
		action();
	}
	return !actions.empty();
}

} // namespace tvagentapi
