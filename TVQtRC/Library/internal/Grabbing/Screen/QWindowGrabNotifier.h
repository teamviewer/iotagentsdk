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

#include <QtCore/QPointer>
#include <QtGui/QWindow>

#include <memory>

namespace tvqtsdk
{

class ILogging;

class QWindowGrabNotifier final : public QObject
{
	Q_OBJECT
public:
	QWindowGrabNotifier(QWindow* window, const std::shared_ptr<ILogging>& logging, QObject* parent = nullptr);
	~QWindowGrabNotifier() override;

	void start();
	void stop();

Q_SIGNALS:
	void grabRequested(QRect rectOfInterest);
	void imageDefinitionChanged(const QString& title, QSize size);

private:
	const QPointer<QWindow> m_window;

	bool m_running = false;
	QPointer<QTimer> m_timer;

	QMetaObject::Connection m_grabNotifyConnection;
	QMetaObject::Connection m_widthChangedConnection;
	QMetaObject::Connection m_heightChangedConnection;
	QMetaObject::Connection m_titleChangedConnection;
};

} // namespace tvqtsdk
