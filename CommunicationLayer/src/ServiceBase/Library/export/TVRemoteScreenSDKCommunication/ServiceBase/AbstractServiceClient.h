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

#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceClient.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h>
#include <TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h>

#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include <memory>
#include <string>
#include <type_traits>

namespace TVRemoteScreenSDKCommunication
{

namespace ServiceBase
{

template<typename ClientInterface, typename gRPCService>
class AbstractServiceClient : public ClientInterface
{
	static_assert(
		std::is_base_of<IServiceClient, ClientInterface>::value,
		"ClientInterface must inherit from IServiceClient");

public:
	using gRPCStub = typename gRPCService::Stub;

	explicit AbstractServiceClient(ServiceType serviceType)
		: m_serviceType{serviceType}
	{
	}

	~AbstractServiceClient() override = default;

	void StartClient(const std::string& destination) override
	{
		m_destination = destination;
		m_channel = ::grpc::CreateChannel(
			m_destination,
			::grpc::InsecureChannelCredentials());
		m_stub = gRPCService::NewStub(m_channel);
	}

	void StopClient(bool force) override
	{
		m_stub.reset();
		m_channel.reset();
	}

	ServiceType GetServiceType() const override
	{
		return m_serviceType;
	}

	const std::string& GetDestination() const override
	{
		return m_destination;
	}

protected:
	using BaseType = AbstractServiceClient;

	struct DefaultMeta
	{
		struct None{};

		using Request = None;
		using Response = None;
		using Return = CallStatus;

		static bool ValidateInput(const std::string& comId, ...)
		{
			return !comId.empty();
		}

		static void PrepareContext(
			::grpc::ClientContext& context,
			const std::string& comId,
			...)
		{
			context.AddMetadata(ServiceBase::CommunicationIdToken, comId);
		}

		static ::grpc::Status Call(...)
		{
			return ::grpc::Status::CANCELLED;
		}

		static Return HandleResponse(const ::grpc::Status& status, ...)
		{
			if (status.ok())
			{
				return Return{CallState::Ok};
			}
			return Return{CallState::Failed, status.error_message()};
		}
	};

	template <typename Meta = DefaultMeta, typename ... Ts>
	typename Meta::Return ClientCall(const std::string& comId, Ts ... args)
	{
		using Return = typename Meta::Return;

		if (!m_channel || !m_stub)
		{
			return Return{
				CallState::Failed,
				TvServiceBase::ErrorMessage_MissingStartClient};
		}

		if (!Meta::ValidateInput(comId, args...))
		{
			return Return{
				CallState::Failed,
				TvServiceBase::ErrorMessage_InvalidInputParameter};
		}

		::grpc::ClientContext context;
		Meta::PrepareContext(context, comId, args...);

		typename Meta::Request request;
		typename Meta::Response response;

		const ::grpc::Status status =
			Meta::Call(
				*m_stub.get(),
				context,
				request,
				response,
				args...);

		return Meta::HandleResponse(status, response, args...);
	}

	std::string m_destination;
	std::shared_ptr<::grpc::ChannelInterface> m_channel;
	std::unique_ptr<gRPCStub> m_stub;
	ServiceType m_serviceType;
};

} // namespace ServiceBase

} // namespace TVRemoteScreenSDKCommunication
