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

#include "prototypes.h"

namespace tvagentapi
{

class IAgentAPI
{
public:
	virtual ~IAgentAPI() = default;

	/**
	 * @brief createAgentConnectionLocal creates an instance for connection to a running instance of TeamViewer IoT Agent
	 * The returned connection is to be used in subsequent calls to create functionality modules (e.g. Instant Support, TV Session Management, Remote Control, etc.).
	 * @param logging would be used internaly for logging, logging ownership remains on caller side and is not transferred to the connection. If logging is nullptr no logging happens.
	 * The caller must ensure that ILogging is valid before at any moment until destroyAgentConnection() is called.
	 * NOTE: The ownership of the created connection is tranferred to the caller. Use destroyAgentConnection() to delete created connection
	 * @return a pointer to the connection or nullptr in case of a resource error.
	 */
	virtual IAgentConnection* createAgentConnectionLocal(ILogging* logging = nullptr) = 0;
	/**
	 * @brief destroyAgentConnection destroys an instance of IAgentConnection created with createAgentConnection()
	 * NOTE: After this call no more logging happens.
	 */
	virtual void destroyAgentConnection(IAgentConnection* connection) = 0;

	/**
	 * @brief createFileLogging creates simple file logging object that creates a file and appends logs to it.
	 * FileLogging is thread safe and its methods can be safely called from user code.
	 * NOTE: The ownership of created logging is transferred to the caller. Use destroyLogging() to delete created logging object
	 * @return a pointer to file logging object or nullptr if log file could not be opened
	 */
	virtual ILogging* createFileLogging(const char* logFilePath) = 0;
	/**
	 * @brief destroyLogging destroys an instance of ILogging created with createFileLogging()
	 * NOTE: Must be called only after destroyAgentConnection()
	 */
	virtual void destroyLogging(ILogging* logging) = 0;
};

} // namespace tvagentapi
