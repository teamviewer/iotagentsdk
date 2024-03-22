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

#include "internal/Grabbing/Screen/ColorFormat.h"

#include "internal/Grabbing/Screen/AbstractScreenGrabMethod.h"
#include "internal/Grabbing/Screen/ScreenGrabResult.h"

#include <QtCore/QPointer>
#include <QtGui/QWindow>
#include <QtCore/QTimer>

#include <atomic>
#include <mutex>

namespace tvqtsdk
{

class QWindowGrabMethod final : public AbstractScreenGrabMethod
{
	Q_OBJECT
public:
	QWindowGrabMethod(QWindow* window, const std::shared_ptr<ILogging>& logging, QObject* parent = nullptr);
	~QWindowGrabMethod() override = default;

	void startGrabbing() override;
	void stopGrabbing() override;

	virtual ScreenGrabResult grab() override;

	ColorFormat getColorFormat() override;

private:
	Q_SLOT void reactOnScreenUpdate();
	Q_SLOT void sendIfScreenChanged();

	void signalImageDefinitionChanged();

#ifdef WIDGETS_EVENT_DRIVEN_GRABBING
	Q_SLOT void conditionalReactOnScreenUpdate();

	QPointer<QObject> m_listener;
	std::atomic_bool m_repainting{true};
#endif

	QMetaObject::Connection m_grabWindowConnection;
	QMetaObject::Connection m_timerConnection;
	const QPointer<QWindow> m_window = nullptr;
	const QPointer<QTimer> m_timer = nullptr;
	QImage::Format m_grabColorFormat = QImage::Format::Format_Invalid;

	ScreenGrabResult m_lastGrabResult;
	std::mutex m_backbufferMutex;
};

} // namespace tvqtsdk
