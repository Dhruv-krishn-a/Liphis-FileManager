#pragma once

#include <QQuickImageProvider>

class ThumbnailManager;

class ThumbnailImageProvider : public QQuickImageProvider
{
public:
    explicit ThumbnailImageProvider(ThumbnailManager* manager);

    QImage requestImage(const QString &id,
                        QSize *size,
                        const QSize &requestedSize) override;

private:
    ThumbnailManager* m_manager;
};
