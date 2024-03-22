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
#include "AgentConnectionProxy.h"
#include "AgentConnection.h"

namespace tvagentapi
{

AgentConnectionProxy::AgentConnectionProxy(std::shared_ptr<AgentConnection> connection)
	: m_connection(std::move(connection))
{
}

IAgentConnection::SetConnectionURLsResult AgentConnectionProxy::setConnectionURLs(
	const char* baseSdkURL,
	const char* agentAPIURL)
{
	return m_connection->setConnectionURLs(baseSdkURL, agentAPIURL);
}

void AgentConnectionProxy::start()
{
	return m_connection->start();
}

void AgentConnectionProxy::stop()
{
	return m_connection->stop();
}

IAgentConnection::Status AgentConnectionProxy::getStatus() const
{
	return m_connection->getStatus();
}

void AgentConnectionProxy::setStatusChangedCallback(StatusChangedCallback callback)
{
	m_connection->setStatusChangedCallback(callback);
}

bool AgentConnectionProxy::processEvents(bool waitForMoreEvents, uint32_t timeoutMs)
{
	return m_connection->processEvents(waitForMoreEvents, timeoutMs);
}

IModule* AgentConnectionProxy::getModule(IModule::Type moduleType)
{
	return m_connection->getModule(moduleType);
}

std::shared_ptr<AgentConnection> AgentConnectionProxy::getConnection() const
{
	return m_connection;
}

} // namespace tvagentapi
