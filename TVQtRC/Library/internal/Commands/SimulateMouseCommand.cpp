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
#include "SimulateMouseCommand.h"

namespace tvqtsdk
{

SimulateMouseCommand::SimulateMouseCommand(MouseButtonState state, MouseAction action, int x, int y, MouseButton button, int angle)
	: m_state(state), m_action(action), m_x(x), m_y(y), m_button(button), m_angle(angle)
{
}

int SimulateMouseCommand::x() const
{
	return m_x;
}

int SimulateMouseCommand::y() const
{
	return m_y;
}

MouseButton SimulateMouseCommand::button() const
{
	return m_button;
}

MouseButtonState SimulateMouseCommand::mouseButtonState() const
{
	return m_state;
}

MouseAction SimulateMouseCommand::mouseAction() const
{
	return m_action;
}

int SimulateMouseCommand::angle() const
{
	return m_angle;
}

} // namespace tvqtsdk
