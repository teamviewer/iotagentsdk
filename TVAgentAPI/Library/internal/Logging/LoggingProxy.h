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

#include <TVAgentAPI/ILogging.h>
#include <mutex>

namespace tvagentapi
{

/// @breif Safe LoggingProxy
/// Internal logging object is owned by the user as raw pointer, we don't manage it's lifetime
class LoggingProxy final : public ILogging
{
public:
	explicit LoggingProxy(ILogging* logging);
	~LoggingProxy() override = default;

	void setLogging(ILogging* logging);

	// ILogging
	void logInfo(const char* info) override;
	void logError(const char* error) override;

	LoggingProxy(const LoggingProxy&) = delete;
	LoggingProxy& operator=(const LoggingProxy&) = delete;
private:
	std::mutex m_mutex;
	ILogging* m_logging;
};

} // namespace tvagentapi
