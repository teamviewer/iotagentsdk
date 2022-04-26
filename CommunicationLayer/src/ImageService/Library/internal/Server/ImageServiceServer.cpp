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
#include "ImageServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace ImageService
{

namespace
{

constexpr const char* ErrorMessage_ReadingChunksFailed = "Reading chunks failed.";

} // namespace

ImageServiceServer::ImageServiceServer()
	: BaseType{ServiceType::Image}
{
}

ImageServiceServer::~ImageServiceServer() = default;

void ImageServiceServer::SetImageUpdateCallback(
	const ProcessImageUpdateRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::ImageUpdate>(requestProcessing);
}

void ImageServiceServer::SetImageDefinitionCallback(
	const ProcessImageDefinitionRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::ImageDefinition>(requestProcessing);
}

::grpc::Status ImageServiceServer::UpdateImage(
	::grpc::ServerContext* context,
	::grpc::ServerReader<::tvimageservice::GrabResult>* reader,
	::tvimageservice::ImageUpdateResponse* response)
{
	if (context == nullptr || reader == nullptr || response == nullptr)
	{
		return ::grpc::Status(::grpc::StatusCode::INTERNAL, std::string());
	}

	auto callback = std::get<CallbackIndex<CBEnum::ImageUpdate>::value>(m_callbacks);

	if (!callback)
	{
		return ::grpc::Status(::grpc::StatusCode::UNAVAILABLE, TvServiceBase::ErrorMessage_NoProcessingCallback);
	}

	::tvimageservice::GrabResult message;

	int32_t x = 0;
	int32_t y = 0;
	int32_t width = 0;
	int32_t height = 0;
	std::string pictureData;
	std::string comId;

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

	::grpc::Status returnStatus = ::grpc::Status(::grpc::StatusCode::CANCELLED, TvServiceBase::ErrorMessage_ResponseCallbackNotCalled);
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
		callback(comId, x, y, width, height, pictureData, responseProcessing);
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
			callback(comId, x, y, width, height, pictureData, responseProcessing);
		}
		else
		{
			return ::grpc::Status(::grpc::StatusCode::ABORTED, ErrorMessage_ReadingChunksFailed);
		}
	}

	return returnStatus;
}

::grpc::Status ImageServiceServer::UpdateImageDefinition(
	::grpc::ServerContext* context,
	const ::tvimageservice::ImageDefinitionRequest* request,
	::tvimageservice::ImageDefinitionResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::ImageDefinition,
		::tvimageservice::ImageDefinitionRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			ColorFormat colorFormat = ColorFormat::Unknown;
			switch (request.format())
			{
				case ::tvimageservice::ColorFormat::BGRA32:
					colorFormat = ColorFormat::BGRA32;
					break;
				case ::tvimageservice::ColorFormat::R5G6B5:
					colorFormat = ColorFormat::R5G6B5;
					break;
				case ::tvimageservice::ColorFormat::RGBA32:
					colorFormat = ColorFormat::RGBA32;
					break;
				case ::tvimageservice::ColorFormat::Unknown:
				default:;
			}

			callback(
				comId,
				request.imagesourcetitle(),
				request.width(),
				request.height(),
				colorFormat,
				request.dpi(),
				responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

} // namespace ImageService
} // namespace TVRemoteScreenSDKCommunication
