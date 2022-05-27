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
project(CommunicationLayerBase)

set(SOURCES_EXPORT
	export/TVRemoteScreenSDKCommunication/CommunicationLayerBase/CallStatus.h
	export/TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceClient.h
	export/TVRemoteScreenSDKCommunication/CommunicationLayerBase/IServiceServer.h
	export/TVRemoteScreenSDKCommunication/CommunicationLayerBase/ServiceType.h
)

add_library(${PROJECT_NAME} INTERFACE)

foreach(_item IN LISTS SOURCES_EXPORT)
	target_sources(${PROJECT_NAME} INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${_item}>
	)
endforeach()

target_include_directories(${PROJECT_NAME} INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/export>
)

install_tvsdk(${PROJECT_NAME})
