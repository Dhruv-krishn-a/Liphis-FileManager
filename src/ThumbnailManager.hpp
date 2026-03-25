#pragma once

#include <QObject>
#include <QCache>
#include <QMutex>
#include <QQueue>
#include <QSet>
#include <QImage>

class ThumbnailManager : public QObject
{
    Q_OBJECT

public:
    explicit ThumbnailManager(QObject *parent = nullptr);

    void requestThumbnail(const QString &filePath);
    void setMemoryCacheBytes(int bytes);

    QImage getFromMemoryCache(const QString &key);
    QString cachePathFor(const QString &filePath) const;

signals:
    void thumbnailReady(const QString &filePath,
                        const QString &thumbnailUrl);

private:
    // ---- Internal helpers ----
    void rebuildMemCache();
    qsizetype imageCost(const QImage *img) const;
    Q_INVOKABLE void processQueue();
    void workerGenerate(const QString &filePath,
                        const QString &cachePath);

    // ---- Disk cache ----
    QString m_diskCacheDir;

    // ---- Memory cache ----
    int m_memCacheLimit = 32 * 1024 * 1024; // 32MB default
    QCache<QString, QImage> m_memCache;

    // ---- Concurrency ----
    QMutex m_mutex;
    QQueue<QString> m_queue;
    QSet<QString> m_pendingSet;

    int m_activeJobs = 0;
    int m_maxConcurrentJobs = 2; // limit parallel workers
};
