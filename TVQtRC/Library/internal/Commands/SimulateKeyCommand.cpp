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
#include "SimulateKeyCommand.h"

namespace tvqtsdk
{

SimulateKeyCommand::SimulateKeyCommand(KeyState keystate, uint32_t xkbSymbol, uint32_t unicodeCharacter, uint32_t xkbModifiers)
	: m_keystate{keystate}, m_xkbSymbol(xkbSymbol), m_unicodeCharacter(unicodeCharacter), m_xkbModifiers(xkbModifiers)
{
}

KeyState SimulateKeyCommand::keyState() const
{
	return m_keystate;
}

uint32_t SimulateKeyCommand::xkbSymbol() const
{
	return m_xkbSymbol;
}

uint32_t SimulateKeyCommand::unicodeCharacter() const
{
	return m_unicodeCharacter;
}

uint32_t SimulateKeyCommand::xkbModifiers() const
{
	return m_xkbModifiers;
}

} // namespace tvqtsdk
