// src/ThumbnailManager.cpp
#include "ThumbnailManager.hpp"
#include "ThreadPool.hpp"

#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QImageReader>
#include <QImage>
#include <QUrl>
#include <QProcess>
#include <QMutexLocker>
#include <QThread>

namespace {
QString getStandardThumbnailPath(const QString &filePath) {
    // Freedesktop standard uses MD5 of the URI (file:///path/to/file)
    QString uri = QUrl::fromLocalFile(filePath).toString();
    QByteArray hash = QCryptographicHash::hash(uri.toUtf8(), QCryptographicHash::Md5);
    QString hex = hash.toHex();
    
    QString home = QDir::homePath();
    // Standard paths: ~/.cache/thumbnails/large or normal
    QString p = home + "/.cache/thumbnails/large/" + hex + ".png";
    if (QFileInfo::exists(p)) return p;
    p = home + "/.cache/thumbnails/normal/" + hex + ".png";
    if (QFileInfo::exists(p)) return p;
    return QString();
}
}

// Constructor
ThumbnailManager::ThumbnailManager(QObject *parent)
    : QObject(parent),
      m_memCache(32 * 1024) // 32MB limit for thumbs
{
    m_maxConcurrentJobs = qMax(4, QThread::idealThreadCount());
    QString baseCache = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (baseCache.isEmpty()) baseCache = QDir::homePath() + "/.cache";
    m_diskCacheDir = QDir(baseCache).filePath("liphis_thumbs");
    QDir().mkpath(m_diskCacheDir);
    m_memCacheLimit = 32 * 1024 * 1024;
    rebuildMemCache();
}

QString ThumbnailManager::cachePathFor(const QString &filePath) const
{
    QByteArray h = QCryptographicHash::hash(filePath.toUtf8(), QCryptographicHash::Sha1);
    QString name = h.toHex() + "_256.png";
    return QDir(m_diskCacheDir).filePath(name);
}

void ThumbnailManager::setMemoryCacheBytes(int bytes)
{
    QMutexLocker lk(&m_mutex);
    m_memCacheLimit = bytes;
    rebuildMemCache();
}

void ThumbnailManager::rebuildMemCache()
{
    QMutexLocker lk(&m_mutex);
    m_memCache.setMaxCost(m_memCacheLimit / 1024);
}

qsizetype ThumbnailManager::imageCost(const QImage *img) const
{
    if (!img) return 1;
    return qMax<qsizetype>(1, img->sizeInBytes() / 1024);
}

void ThumbnailManager::requestThumbnail(const QString &filePath)
{
    QFileInfo fi(filePath);
    if (!fi.exists() || !fi.isFile()) return;

    static const QStringList imageExt = { "png","jpg","jpeg","bmp","webp","gif","svg" };
    static const QStringList videoExt = { "mp4","mkv","avi","mov","webm","wmv" };
    
    bool isImage = imageExt.contains(fi.suffix().toLower());
    bool isVideo = videoExt.contains(fi.suffix().toLower());
    if (!isImage && !isVideo) return;

    QString key = filePath;
    {
        QMutexLocker lk(&m_mutex);
        QImage *cached = m_memCache.object(key);
        if (cached && !cached->isNull()) {
            QMetaObject::invokeMethod(this, [this, filePath]() {
                emit thumbnailReady(filePath, "image://thumbs/" + filePath);
            }, Qt::QueuedConnection);
            return;
        }
    }

    // 1. Check Standard System Cache (Dolphin/Thunar)
    QString stdPath = getStandardThumbnailPath(filePath);
    if (!stdPath.isEmpty()) {
        QString url = QUrl::fromLocalFile(stdPath).toString();
        QMetaObject::invokeMethod(this, [this, filePath, url]() {
            emit thumbnailReady(filePath, url);
        }, Qt::QueuedConnection);
        return;
    }

    QString diskCachePath = cachePathFor(filePath);
    if (QFileInfo::exists(diskCachePath)) {
        ThreadPool::instance().submit([this, filePath, diskCachePath, key]() {
            QImage *img = new QImage(diskCachePath);
            if (!img->isNull()) {
                QMutexLocker lk(&m_mutex);
                m_memCache.insert(key, img, imageCost(img));
                QString url = QUrl::fromLocalFile(diskCachePath).toString();
                QMetaObject::invokeMethod(this, [this, filePath, url]() {
                    emit thumbnailReady(filePath, url);
                }, Qt::QueuedConnection);
            } else { delete img; }
        });
        return;
    }

    {
        QMutexLocker lk(&m_mutex);
        if (m_pendingSet.contains(key)) return;
        m_pendingSet.insert(key);
        m_queue.enqueue(key);
    }
    processQueue();
}

void ThumbnailManager::processQueue()
{
    while (true) {
        QString next;
        {
            QMutexLocker lk(&m_mutex);
            if (m_activeJobs >= m_maxConcurrentJobs || m_queue.isEmpty()) return;
            next = m_queue.dequeue();
            ++m_activeJobs;
        }

        ThreadPool::instance().submit([this, next]() {
            workerGenerate(next, cachePathFor(next));
            {
                QMutexLocker lk(&m_mutex);
                m_activeJobs = qMax(0, m_activeJobs - 1);
                m_pendingSet.remove(next);
            }
            QMetaObject::invokeMethod(this, "processQueue", Qt::QueuedConnection);
        });
    }
}

QImage ThumbnailManager::getFromMemoryCache(const QString &key)
{
    QMutexLocker locker(&m_mutex);
    QImage *p = m_memCache.object(key);
    return p ? *p : QImage();
}

void ThumbnailManager::workerGenerate(const QString &filePath, const QString &cachePath)
{
    QFileInfo fi(filePath);
    static const QStringList videoExt = { "mp4","mkv","avi","mov","webm","wmv" };
    
    if (videoExt.contains(fi.suffix().toLower())) {
        QProcess proc;
        proc.start("ffmpegthumbnailer", {"-i", filePath, "-o", cachePath, "-s", "256"});
        if (proc.waitForFinished(3000) && QFileInfo::exists(cachePath)) {
            QString url = QUrl::fromLocalFile(cachePath).toString();
            QMetaObject::invokeMethod(this, [this, filePath, url]() { emit thumbnailReady(filePath, url); }, Qt::QueuedConnection);
            return;
        }
    }

    QImageReader reader(filePath);
    reader.setAutoTransform(true);
    
    QSize sz = reader.size();
    if (sz.isValid() && (sz.width() > 12000 || sz.height() > 12000)) return;

    reader.setScaledSize(QSize(512, 512));
    QImage img = reader.read();
    if (img.isNull()) return;

    QImage thumb = img.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QDir d = QFileInfo(cachePath).dir();
    if (!d.exists()) d.mkpath(".");
    thumb.save(cachePath, "PNG");

    QImage *px = new QImage(thumb);
    {
        QMutexLocker lk(&m_mutex);
        m_memCache.insert(filePath, px, imageCost(px));
    }
    QString url = QUrl::fromLocalFile(cachePath).toString();
    QMetaObject::invokeMethod(this, [this, filePath, url]() {
        emit thumbnailReady(filePath, url);
    }, Qt::QueuedConnection);
}
