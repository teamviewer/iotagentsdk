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

find_package(gRPC REQUIRED)

set(SOURCES_EXPORT
	export/TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceClient.h
	export/TVRemoteScreenSDKCommunication/ServiceBase/AbstractServiceServer.h
	export/TVRemoteScreenSDKCommunication/ServiceBase/ClientErrorMessage.h
	export/TVRemoteScreenSDKCommunication/ServiceBase/RequestMetadata.h
	export/TVRemoteScreenSDKCommunication/ServiceBase/ServerFactory.cpp
	export/TVRemoteScreenSDKCommunication/ServiceBase/ServerFactory.h
	export/TVRemoteScreenSDKCommunication/ServiceBase/ServerParams.h
	export/TVRemoteScreenSDKCommunication/ServiceBase/ServiceErrorMessage.h
)

add_library(${PROJECT_NAME} STATIC
	${SOURCES_EXPORT}
)
add_library(${TV_NAMESPACE}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/export>
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		gRPC::grpc++_reflection
		CommunicationLayerBase
)

install_tvsdk(${PROJECT_NAME})
