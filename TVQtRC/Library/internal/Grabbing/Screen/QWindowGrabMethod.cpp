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
#include "QWindowGrabMethod.h"

#include "internal/Grabbing/Screen/ScreenGrabResult.h"

#include "internal/Logging/ILogging.h"

#include <QtCore/QMetaObject>
#include <QtCore/QTimer>

#include <QtGui/QScreen>

#include <QtQuick/QQuickWindow>

extern Q_GUI_EXPORT QImage qt_gl_read_framebuffer(const QSize& size, bool alpha_format, bool include_alpha);

#define DIRECT_OPENGL_GRABBING

namespace tvqtsdk
{

namespace
{

constexpr uint32_t PicturesPerSecond = 25;

QImage GrabWindow(QWindow* window)
{
	if (window == nullptr)
	{
		return QImage();
	}

	QScreen* screen = window->screen();
	if (screen == nullptr)
	{
		return QImage();
	}

	QPixmap pixmap = screen->grabWindow(window->winId(), 0, 0);

	if (pixmap.isNull())
	{
		if (QQuickWindow* quickWindow = qobject_cast<QQuickWindow*>(window))
		{
			return quickWindow->grabWindow();
		}
		else
		{
			return QImage();
		}
	}
	else
	{
		return pixmap.toImage();
	}
}

} // namespace

QWindowGrabMethod::QWindowGrabMethod(QWindow* window, const std::shared_ptr<ILogging>& logging, QObject* parent)
	: AbstractScreenGrabMethod(logging, parent), m_window(window)
{
	QObject::connect(
		m_window,
		&QWindow::heightChanged,
		this,
		[this](int /*height*/)
		{
			signalImageDefinitionChanged();
			reactOnScreenUpdate();
		});

	QObject::connect(
		m_window,
		&QWindow::widthChanged,
		this,
		[this](int /*width*/)
		{
			signalImageDefinitionChanged();
			reactOnScreenUpdate();
		});

	QObject::connect(
		m_window,
		&QWindow::windowTitleChanged,
		this,
		[this](QString /*title*/)
		{
			signalImageDefinitionChanged();
			reactOnScreenUpdate();
		});

	QObject::connect(
		m_window,
		&QWindow::screenChanged,
		this,
		[this](QScreen* /*screen*/)
		{
			m_grabColorFormat = QImage::Format::Format_Invalid;
			signalImageDefinitionChanged();
			reactOnScreenUpdate();
		});

		QObject::connect(
		m_window,
		&QWindow::visibleChanged,
		this,
		[this](bool /*visible*/)
		{
			m_grabColorFormat = QImage::Format::Format_Invalid;
			signalImageDefinitionChanged();
			reactOnScreenUpdate();
		});
}

void QWindowGrabMethod::signalImageDefinitionChanged()
{
	if (m_window == nullptr || m_window->screen() == nullptr || !m_window->isVisible())
	{
		return;
	}

	imageDefinitionChanged(
		m_window->title(),
		m_window->size(),
		m_window->screen()->physicalDotsPerInch(),
		getColorFormat());
}

ScreenGrabResult QWindowGrabMethod::grab()
{
	if (m_window == nullptr || m_window->screen() == nullptr || !m_window->isVisible())
	{
		return ScreenGrabResult();
	}

	QImage image = GrabWindow(m_window);

	//FullApp Grab
	const QPoint topLeft(0,0);
	const QSize windowSize = m_window->size();

	QRect dirtyRect(topLeft, windowSize);

	ScreenGrabResult screenGrabResult(std::move(image), std::move(dirtyRect));
	return screenGrabResult;
}

void QWindowGrabMethod::reactOnScreenUpdate()
{
	ScreenGrabResult grabResult = grab();
	if (grabResult.isValid())
	{
		Q_EMIT grabFinished(grabResult);
	}
}

void QWindowGrabMethod::startGrabbing()
{
	if (m_window == nullptr)
	{
		return;
	}

#ifdef DIRECT_OPENGL_GRABBING
	if (const QQuickWindow* quickWindow = qobject_cast<QQuickWindow*>(m_window))
	{
		m_grabResultContainer.reset(new GrabResultProxy());
		QObject::connect(m_grabResultContainer.data(), &GrabResultProxy::grabResultProvided, this, &QWindowGrabMethod::grabFinished);

		auto sendUpdateAction = [contextWeak = m_grabResultContainer.toWeakRef(), quickWindow]()
		{
			if (auto context = contextWeak.toStrongRef())
			{
				const bool alpha = quickWindow->format().alphaBufferSize() > 0 && quickWindow->color().alpha() < 255;
				QImage grabImage = qt_gl_read_framebuffer(quickWindow->size() * quickWindow->devicePixelRatio(), alpha, alpha);

				QRect dirtyRect(QPoint(0, 0), grabImage.size());

				ScreenGrabResult screenGrabResult(std::move(grabImage), std::move(dirtyRect));

				// Safe signal call. The direct call would lead to emitting the signal from
				// the thread different to the thread `this` object belongs to.
				QMetaObject::invokeMethod(
					context.data(),
					"grabResultProvided",
					Qt::QueuedConnection,
					Q_ARG(tvqtsdk::ScreenGrabResult, std::move(screenGrabResult)));
			}
		};

		// The slot gets called from the scene graph thread, since it's connected directly
		// See https://doc.qt.io/qt-5/qquickwindow.html#afterRendering
		// It's necessary for proper image grabbing from the framebuffer.
		m_grabWindowConnection = QObject::connect(quickWindow, &QQuickWindow::afterRendering, this, sendUpdateAction, Qt::DirectConnection);
	}
	else
#endif
	{
		m_timer = new QTimer(this);
		connect(m_timer, SIGNAL(timeout()), this, SLOT(reactOnScreenUpdate()));
		m_timer->start(1000 / PicturesPerSecond);
	}

	if (m_window->isVisible())
	{
		signalImageDefinitionChanged();
		reactOnScreenUpdate();
	}
}

void QWindowGrabMethod::stopGrabbing()
{
	QObject::disconnect(m_grabWindowConnection);

	if (m_timer != nullptr)
	{
		m_timer->stop();
	}
}

ColorFormat QWindowGrabMethod::getColorFormat()
{
	ColorFormat internalColorFormat = ColorFormat::Unsupported;
	if (m_window == nullptr)
	{
		m_logging->logError("[QWindowGrabMethod] unable to retrieve the color format: no window is set");
	}
	else
	{
		if (m_grabColorFormat == QImage::Format::Format_Invalid)
		{
			QImage image = GrabWindow(m_window);
			m_grabColorFormat = image.format();
		}

		internalColorFormat = toColorFormat(m_grabColorFormat);

		m_logging->logInfo("[QWindowGrabMethod] QImage::Format of grabbing is: " + QString::number(m_grabColorFormat));
		m_logging->logInfo("[QWindowGrabMethod] Internal ColorFormat of grabbing is:" + QString::number(static_cast<uint8_t>(internalColorFormat)));
	}

	return internalColorFormat;
}

} // namespace tvqtsdk
