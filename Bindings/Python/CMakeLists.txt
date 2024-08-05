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
cmake_minimum_required(VERSION 3.12)
project(bindings_Python)

# Stick to https://devguide.python.org/versions/
find_package (Python3 3.8 REQUIRED COMPONENTS Development)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE
	PyAccessControlModule.cpp
	PyAccessControlModule.h
	PyAgentConnection.cpp
	PyAgentConnection.h
	PyAugmentRCSessionModule.cpp
	PyAugmentRCSessionModule.h
	PyChatModule.cpp
	PyChatModule.h
	PyInstantSupportModule.cpp
	PyInstantSupportModule.h
	PyLogging.cpp
	PyLogging.h
	PyTVAgentAPI.cpp
	PyTVAgentAPI.h
	PyTVSessionManagementModule.cpp
	PyTVSessionManagementModule.h
	PyModuleType.cpp
	PyModuleType.h
	PyWeakObject.cpp
	PyWeakObject.h
	PythonHelpers.cpp
	PythonHelpers.h
	Typesystem.cpp
	Typesystem.h
	module.cpp
	prototypes.h)

# The module can be tested from the build directory,
# just set PYTHONPATH environment variable to
# the build directory path (where the linked .so module resides).
# See https://docs.python.org/3.8/using/cmdline.html#envvar-PYTHONPATH
# Very useful for debugging.

Python3_add_library(${PROJECT_NAME} MODULE ${SOURCE})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "tvagentapi")

target_link_libraries(${PROJECT_NAME} PRIVATE TVAgentApi)
