//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2021 TeamViewer Germany GmbH                                     //
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

#include "CommunicationAdapter.h"

#include "internal/Commands/SimulateKeyCommand.h"
#include "internal/Commands/SimulateMouseCommand.h"

#include "internal/Grabbing/Screen/ScreenGrabResult.h"

#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlInServiceClient.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/InServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlOutServiceServer.h>
#include <TVRemoteScreenSDKCommunication/AccessControlService/OutServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ChatService/IChatInServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ChatService/InServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ChatService/IChatOutServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ChatService/OutServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationRequestServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationResponseServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ConnectionConfirmationService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ConnectivityService/IConnectivityServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ConnectivityService/IConnectivityServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ConnectivityService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ImageService/IImageServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ImageService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ImageNotificationService/IImageNotificationServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ImageNotificationService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/InputService/IInputServiceServer.h>
#include <TVRemoteScreenSDKCommunication/InputService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportNotificationServiceServer.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportServiceClient.h>
#include <TVRemoteScreenSDKCommunication/InstantSupportService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/RegistrationService/IRegistrationServiceClient.h>
#include <TVRemoteScreenSDKCommunication/RegistrationService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/SessionControlService/ISessionControlServiceClient.h>
#include <TVRemoteScreenSDKCommunication/SessionControlService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/SessionStatusService/ISessionStatusServiceServer.h>
#include <TVRemoteScreenSDKCommunication/SessionStatusService/ServiceFactory.h>

#include <TVRemoteScreenSDKCommunication/ViewGeometryService/IViewGeometryServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ViewGeometryService/ServiceFactory.h>


namespace tvqtsdk
{

namespace
{

constexpr const char* ClientVersion = "1.0";
constexpr const char* AccessControlOutLocation = "TVQtRC/aCOut/";
constexpr const char* ChatOutLocation = "TVQtRC/chatOut/";
constexpr const char* ConnectivityLocation = "TVQtRC/conn/";
constexpr const char* InputLocation = "TVQtRC/input/";
constexpr const char* SessionStatusLocation = "TVQtRC/sesStat/";
constexpr const char* InstantSupportNotificationLocation = "TVQtRC/insSupE/";
constexpr const char* ConnectionConfirmationRequestLocation = "TVQtRC/ConConRe/";
// when adding a new service, also add it to the checks inside the setRemoteScreenSdkBaseUrlChecked function

constexpr uint32_t MaxSizeOfSocketPath = 107; // Socket paths under linux have a limit of around 100 characters. GRPC itself has a hard limit on 107 character.
constexpr uint32_t UuidSize = 32;

TVRemoteScreenSDKCommunication::ImageService::ColorFormat getSdkCommunicationColorFormat(const tvqtsdk::ColorFormat colorFormat)
{
	switch (colorFormat)
	{
		case tvqtsdk::ColorFormat::BGRA32: return TVRemoteScreenSDKCommunication::ImageService::ColorFormat::BGRA32;
		case tvqtsdk::ColorFormat::R5G6B5: return TVRemoteScreenSDKCommunication::ImageService::ColorFormat::R5G6B5;
		case tvqtsdk::ColorFormat::RGBA32: return TVRemoteScreenSDKCommunication::ImageService::ColorFormat::RGBA32;
		case tvqtsdk::ColorFormat::Unsupported: return TVRemoteScreenSDKCommunication::ImageService::ColorFormat::Unknown;
	}

	return TVRemoteScreenSDKCommunication::ImageService::ColorFormat::Unknown;
}

ScreenGrabStrategy getQtSdkGrabbingStrategy(const TVRemoteScreenSDKCommunication::SessionStatusService::GrabStrategy strategy)
{
	switch (strategy)
	{
		case TVRemoteScreenSDKCommunication::SessionStatusService::GrabStrategy::ChangeNotificationOnly: return tvqtsdk::ScreenGrabStrategy::ChangeNotificationOnly;
		case TVRemoteScreenSDKCommunication::SessionStatusService::GrabStrategy::EventDrivenByApp: return tvqtsdk::ScreenGrabStrategy::EventDrivenByApp;
		case TVRemoteScreenSDKCommunication::SessionStatusService::GrabStrategy::NoGrabbing: return tvqtsdk::ScreenGrabStrategy::NoGrabbing;
		case TVRemoteScreenSDKCommunication::SessionStatusService::GrabStrategy::Unknown: return tvqtsdk::ScreenGrabStrategy::NoGrabbing;
	}

	return tvqtsdk::ScreenGrabStrategy::NoGrabbing;
}

TVRemoteScreenSDKCommunication::SessionControlService::ControlMode getSdkCommunicationControlMode(ControlMode mode)
{
	switch (mode)
	{
		case ControlMode::Disabled:	return TVRemoteScreenSDKCommunication::SessionControlService::ControlMode::Disable;
		case ControlMode::ViewOnly:	return TVRemoteScreenSDKCommunication::SessionControlService::ControlMode::ScreenSharing;
		case ControlMode::FullControl: return TVRemoteScreenSDKCommunication::SessionControlService::ControlMode::FullControl;
	}

	return TVRemoteScreenSDKCommunication::SessionControlService::ControlMode::Unknown;
}

bool getSdkCommunicationAccessControl(AccessControl feature, TVRemoteScreenSDKCommunication::AccessControlService::AccessControl& accessControl)
{
	switch (feature)
	{
		case AccessControl::FileTransfer:
			accessControl = TVRemoteScreenSDKCommunication::AccessControlService::AccessControl::FileTransfer;
			break;
		case AccessControl::AllowPartnerViewDesktop:
			accessControl = TVRemoteScreenSDKCommunication::AccessControlService::AccessControl::AllowPartnerViewDesktop;
			break;
		case AccessControl::RemoteControl:
			accessControl = TVRemoteScreenSDKCommunication::AccessControlService::AccessControl::RemoteControl;
			break;
		default:
			return false;
	}
	return true;
}

bool getSdkCommunicationAccess(Access access, TVRemoteScreenSDKCommunication::AccessControlService::Access& sdkCommunicationAccess)
{
	switch (access)
	{
		case Access::Allowed:
			sdkCommunicationAccess = TVRemoteScreenSDKCommunication::AccessControlService::Access::Allowed;
			break;
		case Access::AfterConfirmation:
			sdkCommunicationAccess = TVRemoteScreenSDKCommunication::AccessControlService::Access::AfterConfirmation;
			break;
		case Access::Denied:
			sdkCommunicationAccess = TVRemoteScreenSDKCommunication::AccessControlService::Access::Denied;
			break;
		default:
			return false;
	}
	return true;
}

bool getSdkConnectionUserConfirmation(
	ConnectionUserConfirmation confirmation,
	TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionUserConfirmation& sdkConfirmation)
{
	switch (confirmation)
	{
		case ConnectionUserConfirmation::Accepted:
			sdkConfirmation = TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionUserConfirmation::Accepted;
			break;
		case ConnectionUserConfirmation::Denied:
			sdkConfirmation = TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionUserConfirmation::Denied;
			break;
		case ConnectionUserConfirmation::NoResponse:
			sdkConfirmation = TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionUserConfirmation::NoResponse;
			break;
		default:
			return false;
	}
	return true;
}

bool getSdkCommunicationConnectionType(
	ConnectionType connectionType,
	TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType& connectionTypeSdk)
{
	switch (connectionType)
	{
		case ConnectionType::InstantSupport:
			connectionTypeSdk = TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType::InstantSupport;
			break;
		case ConnectionType::Undefined:
		default:
			return false;
	}
	return true;
}

bool ConvertFromInternalViewGeometrySendResult(
	tvagentapi::ViewGeometrySendResult resultInternal,
	ViewGeometrySendResult& result)
{
	switch (resultInternal)
	{
		case tvagentapi::ViewGeometrySendResult::Ok:
			result = ViewGeometrySendResult::Ok;
			break;
		case tvagentapi::ViewGeometrySendResult::NoService:
			result = ViewGeometrySendResult::NoService;
			break;
		case tvagentapi::ViewGeometrySendResult::CallError:
			result = ViewGeometrySendResult::CallError;
			break;
		case tvagentapi::ViewGeometrySendResult::Rejected:
			result = ViewGeometrySendResult::Rejected;
			break;
		default:
			return false;
	}
	return true;	
}

bool ConvertFromInternalBaseUrlParseResultCode(
	tvagentapi::BaseUrlParseResultCode resultInternal,
	BaseUrlParseResultCode& result)
{
	switch (resultInternal)
	{
		case tvagentapi::BaseUrlParseResultCode::Success:
			result = BaseUrlParseResultCode::Success;
			break;
		case tvagentapi::BaseUrlParseResultCode::CharacterLimitForPathExceeded:
			result = BaseUrlParseResultCode::CharacterLimitForPathExceeded;
			break;
		case tvagentapi::BaseUrlParseResultCode::SchemaNotValid:
			result = BaseUrlParseResultCode::SchemaNotValid;
			break;
		default:
			return false;
	}
	return true;	
}

bool ConvertFromSdkChatType(TVRemoteScreenSDKCommunication::ChatService::ChatType sdkChatType, ChatType& chatType)
{
	switch (sdkChatType)
	{
		case TVRemoteScreenSDKCommunication::ChatService::ChatType::Machine:
			chatType = ChatType::Machine;
			break;
		case TVRemoteScreenSDKCommunication::ChatService::ChatType::Session:
			chatType = ChatType::Session;
			break;
		default:
			return false;
	}
	return true;
}

bool ConvertFromSdkChatState(TVRemoteScreenSDKCommunication::ChatService::ChatState sdkChatState, ChatState& chatState)
{
	switch (sdkChatState)
	{
		case TVRemoteScreenSDKCommunication::ChatService::ChatState::Open:
			chatState = ChatState::Open;
			break;
		case TVRemoteScreenSDKCommunication::ChatService::ChatState::Closed:
			chatState = ChatState::Closed;
			break;
		default:
			return false;
	}
	return true;
}

void getSdkObtainChatsResult(const std::vector<TVRemoteScreenSDKCommunication::ChatService::ChatInfo>& chats, QVector<ChatInfo>& sdkChats)
{
	for (const auto& chatItr: chats)
	{
		ChatType chatType;
		ChatState chatState;

		if (ConvertFromSdkChatType(chatItr.chatType, chatType) && ConvertFromSdkChatState(chatItr.chatState, chatState))
		{
			sdkChats.push_back({
				QUuid(QString::fromStdString(chatItr.chatId)),
				QString::fromStdString(chatItr.title),
				chatType,
				chatItr.chatTypeId,
				chatState});
		}
	}
}

void getSdkChatIds(const std::vector<std::string>& chats, QVector<QUuid>& sdkChatIds)
{
	for (const auto& chatItr: chats)
	{
		sdkChatIds.push_back(QUuid(QString::fromStdString(chatItr)));
	}
}

void getSdkReceivedMessages(std::vector<TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage> messages, QVector<ReceivedMessage>& sdkMessages)
{
	for (const auto& msg: messages)
	{
		sdkMessages.push_back({
			QString::fromStdString(msg.chatId),
			QString::fromStdString(msg.messageId),
			QString::fromStdString(msg.content),
			QDateTime::fromMSecsSinceEpoch(msg.timeStamp),
			QString::fromStdString(msg.sender)});
	}
}

bool getQtSdkAccess(TVRemoteScreenSDKCommunication::AccessControlService::Access accessMode, Access& sdkAccessMode)
{
	switch (accessMode)
	{
		case TVRemoteScreenSDKCommunication::AccessControlService::Access::Allowed:
			sdkAccessMode = Access::Allowed;
			break;
		case TVRemoteScreenSDKCommunication::AccessControlService::Access::AfterConfirmation:
			sdkAccessMode = Access::AfterConfirmation;
			break;
		case TVRemoteScreenSDKCommunication::AccessControlService::Access::Denied:
			sdkAccessMode = Access::Denied;
			break;
		default:
			return false;
	}
	return true;
}

bool getQtSdkAccessControl(TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature, AccessControl& sdkFeature)
{
	switch (feature)
	{
		case TVRemoteScreenSDKCommunication::AccessControlService::AccessControl::FileTransfer:
			sdkFeature = AccessControl::FileTransfer;
			break;
		case TVRemoteScreenSDKCommunication::AccessControlService::AccessControl::AllowPartnerViewDesktop:
			sdkFeature = AccessControl::AllowPartnerViewDesktop;
			break;
		case TVRemoteScreenSDKCommunication::AccessControlService::AccessControl::RemoteControl:
			sdkFeature = AccessControl::RemoteControl;
			break;
		default:
			return false;
	}
	return true;
}

MouseButton getQtSdkMouseButton(const TVRemoteScreenSDKCommunication::InputService::MouseButton button)
{
	MouseButton mouseButton = MouseButton::Unknown;

	switch (button)
	{
	case TVRemoteScreenSDKCommunication::InputService::MouseButton::Left:
		mouseButton = MouseButton::Left;
		break;
	case TVRemoteScreenSDKCommunication::InputService::MouseButton::Middle:
		mouseButton = MouseButton::Middle;
		break;
	case TVRemoteScreenSDKCommunication::InputService::MouseButton::Right:
		mouseButton = MouseButton::Right;
		break;
	case TVRemoteScreenSDKCommunication::InputService::MouseButton::Unknown:
		mouseButton = MouseButton::Unknown;
		break;
	}

	return mouseButton;
}

InstantSupportError getQtSdkInstantSupportError(const TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError errorToConvert)
{
	switch (errorToConvert)
	{
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::InvalidToken:
			return InstantSupportError::InvalidToken;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::InvalidGroup:
			return InstantSupportError::InvalidGroup;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::InvalidSessionCode:
			return InstantSupportError::InvalidSessionCode;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::Busy:
			return InstantSupportError::Busy;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::InternalError:
			return InstantSupportError::InternalError;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError::InvalidEmail:
			return InstantSupportError::InvalidEmail;
	}

	return InstantSupportError::InternalError;
}

InstantSupportData getQtSdkInstantSupportData(const TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportData& data)
{
	InstantSupportData modifiedData;

	modifiedData.sessionCode = data.sessionCode;
	modifiedData.name = data.name;
	modifiedData.description = data.description;

	switch (data.state)
	{
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportState::Open:
			modifiedData.state = InstantSupportState::Open;
			break;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportState::Closed:
			modifiedData.state = InstantSupportState::Closed;
			break;
		case TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportState::Undefined:
		default:
			modifiedData.state = InstantSupportState::Undefined;
			break;
	}

	return modifiedData;
}

std::string QUuidToString(const QUuid& quuid)
{
	const std::string str = quuid.toString().toStdString();
	return str.substr(1, str.size() - 2); // omit enclosing curly braces
}

} // namespace

std::shared_ptr<CommunicationAdapter> CommunicationAdapter::Create(
	const std::shared_ptr<tvqtsdk::ILogging>& logging,
	const std::shared_ptr<tvagentapi::ILoggingPrivate>& loggingPvt,
	const std::string& registrationServiceLocation,
	QObject* parent /*= nullptr*/)
{
	if (!logging || !loggingPvt)
	{
		return nullptr;
	}

	const std::shared_ptr<CommunicationAdapter> communicationAdapter(
		new CommunicationAdapter(
			logging,
			loggingPvt,
			registrationServiceLocation,
			parent));

	communicationAdapter->m_weakThis = communicationAdapter;

	communicationAdapter->setup();

	return communicationAdapter;
}

CommunicationAdapter::CommunicationAdapter(
	const std::shared_ptr<tvqtsdk::ILogging>& logging,
	const std::shared_ptr<tvagentapi::ILoggingPrivate>& loggingPvt,
	const std::string& registrationServiceLocation,
	QObject* parent /*= nullptr*/)
		: QObject(parent)
		, m_logging(logging)
		, m_communicationChannel(tvagentapi::CommunicationChannel::Create(
			loggingPvt,
			registrationServiceLocation))
{
}

void CommunicationAdapter::setup()
{
	m_observerConnections.clear();

	m_observerConnections.push_back(m_communicationChannel->accessConfirmationRequested().registerCallback(
		[weakThis = m_weakThis](
			TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
			uint32_t timeout)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				AccessControl sdkAccessControl;
				if (getQtSdkAccessControl(feature, sdkAccessControl))
				{
					Q_EMIT sharedThis->accessConfirmationRequested(sdkAccessControl, timeout);
				}
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->accessModeChangeNotified().registerCallback(
		[weakThis = m_weakThis](
			TVRemoteScreenSDKCommunication::AccessControlService::AccessControl feature,
			TVRemoteScreenSDKCommunication::AccessControlService::Access accessMode)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				Access sdkAccessMode;
				AccessControl sdkFeature;
				if (getQtSdkAccess(accessMode, sdkAccessMode) && getQtSdkAccessControl(feature, sdkFeature))
				{
					Q_EMIT sharedThis->accessModeChangeNotified(sdkFeature, sdkAccessMode);
				}
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->agentCommunicationEstablished().registerCallback(
		[weakThis = m_weakThis]()
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				Q_EMIT sharedThis->agentCommunicationEstablished();
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->agentCommunicationLost().registerCallback(
		[weakThis = m_weakThis]()
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				Q_EMIT sharedThis->agentCommunicationLost();
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->instantSupportErrorNotification().registerCallback(
		[weakThis = m_weakThis](
			TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportError responseError)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				InstantSupportError sdkResponseError = getQtSdkInstantSupportError(responseError);
				Q_EMIT sharedThis->instantSupportErrorNotification(sdkResponseError);
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->instantSupportModifiedNotification().registerCallback(
		[weakThis = m_weakThis](
			TVRemoteScreenSDKCommunication::InstantSupportService::InstantSupportData data)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				InstantSupportData sdkResponseData = getQtSdkInstantSupportData(data);
				Q_EMIT sharedThis->instantSupportModifiedNotification(sdkResponseData);
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->instantSupportConnectionConfirmationRequested().registerCallback(
		[weakThis = m_weakThis]()
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				Q_EMIT sharedThis->instantSupportConnectionConfirmationRequested();
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->rcSessionStarted().registerCallback(
		[weakThis = m_weakThis](
			TVRemoteScreenSDKCommunication::SessionStatusService::GrabStrategy strategy)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				Q_EMIT sharedThis->tvSessionStarted(getQtSdkGrabbingStrategy(strategy));
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->rcSessionStopped().registerCallback(
		[weakThis = m_weakThis]()
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				Q_EMIT sharedThis->tvSessionStopped();
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->simulateKeyInputRequested().registerCallback(
		[weakThis = m_weakThis](
			TVRemoteScreenSDKCommunication::InputService::KeyState keyState,
			uint32_t xkbSymbol,
			uint32_t unicodeCharacter,
			uint32_t xkbModifiers)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				std::shared_ptr<SimulateKeyCommand> command;

				switch(keyState)
				{
				case TVRemoteScreenSDKCommunication::InputService::KeyState::Down:
					command = std::make_shared<SimulateKeyCommand>(tvqtsdk::KeyState::Pressed, xkbSymbol, unicodeCharacter, xkbModifiers);
					break;
				case TVRemoteScreenSDKCommunication::InputService::KeyState::Up:
					command = std::make_shared<SimulateKeyCommand>(tvqtsdk::KeyState::Released, xkbSymbol, unicodeCharacter, xkbModifiers);
					break;
				case TVRemoteScreenSDKCommunication::InputService::KeyState::Unknown:
					break;
				}

				if (command)
				{
					Q_EMIT sharedThis->simulateKeyInputRequested(command);
				}
			}
		}));

	// NB: 3 different mouse callbacks emit same Qt signal

	m_observerConnections.push_back(m_communicationChannel->simulateMouseMoveRequested().registerCallback(
		[weakThis = m_weakThis](
			int32_t posX,
			int32_t posY)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				const std::shared_ptr<tvqtsdk::SimulateMouseCommand> command =
					std::make_shared<tvqtsdk::SimulateMouseCommand>(MouseButtonState::None, MouseAction::Move, posX, posY, MouseButton::Unknown, 0);

				Q_EMIT sharedThis->simulateMouseInputRequested(command);
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->simulateMousePressReleaseRequested().registerCallback(
		[weakThis = m_weakThis](
			TVRemoteScreenSDKCommunication::InputService::KeyState keyState,
			int32_t posX,
			int32_t posY,
			TVRemoteScreenSDKCommunication::InputService::MouseButton button)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				std::shared_ptr<tvqtsdk::SimulateMouseCommand> command;

				const MouseButton mouseButton = getQtSdkMouseButton(button);

				if (mouseButton != MouseButton::Unknown)
				{
					switch (keyState)
					{
					case TVRemoteScreenSDKCommunication::InputService::KeyState::Down:
						command = std::make_shared<tvqtsdk::SimulateMouseCommand>(MouseButtonState::Pressed, MouseAction::PressOrRelease, posX, posY, mouseButton, 0);
						break;
					case TVRemoteScreenSDKCommunication::InputService::KeyState::Up:
						command = std::make_shared<tvqtsdk::SimulateMouseCommand>(MouseButtonState::Released, MouseAction::PressOrRelease, posX, posY, mouseButton, 0);
						break;
					case TVRemoteScreenSDKCommunication::InputService::KeyState::Unknown:
						break;
					}
				}

				if (command)
				{
					Q_EMIT sharedThis->simulateMouseInputRequested(command);
				}
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->simulateMouseWheelRequested().registerCallback(
		[weakThis = m_weakThis](
			int32_t posX,
			int32_t posY,
			int32_t angle)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				const std::shared_ptr<tvqtsdk::SimulateMouseCommand> command =
					std::make_shared<tvqtsdk::SimulateMouseCommand>(MouseButtonState::None, MouseAction::Wheel, posX, posY, MouseButton::Unknown, angle);

				Q_EMIT sharedThis->simulateMouseInputRequested(command);
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->chatCreated().registerCallback(
		[weakThis = m_weakThis](
			std::string chatId,
			std::string title,
			TVRemoteScreenSDKCommunication::ChatService::ChatType sdkchatType,
			uint32_t chatTypeId)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				ChatType chatType;

				if (ConvertFromSdkChatType(sdkchatType, chatType))
				{
					Q_EMIT sharedThis->chatCreated(QString::fromStdString(chatId), QString::fromStdString(title), chatType, chatTypeId);
				}
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->chatsRemoved().registerCallback(
		[weakThis = m_weakThis](
			std::vector<std::string> chatIds)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				QVector<QUuid> sdkChatIds;
				sdkChatIds.reserve(chatIds.size());
				getSdkChatIds(chatIds, sdkChatIds);

				Q_EMIT sharedThis->chatsRemoved(sdkChatIds);
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->receivedMessages().registerCallback(
		[weakThis = m_weakThis](
			std::vector<TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage> messages)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				QVector<ReceivedMessage> sdkMessages;
				sdkMessages.reserve(messages.size());
				getSdkReceivedMessages(messages, sdkMessages);

				Q_EMIT sharedThis->receivedMessages(sdkMessages);
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->messageSent().registerCallback(
		[weakThis = m_weakThis](
			uint32_t localId,
			std::string messageId,
			uint64_t timeStamp)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				Q_EMIT sharedThis->messageSent(localId, QUuid(QString::fromStdString(messageId)), QDateTime::fromMSecsSinceEpoch(timeStamp));
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->messageNotSent().registerCallback(
		[weakThis = m_weakThis](
			uint32_t localId)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				Q_EMIT sharedThis->messageNotSent(localId);
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->loadedMessages().registerCallback(
		[weakThis = m_weakThis](
			std::vector<TVRemoteScreenSDKCommunication::ChatService::ReceivedMessage> messages,
			bool hasMore)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				QVector<ReceivedMessage> sdkMessages;
				sdkMessages.reserve(messages.size());
				getSdkReceivedMessages(messages, sdkMessages);

				Q_EMIT sharedThis->loadedMessages(sdkMessages, hasMore);
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->deletedHistory().registerCallback(
		[weakThis = m_weakThis](
			std::string chatId)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				Q_EMIT sharedThis->deletedHistory(QString::fromStdString(chatId));
			}
		}));

	m_observerConnections.push_back(m_communicationChannel->closedChat().registerCallback(
		[weakThis = m_weakThis](
			std::string chatId)
		{
			if (const std::shared_ptr<CommunicationAdapter> sharedThis = weakThis.lock())
			{
				Q_EMIT sharedThis->closedChat(QString::fromStdString(chatId));
			}
		}));
}

void CommunicationAdapter::setRemoteScreenSdkBaseUrl(QUrl remoteScreenSdkBaseUrl)
{
	m_communicationChannel->setRemoteScreenSdkBaseUrl(remoteScreenSdkBaseUrl.toString().toStdString());
}

BaseUrlParseResultCode CommunicationAdapter::setRemoteScreenSdkBaseUrlChecked(QUrl remoteScreenSdkBaseUrl)
{
	Q_ASSERT(remoteScreenSdkBaseUrl.isValid());

	if (!remoteScreenSdkBaseUrl.isValid())
	{
		m_logging->logError(QStringLiteral("[Communication Channel] setRemoteScreenSdkBaseUrl(): invalid value \"%1\".")
			.arg(remoteScreenSdkBaseUrl.toString()));
		return BaseUrlParseResultCode::SchemaNotValid;
	}

	tvagentapi::BaseUrlParseResultCode resultInternal = m_communicationChannel->setRemoteScreenSdkBaseUrlChecked(
		remoteScreenSdkBaseUrl.toString().toStdString());

	BaseUrlParseResultCode result;
	ConvertFromInternalBaseUrlParseResultCode(resultInternal, result);

	return result;
}

void CommunicationAdapter::startup()
{
	m_communicationChannel->startup();
}

void CommunicationAdapter::shutdown()
{
	m_communicationChannel->shutdown();
}

bool CommunicationAdapter::requestInstantSupport(
	QString accessToken,
	QString name,
	QString group,
	QString description,
	QString sessionCode,
	QString email)
{
	return m_communicationChannel->requestInstantSupport(
		accessToken.toStdString(),
		name.toStdString(),
		group.toStdString(),
		description.toStdString(),
		sessionCode.toStdString(),
		email.toStdString());
}

bool CommunicationAdapter::confirmConnectionRequest(tvqtsdk::ConnectionType connectionType, tvqtsdk::ConnectionUserConfirmation confirmation)
{
	TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionUserConfirmation sdkConfirmation =
		TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionUserConfirmation::NoResponse;

	if (!getSdkConnectionUserConfirmation(confirmation, sdkConfirmation))
	{
		m_logging->logError("[CommunicationAdapter] Unknown Instant Support User confirmation value.");
		return false;
	}

	TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType connectionTypeSdk =
		TVRemoteScreenSDKCommunication::ConnectionConfirmationService::ConnectionType::Undefined;

	if (!getSdkCommunicationConnectionType(connectionType, connectionTypeSdk))
	{
		m_logging->logError("[CommunicationAdapter] Unknown connection type.");
		return false;
	}

	return m_communicationChannel->confirmConnectionRequest(connectionTypeSdk, sdkConfirmation);
}

void CommunicationAdapter::sendStop()
{
	m_communicationChannel->sendStop();
}

void CommunicationAdapter::sendControlMode(ControlMode mode)
{
	const TVRemoteScreenSDKCommunication::SessionControlService::ControlMode requestedControlMode = getSdkCommunicationControlMode(mode);

	m_communicationChannel->sendControlMode(requestedControlMode);
}

bool CommunicationAdapter::sendGetAccessMode(AccessControl feature, Access& access)
{
	TVRemoteScreenSDKCommunication::AccessControlService::AccessControl requestedFeature;
	if (!getSdkCommunicationAccessControl(feature, requestedFeature))
	{
		m_logging->logError("[CommunicationAdapter] Unknown access control value.");
		return false;
	}

	TVRemoteScreenSDKCommunication::AccessControlService::Access accessMode;
	bool success = m_communicationChannel->sendGetAccessMode(requestedFeature, accessMode);
	success &= getQtSdkAccess(accessMode, access);

	return success;
}

bool CommunicationAdapter::sendSetAccessMode(AccessControl feature, Access access)
{
	TVRemoteScreenSDKCommunication::AccessControlService::AccessControl requestedFeature;
	TVRemoteScreenSDKCommunication::AccessControlService::Access requestedAccess;
	if (!(getSdkCommunicationAccessControl(feature, requestedFeature) && getSdkCommunicationAccess(access, requestedAccess)))
	{
		m_logging->logError("[CommunicationAdapter] Unknown access mode or access control value.");
		return false;
	}

	return m_communicationChannel->sendSetAccessMode(requestedFeature, requestedAccess);
}

bool CommunicationAdapter::sendAccessConfirmationReply(AccessControl feature, bool confirmed)
{
	TVRemoteScreenSDKCommunication::AccessControlService::AccessControl requestedFeature;
	if (!getSdkCommunicationAccessControl(feature, requestedFeature))
	{
		m_logging->logError("[CommunicationAdapter] Unknown access control value.");
		return false;
	}

	return m_communicationChannel->sendAccessConfirmationReply(requestedFeature, confirmed);
}

void CommunicationAdapter::sendScreenGrabResult(const tvqtsdk::ScreenGrabResult& result) const
{
	const int x = result.getDirtyRect().x();
	const int y = result.getDirtyRect().y();
	const int width = result.getDirtyRect().width();
	const int height = result.getDirtyRect().height();

	const QImage image = result.getImage();
	const std::string pictureData(
		reinterpret_cast<const char*>(image.constBits()),
		static_cast<std::size_t>(image.bytesPerLine()) * static_cast<std::size_t>(image.height()));

	m_communicationChannel->sendScreenGrabResult(
		x,
		y,
		width,
		height,
		pictureData);
}

void CommunicationAdapter::sendImageDefinitionForGrabResult(
	const QString& title,
	QSize size,
	double dpi,
	ColorFormat colorFormat) const
{
	m_communicationChannel->sendImageDefinitionForGrabResult(
		title.toStdString(),
		size.width(),
		size.height(),
		getSdkCommunicationColorFormat(colorFormat),
		dpi);
}

void CommunicationAdapter::sendGrabRequest(const QRect& rectOfInterest) const
{
	m_communicationChannel->sendGrabRequest(
		rectOfInterest.x(),
		rectOfInterest.y(),
		rectOfInterest.width(),
		rectOfInterest.height());
}

void CommunicationAdapter::sendImageDefinitionForGrabRequest(const QString& title, QSize size) const
{
	m_communicationChannel->sendImageDefinitionForGrabRequest(
		title.toStdString(),
		size.width(),
		size.height());
}

ViewGeometrySendResult CommunicationAdapter::sendVirtualDesktopGeometry(
	const VirtualDesktop& virtualDesktop) const
{
	TVRemoteScreenSDKCommunication::ViewGeometryService::VirtualDesktop geometry;
	geometry.width = virtualDesktop.width;
	geometry.height = virtualDesktop.height;
	geometry.screens.reserve(virtualDesktop.screens.size());
	for (const auto& screen: virtualDesktop.screens)
	{
		geometry.screens.push_back(
			TVRemoteScreenSDKCommunication::ViewGeometryService::Screen
			{
				screen.name.toStdString(),
				TVRemoteScreenSDKCommunication::ViewGeometryService::Rect
				{
					screen.geometry.x(),
					screen.geometry.y(),
					screen.geometry.width(),
					screen.geometry.height()
				}
			});
	}

	tvagentapi::ViewGeometrySendResult resultInternal = m_communicationChannel->sendVirtualDesktopGeometry(geometry);

	ViewGeometrySendResult result;
	ConvertFromInternalViewGeometrySendResult(resultInternal, result);

	return result;	
}

ViewGeometrySendResult CommunicationAdapter::sendAreaOfInterest(const QRect& areaOfInterest) const
{
	TVRemoteScreenSDKCommunication::ViewGeometryService::Rect rect;
	areaOfInterest.getRect(&rect.x, &rect.y, &rect.width, &rect.height);

	tvagentapi::ViewGeometrySendResult resultInternal = m_communicationChannel->sendAreaOfInterest(rect);

	ViewGeometrySendResult result;
	ConvertFromInternalViewGeometrySendResult(resultInternal, result);

	return result;	
}

bool CommunicationAdapter::sendObtainChatsRequest(QVector<ChatInfo>& chats)
{
	std::vector<TVRemoteScreenSDKCommunication::ChatService::ChatInfo> sdkChats;

	bool success = m_communicationChannel->sendObtainChatsRequest(sdkChats);

	if (success)
	{
		chats.reserve(sdkChats.size());
		getSdkObtainChatsResult(sdkChats, chats);
	}

	return success;
}

bool CommunicationAdapter::sendSelectChatResult(QUuid chatId)
{
	std::string selectedChatId = QUuidToString(chatId);
	return m_communicationChannel->sendSelectChatResult(selectedChatId);
}

bool CommunicationAdapter::sendMessage(uint32_t localId, QString content)
{
	return m_communicationChannel->sendMessage(localId, content.toStdString());
}

bool CommunicationAdapter::loadMessages(uint32_t count, QUuid lastId)
{
	std::string strLastId = QUuidToString(lastId);
	return m_communicationChannel->loadMessages(count, strLastId);
}

bool CommunicationAdapter::deleteHistory()
{
	return m_communicationChannel->deleteHistory();
}

bool CommunicationAdapter::deleteChat()
{
	return m_communicationChannel->deleteChat();
}

} // namespace tvqtsdk
