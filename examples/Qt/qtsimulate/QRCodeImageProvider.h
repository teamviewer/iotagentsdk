#pragma once

#include <QtGui/QImage>
#include <QtQuick/QQuickImageProvider>

class QRCodeImageProvider : public QQuickImageProvider
{
public:
	QRCodeImageProvider();

	QImage requestImage(const QString& data, QSize* size, const QSize& requestedSize) override;

private:
	QString m_data;
	QImage m_image;
};
