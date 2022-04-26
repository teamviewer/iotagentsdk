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
#include "ViewGeometryServiceClient.h"

namespace TVRemoteScreenSDKCommunication
{
namespace ViewGeometryService
{

ViewGeometryServiceClient::ViewGeometryServiceClient()
	: BaseType{ServiceType::ViewGeometry}
{
}

IViewGeometryServiceClient::UpdateResult ViewGeometryServiceClient::UpdateAreaOfInterest(
	const std::string& comId,
	const Rect& rect)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvviewgeometryservice::UpdateAreaOfInterestRequest;
		using Response = ::tvviewgeometryservice::UpdateAreaOfInterestResponse;
		using Return = UpdateResult;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const Rect& rect)
		{
			auto allocatedRect = std::make_unique<::tvviewgeometryservice::Rect>();

			allocatedRect->set_x(rect.x);
			allocatedRect->set_y(rect.y);
			allocatedRect->set_width(rect.width);
			allocatedRect->set_height(rect.height);

			request.set_allocated_areaofinterest(allocatedRect.release());

			return stub.UpdateAreaOfInterest(&context, request, &response);
		}

		static Return HandleResponse(
			const ::grpc::Status& status,
			const Response& response,
			const Rect& /*rect*/)
		{
			Return returnValue;
			if (status.ok())
			{
				returnValue.state = CallState::Ok;
				returnValue.result =
					response.result() == tvviewgeometryservice::
						UpdateAreaOfInterestResponse_UpdateAreaOfInterestResult::
						UpdateAreaOfInterestResponse_UpdateAreaOfInterestResult_Ok ?
					UpdateGeometryResult::Ok : UpdateGeometryResult::Failed;
			}
			else
			{
				returnValue.errorMessage = status.error_message();
			}

			return returnValue;
		}
	};

	return ClientCall<Meta>(comId, rect);
}

IViewGeometryServiceClient::UpdateResult ViewGeometryServiceClient::UpdateVirtualDesktop(
	const std::string& comId,
	const VirtualDesktop& virtualDesktop)
{
	struct Meta: DefaultMeta
	{
		using Request = ::tvviewgeometryservice::UpdateVirtualDesktopRequest;
		using Response = ::tvviewgeometryservice::UpdateVirtualDesktopResponse;
		using Return = UpdateResult;

		static ::grpc::Status Call(
			gRPCStub& stub,
			::grpc::ClientContext& context,
			Request& request,
			Response& response,
			const VirtualDesktop& virtualDesktop)
		{
			auto allocatedVD = std::make_unique<::tvviewgeometryservice::VirtualDesktop>();

			allocatedVD->set_width(virtualDesktop.width);
			allocatedVD->set_height(virtualDesktop.height);

			for (const auto& screen: virtualDesktop.screens)
			{
				// Already owned by the internal container `allocatedVD->screens`
				::tvviewgeometryservice::Screen* allocatedScreen = allocatedVD->add_screens();

				allocatedScreen->set_name(screen.name);

				auto allocatedRect = std::make_unique<::tvviewgeometryservice::Rect>();
				allocatedRect->set_x(screen.geometry.x);
				allocatedRect->set_y(screen.geometry.y);
				allocatedRect->set_width(screen.geometry.width);
				allocatedRect->set_height(screen.geometry.height);

				allocatedScreen->set_allocated_geometry(allocatedRect.release());
			}

			request.set_allocated_virtualdesktopgeometry(allocatedVD.release());

			return stub.UpdateVirtualDesktop(&context, request, &response);
		}

		static Return HandleResponse(
			const ::grpc::Status& status,
			const Response& response,
			const VirtualDesktop& /*virtualDesktop*/)
		{
			Return returnValue;
			if (status.ok())
			{
				returnValue.state = CallState::Ok;
				returnValue.result =
					response.result() == tvviewgeometryservice::
					UpdateVirtualDesktopResponse_UpdateVirtualDesktopResult::
						UpdateVirtualDesktopResponse_UpdateVirtualDesktopResult_Ok ?
						UpdateGeometryResult::Ok : UpdateGeometryResult::Failed;
			}
			else
			{
				returnValue.errorMessage = status.error_message();
			}

			return returnValue;
		}
	};

	return ClientCall<Meta>(comId, virtualDesktop);
}

} // namespace ViewGeometryService
} // namespace TVRemoteScreenSDKCommunication