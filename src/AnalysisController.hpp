#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <atomic>
#include <mutex>
#include "ThreadPool.hpp"
#include "AnalysisEngine.hpp"

class AnalysisController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    explicit AnalysisController(QObject *parent = nullptr);
    ~AnalysisController();

    bool busy() const;

    Q_INVOKABLE void analyseFolder(const QString &path);
    Q_INVOKABLE void calculateFolderSize(const QString &path);
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void copyTreeToClipboard();

signals:
    void busyChanged();
    void analysisStarted();
    void analysisFinished(QVariantMap result);
    void nodeFound(QVariantMap node);
    void analysisProgress(qlonglong size, int files, int dirs, QString currentItem);
    void folderSizeCalculated(QString path, qlonglong size);

private:
    std::atomic<bool> m_cancelRequested{false};
    bool m_busy{false};
    AnalysisEngine::DirNode m_lastResult;
    std::mutex m_resultMutex;
};
