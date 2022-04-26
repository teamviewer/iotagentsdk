//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2022 TeamViewer Germany GmbH                                     //
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
#include <TVAgentAPIPrivate/Observer.h>

#include <iostream>

bool testObserverWithOneCallback()
{
	std::cout << "Test Observer with one callback: ";
	bool c1Called = false;
	{
		tvagentapi::Observer<void()> obs;
		auto connection1 = obs.registerCallback([&c1Called](){c1Called = true;});
		obs.notifyAll();
	}
	(c1Called) ? std::cout << "SUCCESSFUL\n" : std::cout << "FAILED\n";
	return c1Called;
}

bool testObserverCallbackWithArguments()
{
	std::cout << "Test Observer callback with arguments: ";
	bool c1CalledWithArgs = false;
	bool c2CalledWithArgs = false;
	{
		tvagentapi::Observer<void(int, bool, std::string)> obs;
		auto connection1 = obs.registerCallback([&c1CalledWithArgs](int i, bool b, std::string str)
		{
			c1CalledWithArgs = (i == 123 && b && str == "foo");
		});
		auto connection2 = obs.registerCallback([&c2CalledWithArgs](int i, bool b, std::string str)
		{
			c2CalledWithArgs = (i == 123 && b && str == "foo");
		});
		obs.notifyAll(123, true, "foo");
	}
	(c1CalledWithArgs && c2CalledWithArgs) ? std::cout << "SUCCESSFUL\n" : std::cout << "FAILED\n";
	return c1CalledWithArgs;
}

bool testObserverWithMultipleCallbacks()
{
	std::cout << "Test Observer with multiple callbacks: ";
	bool c1Called = false;
	bool c2Called = false;
	bool c3Called = false;
	{
		tvagentapi::Observer<void()> obs;
		auto connection1 = obs.registerCallback([&c1Called](){c1Called = true;});
		auto connection2 = obs.registerCallback([&c2Called](){c2Called = true;});
		auto connection3 = obs.registerCallback([&c3Called](){c3Called = true;});
		connection2.disconnect();
		obs.notifyAll();
	}
	(c1Called && !c2Called && c3Called) ? std::cout << "SUCCESSFUL\n" : std::cout << "FAILED\n";
	return c1Called && !c2Called && c3Called;
}

bool testObserverCallbackRemovedByDisconnectMethod()
{
	std::cout << "Test Observer removing callback by disconnect method: ";
	bool c1Called = false;
	bool c2Called = false;
	bool c3Called = false;
	{
		tvagentapi::Observer<void()> obs;
		auto connection1 = obs.registerCallback([&c1Called](){c1Called = true;});
		auto connection2 = obs.registerCallback([&c2Called](){c2Called = true;});
		auto connection3 = obs.registerCallback([&c3Called](){c3Called = true;});
		connection2.disconnect();
		obs.notifyAll();
	}
	(c1Called && !c2Called && c3Called) ? std::cout << "SUCCESSFUL\n" : std::cout << "FAILED\n";
	return c1Called && !c2Called && c3Called;
}

bool testObserverCallbackRemovedByReassignment()
{
	std::cout << "Test Observer removing callback by reassignment: ";
	bool c1Called = false;
	bool c2Called = false;
	bool c3Called = false;
	{
		tvagentapi::Observer<void()> obs;
		auto connection1 = obs.registerCallback([&c1Called](){c1Called = true;});
		auto connection2 = obs.registerCallback([&c2Called](){c2Called = true;});
		connection2 = obs.registerCallback([&c3Called](){c3Called = true;});
		obs.notifyAll();
	}
	(c1Called && !c2Called && c3Called) ? std::cout << "SUCCESSFUL\n" : std::cout << "FAILED\n";
	return c1Called && !c2Called && c3Called;
}

bool testObserverCallbackRemovedByMove()
{
	std::cout << "Test Observer removing callback by move: ";
	bool c1Called = false;
	bool c2Called = false;
	{
		tvagentapi::Observer<void()> obs;
		auto connection1 = obs.registerCallback([&c1Called](){c1Called = true;});
		auto connection2 = obs.registerCallback([&c2Called](){c2Called = true;});
		connection2 = std::move(connection1);
		obs.notifyAll();
	}
	(c1Called && !c2Called) ? std::cout << "SUCCESSFUL\n" : std::cout << "FAILED\n";
	return c1Called && !c2Called;
}

bool testObserverCallbackRemovedByLeavingScope()
{
	std::cout << "Test Observer removing callback by leaving scope: ";
	bool c1Called = false;
	bool c2Called = false;
	{
		tvagentapi::Observer<void()> obs;
		auto connection1 = obs.registerCallback([&c1Called](){c1Called = true;});
		{
			auto connection2 = obs.registerCallback([&c2Called](){c2Called = true;});
		}
		obs.notifyAll();
	}
	(c1Called && !c2Called) ? std::cout << "SUCCESSFUL\n" : std::cout << "FAILED\n";
	return c1Called && !c2Called;
}

int main()
{
	bool success = true;
	success &= testObserverWithOneCallback();
	success &= testObserverCallbackWithArguments();
	success &= testObserverWithMultipleCallbacks();
	success &= testObserverCallbackRemovedByDisconnectMethod();
	success &= testObserverCallbackRemovedByReassignment();
	success &= testObserverCallbackRemovedByMove();
	success &= testObserverCallbackRemovedByLeavingScope();
	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
