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
#include "AccessControlModule.h"

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
std::shared_ptr<IModule> CreateModule<IModule::Type::AccessControl>(
	std::weak_ptr<AgentConnection> connection)
{
	return AccessControlModule::Create(std::move(connection));
}

using Feature = IAccessControlModule::Feature;
using Access = IAccessControlModule::Access;
using ControlMode = IAccessControlModule::ControlMode;
using CommunicationFeature = TVRemoteScreenSDKCommunication::AccessControlService::AccessControl;
using CommunicationAccess = TVRemoteScreenSDKCommunication::AccessControlService::Access;
using CommunicationControlMode = TVRemoteScreenSDKCommunication::SessionControlService::ControlMode;

namespace
{

bool featureFromCommunication(CommunicationFeature feature, Feature& outFeature)
{
	switch (feature)
	{
		case CommunicationFeature::FileTransfer:
			outFeature = Feature::FileTransfer;
			return true;
		case CommunicationFeature::RemoteView:
			outFeature = Feature::RemoteView;
			return true;
		case CommunicationFeature::RemoteControl:
			outFeature = Feature::RemoteControl;
			return true;
		case CommunicationFeature::ScreenRecording:
			outFeature = Feature::ScreenRecording;
			return true;
	}
	return false;
}

bool featureToCommunication(Feature feature, CommunicationFeature& outFeature)
{
	switch (feature)
	{
		case Feature::FileTransfer:
			outFeature = CommunicationFeature::FileTransfer;
			return true;
		case Feature::RemoteView:
			outFeature = CommunicationFeature::RemoteView;
			return true;
		case Feature::RemoteControl:
			outFeature = CommunicationFeature::RemoteControl;
			return true;
		case Feature::ScreenRecording:
			outFeature = CommunicationFeature::ScreenRecording;
			return true;
	}
	return false;
}

bool accessFromCommunication(CommunicationAccess access, Access& outAccess)
{
	switch (access)
	{
		case CommunicationAccess::Allowed:
			outAccess = Access::Allowed;
			return true;
		case CommunicationAccess::AfterConfirmation:
			outAccess = Access::AfterConfirmation;
			return true;
		case CommunicationAccess::Denied:
			outAccess = Access::Denied;
			return true;
	}
	return false;
}

bool accessToCommunication(Access access, CommunicationAccess& outAccess)
{
	switch (access)
	{
		case Access::Allowed:
			outAccess = CommunicationAccess::Allowed;
			return true;
		case Access::AfterConfirmation:
			outAccess = CommunicationAccess::AfterConfirmation;
			return true;
		case Access::Denied:
			outAccess = CommunicationAccess::Denied;
			return true;
	}
	return false;
}

CommunicationControlMode controlModeToCommunication (ControlMode mode)
{
	switch (mode)
	{
		case ControlMode::Disabled: return CommunicationControlMode::Disable;
		case ControlMode::ViewOnly: return CommunicationControlMode::ScreenSharing;
		case ControlMode::FullControl: return CommunicationControlMode::FullControl;
	}

	return CommunicationControlMode::Unknown;
}

bool sendAccessConfirmationReply(Feature feature, bool isAllowed, const std::weak_ptr<AgentConnection>& weakConnection)
{
	auto connection = weakConnection.lock();
	CommunicationFeature communicationFeature;
	if (connection && featureToCommunication(feature, communicationFeature))
	{
		return connection->getCommunicationChannel()->sendAccessConfirmationReply(
			communicationFeature,
			isAllowed);
	}
	return false;
}

} // namespace

std::shared_ptr<AccessControlModule> AccessControlModule::Create(std::weak_ptr<AgentConnection> connection)
{
	std::shared_ptr<AccessControlModule> instance{new AccessControlModule(std::move(connection))};
	instance->m_weakThis = instance;
	if (!instance->registerCallbacks())
	{
		return {};
	}
	return instance;
}

AccessControlModule::AccessControlModule(std::weak_ptr<AgentConnection> connection)
	: m_connection(std::move(connection))
{
}

void AccessControlModule::setCallbacks(const Callbacks& callbacks)
{
	m_callbacks = callbacks;
}

bool AccessControlModule::getAccess(Feature feature, Access& outAccess)
{
	auto connection = m_connection.lock();
	CommunicationFeature communicationFeature;

	if (!connection || !featureToCommunication(feature, communicationFeature))
	{
		return false;
	}

	CommunicationAccess communicationAccess;
	bool result = connection->getCommunicationChannel()->sendGetAccessMode(
		communicationFeature,
		communicationAccess);

	if (result)
	{
		result = accessFromCommunication(communicationAccess, outAccess);
	}
	return result;
}

bool AccessControlModule::setAccess(Feature feature, Access access)
{
	auto connection = m_connection.lock();
	CommunicationFeature communicationFeature;
	CommunicationAccess communicationAccess;

	if (!connection ||
		!featureToCommunication(feature, communicationFeature) ||
		!accessToCommunication(access, communicationAccess))
	{
		return false;
	}
	return connection->getCommunicationChannel()->sendSetAccessMode(
			communicationFeature,
			communicationAccess);
}

bool AccessControlModule::isSupported() const
{
	if (auto connection = m_connection.lock())
	{
		// TODO IOT-15139 implement IModule::isSupported()
		// return connection->isModuleSupported("AccessControlModule"); // str or enum
		return true;
	}
	return false;
}

bool AccessControlModule::registerCallbacks()
{
	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	auto communicationChannel = connection->getCommunicationChannel();
	auto weakDispatcher = std::weak_ptr<IDispatcher>{connection->getDispatcher()};

	const auto weakThis = m_weakThis;
	m_accessChangeNotificationConnection = communicationChannel->accessModeChangeNotified().registerCallback(
		[weakThis, weakDispatcher]
		(CommunicationFeature communicationFeature, CommunicationAccess communicationAccess)
		{
			Feature feature;
			Access access;
			if (!featureFromCommunication(communicationFeature, feature) || !accessFromCommunication(communicationAccess, access))
			{
				return;
			}

			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[feature, access](const std::shared_ptr<AccessControlModule>& self)
				{
					util::safeCall(self->m_callbacks.accessChangedCallback, feature, access);
				});
		});

	m_accessConfirmationRequestedConnection = communicationChannel->accessConfirmationRequested().registerCallback(
		[weakThis, weakDispatcher]
		(CommunicationFeature communicationFeature, uint32_t timeout)
		{
			(void)timeout; // c++ api doesn't support timeout

			Feature feature;
			if (!featureFromCommunication(communicationFeature, feature))
			{
				return;
			}

			util::weakDispatcherPost(
				weakDispatcher,
				weakThis,
				[feature](const std::shared_ptr<AccessControlModule>& self)
				{
					util::safeCall(self->m_callbacks.accessRequestCallback, feature);
				});
		});

	return true;
}

bool AccessControlModule::acceptAccessRequest(Feature feature)
{
	return sendAccessConfirmationReply(feature, true, m_connection);
}

bool AccessControlModule::rejectAccessRequest(Feature feature)
{
	return sendAccessConfirmationReply(feature, false, m_connection);
}

bool AccessControlModule::setControlMode(ControlMode controlMode)
{
	auto connection = m_connection.lock();
	if (!connection)
	{
		return false;
	}

	auto communicationChannel = connection->getCommunicationChannel();
	communicationChannel->sendControlMode(controlModeToCommunication(controlMode));
	return true;
}

} // namespace tvagentapi
