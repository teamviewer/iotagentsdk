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
#pragma once

#include "IAccessControlModule.h"
#include "IAgentAPI.h"
#include "IAgentConnection.h"
#include "IInstantSupportModule.h"
#include "ITVSessionManagementModule.h"

#include "AccessControlModuleStringify.h"
#include "AgentConnectionStringify.h"
#include "InstantSupportModuleStringify.h"
#include "ModuleStringify.h"

/**
 * @breif TVGetAgentAPI returns a pointer to a shared IAgentAPI object.
 * Subsequent calls to TVGetAgentAPI will return pointer to the same object
 * @returns IAgentAPI or nullptr if IAgentAPI creation failed
 */
extern "C" tvagentapi::IAgentAPI* TVGetAgentAPI();

/**
 * @breif TVDestroyAgentAPI destroys the shared IAgentAPI object.
 */
extern "C" void TVDestroyAgentAPI();
