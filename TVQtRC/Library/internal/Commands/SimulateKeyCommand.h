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
#pragma once

#include <cstdint>

namespace tvqtsdk
{

enum class KeyState
{
	Invalid,
	Pressed,
	Released
};

class SimulateKeyCommand final
{
public:
	SimulateKeyCommand(KeyState keystate, uint32_t xkbSymbol, uint32_t unicodeCharacter, uint32_t xkbModifiers);
	KeyState keyState() const;
	uint32_t xkbSymbol() const;
	uint32_t unicodeCharacter() const;
	uint32_t xkbModifiers() const;

private:
	KeyState m_keystate = KeyState::Invalid;
	uint32_t m_xkbSymbol = 0;
	uint32_t m_unicodeCharacter = 0;
	uint32_t m_xkbModifiers = 0;
};

} // namespace tvqtsdk
