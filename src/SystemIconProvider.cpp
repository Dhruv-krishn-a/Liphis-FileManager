#include "SystemIconProvider.hpp"
#include <QIcon>
#include <QPixmap>

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

    QIcon icon = QIcon::fromTheme(id);
    if (icon.isNull()) {
        // Fallback to generic if theme icon missing
        icon = QIcon::fromTheme("text-x-generic");
    }

    QImage img = icon.pixmap(width, height).toImage();
    if (size)
        *size = img.size();

    return img;
}
