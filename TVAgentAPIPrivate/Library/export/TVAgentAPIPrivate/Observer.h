//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2021 TeamViewer Germany GmbH                                     //
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

#include <functional>
#include <list>
#include <memory>
#include <mutex>

namespace tvagentapi
{

class ObserverConnection final
{
public:
	ObserverConnection() = default;

	template<typename F>
	ObserverConnection(F&& f)
		: m_teardownFunc(std::forward<F>(f))
	{
	}

	void disconnect() noexcept
	{
		if (m_teardownFunc)
		{
			m_teardownFunc();
		}
		m_teardownFunc = nullptr;
	}

	~ObserverConnection()
	{
		if (m_teardownFunc)
		{
			m_teardownFunc();
		}
	}

	ObserverConnection(const ObserverConnection&) = delete;
	ObserverConnection& operator=(const ObserverConnection&) = delete;

	ObserverConnection(ObserverConnection&& other)
	{
		m_teardownFunc.swap(other.m_teardownFunc);
	}

	ObserverConnection& operator=(ObserverConnection&& other)
	{
		disconnect();
		m_teardownFunc.swap(other.m_teardownFunc);
		return *this;
	}

private:
	std::function<void()> m_teardownFunc;
};

template <typename Func>
class Observer final
{
public:
	Observer() : m_observerControl(std::make_shared<ObserverControl>())
	{
	};

	template <typename F>
#if __cplusplus >= 201703L // >= C++17
	[[nodiscard]]
#endif
	ObserverConnection registerCallback(F&& cb)
	{
		std::lock_guard<std::mutex> lk(m_observerControl->accessMtx);
		m_observerControl->callbacks.emplace_back(std::forward<F>(cb));

		return ObserverConnection
		{
			[oC = std::weak_ptr<ObserverControl>(m_observerControl), it = --m_observerControl->callbacks.end()]()
			{
				if (const auto obsCtrl = oC.lock())
				{
					std::lock_guard<std::mutex> lock(obsCtrl->accessMtx);
					obsCtrl->callbacks.erase(it);
				}
			}
		};
	}

	template<typename... Args>
	void notifyAll(Args&&... arg)
	{
		std::lock_guard<std::mutex> lk(m_observerControl->accessMtx);
		for (const auto& cb : m_observerControl->callbacks)
		{
			cb(arg...);
		}
	}

private:
	struct ObserverControl
	{
		std::list<std::function<Func>> callbacks;
		std::mutex accessMtx;
	};

	std::shared_ptr<ObserverControl> m_observerControl;
};

} // namespace tvagentapi

