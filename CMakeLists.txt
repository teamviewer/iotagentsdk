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

project(TVAgentSDK)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 14)
add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(CommunicationLayer/src)

option(ENABLE_QT_PLUGIN "Enable TVQtRC plugin and Qt example application" ON)
option(ENABLE_PYTHON_MODULE "Enable Python module for TVAgentAPI library" OFF)

set(TV_PACKAGE_NAME ${PROJECT_NAME})
set(TV_NAMESPACE ${TV_PACKAGE_NAME})

set(TV_PACKAGE_NAME_LEGACY TVRemoteScreenSDK)
set(TV_NAMESPACE_LEGACY ${TV_PACKAGE_NAME_LEGACY})

if(NOT (DEFINED ${TV_PACKAGE_NAME}_VERSION))
	set(TV_VERSION_FILE "${CMAKE_SOURCE_DIR}/../VERSION.txt")
	if(EXISTS ${TV_VERSION_FILE})
		file(STRINGS ${TV_VERSION_FILE} ${TV_PACKAGE_NAME}_VERSION LIMIT_COUNT 1 REGEX "[1-9][0-9]*\.[0-9]+\.[0-9]+")
	else()
		set(${TV_PACKAGE_NAME}_VERSION 1.0.0)
	endif()
endif()

include(CMakePackageConfigHelpers)

macro(tv_configure_package package_name package_name_version package_namespace package_compatibility)
	message(STATUS "Configuring Package ${package_name}: Version: ${package_name_version}, Namespace: ${package_namespace}")

	write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/${package_name}/${package_name}ConfigVersion.cmake"
		VERSION ${package_name_version}
		COMPATIBILITY ${package_compatibility}
	)

	if(ENABLE_QT_PLUGIN)
		set(_CONFIG_PREAMBLE_ "find_package(Qt5 5.3 COMPONENTS Core REQUIRED)")
	endif()

	set(_TV_PACKAGE_NAME_ ${package_name})

	configure_file(
		CMake/Config.cmake.in
		"${CMAKE_CURRENT_BINARY_DIR}/${package_name}/${package_name}Config.cmake"
		@ONLY
	)

	unset(_CONFIG_PREAMBLE_)
	unset(_TV_PACKAGE_NAME_)

	install(
		EXPORT ${package_name}
		NAMESPACE ${package_namespace}::
		DESTINATION lib/cmake/${package_name}
	)
	install(
		FILES
			"${CMAKE_CURRENT_BINARY_DIR}/${package_name}/${package_name}Config.cmake"
			"${CMAKE_CURRENT_BINARY_DIR}/${package_name}/${package_name}ConfigVersion.cmake"
		DESTINATION lib/cmake/${package_name}
	)
endmacro()

add_subdirectory(TVAgentAPI)
add_subdirectory(TVAgentAPIPrivate)

if(ENABLE_QT_PLUGIN)
	add_subdirectory(TVQtRC)
	add_subdirectory(app)
endif()

add_subdirectory(examples)

if(ENABLE_PYTHON_MODULE)
	add_subdirectory(Bindings/Python)
endif()

tv_configure_package(${TV_PACKAGE_NAME} ${${TV_PACKAGE_NAME}_VERSION} ${TV_NAMESPACE} SameMajorVersion)
if(ENABLE_QT_PLUGIN)
	tv_configure_package(${TV_PACKAGE_NAME_LEGACY} ${${TV_PACKAGE_NAME}_VERSION} ${TV_NAMESPACE_LEGACY} AnyNewerVersion)
endif()
