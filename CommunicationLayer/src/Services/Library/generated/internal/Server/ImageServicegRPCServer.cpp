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

#include "ImageServicegRPCServer.h"

#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/ServiceErrorMessage.h>

#include <grpc++/grpc++.h>

namespace TVRemoteScreenSDKCommunication
{

namespace ImageService
{

bool ImageServicegRPCServer::StartServer(const std::string& location)
{
	m_location = location;

	m_server = TransportFW::CreateAndStartSyncServer(m_location,
		this); // Register this "service" as the instance through which we'll communicate with clients.

	return m_server != nullptr;
}

void ImageServicegRPCServer::StopServer(bool /*force*/)
{
	if (m_server)
	{
		m_server->Shutdown();
	}
	m_server.reset();
}

ServiceType ImageServicegRPCServer::GetServiceType() const
{
	return ServiceType::Image;
}

const std::string& ImageServicegRPCServer::GetLocation() const
{
	return m_location;
}

void ImageServicegRPCServer::SetUpdateImageCallback(const ProcessUpdateImageRequestCallback& requestProcessing)
{
	m_updateImageProcessing = requestProcessing;
}

void ImageServicegRPCServer::SetUpdateImageDefinitionCallback(const ProcessUpdateImageDefinitionRequestCallback& requestProcessing)
{
	m_updateImageDefinitionProcessing = requestProcessing;
}

::grpc::Status ImageServicegRPCServer::UpdateImage(::grpc::ServerContext* context,
	::grpc::ServerReader<::tvimageservice::GrabResult>* reader,
	::tvimageservice::ImageUpdateResponse* responsePtr)
{
	if (context == nullptr || reader == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_updateImageProcessing)
	{
		return ::grpc::Status(::grpc::StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback);
	}

	auto& response = *responsePtr;
	(void)response;

	std::string comId;
	::tvimageservice::GrabResult message;

	int32_t x = 0;
	int32_t y = 0;
	int32_t width = 0;
	int32_t height = 0;
	std::string pictureData;

	uint32_t chunksEntryInProtBuf = 0;
	if (reader->Read(&message))
	{
		const auto searchComId = context->client_metadata().find(ServiceBase::CommunicationIdToken);
		if (searchComId != context->client_metadata().end())
		{
			comId = std::string((searchComId->second).data(), (searchComId->second).length());
		}
		chunksEntryInProtBuf = message.chunks();
	}
	else
	{
		return ::grpc::Status::CANCELLED;
	}
	::grpc::Status returnStatus =
		::grpc::Status(::grpc::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled);

	auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
	{
		if (callStatus.IsOk())
		{
			returnStatus = ::grpc::Status::OK;
		}
		else
		{
			returnStatus = ::grpc::Status(::grpc::StatusCode::ABORTED, callStatus.errorMessage);
		}
	};

	if (chunksEntryInProtBuf == 0)
	{
		const ::tvimageservice::Rect& dirtyRect = message.dirtyrect();
		x = dirtyRect.x();
		y = dirtyRect.y();
		height = dirtyRect.height();
		width = dirtyRect.width();
		pictureData.swap(*message.mutable_pixeldata()->mutable_picture());
		message.Clear();
		m_updateImageProcessing(comId, x, y, width, height, pictureData, responseProcessing);
	}
	else
	{
		const ::tvimageservice::Rect& dirtyRect = message.dirtyrect();
		x = dirtyRect.x();
		y = dirtyRect.y();
		height = dirtyRect.height();
		width = dirtyRect.width();
		pictureData.swap(*message.mutable_pixeldata()->mutable_picture());

		bool error = false;

		for (uint32_t chunkCounter = 1; chunkCounter < chunksEntryInProtBuf; ++chunkCounter)
		{
			if (reader->Read(&message))
			{
				pictureData += message.pixeldata().picture();
			}
			else
			{
				error = true;
				break;
			}
		}

		message.Clear();

		if (!error)
		{
			m_updateImageProcessing(comId, x, y, width, height, pictureData, responseProcessing);
		}
		else
		{
			return ::grpc::Status{::grpc::StatusCode::ABORTED, "Reading chunks failed"};
		}
	}

	return returnStatus;
}

::grpc::Status ImageServicegRPCServer::UpdateImageDefinition(::grpc::ServerContext* context,
	const ::tvimageservice::ImageDefinitionRequest* requestPtr,
	::tvimageservice::ImageDefinitionResponse* responsePtr)
{
	if (context == nullptr || requestPtr == nullptr || responsePtr == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string{});
	}

	if (!m_updateImageDefinitionProcessing)
	{
		return ::grpc::Status(::grpc::StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback);
	}
	auto& request = *requestPtr;
	(void)request;

	auto& response = *responsePtr;
	(void)response;

	std::string comId;

	const auto foundComId = context->client_metadata().find(ServiceBase::CommunicationIdToken);
	if (foundComId == context->client_metadata().end())
	{
		return ::grpc::Status(::grpc::StatusCode::FAILED_PRECONDITION, TvServiceBase::ErrorMessage_NoComId);
	}
	comId = std::string((foundComId->second).data(), (foundComId->second).length());

	::grpc::Status returnStatus =
		::grpc::Status(::grpc::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled);

	auto responseProcessing = [&returnStatus](const CallStatus& callStatus)
	{
		if (callStatus.IsOk())
		{
			returnStatus = ::grpc::Status::OK;
		}
		else
		{
			returnStatus = ::grpc::Status(::grpc::StatusCode::ABORTED, callStatus.errorMessage);
		}
	};

	const auto format = request.format();

	ColorFormat formatEnumValue = ColorFormat::Unknown;

	switch (format)
	{
		case ::tvimageservice::ColorFormat::Unknown:
			formatEnumValue = ColorFormat::Unknown;
			break;
		case ::tvimageservice::ColorFormat::BGRA32:
			formatEnumValue = ColorFormat::BGRA32;
			break;
		case ::tvimageservice::ColorFormat::RGBA32:
			formatEnumValue = ColorFormat::RGBA32;
			break;
		case ::tvimageservice::ColorFormat::R5G6B5:
			formatEnumValue = ColorFormat::R5G6B5;
			break;
		default:

			break;
	}

	m_updateImageDefinitionProcessing(comId,
		request.imagesourcetitle(),

		request.width(),

		request.height(),

		formatEnumValue,

		request.dpi(),

		responseProcessing);

	return returnStatus;
}

} // namespace ImageService

} // namespace TVRemoteScreenSDKCommunication