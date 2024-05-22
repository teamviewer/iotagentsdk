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
cmake_minimum_required(VERSION 3.3)

project(TVRemoteScreenSDKCommunication)

option(TV_COMM_ENABLE_GRPC "Enable gRPC for communication" ON)
option(TV_COMM_ENABLE_PLAIN_SOCKET "Enable plain C sockets for communication" ON)
option(ENABLE_TESTS "Enable Tests" ON)

message(STATUS "CommunicationLayer configured transports:")
message(STATUS "  gRPC: ${TV_COMM_ENABLE_GRPC}")
message(STATUS "  plain socket: ${TV_COMM_ENABLE_PLAIN_SOCKET}")

if(NOT TV_COMM_ENABLE_GRPC AND NOT TV_COMM_ENABLE_PLAIN_SOCKET)
	message(FATAL_ERROR "Neither TV_COMM_ENABLE_GRPC nor TV_COMM_ENABLE_PLAIN_SOCKET option is enabled.")
endif()

set(TV_PACKAGE_NAME ${PROJECT_NAME})
set(TV_NAMESPACE ${TV_PACKAGE_NAME})

if(TV_COMM_ENABLE_GRPC)
	add_definitions(-DTV_COMM_ENABLE_GRPC)
	include("CMake/grpc_utils.cmake")
endif()

if(TV_COMM_ENABLE_PLAIN_SOCKET)
	add_definitions(-DTV_COMM_ENABLE_PLAIN_SOCKET)
endif()

if(NOT (DEFINED ${TV_PACKAGE_NAME}_VERSION))
	set(${TV_PACKAGE_NAME}_VERSION 1.0)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake/modules")

# for compatibility with cmake < 3.9 create necessary interface target
find_package(Protobuf REQUIRED)
if(NOT (TARGET protobuf::libprotobuf))
	add_library(protobuf::libprotobuf INTERFACE IMPORTED)
	set_property(TARGET protobuf::libprotobuf PROPERTY INTERFACE_LINK_LIBRARIES ${PROTOBUF_LIBRARIES})
	set_property(TARGET protobuf::libprotobuf PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PROTOBUF_INCLUDE_DIRS})
endif()

macro(install_tvsdk)
	install(TARGETS ${ARGN}
		EXPORT ${TV_PACKAGE_NAME}
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
		INCLUDES DESTINATION include
	)
	install(DIRECTORY export/
		DESTINATION include
		FILES_MATCHING PATTERN "*.h"
	)
endmacro()

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)

add_subdirectory(CommunicationLayerBase)
add_subdirectory(ServiceBase)

add_subdirectory(Services)

if(ENABLE_TESTS)
	enable_testing()
	add_subdirectory(Test)
endif()

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/${TV_PACKAGE_NAME}/${TV_PACKAGE_NAME}ConfigVersion.cmake"
	VERSION ${${TV_PACKAGE_NAME}_VERSION}
	COMPATIBILITY AnyNewerVersion
)

configure_file(CMake/Config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/${TV_PACKAGE_NAME}/${TV_PACKAGE_NAME}Config.cmake"
	@ONLY
)

install(EXPORT ${TV_PACKAGE_NAME}
	NAMESPACE ${TV_NAMESPACE}::
	DESTINATION lib/cmake/${TV_PACKAGE_NAME}
)
install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${TV_PACKAGE_NAME}/${TV_PACKAGE_NAME}Config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/${TV_PACKAGE_NAME}/${TV_PACKAGE_NAME}ConfigVersion.cmake"
	"${CMAKE_CURRENT_LIST_DIR}/CMake/modules/FindgRPC.cmake"
	DESTINATION lib/cmake/${TV_PACKAGE_NAME}
)
