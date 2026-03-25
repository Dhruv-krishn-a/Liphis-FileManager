#include "AnalysisController.hpp"
#include "AnalysisEngine.hpp"
#include "ThreadPool.hpp"
#include <QMetaObject>
#include <QGuiApplication>
#include <QClipboard>
#include <QPointer>
#include <QDebug>

AnalysisController::AnalysisController(QObject *parent)
    : QObject(parent)
{
}

AnalysisController::~AnalysisController()
{
    m_cancelRequested = true;
}

bool AnalysisController::busy() const { return m_busy; }

static QVariantMap dirNodeToMap(const AnalysisEngine::DirNode &node) {
    QVariantMap m;
    m["name"] = QString::fromStdString(node.name);
    m["size"] = static_cast<qlonglong>(node.size);
    m["isDir"] = node.isDir;
    m["fileCount"] = static_cast<int>(node.fileCount);
    return m;
}

void AnalysisController::analyseFolder(const QString &path)
{
    if (m_busy) return;
    
    m_busy = true;
    emit busyChanged();
    emit analysisStarted();
    m_cancelRequested = false;

    QPointer<AnalysisController> safeThis(this);

    ThreadPool::instance().submit([safeThis, path]() {
        if (!safeThis) return;

        auto shouldCancel = [safeThis]() -> bool { 
            return !safeThis || safeThis->m_cancelRequested.load(); 
        };
        
        auto onProgress = [safeThis](std::uint64_t size, std::uint64_t files, std::uint64_t dirs, const std::string& item) {
            if (safeThis) {
                QMetaObject::invokeMethod(safeThis, [safeThis, size, files, dirs, item]() {
                    if (safeThis) emit safeThis->analysisProgress(static_cast<qlonglong>(size), static_cast<int>(files), static_cast<int>(dirs), QString::fromStdString(item));
                }, Qt::QueuedConnection);
            }
        };

        try {
            auto result = AnalysisEngine::analyseDirectoryFast(path.toStdString(), shouldCancel, onProgress);
            
            if (safeThis) {
                {
                    std::lock_guard<std::mutex> lock(safeThis->m_resultMutex);
                    safeThis->m_lastResult = result;
                }

                for (const auto& child : result.children) {
                    QMetaObject::invokeMethod(safeThis, [safeThis, child]() {
                        if (safeThis) emit safeThis->nodeFound(dirNodeToMap(child));
                    }, Qt::QueuedConnection);
                }
                
                QMetaObject::invokeMethod(safeThis, [safeThis, result]() {
                    if (safeThis) {
                        safeThis->m_busy = false;
                        emit safeThis->busyChanged();
                        emit safeThis->analysisFinished(dirNodeToMap(result));
                    }
                }, Qt::QueuedConnection);
            }
        } catch (...) {
            if (safeThis) {
                QMetaObject::invokeMethod(safeThis, [safeThis]() {
                    if (safeThis) {
                        safeThis->m_busy = false;
                        emit safeThis->busyChanged();
                    }
                }, Qt::QueuedConnection);
            }
        }
    });
}

void AnalysisController::calculateFolderSize(const QString &path)
{
    QPointer<AnalysisController> safeThis(this);
    ThreadPool::instance().submit([safeThis, path]() {
        if (!safeThis) return;
        auto shouldCancel = [safeThis]() -> bool { return !safeThis || safeThis->m_cancelRequested.load(); };
        auto size = AnalysisEngine::calculateFolderSize(path.toStdString(), shouldCancel);
        
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, [safeThis, path, size]() {
                if (safeThis) emit safeThis->folderSizeCalculated(path, static_cast<qlonglong>(size));
            }, Qt::QueuedConnection);
        }
    });
}

void AnalysisController::cancel()
{
    m_cancelRequested = true;
}

void AnalysisController::copyTreeToClipboard()
{
    std::string tree;
    {
        std::lock_guard<std::mutex> lock(m_resultMutex);
        if (m_lastResult.name.empty()) return;
        tree = AnalysisEngine::generateTreeString(m_lastResult);
    }
    QGuiApplication::clipboard()->setText(QString::fromStdString(tree));
}
