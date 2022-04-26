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
#include <TVAgentAPI/InstantSupportModuleStringify.h>

namespace tvagentapi
{

const char* toCString(IInstantSupportModule::SessionState state)
{
	using SessionState = IInstantSupportModule::SessionState;
	switch (state)
	{
		case SessionState::Undefined:   return "Undefined";
		case SessionState::Open:        return "Open";
		case SessionState::Closed:      return "Closed";
	}
	return "";
}

const char* toCString(IInstantSupportModule::RequestErrorCode errorCode)
{
	using RequestErrorCode = IInstantSupportModule::RequestErrorCode;
	switch (errorCode)
	{
		case RequestErrorCode::InvalidToken:        return "InvalidToken";
		case RequestErrorCode::InvalidGroup:        return "InvalidGroup";
		case RequestErrorCode::InvalidSessionCode:  return "InvalidSessionCode";
		case RequestErrorCode::Busy:                return "Busy";
		case RequestErrorCode::InternalError:       return "InternalError";
		case RequestErrorCode::InvalidEmail:        return "InvalidEmail";
	}
	return "";
}

} // namespace tvagentapi
