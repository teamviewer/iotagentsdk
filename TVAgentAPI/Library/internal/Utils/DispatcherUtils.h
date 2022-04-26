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

#include "AsyncOperation/IDispatcher.h"

#include <memory>
#include <utility>

namespace tvagentapi
{
namespace util
{

template <typename T, typename F>
void dispatcherPost(IDispatcher *dispatcher, std::weak_ptr<T> weakInstance, F&& func)
{
	dispatcher->post(
		[weakInstance = std::move(weakInstance), f = std::forward<F>(func)]()
		{
			if (auto instance = weakInstance.lock())
			{
				f(std::move(instance));
			}
		});
}

template <typename T, typename F>
void weakDispatcherPost(std::weak_ptr<IDispatcher> weakDispatcher, std::weak_ptr<T> weakInstance, F&& func)
{
	if (auto dispatcher = weakDispatcher.lock())
	{
		dispatcherPost(dispatcher.get(), std::move(weakInstance), std::forward<F>(func));
	}
}

} // namespace util
} // namespace tvagentapi
