//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2024 TeamViewer Germany GmbH                                     //
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
#include "AugmentRCSessionModule.h"

#include <TVAgentAPIPrivate/CommunicationChannel.h>

#include "AgentConnection/AgentConnection.h"
#include "AsyncOperation/IDispatcher.h"
#include "Utils/CallbackUtils.h"
#include "Utils/DispatcherUtils.h"
#include "ModuleFactory.h"

#include <assert.h>

namespace tvagentapi
{

template<>
std::shared_ptr<IModule> CreateModule<IModule::Type::AugmentRCSession>(
	std::weak_ptr<AgentConnection> connection)
{
	return AugmentRCSessionModule::Create(std::move(connection));
}

std::shared_ptr<AugmentRCSessionModule> AugmentRCSessionModule::Create(std::weak_ptr<AgentConnection> connection)
{
	std::shared_ptr<AugmentRCSessionModule> instance{new AugmentRCSessionModule(std::move(connection))};
	instance->m_weakThis = instance;
	if (!instance->registerCallbacks())
	{
		return {};
	}
	return instance;
}

AugmentRCSessionModule::AugmentRCSessionModule(std::weak_ptr<AgentConnection> connection)
	: m_connection(std::move(connection))
{
}

void AugmentRCSessionModule::setCallbacks(const Callbacks& callbacks)
{
	m_callbacks = callbacks;
}

bool AugmentRCSessionModule::augmentRCSessionStartListening()
{
	auto connection = m_connection.lock();

	if (!connection)
	{
		return false;
	}

	return connection->getCommunicationChannel()->sendAugmentRCSessionStartListening();
}

bool AugmentRCSessionModule::augmentRCSessionStopListening()
{
	auto connection = m_connection.lock();

	if (!connection)
	{
		return false;
	}

	return connection->getCommunicationChannel()->sendAugmentRCSessionStopListening();
}

bool AugmentRCSessionModule::isSupported() const
{
	if (auto connection = m_connection.lock())
	{
		// TODO IOT-15139 implement IModule::isSupported()
		// return connection->isModuleSupported("AugmentRCSessionModule"); // str or enum
		return true;
	}
	return false;
}

bool AugmentRCSessionModule::registerCallbacks()
{
	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	auto communicationChannel = connection->getCommunicationChannel();
	auto weakDispatcher = std::weak_ptr<IDispatcher>{connection->getDispatcher()};

	const auto weakThis = m_weakThis;
	m_ReceivedInvitationCallback = communicationChannel->augmentRCSessionInvitationReceived().registerCallback(
		[weakThis, weakDispatcher](std::string url) noexcept
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[url](const std::shared_ptr<AugmentRCSessionModule>& self)
				{
					util::safeCall(self->m_callbacks.receivedInvitationCallback, url.c_str());
				});
		});

	return true;
}

} // namespace tvagentapi
