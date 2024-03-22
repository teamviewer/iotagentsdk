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
project(ServiceBase)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES_EXPORT
	export/TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h
	export/TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h
	export/TVRemoteScreenSDKCommunication/ServiceBase/ServiceErrorMessage.h
	export/TVRemoteScreenSDKCommunication/ServiceBase/ServiceFactory.h
)

set(LINK_LIBRARIES
	protobuf::libprotobuf
)

if(TV_COMM_ENABLE_GRPC)
	find_package(gRPC REQUIRED)

	list(APPEND SOURCES_EXPORT
		export/TVRemoteScreenSDKCommunication/ServiceBase/gRPCTransport.cpp
		export/TVRemoteScreenSDKCommunication/ServiceBase/gRPCTransport.h
	)

	list(APPEND LINK_LIBRARIES
		gRPC::grpc++_reflection
	)
endif()

if(TV_COMM_ENABLE_PLAIN_SOCKET)
	find_package(Protobuf 3.0 REQUIRED)

	list(APPEND SOURCES_EXPORT
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIOTransport.cpp
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIOTransport.h
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/ServiceMeta.h
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/ChannelInterface.cpp
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/ChannelInterface.h
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/ClientContext.cpp
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/ClientContext.h
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/ServerContext.cpp
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/ServerContext.h
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/Server.cpp
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/Server.h
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/Service.h
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/Socket.cpp
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/Socket.h
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/Status.cpp
		export/TVRemoteScreenSDKCommunication/ServiceBase/SocketIO/Status.h
	)

	list(APPEND SOURCES_INTERNAL_PROTOBUFFERS
		internal/proto/Envelope.proto
	)

	protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${SOURCES_INTERNAL_PROTOBUFFERS})
endif()

add_library(${PROJECT_NAME} STATIC
	${SOURCES_EXPORT}
	${SOURCES_INTERNAL_PROTOBUFFERS}
	${PROTO_SRCS}
	${PROTO_HDRS}
)

add_library(${TV_NAMESPACE}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/export>
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		${LINK_LIBRARIES}
		CommunicationLayerBase
)

install_tvsdk(${PROJECT_NAME})
