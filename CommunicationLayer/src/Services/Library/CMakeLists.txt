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
project(Services)

set(SOURCES_PROTOBUF_SERVICES
	internal/AccessControlService/proto/AccessControlInService.proto
	internal/AccessControlService/proto/AccessControlOutService.proto
	internal/AugmentRCSessionService/proto/AugmentRCSessionConsumerService.proto
	internal/AugmentRCSessionService/proto/AugmentRCSessionControlService.proto
	internal/ChatService/proto/ChatInService.proto
	internal/ChatService/proto/ChatOutService.proto
	internal/ConnectionConfirmationService/proto/ConnectionConfirmationRequestService.proto
	internal/ConnectionConfirmationService/proto/ConnectionConfirmationResponseService.proto
	internal/ConnectivityService/proto/ConnectivityService.proto
	internal/ImageNotificationService/proto/ImageNotificationService.proto
	internal/ImageService/proto/ImageService.proto
	internal/InputService/proto/InputService.proto
	internal/InstantSupportService/proto/InstantSupportNotificationService.proto
	internal/InstantSupportService/proto/InstantSupportService.proto
	internal/RegistrationService/proto/RegistrationService.proto
	internal/SessionControlService/proto/SessionControlService.proto
	internal/SessionStatusService/proto/SessionStatusService.proto
	internal/ViewGeometryService/proto/ViewGeometryService.proto
)

set(SOURCES_PROTOBUF_MESSAGES
	internal/AccessControlService/proto/AccessControlOutMessages.proto
	internal/AccessControlService/proto/AccessControl.proto
	internal/AccessControlService/proto/ConfirmationReplyRequest.proto
	internal/AccessControlService/proto/ConfirmationReplyResponse.proto
	internal/AccessControlService/proto/GetAccessRequest.proto
	internal/AccessControlService/proto/GetAccessResponse.proto
	internal/AccessControlService/proto/SetAccessRequest.proto
	internal/AccessControlService/proto/SetAccessResponse.proto
	internal/AugmentRCSessionService/proto/AugmentRCSessionConsumerMessages.proto
	internal/AugmentRCSessionService/proto/AugmentRCSessionControlMessages.proto
	internal/AugmentRCSessionService/proto/AugmentRCSession.proto
	internal/ChatService/proto/ChatInMessages.proto
	internal/ChatService/proto/ChatOutMessages.proto
	internal/ChatService/proto/Chat.proto
	internal/ConnectionConfirmationService/proto/ConnectionConfirmationRequestMessages.proto
	internal/ConnectionConfirmationService/proto/ConnectionConfirmationResponseMessages.proto
	internal/ConnectivityService/proto/DisconnectRequest.proto
	internal/ConnectivityService/proto/DisconnectResponse.proto
	internal/ConnectivityService/proto/HeartbeatRequest.proto
	internal/ConnectivityService/proto/HeartbeatResponse.proto
	internal/ImageNotificationService/proto/ChangeImageDefinitionRequest.proto
	internal/ImageNotificationService/proto/ChangeImageDefinitionResponse.proto
	internal/ImageNotificationService/proto/NotifyImageChangedRequest.proto
	internal/ImageNotificationService/proto/NotifyImageChangedResponse.proto
	internal/ImageService/proto/ColorFormat.proto
	internal/ImageService/proto/GrabResult.proto
	internal/ImageService/proto/ImageDefinitionRequest.proto
	internal/ImageService/proto/ImageDefinitionResponse.proto
	internal/ImageService/proto/ImageUpdateResponse.proto
	internal/InputService/proto/KeyRequest.proto
	internal/InputService/proto/KeyResponse.proto
	internal/InputService/proto/MouseButton.proto
	internal/InputService/proto/MouseMoveRequest.proto
	internal/InputService/proto/MouseMoveResponse.proto
	internal/InputService/proto/MousePressReleaseRequest.proto
	internal/InputService/proto/MousePressReleaseResponse.proto
	internal/InputService/proto/MouseWheelRequest.proto
	internal/InputService/proto/MouseWheelResponse.proto
	internal/InstantSupportService/proto/InstantSupportMessages.proto
	internal/InstantSupportService/proto/InstantSupportNotificationMessages.proto
	internal/RegistrationService/proto/DiscoverRequest.proto
	internal/RegistrationService/proto/DiscoverResponse.proto
	internal/RegistrationService/proto/ExchangeVersionRequest.proto
	internal/RegistrationService/proto/ExchangeVersionResponse.proto
	internal/RegistrationService/proto/RegisterRequest.proto
	internal/RegistrationService/proto/RegisterResponse.proto
	internal/RegistrationService/proto/ServiceType.proto
	internal/SessionControlService/proto/ChangeControlModeRequest.proto
	internal/SessionControlService/proto/ChangeControlModeResponse.proto
	internal/SessionControlService/proto/CloseRcRequest.proto
	internal/SessionControlService/proto/CloseRcResponse.proto
	internal/SessionControlService/proto/RunningSessionsRequest.proto
	internal/SessionControlService/proto/RunningSessionsResponse.proto
	internal/SessionStatusService/proto/RemoteControlStartedRequest.proto
	internal/SessionStatusService/proto/RemoteControlStartedResponse.proto
	internal/SessionStatusService/proto/RemoteControlStoppedRequest.proto
	internal/SessionStatusService/proto/RemoteControlStoppedResponse.proto
	internal/SessionStatusService/proto/TVSessionStartedRequest.proto
	internal/SessionStatusService/proto/TVSessionStartedResponse.proto
	internal/SessionStatusService/proto/TVSessionStoppedRequest.proto
	internal/SessionStatusService/proto/TVSessionStoppedResponse.proto
	internal/ViewGeometryService/proto/UpdateAreaOfInterestRequest.proto
	internal/ViewGeometryService/proto/UpdateAreaOfInterestResponse.proto
	internal/ViewGeometryService/proto/UpdateVirtualDesktopRequest.proto
	internal/ViewGeometryService/proto/UpdateVirtualDesktopResponse.proto
	internal/ViewGeometryService/proto/VirtualDesktop.proto
)

set(SOURCES_STATIC
	export/TVRemoteScreenSDKCommunication/AccessControlService/AccessControl.h
	export/TVRemoteScreenSDKCommunication/AugmentRCSessionService/AugmentRCSession.h
	export/TVRemoteScreenSDKCommunication/ChatService/Chat.h
	export/TVRemoteScreenSDKCommunication/ConnectionConfirmationService/ConnectionData.h
	export/TVRemoteScreenSDKCommunication/ImageService/ColorFormat.h
	export/TVRemoteScreenSDKCommunication/InputService/KeyState.h
	export/TVRemoteScreenSDKCommunication/InputService/MouseButton.h
	export/TVRemoteScreenSDKCommunication/InstantSupportService/InstantSupportData.h
	export/TVRemoteScreenSDKCommunication/InstantSupportService/InstantSupportError.h
	export/TVRemoteScreenSDKCommunication/RegistrationService/ServiceInformation.h
	export/TVRemoteScreenSDKCommunication/SessionControlService/ControlMode.h
	export/TVRemoteScreenSDKCommunication/SessionStatusService/GrabStrategy.h
	export/TVRemoteScreenSDKCommunication/ViewGeometryService/VirtualDesktop.h
)

set(LINK_LIBRARIES_PUBLIC
	ServiceBase
	CommunicationLayerBase
)

set(LINK_LIBRARIES_PRIVATE
	protobuf::libprotobuf
)

if(TV_COMM_ENABLE_GRPC)
	find_package(gRPC REQUIRED)

	grpc_generate_cpp(SERVICES_GRPC_SRCS SERVICES_GRPC_HDRS ${SOURCES_PROTOBUF_SERVICES})
	protobuf_generate_cpp(SERVICES_PROTO_SRCS SERVICES_PROTO_HDRS ${SOURCES_PROTOBUF_SERVICES})

	list(APPEND LINK_LIBRARIES_PRIVATE
		gRPC::grpc++_reflection
	)
endif()

protobuf_generate_cpp(MESSAGES_PROTO_SRCS MESSAGES_PROTO_HDRS ${SOURCES_PROTOBUF_MESSAGES})

include("${CMAKE_CURRENT_SOURCE_DIR}/generated/sources_list.cmake")

add_library(${PROJECT_NAME} STATIC
	${SOURCES_STATIC}
	${SOURCES_GENERATED}
	${SOURCES_PROTOBUF_SERVICES}
	${SOURCES_PROTOBUF_MESSAGES}
	${SERVICES_GRPC_SRCS}
	${SERVICES_GRPC_HDRS}
	${SERVICES_PROTO_SRCS}
	${SERVICES_PROTO_HDRS}
	${MESSAGES_PROTO_SRCS}
	${MESSAGES_PROTO_HDRS}
)
add_library(${TV_NAMESPACE}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	SYSTEM PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/export>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated/export>
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}/generated"
		"${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC  ${LINK_LIBRARIES_PUBLIC}
	PRIVATE ${LINK_LIBRARIES_PRIVATE}
)

install_tvsdk(${PROJECT_NAME})

# install generated export folder as well
install(DIRECTORY generated/export/
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
)
