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

#include "internal/InputSimulation/AbstractInputSimulator.h"
#include "internal/Commands/SimulateMouseCommand.h"

#include <QtCore/QPointer>
#include <QtCore/QPoint>
#include <QtGui/QWindow>

#include <memory>

namespace tvqtsdk
{

class CommunicationAdapter;
class SimulateKeyCommand;
class SimulateMouseCommand;

class InputSimulator final : public AbstractInputSimulator
{
	Q_OBJECT
public:
	InputSimulator(
		const std::weak_ptr<CommunicationAdapter>& adapter,
		const std::shared_ptr<ILogging>& logging,
		QWindow* window,
		QObject* parent = nullptr);

	~InputSimulator() override = default;

	// AbstractInputSimulator interface
	void enable() override;
	void disable() override;

private:
	void simulateKey(const std::shared_ptr<SimulateKeyCommand>& command);
	void simulateMouse(const std::shared_ptr<SimulateMouseCommand>& command);

	void simulateKeyboardRelease(uint32_t xkbSymbol, uint32_t unicodeCharacter, uint32_t xkbModifiers);
	void simulateKeyboardPress(uint32_t xkbSymbol, uint32_t unicodeCharacter, uint32_t xkbModifiers);
	void simulateMouseWheelRequested(QWindow* window, const QPoint& point, int angle);
	void simulateMouseMove(QWindow* window,const QPoint& point);
	void simulateMouseRelease(QWindow* window, const QPoint& point, MouseButton button);
	void simulateMousePress(QWindow* window, const QPoint& point, MouseButton button);

	const std::weak_ptr<CommunicationAdapter> m_communicationAdapter;
	const QPointer<QWindow> m_applicationWindow;

	QMetaObject::Connection m_mouseConnection;
	QMetaObject::Connection m_keyConnection;
	Qt::MouseButtons m_pressedMouseButtons = Qt::NoButton;
	Qt::KeyboardModifiers m_convertedModifiers = Qt::NoModifier;
	Qt::MouseButton m_lastMouseButtonReleased = Qt::NoButton;
	qint64 m_lastMouseButtonReleasedTimestamp = 0;
	QPointer<QWindow> m_lastPressedWindow;
	QPointer<QWindow> m_focusedWindow;
};

} // namespace tvqtsdk
