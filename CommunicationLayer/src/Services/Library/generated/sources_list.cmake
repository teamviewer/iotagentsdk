#********************************************************************************#
# MIT License                                                                    #
#                                                                                #
# Copyright (c) 2024 TeamViewer Germany GmbH                                     #
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
# ==================================
# Generated by TVCMGen. DO NOT EDIT!
# ==================================

set(SOURCES_GENERATED
	generated/export/TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlInServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlInServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlOutServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/AccessControlService/IAccessControlOutServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/AccessControlService/InServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/AccessControlService/InServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/AccessControlService/OutServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/AccessControlService/OutServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/AugmentRCSessionService/ConsumerServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/AugmentRCSessionService/ConsumerServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/AugmentRCSessionService/ControlServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/AugmentRCSessionService/ControlServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/AugmentRCSessionService/IAugmentRCSessionConsumerServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/AugmentRCSessionService/IAugmentRCSessionConsumerServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/AugmentRCSessionService/IAugmentRCSessionControlServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/AugmentRCSessionService/IAugmentRCSessionControlServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/ChatService/IChatInServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/ChatService/IChatInServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/ChatService/IChatOutServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/ChatService/IChatOutServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/ChatService/InServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/ChatService/InServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/ChatService/OutServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/ChatService/OutServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationRequestServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationRequestServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationResponseServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/ConnectionConfirmationService/IConnectionConfirmationResponseServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/ConnectionConfirmationService/RequestServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/ConnectionConfirmationService/RequestServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/ConnectionConfirmationService/ResponseServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/ConnectionConfirmationService/ResponseServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/ConnectivityService/IConnectivityServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/ConnectivityService/IConnectivityServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/ConnectivityService/ServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/ConnectivityService/ServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/ImageNotificationService/IImageNotificationServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/ImageNotificationService/IImageNotificationServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/ImageNotificationService/ServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/ImageNotificationService/ServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/ImageService/IImageServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/ImageService/IImageServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/ImageService/ServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/ImageService/ServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/InputService/IInputServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/InputService/IInputServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/InputService/ServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/InputService/ServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportNotificationServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportNotificationServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/InstantSupportService/IInstantSupportServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/InstantSupportService/NotificationServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/InstantSupportService/NotificationServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/InstantSupportService/ServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/InstantSupportService/ServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/RegistrationService/IRegistrationServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/RegistrationService/IRegistrationServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/RegistrationService/ServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/RegistrationService/ServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/SessionControlService/ISessionControlServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/SessionControlService/ISessionControlServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/SessionControlService/ServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/SessionControlService/ServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/SessionStatusService/ISessionStatusServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/SessionStatusService/ISessionStatusServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/SessionStatusService/ServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/SessionStatusService/ServiceFactory.h
	generated/export/TVRemoteScreenSDKCommunication/ViewGeometryService/IViewGeometryServiceClient.h
	generated/export/TVRemoteScreenSDKCommunication/ViewGeometryService/IViewGeometryServiceServer.h
	generated/export/TVRemoteScreenSDKCommunication/ViewGeometryService/ServiceFactory.cpp
	generated/export/TVRemoteScreenSDKCommunication/ViewGeometryService/ServiceFactory.h
)

if(TV_COMM_ENABLE_GRPC)
	list(APPEND SOURCES_GENERATED
		generated/internal/Client/AccessControlInServicegRPCClient.cpp
		generated/internal/Client/AccessControlInServicegRPCClient.h
		generated/internal/Client/AccessControlOutServicegRPCClient.cpp
		generated/internal/Client/AccessControlOutServicegRPCClient.h
		generated/internal/Client/AugmentRCSessionConsumerServicegRPCClient.cpp
		generated/internal/Client/AugmentRCSessionConsumerServicegRPCClient.h
		generated/internal/Client/AugmentRCSessionControlServicegRPCClient.cpp
		generated/internal/Client/AugmentRCSessionControlServicegRPCClient.h
		generated/internal/Client/ChatInServicegRPCClient.cpp
		generated/internal/Client/ChatInServicegRPCClient.h
		generated/internal/Client/ChatOutServicegRPCClient.cpp
		generated/internal/Client/ChatOutServicegRPCClient.h
		generated/internal/Client/ConnectionConfirmationRequestServicegRPCClient.cpp
		generated/internal/Client/ConnectionConfirmationRequestServicegRPCClient.h
		generated/internal/Client/ConnectionConfirmationResponseServicegRPCClient.cpp
		generated/internal/Client/ConnectionConfirmationResponseServicegRPCClient.h
		generated/internal/Client/ConnectivityServicegRPCClient.cpp
		generated/internal/Client/ConnectivityServicegRPCClient.h
		generated/internal/Client/ImageNotificationServicegRPCClient.cpp
		generated/internal/Client/ImageNotificationServicegRPCClient.h
		generated/internal/Client/ImageServicegRPCClient.cpp
		generated/internal/Client/ImageServicegRPCClient.h
		generated/internal/Client/InputServicegRPCClient.cpp
		generated/internal/Client/InputServicegRPCClient.h
		generated/internal/Client/InstantSupportNotificationServicegRPCClient.cpp
		generated/internal/Client/InstantSupportNotificationServicegRPCClient.h
		generated/internal/Client/InstantSupportServicegRPCClient.cpp
		generated/internal/Client/InstantSupportServicegRPCClient.h
		generated/internal/Client/RegistrationServicegRPCClient.cpp
		generated/internal/Client/RegistrationServicegRPCClient.h
		generated/internal/Client/SessionControlServicegRPCClient.cpp
		generated/internal/Client/SessionControlServicegRPCClient.h
		generated/internal/Client/SessionStatusServicegRPCClient.cpp
		generated/internal/Client/SessionStatusServicegRPCClient.h
		generated/internal/Client/ViewGeometryServicegRPCClient.cpp
		generated/internal/Client/ViewGeometryServicegRPCClient.h
		generated/internal/Server/AccessControlInServicegRPCServer.cpp
		generated/internal/Server/AccessControlInServicegRPCServer.h
		generated/internal/Server/AccessControlOutServicegRPCServer.cpp
		generated/internal/Server/AccessControlOutServicegRPCServer.h
		generated/internal/Server/AugmentRCSessionConsumerServicegRPCServer.cpp
		generated/internal/Server/AugmentRCSessionConsumerServicegRPCServer.h
		generated/internal/Server/AugmentRCSessionControlServicegRPCServer.cpp
		generated/internal/Server/AugmentRCSessionControlServicegRPCServer.h
		generated/internal/Server/ChatInServicegRPCServer.cpp
		generated/internal/Server/ChatInServicegRPCServer.h
		generated/internal/Server/ChatOutServicegRPCServer.cpp
		generated/internal/Server/ChatOutServicegRPCServer.h
		generated/internal/Server/ConnectionConfirmationRequestServicegRPCServer.cpp
		generated/internal/Server/ConnectionConfirmationRequestServicegRPCServer.h
		generated/internal/Server/ConnectionConfirmationResponseServicegRPCServer.cpp
		generated/internal/Server/ConnectionConfirmationResponseServicegRPCServer.h
		generated/internal/Server/ConnectivityServicegRPCServer.cpp
		generated/internal/Server/ConnectivityServicegRPCServer.h
		generated/internal/Server/ImageNotificationServicegRPCServer.cpp
		generated/internal/Server/ImageNotificationServicegRPCServer.h
		generated/internal/Server/ImageServicegRPCServer.cpp
		generated/internal/Server/ImageServicegRPCServer.h
		generated/internal/Server/InputServicegRPCServer.cpp
		generated/internal/Server/InputServicegRPCServer.h
		generated/internal/Server/InstantSupportNotificationServicegRPCServer.cpp
		generated/internal/Server/InstantSupportNotificationServicegRPCServer.h
		generated/internal/Server/InstantSupportServicegRPCServer.cpp
		generated/internal/Server/InstantSupportServicegRPCServer.h
		generated/internal/Server/RegistrationServicegRPCServer.cpp
		generated/internal/Server/RegistrationServicegRPCServer.h
		generated/internal/Server/SessionControlServicegRPCServer.cpp
		generated/internal/Server/SessionControlServicegRPCServer.h
		generated/internal/Server/SessionStatusServicegRPCServer.cpp
		generated/internal/Server/SessionStatusServicegRPCServer.h
		generated/internal/Server/ViewGeometryServicegRPCServer.cpp
		generated/internal/Server/ViewGeometryServicegRPCServer.h
	)
endif()

if(TV_COMM_ENABLE_PLAIN_SOCKET)
	list(APPEND SOURCES_GENERATED
		generated/internal/AccessControlInServiceInFunctions.h
		generated/internal/AccessControlOutServiceOutFunctions.h
		generated/internal/AugmentRCSessionConsumerServiceConsumerFunctions.h
		generated/internal/AugmentRCSessionControlServiceControlFunctions.h
		generated/internal/ChatInServiceInFunctions.h
		generated/internal/ChatOutServiceOutFunctions.h
		generated/internal/Client/AccessControlInServiceSocketIOClient.cpp
		generated/internal/Client/AccessControlInServiceSocketIOClient.h
		generated/internal/Client/AccessControlOutServiceSocketIOClient.cpp
		generated/internal/Client/AccessControlOutServiceSocketIOClient.h
		generated/internal/Client/AugmentRCSessionConsumerServiceSocketIOClient.cpp
		generated/internal/Client/AugmentRCSessionConsumerServiceSocketIOClient.h
		generated/internal/Client/AugmentRCSessionControlServiceSocketIOClient.cpp
		generated/internal/Client/AugmentRCSessionControlServiceSocketIOClient.h
		generated/internal/Client/ChatInServiceSocketIOClient.cpp
		generated/internal/Client/ChatInServiceSocketIOClient.h
		generated/internal/Client/ChatOutServiceSocketIOClient.cpp
		generated/internal/Client/ChatOutServiceSocketIOClient.h
		generated/internal/Client/ConnectionConfirmationRequestServiceSocketIOClient.cpp
		generated/internal/Client/ConnectionConfirmationRequestServiceSocketIOClient.h
		generated/internal/Client/ConnectionConfirmationResponseServiceSocketIOClient.cpp
		generated/internal/Client/ConnectionConfirmationResponseServiceSocketIOClient.h
		generated/internal/Client/ConnectivityServiceSocketIOClient.cpp
		generated/internal/Client/ConnectivityServiceSocketIOClient.h
		generated/internal/Client/ImageNotificationServiceSocketIOClient.cpp
		generated/internal/Client/ImageNotificationServiceSocketIOClient.h
		generated/internal/Client/ImageServiceSocketIOClient.cpp
		generated/internal/Client/ImageServiceSocketIOClient.h
		generated/internal/Client/InputServiceSocketIOClient.cpp
		generated/internal/Client/InputServiceSocketIOClient.h
		generated/internal/Client/InstantSupportNotificationServiceSocketIOClient.cpp
		generated/internal/Client/InstantSupportNotificationServiceSocketIOClient.h
		generated/internal/Client/InstantSupportServiceSocketIOClient.cpp
		generated/internal/Client/InstantSupportServiceSocketIOClient.h
		generated/internal/Client/RegistrationServiceSocketIOClient.cpp
		generated/internal/Client/RegistrationServiceSocketIOClient.h
		generated/internal/Client/SessionControlServiceSocketIOClient.cpp
		generated/internal/Client/SessionControlServiceSocketIOClient.h
		generated/internal/Client/SessionStatusServiceSocketIOClient.cpp
		generated/internal/Client/SessionStatusServiceSocketIOClient.h
		generated/internal/Client/ViewGeometryServiceSocketIOClient.cpp
		generated/internal/Client/ViewGeometryServiceSocketIOClient.h
		generated/internal/ConnectionConfirmationRequestServiceRequestFunctions.h
		generated/internal/ConnectionConfirmationResponseServiceResponseFunctions.h
		generated/internal/ConnectivityServiceFunctions.h
		generated/internal/ImageNotificationServiceFunctions.h
		generated/internal/ImageServiceFunctions.h
		generated/internal/InputServiceFunctions.h
		generated/internal/InstantSupportNotificationServiceNotificationFunctions.h
		generated/internal/InstantSupportServiceFunctions.h
		generated/internal/RegistrationServiceFunctions.h
		generated/internal/Server/AccessControlInServiceSocketIOServer.cpp
		generated/internal/Server/AccessControlInServiceSocketIOServer.h
		generated/internal/Server/AccessControlOutServiceSocketIOServer.cpp
		generated/internal/Server/AccessControlOutServiceSocketIOServer.h
		generated/internal/Server/AugmentRCSessionConsumerServiceSocketIOServer.cpp
		generated/internal/Server/AugmentRCSessionConsumerServiceSocketIOServer.h
		generated/internal/Server/AugmentRCSessionControlServiceSocketIOServer.cpp
		generated/internal/Server/AugmentRCSessionControlServiceSocketIOServer.h
		generated/internal/Server/ChatInServiceSocketIOServer.cpp
		generated/internal/Server/ChatInServiceSocketIOServer.h
		generated/internal/Server/ChatOutServiceSocketIOServer.cpp
		generated/internal/Server/ChatOutServiceSocketIOServer.h
		generated/internal/Server/ConnectionConfirmationRequestServiceSocketIOServer.cpp
		generated/internal/Server/ConnectionConfirmationRequestServiceSocketIOServer.h
		generated/internal/Server/ConnectionConfirmationResponseServiceSocketIOServer.cpp
		generated/internal/Server/ConnectionConfirmationResponseServiceSocketIOServer.h
		generated/internal/Server/ConnectivityServiceSocketIOServer.cpp
		generated/internal/Server/ConnectivityServiceSocketIOServer.h
		generated/internal/Server/ImageNotificationServiceSocketIOServer.cpp
		generated/internal/Server/ImageNotificationServiceSocketIOServer.h
		generated/internal/Server/ImageServiceSocketIOServer.cpp
		generated/internal/Server/ImageServiceSocketIOServer.h
		generated/internal/Server/InputServiceSocketIOServer.cpp
		generated/internal/Server/InputServiceSocketIOServer.h
		generated/internal/Server/InstantSupportNotificationServiceSocketIOServer.cpp
		generated/internal/Server/InstantSupportNotificationServiceSocketIOServer.h
		generated/internal/Server/InstantSupportServiceSocketIOServer.cpp
		generated/internal/Server/InstantSupportServiceSocketIOServer.h
		generated/internal/Server/RegistrationServiceSocketIOServer.cpp
		generated/internal/Server/RegistrationServiceSocketIOServer.h
		generated/internal/Server/SessionControlServiceSocketIOServer.cpp
		generated/internal/Server/SessionControlServiceSocketIOServer.h
		generated/internal/Server/SessionStatusServiceSocketIOServer.cpp
		generated/internal/Server/SessionStatusServiceSocketIOServer.h
		generated/internal/Server/ViewGeometryServiceSocketIOServer.cpp
		generated/internal/Server/ViewGeometryServiceSocketIOServer.h
		generated/internal/SessionControlServiceFunctions.h
		generated/internal/SessionStatusServiceFunctions.h
		generated/internal/ViewGeometryServiceFunctions.h
	)
endif()
