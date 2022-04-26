#********************************************************************************#
# MIT License                                                                    #
#                                                                                #
# Copyright (c) 2021 TeamViewer Germany GmbH                                     #
#                                                                                #
# Permission is hereby granted, free of charge, to any person obtaining a copy   #
# of this software and associated documentation files (the "Software"), to deal  #
# in the Software without restriction, including without limitation the rights   #
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      #
# copies of the Software, and to permit persons to whom the Software is          #
# furnished to do so, subject to the following conditions:                       #
#                                                                                #
# The above copyright notice and this permission notice shall be included in all #
# copies or substantial portions of the Software.                                #
#                                                                                #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    #
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  #
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  #
# SOFTWARE.                                                                      #
#********************************************************************************#
project(ChatService)

find_package(Protobuf 3.0 REQUIRED)
find_package(gRPC REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES_EXPORT
	export/TVRemoteScreenSDKCommunication/ChatService/Chat.h
	export/TVRemoteScreenSDKCommunication/ChatService/IChatInServiceClient.h
	export/TVRemoteScreenSDKCommunication/ChatService/IChatInServiceServer.h
	export/TVRemoteScreenSDKCommunication/ChatService/InServiceFactory.cpp
	export/TVRemoteScreenSDKCommunication/ChatService/InServiceFactory.h

	export/TVRemoteScreenSDKCommunication/ChatService/IChatOutServiceClient.h
	export/TVRemoteScreenSDKCommunication/ChatService/IChatOutServiceServer.h
	export/TVRemoteScreenSDKCommunication/ChatService/OutServiceFactory.cpp
	export/TVRemoteScreenSDKCommunication/ChatService/OutServiceFactory.h
)

set(SOURCES_INTERNAL
	internal/Client/ChatInServiceClient.cpp
	internal/Client/ChatInServiceClient.h
	internal/Server/ChatInServiceServer.cpp
	internal/Server/ChatInServiceServer.h

	internal/Client/ChatOutServiceClient.cpp
	internal/Client/ChatOutServiceClient.h
	internal/Server/ChatOutServiceServer.cpp
	internal/Server/ChatOutServiceServer.h
)

set(SOURCES_INTERNAL_PROTOBUFFERS_SERVICES
	internal/proto/ChatInService.proto
	internal/proto/ChatOutService.proto
)

set(SOURCES_INTERNAL_PROTOBUFFERS
	${SOURCES_INTERNAL_PROTOBUFFERS_SERVICES}
	internal/proto/Chat.proto
	internal/proto/ChatOutMessages.proto
	internal/proto/ChatInMessages.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${SOURCES_INTERNAL_PROTOBUFFERS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${SOURCES_INTERNAL_PROTOBUFFERS_SERVICES})

add_library(${PROJECT_NAME} STATIC
	${SOURCES_EXPORT}
	${SOURCES_INTERNAL}
	${SOURCES_INTERNAL_PROTOBUFFERS}
	${PROTO_SRCS} ${PROTO_HDRS}
	${GRPC_SRCS} ${GRPC_HDRS}
)
add_library(${TV_NAMESPACE}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	SYSTEM PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/export>
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		protobuf::libprotobuf gRPC::grpc++_reflection
		ServiceBase
	PUBLIC
		CommunicationLayerBase
)

install_tvsdk(${PROJECT_NAME})
