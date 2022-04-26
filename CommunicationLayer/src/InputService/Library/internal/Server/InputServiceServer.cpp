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
#include "InputServiceServer.h"

namespace TVRemoteScreenSDKCommunication
{
namespace InputService
{

InputServiceServer::InputServiceServer()
	: BaseType{ServiceType::Input}
{
}

InputServiceServer::~InputServiceServer() = default;

void InputServiceServer::SetReceivedKeyCallback(
	const ProcessKeyRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::KeyRequest>(requestProcessing);
}

void InputServiceServer::SetMouseMoveCallback(
	const ProcessMouseMoveRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::MouseMove>(requestProcessing);
}

void InputServiceServer::SetMousePressReleaseCallback(
	const ProcessMousePressReleaseRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::MousePressRelease>(requestProcessing);
}

void InputServiceServer::SetMouseWheelCallback(
	const ProcessMouseWheelRequestCallback& requestProcessing)
{
	SetCallback<CBEnum::MouseWheel>(requestProcessing);
}

::grpc::Status InputServiceServer::SimulateKey(
	::grpc::ServerContext* context,
	const ::tvinputservice::KeyRequest* request,
	::tvinputservice::KeyResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::KeyRequest,
		::tvinputservice::KeyRequest>
	{
		static ::grpc::Status ValidateRequest(const Request& request)
		{
			const ::tvinputservice::KeyRequest_KeyState state = request.keystate();
			if (state == ::tvinputservice::KeyRequest_KeyState_Down ||
				state == ::tvinputservice::KeyRequest_KeyState_Up)
			{
				return ::grpc::Status::OK;
			}
			return ::grpc::Status{
				::grpc::StatusCode::CANCELLED,
				TvServiceBase::ErrorMessage_UnexpectedEnumValue};
		}

		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{

			KeyState keyState = KeyState::Unknown;
			switch (request.keystate())
			{
				case ::tvinputservice::KeyRequest_KeyState_Down:
					keyState = KeyState::Down;
					break;
				case ::tvinputservice::KeyRequest_KeyState_Up:
					keyState = KeyState::Up;
					break;
				case ::tvinputservice::KeyRequest_KeyState_Unknown:
				default:;
			}

			callback(
				comId,
				keyState,
				request.xkbsymbol(),
				request.unicodecharacter(),
				request.xkbmodifiers(),
				responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status InputServiceServer::SimulateMouseMove(
	::grpc::ServerContext* context,
	const ::tvinputservice::MouseMoveRequest* request,
	::tvinputservice::MouseMoveResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::MouseMove,
		::tvinputservice::MouseMoveRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(comId, request.posx(), request.posy(), responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status InputServiceServer::SimulateMousePressRelease(
	::grpc::ServerContext* context,
	const ::tvinputservice::MousePressReleaseRequest* request,
	::tvinputservice::MousePressReleaseResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::MousePressRelease,
		::tvinputservice::MousePressReleaseRequest>
	{
		static ::grpc::Status ValidateRequest(const Request& request)
		{
			const ::tvinputservice::MousePressReleaseRequest_MouseButtonState state =
				request.mousebuttonstate();
			if (!(state == ::tvinputservice::MousePressReleaseRequest_MouseButtonState_Pressed ||
				  state == ::tvinputservice::MousePressReleaseRequest_MouseButtonState_Released))
			{
				return ::grpc::Status{
					::grpc::StatusCode::CANCELLED,
					TvServiceBase::ErrorMessage_UnexpectedEnumValue};
			}

			const ::tvinputservice::MouseButton button = request.button();
			if (!(button == ::tvinputservice::MouseButton::Left ||
				  button == ::tvinputservice::MouseButton::Middle ||
				  button == ::tvinputservice::MouseButton::Right))
			{
				return ::grpc::Status{
					::grpc::StatusCode::CANCELLED,
					TvServiceBase::ErrorMessage_UnexpectedEnumValue};
			}

			return ::grpc::Status::OK;
		}

		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			KeyState keyState = KeyState::Unknown;
			switch (request.mousebuttonstate())
			{
				case ::tvinputservice::MousePressReleaseRequest_MouseButtonState_Pressed:
					keyState = KeyState::Down;
					break;
				case ::tvinputservice::MousePressReleaseRequest_MouseButtonState_Released:
					keyState = KeyState::Up;
					break;
				case ::tvinputservice::MousePressReleaseRequest_MouseButtonState_Unknown:
				default:;
			}

			MouseButton mouseButton = MouseButton::Unknown;
			switch (request.button())
			{
				case ::tvinputservice::MouseButton::Left:
					mouseButton = MouseButton::Left;
					break;
				case ::tvinputservice::MouseButton::Middle:
					mouseButton = MouseButton::Middle;
					break;
				case ::tvinputservice::MouseButton::Right:
					mouseButton = MouseButton::Right;
					break;
				case ::tvinputservice::MouseButton::Unknown:
				default:;
			}

			callback(
				comId,
				keyState,
				request.posx(),
				request.posy(),
				mouseButton,
				responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

::grpc::Status InputServiceServer::SimulateMouseWheel(
	::grpc::ServerContext* context,
	const ::tvinputservice::MouseWheelRequest* request,
	::tvinputservice::MouseWheelResponse* response)
{
	struct Meta: DefaultMeta<
		CBEnum::MouseWheel,
		::tvinputservice::MouseWheelRequest>
	{
		static void InvokeCallback(
			const Callback& callback,
			const std::string& comId,
			const Request& request,
			const ResponseProcessing& responseProcessing)
		{
			callback(
				comId,
				request.posx(),
				request.posy(),
				request.angle(),
				responseProcessing);
		}
	};

	return ServerCall<Meta>(context, request, response);
}

} // namespace InputService
} // namespace TVRemoteScreenSDKCommunication
