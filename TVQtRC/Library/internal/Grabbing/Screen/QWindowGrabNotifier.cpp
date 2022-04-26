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
#include "QWindowGrabNotifier.h"

#include "internal/Logging/ILogging.h"

#include <QtCore/QTimer>
#include <QtQuick/QQuickWindow>

namespace
{

constexpr uint32_t RequestsPerSecond = 25;

} // namespace

namespace tvqtsdk
{

QWindowGrabNotifier::QWindowGrabNotifier(QWindow* window, const std::shared_ptr<ILogging>&, QObject* parent)
	: QObject(parent), m_window(window)
{
}

QWindowGrabNotifier::~QWindowGrabNotifier()
{
	stop();
}

void QWindowGrabNotifier::start()
{
	if (m_window.isNull() || m_running)
	{
		return;
	}

	m_running = true;

	auto emitGrabRequestAction = [this]()
	{
		grabRequested(QRect(0, 0, m_window->width(), m_window->height()));
	};

	auto emitImageDefinitionChangedAction = [this]()
	{
		imageDefinitionChanged(m_window->title(), m_window->size());
	};

	if (const QQuickWindow* quickWindow = qobject_cast<QQuickWindow*>(m_window))
	{
		m_grabNotifyConnection = QObject::connect(quickWindow, &QQuickWindow::afterRendering, this, emitGrabRequestAction);
	}
	else
	{
		m_timer = new QTimer(this);
		m_grabNotifyConnection = QObject::connect(m_timer, &QTimer::timeout, this, emitGrabRequestAction);
		m_timer->start(1000 / RequestsPerSecond);
	}

	m_widthChangedConnection = QObject::connect(m_window, &QWindow::widthChanged, this, emitImageDefinitionChangedAction);
	m_heightChangedConnection = QObject::connect(m_window, &QWindow::heightChanged, this, emitImageDefinitionChangedAction);
	m_titleChangedConnection = QObject::connect(m_window, &QWindow::windowTitleChanged, this, emitImageDefinitionChangedAction);

	// initial triger
	emitImageDefinitionChangedAction();
	emitGrabRequestAction();
}

void QWindowGrabNotifier::stop()
{
	if (!m_running)
	{
		return;
	}

	m_running = false;

	if (m_timer)
	{
		m_timer->stop();
		delete m_timer;
	}

	QObject::disconnect(m_grabNotifyConnection);
	QObject::disconnect(m_widthChangedConnection);
	QObject::disconnect(m_heightChangedConnection);
	QObject::disconnect(m_titleChangedConnection);
}

} // namespace tvqtsdk
