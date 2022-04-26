//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2019 TeamViewer Germany GmbH                                     //
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
#pragma once

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceServer.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/ServerFactory.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/ServiceErrorMessage.h>

#include <grpc++/grpc++.h>

#include <grpcpp/server.h>

#include <memory>
#include <string>
#include <tuple>
#include <type_traits>

namespace TVRemoteScreenSDKCommunication
{

namespace ServiceBase
{

template <typename ServerInterface, typename gRPCService, typename CallbacksMeta>
class AbstractServiceServer : public gRPCService::Service, public ServerInterface
{
	static_assert(
		std::is_base_of<::grpc::Service, typename gRPCService::Service>::value,
		"gRPCService must inherit from ::grpc::Service");

	static_assert(
		std::is_base_of<IServiceServer, ServerInterface>::value,
		"ServerInterface must inherit from IServiceServer");

	static_assert(
		std::is_same<std::underlying_type_t<typename CallbacksMeta::Enum>, size_t>::value,
		"CallbacksMeta::Enum must inherit from size_t");

	static_assert(
		std::tuple_size<typename CallbacksMeta::Callbacks>::value ==
			static_cast<size_t>(CallbacksMeta::Enum::EnumSize),
		"Callbacks enum size mismatch");

public:
	using BaseType = AbstractServiceServer;

	using Callbacks = typename CallbacksMeta::Callbacks;

	using CBEnum = typename CallbacksMeta::Enum;

	template <CBEnum Name>
	using CallbackIndex = std::integral_constant<size_t, static_cast<size_t>(Name)>;

	template <CBEnum Name>
	using CallbackType = std::tuple_element_t<CallbackIndex<Name>::value, Callbacks>;

	template <CBEnum Name, typename RequestType>
	struct DefaultMeta
	{
		static_assert(
			CallbackIndex<Name>::value <= std::tuple_size<Callbacks>::value,
			"Callback name index is out of bounds");

		using Callback = CallbackType<Name>;
		using CallbackName = std::integral_constant<CBEnum, Name>;

		using ResponseProcessing = std::function<void(const CallStatus& callStatus)>;

		using Request = RequestType;

		static ::grpc::Status ValidateRequest(const Request& /*request*/)
		{
			return ::grpc::Status::OK;
		}

		static bool GetComId(std::string& comId, const ::grpc::ServerContext& context)
		{
			const auto searchComId =
				context.client_metadata().find(ServiceBase::CommunicationIdToken);

			if (searchComId == context.client_metadata().end())
			{
				return false;
			}

			comId.assign(
				(searchComId->second).data(),
				(searchComId->second).length());

			return true;
		}

		template <typename Response>
		static ResponseProcessing MakeResponseProcessing(
			::grpc::Status& returnStatus,
			Response& /*response*/)
		{
			return [&returnStatus](const CallStatus& callStatus)
			{
				if (callStatus.IsOk())
				{
					returnStatus = ::grpc::Status::OK;
					return;
				}
				returnStatus =
					::grpc::Status{::grpc::StatusCode::ABORTED, callStatus.errorMessage};
			};
		}

		static void InvokeCallback(
			const Callback& /*callback*/,
			const std::string& /*comId*/,
			const Request& /*request*/,
			const ResponseProcessing& /*responseProcessing*/)
		{
		}
	};

	explicit AbstractServiceServer(ServiceType serviceType)
		: m_serviceType{serviceType}
	{
	}

	bool StartServer(const std::string& location) override
	{
		m_location = location;

		m_server = CreateAndStartSyncServer(
			m_location,
			// Listen on the given address without any authentication mechanism.
			::grpc::InsecureServerCredentials(),
			// Register this "service" as the instance through
			// which we'll communicate with clients.
			this);

		return !!m_server;
	}

	void StopServer(bool /*force*/) override
	{
		if (m_server)
		{
			m_server->Shutdown();
			m_server.reset();
		}
	}

	ServiceType GetServiceType() const override
	{
		return m_serviceType;
	}

	const std::string& GetLocation() const override
	{
		return m_location;
	}

protected:
	template <typename Meta, typename Response>
	::grpc::Status ServerCall(
		::grpc::ServerContext* context,
		const typename Meta::Request* request,
		Response* response)
	{
		if (!context || !request || !response)
		{
			return ::grpc::Status{::grpc::StatusCode::INTERNAL, std::string{}};
		}

		const CallbackType<Meta::CallbackName::value>& callback =
			std::get<CallbackIndex<Meta::CallbackName::value>::value>(m_callbacks);
		if (!callback)
		{
			return ::grpc::Status{
				::grpc::StatusCode::UNAVAILABLE,
				TvServiceBase::ErrorMessage_NoProcessingCallback};
		}

		{
			::grpc::Status status = Meta::ValidateRequest(*request);
			if (!status.ok())
			{
				return status;
			}
		}

		std::string comId;
		if (!Meta::GetComId(comId, *context))
		{
			return ::grpc::Status{
				::grpc::StatusCode::FAILED_PRECONDITION,
				TvServiceBase::ErrorMessage_NoComId};
		}

		::grpc::Status returnStatus{
			::grpc::StatusCode::CANCELLED,
			TvServiceBase::ErrorMessage_ResponseCallbackNotCalled};

		auto responseProcessing =
			Meta::MakeResponseProcessing(returnStatus, *response);

		if (responseProcessing)
		{
			Meta::InvokeCallback(callback, comId, *request, responseProcessing);
		}

		return returnStatus;
	}

	template <CBEnum Name>
	void SetCallback(const CallbackType<Name>& callback)
	{
		std::get<CallbackIndex<Name>::value>(m_callbacks) = callback;
	}

	std::string m_location;
	std::unique_ptr<::grpc::Server> m_server;
	ServiceType m_serviceType;
	Callbacks m_callbacks;
};

} // namespace ServiceBase

} // namespace TVRemoteScreenSDKCommunication