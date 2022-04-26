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
#include "ImageServiceClient.h"

namespace TVRemoteScreenSDKCommunication
{
namespace ImageService
{

namespace
{

constexpr uint64_t ChunkSize = (2*1024*1024); // MB

} // namespace

ImageServiceClient::ImageServiceClient()
	: BaseType{ServiceType::Image}
{
}

CallStatus ImageServiceClient::UpdateImage(
	const std::string& comId,
	int32_t x,
	int32_t y,
	int32_t width,
	int32_t height,
	const std::string& pictureData)
{
	struct Meta: DefaultMeta
	{
		using Response = ::tvimageservice::ImageUpdateResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& /*request*/,
			Response& response,
			int32_t x,
			int32_t y,
			int32_t width,
			int32_t height,
			const std::string& pictureData)
		{
			std::unique_ptr<grpc::ClientWriter<::tvimageservice::GrabResult>> writer =
				stub.UpdateImage(&context, &response);

			size_t chunkCounter = 0;

			size_t chunks = pictureData.size() / ChunkSize;

			if (pictureData.size() % ChunkSize != 0)
			{
				chunks++;
			}

			for (chunkCounter = 0; chunkCounter < chunks; ++chunkCounter)
			{
				::tvimageservice::GrabResult grabResult;
				if (chunkCounter == 0)
				{
					grabResult.set_chunks(chunks);
					::tvimageservice::Rect* dirtyRect = grabResult.mutable_dirtyrect();
					dirtyRect->set_x(x);
					dirtyRect->set_y(y);
					dirtyRect->set_height(height);
					dirtyRect->set_width(width);
				}

				auto buffer = new std::string();
				if (chunkCounter == chunks-1)
				{
					buffer->assign(pictureData.substr(chunkCounter * ChunkSize));
				}
				else
				{
					buffer->assign(pictureData.substr(chunkCounter * ChunkSize, ChunkSize));
				}

				grabResult.mutable_pixeldata()->set_allocated_picture(buffer);

				if (!writer->Write(grabResult))
				{
					break;
				}
			}

			writer->WritesDone();
			return writer->Finish();
		}
	};

	return ClientCall<Meta>(comId, x, y, width, height, pictureData);
}

CallStatus ImageServiceClient::UpdateImageDefinition(
	const std::string& comId,
	const std::string& imageSourceTitle,
	int32_t width,
	int32_t height,
	ColorFormat format,
	double dpi)
{
	::tvimageservice::ColorFormat colorFormat = ::tvimageservice::ColorFormat::Unknown;
	switch (format)
	{
		case ColorFormat::BGRA32:
			colorFormat = ::tvimageservice::ColorFormat::BGRA32;
			break;
		case ColorFormat::R5G6B5:
			colorFormat = ::tvimageservice::ColorFormat::R5G6B5;
			break;
		case ColorFormat::RGBA32:
			colorFormat = ::tvimageservice::ColorFormat::RGBA32;
			break;
		case ColorFormat::Unknown:
			break;
	}

	struct Meta: DefaultMeta
	{
		using Request = ::tvimageservice::ImageDefinitionRequest;
		using Response = ::tvimageservice::ImageDefinitionResponse;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const std::string &imageSourceTitle,
			int32_t width,
			int32_t height,
			::tvimageservice::ColorFormat colorFormat,
			double dpi)
		{
			request.set_imagesourcetitle(imageSourceTitle);
			request.set_width(width);
			request.set_height(height);
			request.set_dpi(dpi);
			request.set_format(colorFormat);

			return stub.UpdateImageDefinition(&context, request, &response);
		}
	};

	return ClientCall<Meta>(comId, imageSourceTitle, width, height, colorFormat, dpi);
}

} // namespace ImageService
} // namespace TVRemoteScreenSDKCommunication
