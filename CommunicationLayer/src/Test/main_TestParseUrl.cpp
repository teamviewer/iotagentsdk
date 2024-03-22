//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2024 TeamViewer Germany GmbH                                     //
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
#include <TVRemoteScreenSDKCommunication/CommunicationLayerBase/ParseUrl.h>
#include <iostream>

using namespace TVRemoteScreenSDKCommunication;

namespace
{
struct TestCase
{
	std::string url;
	UrlComponents components;
	TransportFramework framework;
};

const TestCase TestDataCorrect[] = {
	{ "unix:///tmp"                         , { "unix"    , ""                , 0    , "/tmp"               }, gRPCTransport      },
	{ "Unix:///tmp"                         , { "unix"    , ""                , 0    , "/tmp"               }, gRPCTransport      },
	{ "UNIX:///tmp"                         , { "unix"    , ""                , 0    , "/tmp"               }, gRPCTransport      },
	{ "unix:///Path/_SubDir/d.-2/"          , { "unix"    , ""                , 0    , "/Path/_SubDir/d.-2/"}, gRPCTransport      },
	{ "unix:///"                            , { "unix"    , ""                , 0    , "/"                  }, gRPCTransport      },
	{ "foo:///path/to/file"                 , { "foo"     , ""                , 0    , "/path/to/file"      }, UnknownTransport   },
	{ "http://localhost:80"                 , { "http"    , "localhost"       , 80   , ""                   }, UnknownTransport   },
	{ "http://localhost:80/"                , { "http"    , "localhost"       , 80   , "/"                  }, UnknownTransport   },
	{ "http://localhost:8000/path"          , { "http"    , "localhost"       , 8000 , "/path"              }, UnknownTransport   },
	{ "http://localhost:8000/path/"         , { "http"    , "localhost"       , 8000 , "/path/"             }, UnknownTransport   },
	{ "http://localhost:8000/path/subpath"  , { "http"    , "localhost"       , 8000 , "/path/subpath"      }, UnknownTransport   },
	{ "https://example.com:433/path/subpath", { "https"   , "example.com"     , 433  , "/path/subpath"      }, UnknownTransport   },
	{ "ftp://host.example.com:21/path_1/_2" , { "ftp"     , "host.example.com", 21   , "/path_1/_2"         }, UnknownTransport   },
	{ "tcp://localhost:12345"               , { "tcp"     , "localhost"       , 12345, ""                   }, UnknownTransport   },
	{ "svn+ssh://localhost:12345"           , { "svn+ssh" , "localhost"       , 12345, ""                   }, UnknownTransport   },
	{ "TCP+TV://host:456"                   , { "tcp+tv"  , "host"            , 456  , ""                   }, TCPSocketTransport },
	{ "tCp+tV://192.168.52.1:999"           , { "tcp+tv"  , "192.168.52.1"    , 999  , ""                   }, TCPSocketTransport },
	{ "tcp+tv+0://192.168.52.1:999"         , { "tcp+tv+0", "192.168.52.1"    , 999  , ""                   }, UnknownTransport   },
	{ "tv+tcp://myhost:2"                   , { "tv+tcp"  , "myhost"          , 2    , ""                   }, TCPSocketTransport },
	{ "tv+tcp://myhost"                     , { "tv+tcp"  , "myhost"          , 0    , ""                   }, TCPSocketTransport },
	{ "tcp+tv+2://192.168.52.1:999"         , { "tcp+tv+2", "192.168.52.1"    , 999  , ""                   }, UnknownTransport   },
};

const std::string TestUrlsMalformed[] = {
	"",
	"?",
	"foo",
	"12",
	"123://",
	"123://path",
	"123:///path",
	"unix:///tmp//",
	"unix:///tmp//foo",
	"unix:///tmp//foo/",
	"unix:////tmp",
//	"unix://",
	"unix:////",
	"http://localhost:0",
	"http://localhost:00",
	"http://localhost:01",
	"http://localhost:80:80",
	"http://localhost:65536",
	"http://localhost:9999999999999999999999999999999999999",
	"http:///localhost:80",
	"http:///localhost:80/path",
	"http://localhost:80/path//",
	"http://localhost:80/path:123",
};

constexpr const char* LogPrefix = "[ParseUrl] ";
}

int main()
{
	for (const TestCase& goodCase : TestDataCorrect)
	{
		TVRemoteScreenSDKCommunication::UrlComponents components;

		if (ParseUrl(goodCase.url, components)
			&& goodCase.components.scheme == components.scheme
			&& goodCase.components.host == components.host
			&& goodCase.components.port == components.port
			&& goodCase.components.path == components.path)
		{
			std::cout << LogPrefix << "OK: Correct URL parsed: " << goodCase.url << std::endl;
			if (GetTransportFramework(components.scheme) == goodCase.framework)
			{
				std::cout << LogPrefix << "OK: Framework deduced" << std::endl;
			}
			else
			{
				std::cerr << LogPrefix << "ERROR: Unexpected framework deduction result for: "
					<< goodCase.url << std::endl;
				return EXIT_FAILURE;
			}
		}
		else
		{
			std::cerr << LogPrefix << "ERROR: Correct URL failed to parse: " << goodCase.url << std::endl;
			return EXIT_FAILURE;
		}
	}

	for (const std::string& badUrl : TestUrlsMalformed)
	{
		TVRemoteScreenSDKCommunication::UrlComponents components;

		if (ParseUrl(badUrl, components) == false)
		{
			std::cout << LogPrefix << "OK: Malformed URL not parsed: " << badUrl << std::endl;
		}
		else
		{
			std::cerr << LogPrefix << "ERROR: Malformed URL parsed: " << badUrl << std::endl;
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
