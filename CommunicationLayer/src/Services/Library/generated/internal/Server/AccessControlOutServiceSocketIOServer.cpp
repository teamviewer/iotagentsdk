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
// ==================================
// Generated by TVCMGen. DO NOT EDIT!
// ==================================

#include "AccessControlOutServiceSocketIOServer.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/ServiceErrorMessage.h>

#include "internal/AccessControlOutServiceOutFunctions.h"

#include "AccessControl.pb.h"
#include "AccessControlOutMessages.pb.h"

namespace TVRemoteScreenSDKCommunication
{

namespace AccessControlService
{

bool AccessControlOutServiceSocketIOServer::StartServer(const std::string& location)
{
	using namespace Transport::SocketIO;

	m_location = location;
	Server::ServerFunctionMap functions;
	{
		auto* requestProcessing = &m_AskForConfirmationProcessing;
		functions[Function_AskForConfirmation] = [requestProcessing](const std::string& comIdValue,
													 std::shared_ptr<std::string> requestRaw,
													 std::shared_ptr<std::string> responseRaw)
		{
			if (!*requestProcessing)
			{
				return Status{StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback};
			}
			::tvaccesscontrolservice::AskForConfirmationRequest request;
			if (!request.ParseFromString(*requestRaw))
			{
				return Status(StatusCode::IO_ERROR, "error parsing request");
			}

			requestRaw->clear();

			::tvaccesscontrolservice::AskForConfirmationResponse response;

			Status status = [&]()
			{
				std::string comId = comIdValue;

				if (comIdValue.empty())
				{
					return Status{StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId};
				}

				Status returnStatus{StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled};

				auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
				{
					if (callStatus.IsOk())
					{
						returnStatus = Status::OK;
					}
					else
					{
						returnStatus = Status{StatusCode::ABORTED, callStatus.errorMessage};
					}
				};

				auto& m_askForConfirmationProcessing = *requestProcessing;

				const auto feature = request.feature();

				AccessControl featureEnumValue = AccessControl::FileTransfer;
				bool featureConverted = true;
				switch (feature)
				{
					case ::tvaccesscontrolservice::AccessControl::FileTransfer:
						featureEnumValue = AccessControl::FileTransfer;
						break;
					case ::tvaccesscontrolservice::AccessControl::RemoteView:
						featureEnumValue = AccessControl::RemoteView;
						break;
					case ::tvaccesscontrolservice::AccessControl::RemoteControl:
						featureEnumValue = AccessControl::RemoteControl;
						break;
					case ::tvaccesscontrolservice::AccessControl::ScreenRecording:
						featureEnumValue = AccessControl::ScreenRecording;
						break;
					default:
						featureConverted = false;
						break;
				}

				if (!featureConverted)
				{
					return TransportFW::Status{TransportFW::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_UnexpectedEnumValue};
				}

				m_askForConfirmationProcessing(comId,
					featureEnumValue,

					request.timeout(),

					responseProcessing);

				return returnStatus;
			}();
			if (!status.ok())
			{
				return status;
			}

			if (!response.SerializeToString(responseRaw.get()))
			{
				return Status(StatusCode::IO_ERROR, "response serialization failed");
			}

			return status;
		};
	}

	{
		auto* requestProcessing = &m_NotifyChangeProcessing;
		functions[Function_NotifyChange] = [requestProcessing](const std::string& comIdValue,
											   std::shared_ptr<std::string> requestRaw,
											   std::shared_ptr<std::string> responseRaw)
		{
			if (!*requestProcessing)
			{
				return Status{StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback};
			}
			::tvaccesscontrolservice::NotifyChangeRequest request;
			if (!request.ParseFromString(*requestRaw))
			{
				return Status(StatusCode::IO_ERROR, "error parsing request");
			}

			requestRaw->clear();

			::tvaccesscontrolservice::NotifyChangeResponse response;

			Status status = [&]()
			{
				std::string comId = comIdValue;

				if (comIdValue.empty())
				{
					return Status{StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId};
				}

				Status returnStatus{StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled};

				auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
				{
					if (callStatus.IsOk())
					{
						returnStatus = Status::OK;
					}
					else
					{
						returnStatus = Status{StatusCode::ABORTED, callStatus.errorMessage};
					}
				};

				auto& m_notifyChangeProcessing = *requestProcessing;

				const auto feature = request.feature();

				AccessControl featureEnumValue = AccessControl::FileTransfer;
				bool featureConverted = true;
				switch (feature)
				{
					case ::tvaccesscontrolservice::AccessControl::FileTransfer:
						featureEnumValue = AccessControl::FileTransfer;
						break;
					case ::tvaccesscontrolservice::AccessControl::RemoteView:
						featureEnumValue = AccessControl::RemoteView;
						break;
					case ::tvaccesscontrolservice::AccessControl::RemoteControl:
						featureEnumValue = AccessControl::RemoteControl;
						break;
					case ::tvaccesscontrolservice::AccessControl::ScreenRecording:
						featureEnumValue = AccessControl::ScreenRecording;
						break;
					default:
						featureConverted = false;
						break;
				}

				if (!featureConverted)
				{
					return TransportFW::Status{TransportFW::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_UnexpectedEnumValue};
				}

				const auto access = request.access();

				Access accessEnumValue = Access::Allowed;
				bool accessConverted = true;
				switch (access)
				{
					case ::tvaccesscontrolservice::Access::Allowed:
						accessEnumValue = Access::Allowed;
						break;
					case ::tvaccesscontrolservice::Access::AfterConfirmation:
						accessEnumValue = Access::AfterConfirmation;
						break;
					case ::tvaccesscontrolservice::Access::Denied:
						accessEnumValue = Access::Denied;
						break;
					default:
						accessConverted = false;
						break;
				}

				if (!accessConverted)
				{
					return TransportFW::Status{TransportFW::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_UnexpectedEnumValue};
				}

				m_notifyChangeProcessing(comId,
					featureEnumValue,

					accessEnumValue,

					responseProcessing);

				return returnStatus;
			}();
			if (!status.ok())
			{
				return status;
			}

			if (!response.SerializeToString(responseRaw.get()))
			{
				return Status(StatusCode::IO_ERROR, "response serialization failed");
			}

			return status;
		};
	}

	m_server.reset(new Server(std::move(functions)));

	return m_server->Start(location);
}

void AccessControlOutServiceSocketIOServer::StopServer(bool /*force*/)
{
	if (m_server)
	{
		m_server->Shutdown();
	}
	m_server.reset();
}

ServiceType AccessControlOutServiceSocketIOServer::GetServiceType() const
{
	return ServiceType::AccessControlOut;
}

const std::string& AccessControlOutServiceSocketIOServer::GetLocation() const
{
	return m_location;
}

void AccessControlOutServiceSocketIOServer::SetAskForConfirmationCallback(
	const ProcessAskForConfirmationRequestCallback& requestProcessing)
{
	m_AskForConfirmationProcessing = requestProcessing;
}

void AccessControlOutServiceSocketIOServer::SetNotifyChangeCallback(const ProcessNotifyChangeRequestCallback& requestProcessing)
{
	m_NotifyChangeProcessing = requestProcessing;
}

} // namespace AccessControlService

} // namespace TVRemoteScreenSDKCommunication