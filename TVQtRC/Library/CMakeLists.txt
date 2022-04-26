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
project(TVQtRC)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 5.3 COMPONENTS Core Gui Quick REQUIRED)

set(TVQTRC_INTERFACE
	export/TVQtRC/AbstractChat.h
	export/TVQtRC/AccessControl.h
	export/TVQtRC/ChatInfo.h
	export/TVQtRC/ConnectionData.h
	export/TVQtRC/ControlMode.h
	export/TVQtRC/InstantSupportData.h
	export/TVQtRC/InstantSupportError.h
	export/TVQtRC/BaseUrlParseResultCode.h
	export/TVQtRC/Interface.h
)

set(TVQTRC_SOURCES
	internal/Chat/Chat.cpp
	internal/Chat/Chat.h

	internal/Commands/SimulateKeyCommand.cpp
	internal/Commands/SimulateKeyCommand.h
	internal/Commands/SimulateMouseCommand.cpp
	internal/Commands/SimulateMouseCommand.h

	internal/Communication/CommunicationAdapter.cpp
	internal/Communication/CommunicationAdapter.h
	internal/Communication/ScreenGrabStrategy.h

	internal/Grabbing/Screen/AbstractScreenGrabMethod.cpp
	internal/Grabbing/Screen/AbstractScreenGrabMethod.h
	internal/Grabbing/Screen/ColorFormat.cpp
	internal/Grabbing/Screen/ColorFormat.h
	internal/Grabbing/Screen/QWindowGrabMethod.cpp
	internal/Grabbing/Screen/QWindowGrabMethod.h
	internal/Grabbing/Screen/QWindowGrabNotifier.cpp
	internal/Grabbing/Screen/QWindowGrabNotifier.h
	internal/Grabbing/Screen/ScreenGrabResult.cpp
	internal/Grabbing/Screen/ScreenGrabResult.h

	internal/InputSimulation/AbstractInputSimulator.h
	internal/InputSimulation/InputSimulator.cpp
	internal/InputSimulation/InputSimulator.h
	internal/InputSimulation/XKBMap.cpp
	internal/InputSimulation/XKBMap.h

	internal/Logging/ILogging.h
	internal/Logging/Logging.cpp
	internal/Logging/Logging.h
	internal/Logging/LoggingProxy.cpp
	internal/Logging/LoggingProxy.h
	internal/Logging/LoggingPrivateAdapter.cpp
	internal/Logging/LoggingPrivateAdapter.h
	
	internal/TVQtRCPlugin.cpp
	internal/TVQtRCPlugin.h
)

add_library(${PROJECT_NAME}.interface INTERFACE)
target_include_directories(${PROJECT_NAME}.interface INTERFACE
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/export>
	$<INSTALL_INTERFACE:include>)

option(BUILD_SHARED_LIBS "Build the plugin as a shared lib" ON)

add_library(${PROJECT_NAME} ${TVQTRC_INTERFACE} ${TVQTRC_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}.interface)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Quick)
target_link_libraries(${PROJECT_NAME} PRIVATE TVAgentAPIPrivate)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wshadow)

if(BUILD_SHARED_LIBS)
	target_sources(${PROJECT_NAME}.interface INTERFACE
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/export/TVQtRC/AbstractChat.h>
		$<INSTALL_INTERFACE:include/TVQtRC/AbstractChat.h>)
endif()

if(BUILD_SHARED_LIBS)
	message(STATUS "Configuring ${PROJECT_NAME} to be shared")
else()
	target_compile_definitions(${PROJECT_NAME} PRIVATE QT_STATICPLUGIN)
	target_compile_definitions(${PROJECT_NAME} INTERFACE TV_STATIC_TVQTRCPLUGIN)
	message(STATUS "Configuring ${PROJECT_NAME} to be static")
endif()

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	TVAgentAPIPrivate
	AccessControlService
	ChatService
	ConnectionConfirmationService
	ConnectivityService
	ImageNotificationService
	ImageService
	InputService
	InstantSupportService
	RegistrationService
	SessionControlService
	SessionStatusService
	ViewGeometryService
)

set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(${TV_NAMESPACE}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_library(${TV_NAMESPACE}::${PROJECT_NAME}.interface ALIAS ${PROJECT_NAME}.interface)

add_library(${TV_NAMESPACE_LEGACY}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_library(${TV_NAMESPACE_LEGACY}::${PROJECT_NAME}.interface ALIAS ${PROJECT_NAME}.interface)

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}.interface
	EXPORT ${TV_PACKAGE_NAME}
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	INCLUDES DESTINATION include
)
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}.interface
	EXPORT ${TV_PACKAGE_NAME_LEGACY}
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	INCLUDES DESTINATION include
)
install(DIRECTORY export/
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
)
