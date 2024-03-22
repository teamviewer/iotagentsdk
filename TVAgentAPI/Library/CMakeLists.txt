#********************************************************************************#
# MIT License                                                                    #
#                                                                                #
# Copyright (c) 2022 TeamViewer Germany GmbH                                     #
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
project(TVAgentApi)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(TVAgentApi_INTERFACE
	export/TVAgentAPI/AccessControlModuleStringify.h
	export/TVAgentAPI/AgentConnectionStringify.h
	export/TVAgentAPI/Callback.h
	export/TVAgentAPI/ChatModuleStringify.h
	export/TVAgentAPI/IAccessControlModule.h
	export/TVAgentAPI/IAgentAPI.h
	export/TVAgentAPI/IAgentConnection.h
	export/TVAgentAPI/IChatModule.h
	export/TVAgentAPI/IInstantSupportModule.h
	export/TVAgentAPI/ILogging.h
	export/TVAgentAPI/IModule.h
	export/TVAgentAPI/InstantSupportModuleStringify.h
	export/TVAgentAPI/ITVSessionManagementModule.h
	export/TVAgentAPI/ModuleStringify.h
	export/TVAgentAPI/tvagentapi.h
)

set(TVAgentApi_SOURCES
	internal/AccessControl/AccessControlModule.cpp
	internal/AccessControl/AccessControlModule.h
	internal/AccessControl/AccessControlModuleStringify.cpp
	internal/AgentConnection/AgentConnection.cpp
	internal/AgentConnection/AgentConnection.h
	internal/AgentConnection/AgentConnectionProxy.cpp
	internal/AgentConnection/AgentConnectionProxy.h
	internal/AgentConnection/AgentConnectionStringify.cpp
	internal/AsyncOperation/IDispatcher.h
	internal/AsyncOperation/LazyDispatcher.cpp
	internal/AsyncOperation/LazyDispatcher.h
	internal/Chat/ChatModule.cpp
	internal/Chat/ChatModule.h
	internal/Chat/ChatModuleStringify.cpp
	internal/Logging/FileLogging.cpp
	internal/Logging/FileLogging.h
	internal/Logging/LoggingPrivateAdapter.cpp
	internal/Logging/LoggingPrivateAdapter.h
	internal/Logging/LoggingProxy.cpp
	internal/Logging/LoggingProxy.h
	internal/InstantSupport/InstantSupportModule.cpp
	internal/InstantSupport/InstantSupportModule.h
	internal/InstantSupport/InstantSupportModuleStringify.cpp
	internal/Utils/CallbackUtils.h
	internal/Utils/DispatcherUtils.h
	internal/TVSessionManagement/TVSessionManagementModule.cpp
	internal/TVSessionManagement/TVSessionManagementModule.h
	internal/AgentAPI.cpp
	internal/AgentAPI.h
	internal/ModuleFactory.cpp
	internal/ModuleFactory.h
	internal/ModuleStringify.cpp
	internal/tvagentapi.cpp
)

add_library(${PROJECT_NAME}.interface INTERFACE)
target_include_directories(${PROJECT_NAME}.interface INTERFACE
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/export>
	$<INSTALL_INTERFACE:include>)

option(BUILD_SHARED_LIBS "Build the plugin as a shared lib" ON)

add_library(${PROJECT_NAME} ${TVAgentApi_INTERFACE} ${TVAgentApi_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/internal)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}.interface)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wshadow)

if(BUILD_SHARED_LIBS)
	message(STATUS "Configuring ${PROJECT_NAME} to be shared")
else()
	message(STATUS "Configuring ${PROJECT_NAME} to be static")
endif()

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		TVAgentAPIPrivate
		Services
)

set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(${TV_NAMESPACE}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_library(${TV_NAMESPACE}::${PROJECT_NAME}.interface ALIAS ${PROJECT_NAME}.interface)

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}.interface
	EXPORT ${TV_PACKAGE_NAME}
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	INCLUDES DESTINATION include
)
install(DIRECTORY export/
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
)
