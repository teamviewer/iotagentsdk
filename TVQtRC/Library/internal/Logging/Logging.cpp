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
#include "Logging.h"

#include <QtCore/QDebug>

namespace tvqtsdk
{

namespace
{
constexpr const char* LogFileName = "TVQtRC.log";
constexpr const char* LogPrefixError = "[error] ";
constexpr const char* LogPrefixInfo  = "[info]  ";
}

void Logging::startLogging(const QString& logFolderPath)
{
	std::unique_ptr<QFile> file(new QFile(logFolderPath + "/" + LogFileName));
	if (!file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
	{
		return;
	}

	if (!file->isWritable())
	{
		file->close();
		return;
	}

	std::lock_guard<std::mutex> lock(m_logMutex);
	m_file = std::move(file);
	m_stream.reset(new QTextStream(m_file.get()));
}

void Logging::stopLogging()
{
	shutdown();
}

Logging::~Logging()
{
	shutdown();
}

void Logging::shutdown()
{
	std::lock_guard<std::mutex> lock(m_logMutex);

	if (m_stream)
	{
		m_stream->flush();
		m_stream.reset();
	}

	if (m_file)
	{
		m_file->close();
		m_file.reset();
	}
}

void Logging::logError(const QString& message)
{
	qDebug() << LogPrefixError << message;

	std::lock_guard<std::mutex> lock(m_logMutex);
	if (!m_stream)
	{
		return;
	}

	(*m_stream) << LogPrefixError << message << "\n";

	m_stream->flush();
}

void Logging::logInfo(const QString& message)
{
	qDebug() << LogPrefixInfo << message;

	std::lock_guard<std::mutex> lock(m_logMutex);
	if (!m_stream)
	{
		return;
	}

	(*m_stream) << LogPrefixInfo << message << "\n";

	m_stream->flush();
}
} // namespace tvqtsdk
