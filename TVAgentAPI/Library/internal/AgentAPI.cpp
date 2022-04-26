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
#include "AgentAPI.h"
#include "AccessControl/AccessControlModule.h"
#include "AgentConnection/AgentConnection.h"
#include "AgentConnection/AgentConnectionProxy.h"
#include "InstantSupport/InstantSupportModule.h"
#include "TVSessionManagement/TVSessionManagementModule.h"

#include "Logging/FileLogging.h"

#include <assert.h>

namespace
{
constexpr const char* DefaultRegistrationServiceLocation = "teamviewer-iot-agent-services/remoteScreen/registrationService";
} // namespace

namespace tvagentapi
{

IAgentConnection* AgentAPI::createAgentConnectionLocal(ILogging* logging/* = nullptr*/)
{
	auto sharedConnection = std::shared_ptr<AgentConnection>(
		AgentConnection::Create(logging, DefaultRegistrationServiceLocation));

	if (!sharedConnection)
	{
		return nullptr;
	}

	return new AgentConnectionProxy(std::move(sharedConnection));
}

void AgentAPI::destroyAgentConnection(IAgentConnection* connection)
{
	if (auto proxy = static_cast<AgentConnectionProxy*>(connection))
	{
		proxy->getConnection()->setLogging(nullptr);
		delete proxy;
	}
}

tvagentapi::ILogging* AgentAPI::createFileLogging(const char* logFilePath)
{
	FileLogging* fileLogging = new FileLogging();
	if (fileLogging && fileLogging->startLogging(logFilePath))
	{
		return fileLogging;
	}
	delete fileLogging;
	return nullptr;
}

void AgentAPI::destroyLogging(tvagentapi::ILogging* logging)
{
	delete logging;
}

} // namespace tvagentapi
