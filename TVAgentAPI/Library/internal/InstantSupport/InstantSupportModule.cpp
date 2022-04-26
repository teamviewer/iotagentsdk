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
#include "InstantSupportModule.h"

#include <TVAgentAPIPrivate/CommunicationChannel.h>

#include "AgentConnection/AgentConnection.h"
#include "AsyncOperation/IDispatcher.h"
#include "Utils/CallbackUtils.h"
#include "Utils/DispatcherUtils.h"
#include "ModuleFactory.h"

namespace tvagentapi
{

template <>
std::shared_ptr<IModule> CreateModule<IModule::Type::InstantSupport>(
	std::weak_ptr<AgentConnection> connection)
{
	return InstantSupportModule::Create(std::move(connection));
}

namespace
{

IInstantSupportModule::RequestErrorCode getRequestErrorCodeFromCommunication(TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError errorToConvert)
{
	using RequestErrorCode = IInstantSupportModule::RequestErrorCode;
	switch (errorToConvert)
	{
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::InvalidToken: return RequestErrorCode::InvalidToken;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::InvalidGroup: return RequestErrorCode::InvalidGroup;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::InvalidSessionCode: return RequestErrorCode::InvalidSessionCode;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::Busy: return RequestErrorCode::Busy;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::InternalError: return RequestErrorCode::InternalError;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::InvalidEmail: return RequestErrorCode::InvalidEmail;
	}
	return RequestErrorCode::InternalError;
}

IInstantSupportModule::SessionState getStateFromCommunication(TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportState state)
{
	using SessionState = IInstantSupportModule::SessionState;
	switch (state)
	{
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportState::Open: return SessionState::Open;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportState::Closed: return SessionState::Closed;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportState::Undefined: return SessionState::Undefined;
	}

	return SessionState::Undefined;
}

bool confirmConnectionRequest(
	TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionUserConfirmation confirmation,
	const std::weak_ptr<AgentConnection>& weakConnection)
{
	using ConnectionType = TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType;

	if (auto connection = weakConnection.lock())
	{
		return connection->getCommunicationChannel()->confirmConnectionRequest(
			ConnectionType::InstantSupport,
			confirmation);
	}
	return false;
}

} // namespace


std::shared_ptr<InstantSupportModule> InstantSupportModule::Create(std::weak_ptr<AgentConnection> connection)
{
	std::shared_ptr<InstantSupportModule> instance{new InstantSupportModule(std::move(connection))};
	instance->m_weakThis = instance;
	if (!instance->registerCallbacks())
	{
		return {};
	}
	return instance;
}

InstantSupportModule::InstantSupportModule(std::weak_ptr<AgentConnection> connection)
	: m_connection(std::move(connection))
{
}

void InstantSupportModule::setCallbacks(const Callbacks& callbacks)
{
	m_callbacks = callbacks;
}

bool InstantSupportModule::requestInstantSupport(
	const char* accessToken,
	const char* name,
	const char* group,
	const char* description,
	const char* email,
	const char* sessionCode)
{
	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	auto communicationChannel = connection->getCommunicationChannel();

	return communicationChannel->requestInstantSupport(
		std::string{accessToken ? accessToken : ""},
		std::string{name ? name : ""},
		std::string{group ? group : ""},
		std::string{description ? description : ""},
		std::string{sessionCode ? sessionCode : ""},
		std::string{email ? email : ""});
}

bool InstantSupportModule::isSupported() const
{
	if (auto connection = m_connection.lock())
	{
		// TODO IOT-15139 implement IModule::isSupported()
		// return connection->isModuleSupported("InstantSupportModule"); // str or enum
		return true;
	}
	return false;
}

bool InstantSupportModule::registerCallbacks()
{
	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	auto communicationChannel = connection->getCommunicationChannel();
	auto weakDispatcher = std::weak_ptr<IDispatcher>{connection->getDispatcher()};

	m_instantSupportModifiedNotificationConnection = communicationChannel->instantSupportModifiedNotification().registerCallback(
		[weakThis = m_weakThis, weakDispatcher]
		(TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportData data)
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[data = std::move(data)](const auto& self)
				{
					// pay attention that SessoinData holds pointers to c-strings
					// which are valid while InstantSupportData is valid
					util::safeCall(self->m_callbacks.sessionDataChangedCallback,
						SessionData{
							data.sessionCode.c_str(),
							data.name.c_str(),
							data.description.c_str(),
							getStateFromCommunication(data.state)
						});
				});
		});

	m_requestInstantSupportErrorNotificationConnection = communicationChannel->instantSupportErrorNotification().registerCallback(
		[weakThis = m_weakThis, weakDispatcher]
		(TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError errorCode)
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[errorCode](const auto& self)
				{
					util::safeCall(self->m_callbacks.requestErrorCallback, getRequestErrorCodeFromCommunication(errorCode));
				});
		});

	m_instantSupportRequestConnection = communicationChannel->instantSupportConnectionConfirmationRequested().registerCallback(
		[weakThis = m_weakThis, weakDispatcher]()
		{
			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[](const auto& self)
				{
					util::safeCall(self->m_callbacks.connectionRequestCallback);
				});
		});

	return true;
}

bool InstantSupportModule::acceptConnectionRequest()
{
	using ConnectionUserConfirmation = TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionUserConfirmation;
	return confirmConnectionRequest(ConnectionUserConfirmation::Accepted, m_connection);
}

bool InstantSupportModule::rejectConnectionRequest()
{
	using ConnectionUserConfirmation = TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionUserConfirmation;
	return confirmConnectionRequest(ConnectionUserConfirmation::Denied, m_connection);
}

bool InstantSupportModule::timeoutConnectionRequest()
{
	using ConnectionUserConfirmation = TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionUserConfirmation;
	return confirmConnectionRequest(ConnectionUserConfirmation::NoResponse, m_connection);
}

} // namespace tvagentapi
