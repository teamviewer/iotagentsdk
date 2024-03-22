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
#include <QtCore/QProcessEnvironment>

#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include <QtQuick/QQuickWindow>

#ifdef WIDGETS_EVENT_DRIVEN_GRABBING
#include <QtWidgets/QWidget>
#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
#include <QtWidgets/private/qwidgetwindow_qpa_p.h>
#else
#include <QtWidgets/private/qwidgetwindow_p.h>
#endif
#endif

extern Q_GUI_EXPORT QImage qt_gl_read_framebuffer(const QSize& size, bool alpha_format, bool include_alpha);

#define DIRECT_OPENGL_GRABBING

#ifdef WIDGETS_EVENT_DRIVEN_GRABBING
namespace
{

QWidget* WindowToWidget(QWindow* window)
{
	// The following code is experimental.
	// Neither qobject_cast nor dynamic_cast would work.
	// * qobject_cast references QWidgetWindow::staticMetaObject, which is not exposed
	//   from QtWidgets library => getting unresolved symbol error
	// * dynamic_cast references QWidgetWindow's type info, which is not exposed either,
	//   since the Qt libs are compiled without RTTI flag.
	if (window->metaObject()->className() == QStringLiteral("QWidgetWindow"))
	{
		return static_cast<QWidgetWindow*>(window)->widget();
	}
	return nullptr;
}

} // namespace
#endif

namespace tvqtsdk
{

namespace
{

constexpr const char* ForceIntervalGrabEnvKey = "TV_SDK_QT_FORCE_INTERVAL_GRAB";
constexpr const char* FavourQtQuickGrabEnvKey = "TV_SDK_QT_FAVOUR_QTQUICK_GRAB";
constexpr const char* PicturesPersecondEnvKey = "TV_SDK_QT_GRABS_PER_SECOND";

constexpr uint32_t DefaultPicturesPerSecond = 25;

constexpr const char* EglfsPlatformName = "eglfs";

QImage GrabWindow(QWindow* window)
{
	if (window == nullptr)
	{
		return {};
	}

#ifdef WIDGETS_EVENT_DRIVEN_GRABBING
	// QWidget::grab has more advantages in comparison with QScreen::grabWindow.
	// QWidget::grab serializes and runs the pending rendering events/procedures,
	// so that it always renders the window to the last actual state
	// whereas QScreen::grabWindow might not grab the last state/frame.
	if (QWidget* widget = WindowToWidget(window))
	{
		return widget->grab().toImage();
	}
#endif

	// NOTE:
	// QQuickWindow::grabWindow() imposes a huge performance penalty for the rendering thread.
	// This penalty could render a QtQuick based UI pretty much unusable if there are animations used.
	//
	// On the EGLFS platform QQuickWindow::grabWindow is although favoured as there a QQuickWindow cannot be grabbed
	// using QScreen::grabWIndow.
	//
	// By setting TV_SDK_QT_FAVOUR_QTQUICK_GRAB in the process' environment the QQuickWindow::grabWindow can be
	// requested on platforms explicitly other than EGLFS.

	auto quickWindow = qobject_cast<QQuickWindow*>(window);
	const bool qtQuickGrabWindowWanted =
		QProcessEnvironment::systemEnvironment().contains(FavourQtQuickGrabEnvKey) ||
		QGuiApplication::platformName() == EglfsPlatformName;

	if (quickWindow && qtQuickGrabWindowWanted)
	{
		return quickWindow->grabWindow();
	}

	QScreen* screen = window->screen();
	if (screen == nullptr)
	{
		return {};
	}

	QPixmap pixmap = screen->grabWindow(window->winId(), 0, 0);
	if (pixmap.isNull())
	{
		return {};
	}

	return pixmap.toImage();
}

} // namespace

QWindowGrabMethod::QWindowGrabMethod(QWindow* window, const std::shared_ptr<ILogging>& logging, QObject* parent)
	: AbstractScreenGrabMethod(logging, parent)
	, m_window(window)
	, m_timer(new QTimer(this))
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
		[this](const QString& /*title*/)
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

	Q_EMIT imageDefinitionChanged(
		m_window->title(),
		m_window->size(),
		m_window->screen()->physicalDotsPerInch(),
		getColorFormat());
}

ScreenGrabResult QWindowGrabMethod::grab()
{
	if (m_window == nullptr || m_window->screen() == nullptr || !m_window->isVisible())
	{
		return {};
	}

	QImage image = GrabWindow(m_window);

	// grab whole window
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

#ifdef WIDGETS_EVENT_DRIVEN_GRABBING
void QWindowGrabMethod::conditionalReactOnScreenUpdate()
{
	if (!m_repainting.load())
	{
		return;
	}

	reactOnScreenUpdate();

	m_repainting.store(false);
}
#endif // WIDGETS_EVENT_DRIVEN_GRABBING

void QWindowGrabMethod::sendIfScreenChanged()
{
	ScreenGrabResult screenGrabResult;
	{
		std::lock_guard<std::mutex> backbufferLock(m_backbufferMutex);
		std::swap(screenGrabResult, m_lastGrabResult);
	}

	if (screenGrabResult.isValid())
	{
		Q_EMIT grabFinished(screenGrabResult);
	}
}

void QWindowGrabMethod::startGrabbing()
{
	if (m_window == nullptr)
	{
		return;
	}

	auto timerProc = &QWindowGrabMethod::reactOnScreenUpdate;
#ifdef WIDGETS_EVENT_DRIVEN_GRABBING
	if (QWidget* targetWidget = WindowToWidget(m_window))
	{
		class UpdateListener: public QObject
		{
		public:
			explicit UpdateListener(QWindowGrabMethod* grabMethod)
				: QObject{grabMethod}
				, m_grabMethod{grabMethod}
			{
			}

			bool eventFilter(QObject* object, QEvent* event) override
			{
				Q_UNUSED(object);
				switch (event->type())
				{
					case QEvent::Paint:
						{
							m_grabMethod->m_repainting.store(true);
						}
						break;
					case QEvent::ChildAdded:
						{
							if (auto childAddedEvent = dynamic_cast<QChildEvent*>(event))
							{
								install(childAddedEvent->child());
							}
						}
						break;
					default:;
				}
				return false;
			}

			void install(QObject* target)
			{
				target->installEventFilter(this);
				for (const auto child: target->children())
				{
					install(child);
				}
			}

		private:
			QPointer<QWindowGrabMethod> m_grabMethod;
		};
		QPointer<UpdateListener> listener = new UpdateListener{this};

		listener->install(targetWidget);

		m_listener = listener;

		timerProc = &QWindowGrabMethod::conditionalReactOnScreenUpdate;
	}
#endif // WIDGETS_EVENT_DRIVEN_GRABBING
#ifdef DIRECT_OPENGL_GRABBING
	const QQuickWindow* quickWindow = qobject_cast<QQuickWindow*>(m_window);
	if (!QProcessEnvironment::systemEnvironment().contains(ForceIntervalGrabEnvKey) && quickWindow)
	{
		auto grabScreenAndSetDirty = [quickWindow, this]()
		{
			std::lock_guard<std::mutex> backbufferLock(m_backbufferMutex);

			m_lastGrabResult = {}; // clear old image first to release its memory before allocating anew

			const bool alpha = quickWindow->format().alphaBufferSize() > 0 && quickWindow->color().alpha() < 255;
			QImage grabImage = qt_gl_read_framebuffer(quickWindow->size() * quickWindow->devicePixelRatio(), alpha, alpha);
			QRect dirtyRect = QRect(QPoint(0, 0), grabImage.size());

			m_lastGrabResult = ScreenGrabResult(std::move(grabImage), std::move(dirtyRect));
		};

		// The slot gets called from the scene graph thread, since it's connected directly
		// See https://doc.qt.io/qt-5/qquickwindow.html#afterRendering
		// It's necessary for proper image grabbing from the framebuffer.
		m_grabWindowConnection = QObject::connect(quickWindow, &QQuickWindow::afterRendering, this, grabScreenAndSetDirty, Qt::DirectConnection);

		timerProc = &QWindowGrabMethod::sendIfScreenChanged;
	}
#endif // DIRECT_OPENGL_GRABBING
	m_timerConnection = QObject::connect(m_timer, &QTimer::timeout, this, timerProc);

	const uint32_t picturesPerSecond = []()
	{
		const QString value = QProcessEnvironment::systemEnvironment().value(PicturesPersecondEnvKey);

		bool conversionSuccessful = false;
		const uint32_t valueConverted = value.toUInt(&conversionSuccessful);

		return conversionSuccessful ? valueConverted : DefaultPicturesPerSecond;
	}();

	m_timer->start(1000 / picturesPerSecond);

	if (m_window->isVisible())
	{
		signalImageDefinitionChanged();
		reactOnScreenUpdate();
	}
}

void QWindowGrabMethod::stopGrabbing()
{
	QObject::disconnect(m_grabWindowConnection);
	QObject::disconnect(m_timerConnection);

#ifdef WIDGETS_EVENT_DRIVEN_GRABBING
	if (m_listener)
	{
		m_listener->deleteLater();
		m_listener.clear();
	}
#endif
	m_timer->stop();
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
