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
#include "FileLogging.h"

namespace tvagentapi
{

namespace
{
constexpr const char* LogPrefixError = "[error] ";
constexpr const char* LogPrefixInfo  = "[info] ";
}

bool FileLogging::startLogging(const std::string& logFilePath)
{
	std::ofstream logger;
	logger.open(logFilePath, std::ios::app);

	if (!logger.good())
	{
		return false;
	}

	std::lock_guard<std::mutex> lock(m_logMutex);
	m_stream = std::move(logger);
	return true;
}

void FileLogging::stopLogging()
{
	shutdown();
}

FileLogging::~FileLogging()
{
	shutdown();
}

void FileLogging::shutdown()
{
	std::lock_guard<std::mutex> lock(m_logMutex);

	if (m_stream)
	{
		m_stream.flush();
		m_stream.close();
	}
}

void FileLogging::logError(const char* message)
{
	if (!message)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(m_logMutex);
	if (!m_stream.good())
	{
		return;
	}

	m_stream << LogPrefixError << message << "\n";

	m_stream.flush();
}

void FileLogging::logInfo(const char* message)
{
	if (!message)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(m_logMutex);
	if (!m_stream.good())
	{
		return;
	}

	m_stream << LogPrefixInfo << message << "\n";

	m_stream.flush();
}
} // namespace tvagentapi
