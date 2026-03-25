#include "ThumbnailImageProvider.hpp"
#include "ThumbnailManager.hpp"
#include <QFileInfo>

ThumbnailImageProvider::ThumbnailImageProvider(ThumbnailManager* manager)
    : QQuickImageProvider(QQuickImageProvider::Image),
      m_manager(manager)
{
}

QImage ThumbnailImageProvider::requestImage(const QString &id,
                                            QSize *size,
                                            const QSize &requestedSize)
{
    if (!m_manager)
        return QImage();

    // id is the full path
    QImage img = m_manager->getFromMemoryCache(id);

    if (img.isNull()) {
        QString diskPath = m_manager->cachePathFor(id);
        if (QFileInfo::exists(diskPath)) {
            img.load(diskPath);
        }
    }

    if (!img.isNull()) {
        if (size)
            *size = img.size();
        return img;
    }

    // Return a transparent 1x1 image to suppress QML errors while loading
    QImage placeholder(1, 1, QImage::Format_ARGB32_Premultiplied);
    placeholder.fill(Qt::transparent);
    return placeholder;
}
