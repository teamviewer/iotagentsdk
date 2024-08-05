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
#include "TVSessionManagementModule.h"

#include <TVAgentAPIPrivate/CommunicationChannel.h>

#include "AgentConnection/AgentConnection.h"
#include "AsyncOperation/IDispatcher.h"
#include "Utils/CallbackUtils.h"
#include "Utils/DispatcherUtils.h"
#include "ModuleFactory.h"

#include <assert.h>

namespace tvagentapi
{

template <>
std::shared_ptr<IModule> CreateModule<IModule::Type::TVSessionManagement>(
	std::weak_ptr<AgentConnection> connection)
{
	return TVSessionManagementModule::Create(std::move(connection));
}

std::shared_ptr<TVSessionManagementModule> TVSessionManagementModule::Create(std::weak_ptr<AgentConnection> connection)
{
	std::shared_ptr<TVSessionManagementModule> instance{new TVSessionManagementModule(std::move(connection))};
	instance->m_weakThis = instance;

	if (instance->registerCallbacks())
	{
		return instance;
	}
	else
	{
		return{};
	}
}

void TVSessionManagementModule::setCallbacks(const Callbacks& callbacks)
{
	m_callbacks = callbacks;
}

bool TVSessionManagementModule::isSupported() const
{
	if (auto connection = m_connection.lock())
	{
		// TODO IOT-15139 implement IModule::isSupported()
		// return connection->isModuleSupported("TVSessionManagementModule"); // str or enum
		return true;
	}
	return false;
}

bool TVSessionManagementModule::enumerateRunningSessions(ITVSessionManagementModule::SessionEnumerator enumerator) const
{
	if (!enumerator.callback)
	{
		return false;
	}

	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	auto commChannel = connection->getCommunicationChannel();

	std::vector<ITVSessionManagementModule::TVSessionID> sessionIds;
	if (!commChannel->sendGetRunningTVSessions(sessionIds))
	{
		return false;
	}

	for (ITVSessionManagementModule::TVSessionID sessionID : sessionIds)
	{
		if (!enumerator.callback(sessionID, enumerator.userdata))
		{
			break;
		}
	}

	return true;
}

bool TVSessionManagementModule::terminateTeamViewerSessions()
{
	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	auto communicationChannel = connection->getCommunicationChannel();
	return communicationChannel->sendStop();
}

TVSessionManagementModule::TVSessionManagementModule(std::weak_ptr<AgentConnection> connection)
	: m_connection(std::move(connection))
{
}

bool TVSessionManagementModule::registerCallbacks()
{
	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	auto communicationChannel = connection->getCommunicationChannel();
	auto weakDispatcher = std::weak_ptr<IDispatcher>{connection->getDispatcher()};

	const auto weakThis = m_weakThis;
	m_tvSessionStartedConnection = communicationChannel->tvSessionStarted().registerCallback(
		[weakThis, weakDispatcher]
		(int32_t tvSessionID, int32_t tvSessionsCount) noexcept
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[tvSessionID, tvSessionsCount](const std::shared_ptr<TVSessionManagementModule>& self)
				{
					util::safeCall(self->m_callbacks.sessionStartedCallback, tvSessionID, tvSessionsCount);
				});
		});

	m_tvSessionStoppedConnection = communicationChannel->tvSessionStopped().registerCallback(
		[weakThis, weakDispatcher]
		(int32_t tvSessionID, int32_t tvSessionsCount) noexcept
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[tvSessionID, tvSessionsCount](const std::shared_ptr<TVSessionManagementModule>& self)
				{
					util::safeCall(self->m_callbacks.sessionStoppedCallback, tvSessionID, tvSessionsCount);
				});
		});

	return true;
}

} // namespace tvagentapi
