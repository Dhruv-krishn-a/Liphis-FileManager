#include "SystemIconProvider.hpp"
#include <QIcon>
#include <QPixmap>
#include <QMimeDatabase>
#include <QMimeType>

SystemIconProvider::SystemIconProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage SystemIconProvider::requestImage(const QString &id,
                                        QSize *size,
                                        const QSize &requestedSize)
{
    int width = requestedSize.width() > 0 ? requestedSize.width() : 64;
    int height = requestedSize.height() > 0 ? requestedSize.height() : 64;

    // 1. Try exact match
    QIcon icon = QIcon::fromTheme(id);
    
    // 2. If it looks like a MIME type (e.g. application/pdf), try mapping it to a standard icon name
    if (icon.isNull() && id.contains("/")) {
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForName(id);
        if (mime.isValid()) {
            icon = QIcon::fromTheme(mime.iconName());
            if (icon.isNull()) {
                icon = QIcon::fromTheme(mime.genericIconName());
            }
        }
    }

    // 3. Try standard fallbacks
    if (icon.isNull()) {
        static const QStringList fallbacks = {
            "text-x-generic",
            "document",
            "file",
            "unknown"
        };
        for (const QString &fb : fallbacks) {
            icon = QIcon::fromTheme(fb);
            if (!icon.isNull()) break;
        }
    }

    // 4. Final safety: If still null, return a transparent 1x1 image 
    // to stop QML from flooding the terminal with "Failed to get image" errors.
    if (icon.isNull() || icon.pixmap(width, height).isNull()) {
        QImage empty(1, 1, QImage::Format_ARGB32);
        empty.fill(Qt::transparent);
        if (size) *size = QSize(width, height);
        return empty;
    }

    QImage img = icon.pixmap(width, height).toImage();
    if (size)
        *size = img.size();

    return img;
}
