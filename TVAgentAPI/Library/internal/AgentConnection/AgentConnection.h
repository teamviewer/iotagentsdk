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

#include <TVAgentAPI/prototypes.h>
#include <TVAgentAPI/IAgentConnection.h>
#include <TVAgentAPIPrivate/Observer.h>

#include <map>
#include <memory>

namespace tvagentapi
{

class CommunicationChannel;
class IDispatcher;
class LazyDispatcher;
class LoggingPrivateAdapter;
class LoggingProxy;

class AgentConnection final : public IAgentConnection
{
public:
	static std::shared_ptr<AgentConnection> Create(ILogging* logging, std::string registrationServiceLocation);
	~AgentConnection() override;

	void start() override;
	void stop() override;
	Status getStatus() const override;
	void setStatusChangedCallback(StatusChangedCallback callback) override;
	bool processEvents(bool waitForMoreEvents, uint32_t timeoutMs) override;
	IModule* getModule(IModule::Type moduleType) override;

	// Non virtual, doesn't leak out to user facing IAgentConnection
	std::shared_ptr<CommunicationChannel> getCommunicationChannel() const;
	std::shared_ptr<IDispatcher> getDispatcher() const;
	void setLogging(ILogging* logging);

private:
	AgentConnection(
		std::shared_ptr<CommunicationChannel> communicationChannel,
		std::shared_ptr<LazyDispatcher> dispatcher,
		std::shared_ptr<LoggingPrivateAdapter> loggingPrivateAdapter);
	void setStatusPostNotify(Status status);

	std::map<IModule::Type, std::shared_ptr<IModule>> m_modules;
	std::shared_ptr<CommunicationChannel> m_communicationChannel;
	std::shared_ptr<LazyDispatcher> m_dispatcher;
	std::shared_ptr<LoggingPrivateAdapter> m_loggingPrivateAdapter;
	std::weak_ptr<AgentConnection> m_weakThis;

	ObserverConnection m_agentCommunicationEstablishedConnection;
	ObserverConnection m_agentCommunicationLostConnection;
	IAgentConnection::StatusChangedCallback m_statusChangedCallback;
	IAgentConnection::Status m_status = IAgentConnection::Status::Disconnected;
};

} // namespace tvagentapi
