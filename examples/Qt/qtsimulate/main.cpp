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
#include <TVQtRC/Interface.h>

#include <QtCore/QCommandLineParser>
#include <QtCore/QPluginLoader>

#include <QtGui/QFont>

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QWidget>

#include <QtQuick/QQuickWindow>

#include <iostream>
#include <memory>

#include "AppModel.h"

#include "ui_WidgetsDemoWindow.h"

constexpr auto PluginFileName = "libTVQtRC.so";
constexpr auto ApplicationName = "TVRC-qt-example";
constexpr auto ApplicationVersion = "1.0";
constexpr auto ApplicationDescription = "Example Qt application to demonstrate remote control via TeamViewer";
constexpr auto EglfsPlatformName = "eglfs";

#if defined(TV_STATIC_TVQTRCPLUGIN)
Q_IMPORT_PLUGIN(TVQtRCPlugin);
#endif


class WidgetDemoWindow : public QWidget
{
public:
	WidgetDemoWindow(QWidget* parent = nullptr) : QWidget(parent)
	{
		m_ui.setupUi(this);

		QObject::connect(
			m_ui.pbMessageBox,
			&QPushButton::clicked,
			this,
			[this]()
			{
				QMessageBox::information(
					this,
					QStringLiteral("Test message box"),
					QStringLiteral("Test message"),
					QMessageBox::Yes | QMessageBox::No);
			});

		m_timer.setInterval(10);
		QObject::connect(
			&m_timer,
			&QTimer::timeout,
			this,
			[this]()
			{
				const int val = m_ui.progressBar->value();
				if (val == m_ui.progressBar->maximum())
				{
					m_ui.progressBar->reset();
				}
				m_ui.progressBar->setValue(val + 1);
			});

		m_ui.pbAnimation->setStyleSheet("QPushButton { background-color: yellow } "
										"QPushButton:pressed { border-width: 2px; border-style: outset; border-color: green; } "
										"QPushButton:hover { background-color: purple } ");

		QObject::connect(
			m_ui.pbAnimation,
			&QPushButton::clicked,
			this,
			[this]()
			{
				if (m_timer.isActive())
				{
					m_timer.stop();
				}
				else
				{
					m_timer.start();
				}
			});
	}

private:
	Ui::WidgetsDemoWindow m_ui;
	QTimer m_timer;
};

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

	QApplication app{argc, argv};

	QCoreApplication::setApplicationName(ApplicationName);
	QCoreApplication::setApplicationVersion(ApplicationVersion);

	QCommandLineParser commandlineParser;
	commandlineParser.setApplicationDescription(ApplicationDescription);
	commandlineParser.addHelpOption();
	commandlineParser.addVersionOption();

	QCommandLineOption enableFileLoggingOption(QStringList() << "l" << "file-logging", "Enable file logging to directory <directory>", "directory");
	commandlineParser.addOption(enableFileLoggingOption);

	QCommandLineOption setBaseUrlOption(QStringList() << "u" << "base-url", "Set base URL for sockets", "url");
	commandlineParser.addOption(setBaseUrlOption);

	QCommandLineOption setAgentAPIUrlOption(QStringList() << "a" << "agent-api-url", "Set IoT Agent URL", "agent-api-url");
	commandlineParser.addOption(setAgentAPIUrlOption);

	QCommandLineOption forceMainWindowOption("main-only", "Only create main application window.");
	commandlineParser.addOption(forceMainWindowOption);

	QCommandLineOption forceQWidgetsOption("qwidget-only", "Only create widget-based window(s).");
	commandlineParser.addOption(forceQWidgetsOption);

	QCommandLineOption rcModeOption("control-mode", "Only create widget-based window(s).", "control-mode");
	commandlineParser.addOption(rcModeOption);

	commandlineParser.process(app);

	const QString faceName = "Liberation Sans";

	QFont::insertSubstitutions(faceName, {"Segoe UI","Noto Sans","Droid Sans","Frutiger","Frutiger Linotype","Helvetica Neue","Helvetica","Dejavu Sans","Arial","sans-serif"});
	QFont baseFont{faceName};
	baseFont.setPointSize(10);
	baseFont.setWeight(QFont::Normal);

	QGuiApplication::setFont(baseFont);

	tvqtsdk::TVQtRCPluginInterface* plugin = nullptr;

#if defined(TV_STATIC_TVQTRCPLUGIN)
	for (QObject* instance: QPluginLoader::staticInstances())
	{
		if ((plugin = qobject_cast<tvqtsdk::TVQtRCPluginInterface*>(instance)))
		{
			break;
		}
	}
#else
	QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
	if (!QLibrary::isLibrary(PluginFileName))
	{
		std::cerr << "ERROR: locating plugin failed" << std::endl;
		return EXIT_FAILURE;
	}

	// Load TeamViewer plugin
	QPluginLoader loader(PluginFileName);
	if (QObject* instance = loader.instance())
	{
		plugin = qobject_cast<tvqtsdk::TVQtRCPluginInterface*>(instance);
	}
	else
	{
		std::cerr << loader.errorString().toStdString() << std::endl;
		return EXIT_FAILURE;
	}
#endif // defined(TV_STATIC_TVQTRCPLUGIN)
	if (!plugin)
	{
		std::cerr << "No TVQtRC plugin found\n";
		return EXIT_FAILURE;
	}

	if (commandlineParser.isSet(setBaseUrlOption) && commandlineParser.isSet(setAgentAPIUrlOption))
	{
		tvqtsdk::BaseUrlParseResultCode code = plugin->setRemoteScreenSdkUrls(
			commandlineParser.value(setBaseUrlOption),
			commandlineParser.value(setAgentAPIUrlOption));
		if (code == tvqtsdk::BaseUrlParseResultCode::CharacterLimitForPathExceeded)
		{
			std::cerr << "The provided path for the sdk base url will exceed the size limit for socket paths. Please use a shorter path.\n";
			return EXIT_FAILURE;
		}
		else if (code == tvqtsdk::BaseUrlParseResultCode::SchemaNotValid)
		{
			std::cerr << "The provided path for the sdk base url is not in the required schema of unix://<absolutePath>\n";
			return EXIT_FAILURE;
		}
	}
	else if (commandlineParser.isSet(setBaseUrlOption) || commandlineParser.isSet(setAgentAPIUrlOption))
	{
		std::cerr << "Provide either both url options (--" <<
			setBaseUrlOption.names()[1].toStdString() << ", --" <<
			setAgentAPIUrlOption.names()[1].toStdString() << ") or none of them\n";
		return EXIT_FAILURE;
	}

	if (commandlineParser.isSet(enableFileLoggingOption))
	{
		const QString logDirectoryPath = commandlineParser.value(enableFileLoggingOption);
		plugin->startLogging(logDirectoryPath);
	}

	if (commandlineParser.isSet(rcModeOption))
	{
		const QString rcModeValue = commandlineParser.value(rcModeOption);
		if (rcModeValue == "FullControl")
		{
			plugin->setControlMode(tvqtsdk::ControlMode::FullControl);
		}
		else if (rcModeValue == "ViewOnly")
		{
			plugin->setControlMode(tvqtsdk::ControlMode::ViewOnly);
		}
	}

	std::unique_ptr<QQmlApplicationEngine> engine;

	const bool createQmlMainWindow = !commandlineParser.isSet(forceQWidgetsOption);

	if (createQmlMainWindow)
	{
		engine.reset(new QQmlApplicationEngine);

		// provide application model with TeamViewer Plugin
		auto appModel = new AppModel(plugin, engine.get());
		engine->rootContext()->setContextProperty("appModel", appModel);

		// Load QML UI
		engine->load(QUrl(QStringLiteral("qrc:/main.qml")));
		if (engine->rootObjects().isEmpty())
		{
			return EXIT_FAILURE;
		}
	}

	// NOTE: When using the EGLFS platform mixing QtQuick and QtWidget based windows is not possible and will result in
	// aborting the application. Therefore, we disable the QtWidget-based demo window here for EGLFS.
	const bool createWidgetTestWindow =
		!createQmlMainWindow ||
		(QGuiApplication::platformName() != EglfsPlatformName && !commandlineParser.isSet(forceMainWindowOption));

	std::unique_ptr<WidgetDemoWindow> demoAppWidget;

	if (createWidgetTestWindow)
	{
		demoAppWidget.reset(new WidgetDemoWindow());
		demoAppWidget->show();
	}

	plugin->registerApplication();

	QMetaObject::Connection focusChangeConnection = QObject::connect(
		qGuiApp,
		&QGuiApplication::focusWindowChanged,
		dynamic_cast<QObject*>(plugin),
		[plugin](QWindow* window)
		{
			if (window)
			{
				plugin->registerApplicationWindow(window);
			}
			else
			{
				plugin->deregisterApplicationWindow();
			}
		});
	plugin->registerApplicationWindow(QGuiApplication::focusWindow());

	const int result = app.exec();

	QObject::disconnect(focusChangeConnection);

	if (plugin->isTeamViewerSessionRunning())
	{
		plugin->terminateTeamViewerSessions();
	}
	plugin->setControlMode(tvqtsdk::ControlMode::Disabled);
	plugin->deregisterApplication();

	return result;
}
