#pragma once

#include <QQuickImageProvider>
#include <QIcon>
#include <QImage>

class SystemIconProvider : public QQuickImageProvider
{
public:
    SystemIconProvider();

    QImage requestImage(const QString &id,
                        QSize *size,
                        const QSize &requestedSize) override;
};
