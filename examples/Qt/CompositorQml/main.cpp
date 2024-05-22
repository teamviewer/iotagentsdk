// Forked from https://code.qt.io/cgit/qt/qtwayland.git/tree/examples/wayland/minimal-qml?h=6.2 
// addjustment happend only for the integration of the TVQTRC Plugin

#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include <QtGui/QGuiApplication>

#include <QtQml/QQmlApplicationEngine>

#include <QtCore/QPluginLoader>

#include <QtWaylandCompositor/QtWaylandCompositor>
#include <QtWaylandCompositor/QWaylandQuickOutput>
#include <QtWaylandCompositor/QWaylandOutput>

#include"TVQtRC/Interface.h"

#include <iostream>

constexpr auto PluginFileName = "libTVQtRC.so";

int main(int argc, char *argv[])
{
	// ShareOpenGLContexts is needed for using the threaded renderer
	// on Nvidia EGLStreams
	QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
	QGuiApplication app(argc, argv);

	tvqtsdk::TVQtRCPluginInterface* plugin = nullptr;

	// Load TeamViewer plugin
	QPluginLoader loader(PluginFileName);
	if (QObject* instance = loader.instance())
	{
		plugin = qobject_cast<tvqtsdk::TVQtRCPluginInterface*>(instance);
	}
	else
	{
		std::cerr << "Failed to load " << PluginFileName << " with error: " << loader.errorString().toStdString() << std::endl;
		return EXIT_FAILURE;
	}

	if (!plugin)
	{
		std::cerr << "No TVQtRC plugin found\n";
		return EXIT_FAILURE;
	}

	QQmlApplicationEngine appEngine(QUrl("qrc:///main.qml"));

	const auto rootObjects = appEngine.rootObjects();

	const auto compositor = qobject_cast<QWaylandCompositor*>(rootObjects[0]);
	const auto output = compositor->defaultOutput();
	const auto window = output->window();

	plugin->registerApplicationWindow(window);
	plugin->setControlMode(tvqtsdk::ControlMode::FullControl);

	const auto exitCode = app.exec();

	plugin->deregisterApplication();

	return exitCode;
}
