#include "QRCodeImageProvider.h"

#include "ThirdParty/qrcodegen.hpp"

#include <QtCore/QUrl>

namespace
{

QImage toQImage(const qrcodegen::QrCode& qr)
{
	const qint32 qrcodeSize = qr.getSize();
	QImage image(qrcodeSize, qrcodeSize, QImage::Format_Mono);
	image.setColor(0, QColor{Qt::white}.rgb());
	image.setColor(1, QColor{Qt::black}.rgb());

	for (int y = 0; y < qrcodeSize; y++)
	{
		for (int x = 0; x < qrcodeSize; x++)
		{
			image.setPixel(x, y, qr.getModule(x, y) ? 1 : 0);
		}
	}

	return image;
}

QImage generateQrCodeFromText(const QString& text)
{
	const qrcodegen::QrCode::Ecc errCorLvl = qrcodegen::QrCode::Ecc::LOW;  // Error correction level
	const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(text.toUtf8().constData(), errCorLvl);
	return toQImage(qr);
}

} //namespace

QRCodeImageProvider::QRCodeImageProvider()
	: QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage QRCodeImageProvider::requestImage(const QString& data, QSize* size, const QSize& requestedSize)
{
	if (m_data.isNull() || data != m_data)
	{
		m_data = data;
		const auto decodedData = QUrl::fromPercentEncoding(data.toUtf8());
		m_image = generateQrCodeFromText(decodedData);
	}

	if (m_image.isNull())
	{
		return {};
	}

	if (size)
	{
		*size = m_image.size();
	}

	if (!requestedSize.isEmpty())
	{
		return m_image.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio, Qt::FastTransformation);
	}

	return m_image;
}
