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
#include "InputSimulator.h"

#include "internal/Commands/SimulateKeyCommand.h"
#include "internal/Commands/SimulateMouseCommand.h"

#include "internal/InputSimulation/XKBMap.h"

#include "internal/Communication/CommunicationAdapter.h"
#include "internal/Logging/ILogging.h"

#include <QtCore/QDateTime>
#include <QtCore/QDebug>

#include <QtGui/QGuiApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QStyleHints>

namespace tvqtsdk
{

namespace
{

void ConvertButtonEventToQtValues(MouseButton button, bool pressed, Qt::MouseButton& changedButton, Qt::MouseButtons& buttonsState)
{
	changedButton = Qt::MouseButton::NoButton;

	switch (button)
	{
	case MouseButton::Left:
		changedButton = Qt::MouseButton::LeftButton;
		break;
	case MouseButton::Middle:
		changedButton = Qt::MouseButton::MiddleButton;
		break;
	case MouseButton::Right:
		changedButton = Qt::MouseButton::RightButton;
		break;
	case MouseButton::Unknown:
		changedButton = Qt::MouseButton::NoButton;
	}

	if (pressed)
	{
		buttonsState |= changedButton;
	}
	else
	{
		buttonsState &= ~changedButton;
	}
}

} // namespace

InputSimulator::InputSimulator(
	const std::weak_ptr<CommunicationAdapter>& adapter,
	const std::shared_ptr<ILogging>& logging,
	QWindow* window,
	QObject* parent)
	: AbstractInputSimulator{logging, parent},
	  m_communicationAdapter{adapter},
	  m_applicationWindow{window},
	  m_pressedMouseButtons{Qt::MouseButton::NoButton},
	  m_convertedModifiers{Qt::KeyboardModifier::NoModifier},
	  m_lastMouseButtonReleased{Qt::MouseButton::NoButton},
	  m_lastMouseButtonReleasedTimestamp{0},
	  m_focusedWindow{m_applicationWindow}
{
	if (!m_focusedWindow)
	{
		QObject::connect(
			qGuiApp,
			&QGuiApplication::focusWindowChanged,
			this,
			[this](QWindow* newFocusedWindow)
			{
				m_focusedWindow = newFocusedWindow;
			});
	}
}

void InputSimulator::enable()
{
	qRegisterMetaType<std::shared_ptr<SimulateKeyCommand>>();
	qRegisterMetaType<std::shared_ptr<SimulateMouseCommand>>();

	std::shared_ptr<CommunicationAdapter> communicationAdapter = m_communicationAdapter.lock();

	if (communicationAdapter)
	{
		m_keyConnection = QObject::connect(communicationAdapter.get(),
						 &CommunicationAdapter::simulateKeyInputRequested,
						 this,
						 &InputSimulator::simulateKey, Qt::QueuedConnection);

		m_mouseConnection = QObject::connect(communicationAdapter.get(),
						 &CommunicationAdapter::simulateMouseInputRequested,
						 this,
						 &InputSimulator::simulateMouse, Qt::QueuedConnection);
	}
}

void InputSimulator::disable()
{
	QObject::disconnect(m_keyConnection);
	QObject::disconnect(m_mouseConnection);
}

void InputSimulator::simulateKey(const std::shared_ptr<SimulateKeyCommand>& command)
{
	if (command->keyState() == KeyState::Pressed)
	{
		simulateKeyboardPress(command->xkbSymbol(), command->unicodeCharacter(), command->xkbModifiers());
	}
	else if (command->keyState() == KeyState::Released)
	{
		simulateKeyboardRelease(command->xkbSymbol(), command->unicodeCharacter(), command->xkbModifiers());
	}
}

void InputSimulator::simulateMouse(const std::shared_ptr<SimulateMouseCommand>& command)
{
	QPoint point{command->x(), command->y()};

	QPointer<QWindow> targetWindow = m_applicationWindow;
	if (!targetWindow)
	{
		targetWindow = m_lastPressedWindow ?
			m_lastPressedWindow.data() : QGuiApplication::topLevelAt(point);
		if (targetWindow)
		{
			point -= targetWindow->position();
		}
		else
		{
			return;
		}
	}

	const MouseButton button = command->button();

	switch (command->mouseAction())
	{
		case MouseAction::PressOrRelease:
		{
			if (command->mouseButtonState() == MouseButtonState::Pressed)
			{
				simulateMousePress(targetWindow, point, button);
				if (m_pressedMouseButtons != Qt::NoButton)
				{
					m_lastPressedWindow = targetWindow;
					m_focusedWindow = targetWindow;
				}
			}
			else if(command->mouseButtonState() == MouseButtonState::Released)
			{
				simulateMouseRelease(targetWindow, point, button);
				if (m_pressedMouseButtons == Qt::NoButton)
				{
					m_lastPressedWindow.clear();
				}
			}
			break;
		}
		case MouseAction::Move:
		{
			simulateMouseMove(targetWindow, point);
			break;
		}
		case MouseAction::Wheel:
		{
			simulateMouseWheelRequested(targetWindow, point, command->angle());
			break;
		}
	}

}

void InputSimulator::simulateMousePress(QWindow* window, const QPoint& point, MouseButton button)
{
	Q_ASSERT(window);

	Qt::MouseButton changedButton;
	ConvertButtonEventToQtValues(button, true, changedButton, m_pressedMouseButtons);

	auto ev = new QMouseEvent
		{
			QEvent::MouseButtonPress,
			point,
			point,
			window->mapToGlobal(point),
			changedButton,
			m_pressedMouseButtons,
			m_convertedModifiers
		};
	QCoreApplication::postEvent(window, ev);
}

void InputSimulator::simulateMouseRelease(QWindow* window, const QPoint& point, MouseButton button)
{
	Q_ASSERT(window);

	qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();

	Qt::MouseButton changedButton;
	ConvertButtonEventToQtValues(button, false, changedButton, m_pressedMouseButtons);

	/*
	 * How to know if it s a doubleclick
	 * the single click is always send, just in the aftermath you inspect that this was a doubleclick
	 *
	 *    ulong doubleClickInterval = static_cast<ulong>(QGuiApplication::styleHints()->mouseDoubleClickInterval());
				doubleClick = e->timestamp - mousePressTime < doubleClickInterval && button == mousePressButton;
				mousePressTime = e->timestamp;
				mousePressButton = button;
				const QPoint point = QGuiApplicationPrivate::lastCursorPosition.toPoint();
				mousePressX = point.x();
				mousePressY = point.y();
	*/

	if (changedButton == m_lastMouseButtonReleased && m_lastMouseButtonReleasedTimestamp != 0)
	{
		const auto doubleClickInterval =
			static_cast<qint64>(QGuiApplication::styleHints()->mouseDoubleClickInterval());
		if ((currentTimestamp - m_lastMouseButtonReleasedTimestamp) < doubleClickInterval)
		{
			auto ev = new QMouseEvent
				{
					QEvent::MouseButtonDblClick,
					point,
					point,
					window->mapToGlobal(point),
					changedButton,
					m_pressedMouseButtons,
					m_convertedModifiers
				};
			QCoreApplication::postEvent(window, ev);
		}
	}

	m_lastMouseButtonReleasedTimestamp  = currentTimestamp;
	m_lastMouseButtonReleased = changedButton;

	auto ev = new QMouseEvent
		{
			QEvent::MouseButtonRelease,
			point,
			point,
			window->mapToGlobal(point),
			changedButton,
			m_pressedMouseButtons,
			m_convertedModifiers
		};

	QCoreApplication::postEvent(window, ev);
}

void InputSimulator::simulateMouseMove(QWindow* window, const QPoint& point)
{
	Q_ASSERT(window);

	auto ev = new QMouseEvent
		{
			QEvent::MouseMove,
			point,
			point,
			Qt::NoButton,
			m_pressedMouseButtons,
			m_convertedModifiers
		};
	QCoreApplication::postEvent(window, ev);
}

void InputSimulator::simulateMouseWheelRequested(QWindow* window, const QPoint& point, int angle)
{
	Q_ASSERT(window);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
	auto ev = new QWheelEvent
		{
			point,
			window->position() + point, // global position
			{}, // no pixel delta
			QPoint(0, angle), // vertical scroll
			Qt::MouseButton::NoButton,
			m_convertedModifiers,
			Qt::NoScrollPhase,
			false // not inverted
		};
#else
	auto ev = new QWheelEvent
		{
			point,
			angle,
			Qt::MouseButton::NoButton,
			m_convertedModifiers
		};
#endif
	QCoreApplication::postEvent(window, ev);
}

void InputSimulator::simulateKeyboardPress(uint32_t xkbSymbol, uint32_t unicodeCharacter, uint32_t xkbModifiers)
{
	m_convertedModifiers = xkbToQtModifers(xkbModifiers);

	if (m_focusedWindow && xkbSymbol)
	{
		int key = 0;
		const bool mapResult = xkbToQtKey(xkbSymbol, unicodeCharacter, key, m_convertedModifiers);

		if (mapResult && key != Qt::Key_unknown)
		{
			const QChar character{unicodeCharacter};
			const QString text = character.isPrint() ? QString(character) : QString();

			auto ev = new QKeyEvent{QEvent::KeyPress, key, m_convertedModifiers, text};
			QCoreApplication::postEvent(m_focusedWindow, ev);
		}
	}
}

void InputSimulator::simulateKeyboardRelease(uint32_t xkbSymbol, uint32_t unicodeCharacter, uint32_t xkbModifiers)
{
	m_convertedModifiers = xkbToQtModifers(xkbModifiers);

	if (m_focusedWindow && xkbSymbol)
	{
		int key = 0;
		const bool mapResult = xkbToQtKey(xkbSymbol, unicodeCharacter, key, m_convertedModifiers);

		if (mapResult && key != Qt::Key_unknown)
		{
			const QChar character{unicodeCharacter};
			const QString text = character.isPrint() ? QString(character) : QString();

			auto ev = new QKeyEvent{QEvent::KeyRelease, key, m_convertedModifiers, text};
			QCoreApplication::postEvent(m_focusedWindow, ev);
		}
	}
}
} // namespace tvqtsdk
