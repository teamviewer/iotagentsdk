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
project(InputService)

find_package(Protobuf 3.0 REQUIRED)
find_package(gRPC REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES_EXPORT
	export/TVRemoteScreenSDKCommunication/InputService/IInputServiceClient.h
	export/TVRemoteScreenSDKCommunication/InputService/IInputServiceServer.h
	export/TVRemoteScreenSDKCommunication/InputService/KeyState.h
	export/TVRemoteScreenSDKCommunication/InputService/MouseButton.h
	export/TVRemoteScreenSDKCommunication/InputService/ServiceFactory.cpp
	export/TVRemoteScreenSDKCommunication/InputService/ServiceFactory.h
)

set(SOURCES_INTERNAL
	internal/Client/InputServiceClient.cpp
	internal/Client/InputServiceClient.h
	internal/Server/InputServiceServer.cpp
	internal/Server/InputServiceServer.h
)

set(SOURCES_INTERNAL_PROTOBUFFERS_SERVICES
	internal/proto/InputService.proto
)

set(SOURCES_INTERNAL_PROTOBUFFERS
	${SOURCES_INTERNAL_PROTOBUFFERS_SERVICES}
	internal/proto/KeyRequest.proto
	internal/proto/KeyResponse.proto
	internal/proto/MouseButton.proto
	internal/proto/MouseMoveRequest.proto
	internal/proto/MouseMoveResponse.proto
	internal/proto/MousePressReleaseRequest.proto
	internal/proto/MousePressReleaseResponse.proto
	internal/proto/MouseWheelRequest.proto
	internal/proto/MouseWheelResponse.proto
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
