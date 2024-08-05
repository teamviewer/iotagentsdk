//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2021 TeamViewer Germany GmbH                                     //
//                                                                                //
// Permission is hereby granted, free of charge, to any person obtaining a copy   //
// of this software and associated documentation files (the "Software"), to deal  //
// in the Software without restriction, including without limitation the rights   //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      //
// copies of the Software, and to permit persons to whom the Software is          //
// furnished to do so, subject to the following conditions:                       //
//                                                                                //
// The above copyright notice and this permission notice shall be included in all //
// copies or substantial portions of the Software.                                //
//                                                                                //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  //
// SOFTWARE.                                                                      //
//********************************************************************************//
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/TransportFramework.h>

#include "Client/TestAccessControlInServiceClient.h"
#include "Client/TestAccessControlOutServiceClient.h"
#include "Client/TestAugmentRCSessionConsumerServiceClient.h"
#include "Client/TestAugmentRCSessionControlServiceClient.h"
#include "Client/TestChatInServiceClient.h"
#include "Client/TestChatOutServiceClient.h"
#include "Client/TestConnectionConfirmationServiceClient.h"
#include "Client/TestConnectivityServiceClient.h"
#include "Client/TestImageNotificationServiceClient.h"
#include "Client/TestImageServiceClient.h"
#include "Client/TestInputServiceClient.h"
#include "Client/TestInstantSupportNotificationServiceClient.h"
#include "Client/TestInstantSupportServiceClient.h"
#include "Client/TestRegistrationServiceClient.h"
#include "Client/TestSessionControlServiceClient.h"
#include "Client/TestSessionStatusServiceClient.h"
#include "Client/TestViewGeometryServiceClient.h"

#include "Server/TestAccessControlInServiceServer.h"
#include "Server/TestAccessControlOutServiceServer.h"
#include "Server/TestAugmentRCSessionConsumerServiceServer.h"
#include "Server/TestAugmentRCSessionControlServiceServer.h"
#include "Server/TestChatInServiceServer.h"
#include "Server/TestChatOutServiceServer.h"
#include "Server/TestConnectionConfirmationServiceServer.h"
#include "Server/TestConnectivityService.h"
#include "Server/TestImageNotificationService.h"
#include "Server/TestImageService.h"
#include "Server/TestInputService.h"
#include "Server/TestInstantSupportNotificationServiceServer.h"
#include "Server/TestInstantSupportServiceServer.h"
#include "Server/TestRegistrationService.h"
#include "Server/TestSessionControlService.h"
#include "Server/TestSessionStatusService.h"
#include "Server/TestViewGeometryService.h"

#include <iostream>

template<TVRemoteScreenSDKCommunication::TransportFramework Framework>
int TestOnFramework(int argc, char** argv)
{
	using namespace TVRemoteScreenSDKCommunication;

	{
		const std::shared_ptr<AccessControlService::IAccessControlInServiceServer> accessControlServer =
			TestAccessControlInService::TestAccessControlInServiceServer<Framework>();
		const int testResult = TestAccessControlInService::TestAccessControlInServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<AccessControlService::IAccessControlOutServiceServer> accessControlOutServer =
			TestAccessControlOutService::TestAccessControlOutServiceServer<Framework>();
		const int testResult = TestAccessControlOutService::TestAccessControlOutServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<AugmentRCSessionService::IAugmentRCSessionConsumerServiceServer> augmentRCSessionConsumerServiceServer =
			TestAugmentRCSessionConsumerService::TestAugmentRCSessionConsumerServiceServer<Framework>();
		const int testResult = TestAugmentRCSessionConsumerService::TestAugmentRCSessionConsumerServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<AugmentRCSessionService::IAugmentRCSessionControlServiceServer> augmentRCSessionControlServiceServer =
			TestAugmentRCSessionControlService::TestAugmentRCSessionControlServiceServer<Framework>();
		const int testResult = TestAugmentRCSessionControlService::TestAugmentRCSessionControlServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<ChatService::IChatInServiceServer> ChatServer = TestChatInService::TestChatInServiceServer<Framework>();
		const int testResult = TestChatInService::TestChatInServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<ChatService::IChatOutServiceServer> ChatOutServer = TestChatOutService::TestChatOutServiceServer<Framework>();
		const int testResult = TestChatOutService::TestChatOutServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<ConnectionConfirmationService::IConnectionConfirmationRequestServiceServer>
			connectionConfirmationRequestServiceServer =
				TestConnectionConfirmationService::TestConnectionConfirmationRequestServiceServer<Framework>();
		const int testResult =
			TestConnectionConfirmationService::TestConnectionConfirmationRequestServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<ConnectionConfirmationService::IConnectionConfirmationResponseServiceServer>
			connectionConfirmationResponseServiceServer =
				TestConnectionConfirmationService::TestConnectionConfirmationResponseServiceServer<Framework>();
		const int testResult =
			TestConnectionConfirmationService::TestConnectionConfirmationResponseServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<ConnectivityService::IConnectivityServiceServer> connectivityServer =
			TestConnectivityService::TestConnectivityService<Framework>();
		const int testResult = TestConnectivityService::TestConnectivityServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<ImageService::IImageServiceServer> imageServer =
			TestImageService::TestImageService<Framework>();
		const int testResult = TestImageService::TestImageServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<ImageNotificationService::IImageNotificationServiceServer> imageNotificationServer =
			TestImageNotificationService::TestImageNotificationService<Framework>();
		const int testResult = TestImageNotificationService::TestImageNotificationServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<InputService::IInputServiceServer> inputServer =
			TestInputService::TestInputService<Framework>();
		const int testResult = TestInputService::TestInputServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<InstantSupportService::IInstantSupportServiceServer> instantSupport =
			TestInstantSupportService::TestInstantSupportServiceServer<Framework>();
		const int testResult = TestInstantSupportService::TestInstantSupportServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<InstantSupportService::IInstantSupportNotificationServiceServer> instantSupport =
			TestInstantSupportNotificationService::TestInstantSupportNotificationServiceServer<Framework>();
		const int testResult =
			TestInstantSupportNotificationService::TestInstantSupportNotificationServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<RegistrationService::IRegistrationServiceServer> registrationServer =
			TestRegistrationService::TestRegistrationService<Framework>();
		const int testResult = TestRegistrationService::TestRegistrationServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<SessionControlService::ISessionControlServiceServer> sessionControlServer =
			TestSessionControlService::TestSessionControlService<Framework>();
		const int testResult = TestSessionControlService::TestSessionControlServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<SessionStatusService::ISessionStatusServiceServer> sessionStatusServer =
			TestSessionStatusService::TestSessionStatusService<Framework>();
		const int testResult = TestSessionStatusService::TestSessionStatusServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	{
		const std::shared_ptr<ViewGeometryService::IViewGeometryServiceServer> viewGeometryServer =
			TestViewGeometryService::TestViewGeometryService<Framework>();
		const int testResult = TestViewGeometryService::TestViewGeometryServiceClient<Framework>(argc, argv);
		if (testResult == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	std::cout << "Integration Test with Framework=" << Framework << " successful" << std::endl;
	return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
	return
#ifdef TV_COMM_ENABLE_GRPC
		TestOnFramework<TVRemoteScreenSDKCommunication::gRPCTransport>(argc, argv)
#ifdef TV_COMM_ENABLE_PLAIN_SOCKET
		||
#endif
#endif
#ifdef TV_COMM_ENABLE_PLAIN_SOCKET
		TestOnFramework<TVRemoteScreenSDKCommunication::TCPSocketTransport>(argc, argv)
#endif
	;
}
