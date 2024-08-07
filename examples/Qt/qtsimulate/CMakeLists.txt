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
project(example_Qt)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt${QT_MAJOR_VERSION} COMPONENTS Core Widgets Quick REQUIRED)

add_executable(qt_simulate
	"main.cpp"
	"AppModel.h"
	"AppModel.cpp"
	"qml.qrc"
	"WidgetsDemoWindow.ui"
	"QRCodeImageProvider.h"
	"QRCodeImageProvider.cpp"
	"ThirdParty/qrcodegen.hpp"
	"ThirdParty/qrcodegen.cpp"
)

target_link_libraries(qt_simulate Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Widgets Qt${QT_MAJOR_VERSION}::Quick)

if(BUILD_SHARED_LIBS)
	target_link_libraries(qt_simulate TVQtRC.interface)
	add_dependencies(qt_simulate TVQtRC)
	add_custom_command(TARGET qt_simulate POST_BUILD
		COMMAND [ -L $<TARGET_FILE_NAME:TVQtRC> ] || ln -s $<TARGET_FILE:TVQtRC> $<TARGET_FILE_NAME:TVQtRC>
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
else()
	target_link_libraries(qt_simulate TVQtRC)
endif()
