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
#include "AgentConnection.h"

#include <TVAgentAPIPrivate/CommunicationChannel.h>

#include "AsyncOperation/LazyDispatcher.h"
#include "Logging/LoggingPrivateAdapter.h"
#include "Utils/CallbackUtils.h"
#include "Utils/DispatcherUtils.h"
#include "ModuleFactory.h"

#include <assert.h>

namespace tvagentapi
{

std::shared_ptr<AgentConnection> AgentConnection::Create(ILogging* logging, std::string registrationServiceLocation)
{
	auto loggingPrivateAdapter = std::make_shared<LoggingPrivateAdapter>(logging);

	auto communicationChannel = CommunicationChannel::Create(
		loggingPrivateAdapter,
		std::move(registrationServiceLocation));

	auto dispatcher = std::make_shared<LazyDispatcher>();

	if (communicationChannel && dispatcher && loggingPrivateAdapter)
	{
		if (auto connection = std::shared_ptr<AgentConnection>(
			new AgentConnection(
				std::move(communicationChannel),
				std::move(dispatcher),
				std::move(loggingPrivateAdapter))))
		{
			connection->m_weakThis = connection;
			return connection;
		}
	}
	return nullptr;
}

AgentConnection::AgentConnection(
	std::shared_ptr<CommunicationChannel> communicationChannel,
	std::shared_ptr<LazyDispatcher> dispatcher,
	std::shared_ptr<LoggingPrivateAdapter> loggingPrivateAdapter)
	: m_communicationChannel(std::move(communicationChannel))
	, m_dispatcher(std::move(dispatcher))
	, m_loggingPrivateAdapter(std::move(loggingPrivateAdapter))
{
	assert(m_communicationChannel && m_dispatcher && m_loggingPrivateAdapter);
}

AgentConnection::~AgentConnection()
{
	// reset ILogging* inside logging proxy, in case it's retain in some threads of CommunicationChannel
	setLogging(nullptr);
}

void AgentConnection::start()
{
	if (m_status == Status::Connecting || m_status == Status::Connected || m_status == Status::ConnectionLost)
	{
		return;
	}

	auto weakDispatcher = std::weak_ptr<IDispatcher>{m_dispatcher};

	m_agentCommunicationEstablishedConnection = m_communicationChannel->agentCommunicationEstablished().registerCallback(
		[weakDispatcher, weakThis = m_weakThis]()
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[](const auto& self)
				{
					util::safeCall(self->m_statusChangedCallback, Status::Connected);
				});
		});

	m_agentCommunicationLostConnection = m_communicationChannel->agentCommunicationLost().registerCallback(
		[weakDispatcher, weakThis = m_weakThis]()
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[](const auto& self)
				{
					util::safeCall(self->m_statusChangedCallback, Status::ConnectionLost);
				});
		});

	setStatusPostNotify(Status::Connecting);

	m_communicationChannel->startup();
}

void AgentConnection::stop()
{
	if (m_status == Status::Disconnected || m_status == Status::Disconnecting)
	{
		return;
	}

	m_agentCommunicationEstablishedConnection.disconnect();
	m_agentCommunicationLostConnection.disconnect();

	setStatusPostNotify(Status::Disconnecting);
	m_communicationChannel->shutdown();
	// CommuncationChannel doesn't provide any notification when we are disconnected, so trigger it immediately
	setStatusPostNotify(Status::Disconnected);
}

IAgentConnection::Status AgentConnection::getStatus() const
{
	return m_status;
}

void AgentConnection::setStatusChangedCallback(IAgentConnection::StatusChangedCallback callback)
{
	m_statusChangedCallback = callback;
}

bool AgentConnection::processEvents(bool waitForMoreEvents, uint32_t timeoutMs)
{
	return m_dispatcher->processActions(waitForMoreEvents, timeoutMs);
}

std::shared_ptr<CommunicationChannel> AgentConnection::getCommunicationChannel() const
{
	return m_communicationChannel;
}

std::shared_ptr<IDispatcher> AgentConnection::getDispatcher() const
{
	return m_dispatcher;
}

void AgentConnection::setLogging(ILogging* logging)
{
	m_loggingPrivateAdapter->getLoggingProxy().setLogging(logging);
}

void AgentConnection::setStatusPostNotify(Status status)
{
	m_status = status;
	util::dispatcherPost(
		m_dispatcher.get(),
		m_weakThis,
		[status](const auto& self)
		{
			util::safeCall(self->m_statusChangedCallback, status);
		});
}

IModule* AgentConnection::getModule(IModule::Type moduleType)
{
	auto& module = m_modules[moduleType];
	if (!module)
	{
		module = CreateModule(moduleType, m_weakThis);
	}
	return module.get();
}

} // namespace tvagentapi
