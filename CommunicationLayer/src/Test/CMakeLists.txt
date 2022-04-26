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

foreach(_item IN LISTS SOURCES_TESTDATA)
	target_sources(${PROJECT_NAME}_sharedTestData INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${_item}")
endforeach()


set(SOURCES_CLIENT
	Client/TestAccessControlInServiceClient.cpp
	Client/TestAccessControlInServiceClient.h
	Client/TestAccessControlOutServiceClient.cpp
	Client/TestAccessControlOutServiceClient.h
	Client/TestChatInServiceClient.cpp
	Client/TestChatInServiceClient.h
	Client/TestChatOutServiceClient.cpp
	Client/TestChatOutServiceClient.h
	Client/TestConnectionConfirmationServiceClient.cpp
	Client/TestConnectionConfirmationServiceClient.h
	Client/TestConnectivityServiceClient.cpp
	Client/TestConnectivityServiceClient.h
	Client/TestImageNotificationServiceClient.cpp
	Client/TestImageNotificationServiceClient.h
	Client/TestImageServiceClient.cpp
	Client/TestImageServiceClient.h
	Client/TestInputServiceClient.cpp
	Client/TestInputServiceClient.h
	Client/TestRegistrationServiceClient.cpp
	Client/TestRegistrationServiceClient.h
	Client/TestInstantSupportNotificationServiceClient.cpp
	Client/TestInstantSupportNotificationServiceClient.h
	Client/TestInstantSupportServiceClient.cpp
	Client/TestInstantSupportServiceClient.h
	Client/TestSessionControlServiceClient.cpp
	Client/TestSessionControlServiceClient.h
	Client/TestSessionStatusServiceClient.cpp
	Client/TestSessionStatusServiceClient.h
	Client/TestViewGeometryServiceClient.cpp
	Client/TestViewGeometryServiceClient.h
)

add_library(${PROJECT_NAME}_sharedClient STATIC ${SOURCES_CLIENT})
target_link_libraries(${PROJECT_NAME}_sharedClient
	PRIVATE
		${PROJECT_NAME}_sharedTestData
	PUBLIC
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

set(SOURCES_SERVER
	Server/TestAccessControlInServiceServer.cpp
	Server/TestAccessControlInServiceServer.h
	Server/TestAccessControlOutServiceServer.cpp
	Server/TestAccessControlOutServiceServer.h
	Server/TestChatInServiceServer.cpp
	Server/TestChatInServiceServer.h
	Server/TestChatOutServiceServer.cpp
	Server/TestChatOutServiceServer.h
	Server/TestConnectionConfirmationServiceServer.cpp
	Server/TestConnectionConfirmationServiceServer.h
	Server/TestConnectivityService.cpp
	Server/TestConnectivityService.h
	Server/TestImageNotificationService.cpp
	Server/TestImageNotificationService.h
	Server/TestImageService.cpp
	Server/TestImageService.h
	Server/TestInputService.cpp
	Server/TestInputService.h
	Server/TestInstantSupportNotificationServiceServer.cpp
	Server/TestInstantSupportNotificationServiceServer.h
	Server/TestInstantSupportServiceServer.cpp
	Server/TestInstantSupportServiceServer.h
	Server/TestRegistrationService.cpp
	Server/TestRegistrationService.h
	Server/TestSessionControlService.cpp
	Server/TestSessionControlService.h
	Server/TestSessionStatusService.cpp
	Server/TestSessionStatusService.h
	Server/TestViewGeometryService.cpp
	Server/TestViewGeometryService.h
)

add_library(${PROJECT_NAME}_sharedServer STATIC ${SOURCES_SERVER})
target_link_libraries(${PROJECT_NAME}_sharedServer
	PRIVATE
		${PROJECT_NAME}_sharedTestData
	PUBLIC
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

set(SOURCES_CLIENT_PERFORMANCE
	Client/ImageTestPerformanceClient.cpp
	Client/ImageTestPerformanceClient.h
	Client/InputTestPerformanceClient.cpp
	Client/InputTestPerformanceClient.h
)
add_library(${PROJECT_NAME}_performanceClient STATIC ${SOURCES_CLIENT_PERFORMANCE})
target_link_libraries(${PROJECT_NAME}_performanceClient
	PUBLIC
		ImageService InputService RegistrationService SessionStatusService SessionControlService ConnectivityService
)

set(SOURCES_SERVER_PERFORMANCE
	Server/ImageTestPerformanceServer.cpp
	Server/ImageTestPerformanceServer.h
	Server/InputTestPerformanceServer.cpp
	Server/InputTestPerformanceServer.h
)
add_library(${PROJECT_NAME}_performanceServer STATIC ${SOURCES_SERVER_PERFORMANCE})
target_link_libraries(${PROJECT_NAME}_performanceServer
	PUBLIC
		ImageService InputService RegistrationService SessionStatusService SessionControlService ConnectivityService
)

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
