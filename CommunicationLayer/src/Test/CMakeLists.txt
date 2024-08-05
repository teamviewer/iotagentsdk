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
project(TVRemoteScreenSDKCommunicationTest)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

link_libraries(atomic)

set(SOURCES_TESTDATA
	TestData/TestDataAccessControlIn.h
	TestData/TestDataAccessControlOut.h
	TestData/TestDataAugmentRCSessionConsumer.h
	TestData/TestDataAugmentRCSessionControl.h
	TestData/TestDataChatIn.h
	TestData/TestDataChatOut.h
	TestData/TestDataConnectionConfirmation.h
	TestData/TestDataConnectivity.h
	TestData/TestDataImage.h
	TestData/TestDataImageNotifcation.h
	TestData/TestDataInput.h
	TestData/TestDataInstantSupport.h
	TestData/TestDataRegistration.h
	TestData/TestDataSessionControl.h
	TestData/TestDataSessionStatus.h
	TestData/TestDataViewGeometry.h
)
add_library(${PROJECT_NAME}_sharedTestData INTERFACE)
foreach(_sourceFile IN LISTS SOURCES_TESTDATA)
	target_sources(${PROJECT_NAME}_sharedTestData INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${_sourceFile}")
endforeach()

set(SOURCES_CLIENT
	Client/TestAccessControlInServiceClient.h
	Client/TestAccessControlOutServiceClient.h
	Client/TestAugmentRCSessionConsumerServiceClient.h
	Client/TestAugmentRCSessionControlServiceClient.h
	Client/TestChatInServiceClient.h
	Client/TestChatOutServiceClient.h
	Client/TestConnectionConfirmationServiceClient.h
	Client/TestConnectivityServiceClient.h
	Client/TestImageNotificationServiceClient.h
	Client/TestImageServiceClient.h
	Client/TestInputServiceClient.h
	Client/TestRegistrationServiceClient.h
	Client/TestInstantSupportNotificationServiceClient.h
	Client/TestInstantSupportServiceClient.h
	Client/TestSessionControlServiceClient.h
	Client/TestSessionStatusServiceClient.h
	Client/TestViewGeometryServiceClient.h
)
add_library(${PROJECT_NAME}_sharedClient INTERFACE)
foreach(_sourceFile IN LISTS SOURCES_CLIENT)
	target_sources(${PROJECT_NAME}_sharedClient INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${_sourceFile}")
endforeach()
target_link_libraries(${PROJECT_NAME}_sharedClient
	INTERFACE
		${PROJECT_NAME}_sharedTestData
		Services
)

set(SOURCES_SERVER
	Server/TestAccessControlInServiceServer.h
	Server/TestAccessControlOutServiceServer.h
	Server/TestAugmentRCSessionConsumerServiceServer.h
	Server/TestAugmentRCSessionControlServiceServer.h
	Server/TestChatInServiceServer.h
	Server/TestChatOutServiceServer.h
	Server/TestConnectionConfirmationServiceServer.h
	Server/TestConnectivityService.h
	Server/TestImageNotificationService.h
	Server/TestImageService.h
	Server/TestInputService.h
	Server/TestInstantSupportNotificationServiceServer.h
	Server/TestInstantSupportServiceServer.h
	Server/TestRegistrationService.h
	Server/TestSessionControlService.h
	Server/TestSessionStatusService.h
	Server/TestViewGeometryService.h
)
add_library(${PROJECT_NAME}_sharedServer INTERFACE)
foreach(_sourceFile IN LISTS SOURCES_SERVER)
	target_sources(${PROJECT_NAME}_sharedServer INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${_sourceFile}")
endforeach()
target_link_libraries(${PROJECT_NAME}_sharedServer
	INTERFACE
		${PROJECT_NAME}_sharedTestData
		Services
)

set(SOURCES_CLIENT_PERFORMANCE
	Client/ImageTestPerformanceClient.h
	Client/InputTestPerformanceClient.h
)
add_library(${PROJECT_NAME}_performanceClient INTERFACE)
foreach(_sourceFile IN LISTS SOURCES_CLIENT_PERFORMANCE)
	target_sources(${PROJECT_NAME}_performanceClient INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${_sourceFile}")
endforeach()
target_link_libraries(${PROJECT_NAME}_performanceClient INTERFACE Services)

set(SOURCES_SERVER_PERFORMANCE
	Server/ImageTestPerformanceServer.h
	Server/InputTestPerformanceServer.h
)
add_library(${PROJECT_NAME}_performanceServer INTERFACE)
foreach(_sourceFile IN LISTS SOURCES_SERVER_PERFORMANCE)
	target_sources(${PROJECT_NAME}_performanceServer INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${_sourceFile}")
endforeach()
target_link_libraries(${PROJECT_NAME}_performanceServer INTERFACE Services)

set(SOURCES_PERFORMANCETEST_CLIENT
	main_TestPerformance_Client.cpp
)
add_executable(${PROJECT_NAME}_PerformanceTestClient ${SOURCES_PERFORMANCETEST_CLIENT})
target_link_libraries(${PROJECT_NAME}_PerformanceTestClient PRIVATE ${PROJECT_NAME}_performanceClient)

set(SOURCES_PERFORMANCETEST_SERVER
	main_TestPerformance_Server.cpp
)
add_executable(${PROJECT_NAME}_PerformanceTestServer ${SOURCES_PERFORMANCETEST_SERVER})
target_link_libraries(${PROJECT_NAME}_PerformanceTestServer PRIVATE ${PROJECT_NAME}_performanceServer)

set(SOURCES_INTEGRATIONTEST
	main_TestIntegration.cpp
)

add_executable(${PROJECT_NAME}_Integration ${SOURCES_INTEGRATIONTEST})
target_link_libraries(${PROJECT_NAME}_Integration PRIVATE ${PROJECT_NAME}_sharedClient)
target_link_libraries(${PROJECT_NAME}_Integration PRIVATE ${PROJECT_NAME}_sharedServer)
add_test(NAME ${PROJECT_NAME}_Integration COMMAND ${PROJECT_NAME}_Integration)

set(SOURCES_PARSEURLTEST
	main_TestParseUrl.cpp
)
add_executable(${PROJECT_NAME}_ParseUrl ${SOURCES_PARSEURLTEST})
target_link_libraries(${PROJECT_NAME}_ParseUrl PRIVATE CommunicationLayerBase)
add_test(NAME ${PROJECT_NAME}_ParseUrl COMMAND ${PROJECT_NAME}_ParseUrl)

set(SOURCES_VERSIONNUMBERTEST
	main_TestVersionNumber.cpp
)
add_executable(${PROJECT_NAME}_VersionNumber ${SOURCES_VERSIONNUMBERTEST})
target_link_libraries(${PROJECT_NAME}_VersionNumber PRIVATE CommunicationLayerBase)
add_test(NAME ${PROJECT_NAME}_VersionNumber COMMAND ${PROJECT_NAME}_VersionNumber)
