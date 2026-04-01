#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QFileSystemWatcher>
#include <QTimer>

class DocumentIntelligenceController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString lastScanRoot READ lastScanRoot NOTIFY lastScanRootChanged)
    Q_PROPERTY(QVariantList groups READ groups NOTIFY groupsChanged)
    Q_PROPERTY(QVariantList inboxItems READ inboxItems NOTIFY inboxItemsChanged)
    Q_PROPERTY(QVariantList duplicateExact READ duplicateExact NOTIFY duplicatesChanged)
    Q_PROPERTY(QVariantList duplicateNear READ duplicateNear NOTIFY duplicatesChanged)
    Q_PROPERTY(QVariantMap healthReport READ healthReport NOTIFY healthReportChanged)
    Q_PROPERTY(QVariantList namingPreview READ namingPreview NOTIFY namingPreviewChanged)
    Q_PROPERTY(QString downloadsPath READ downloadsPath CONSTANT)

public:
    explicit DocumentIntelligenceController(QObject *parent = nullptr);

    bool enabled() const { return m_enabled; }
    void setEnabled(bool value);

    bool busy() const { return m_busy; }
    QString lastScanRoot() const { return m_lastScanRoot; }
    QVariantList groups() const { return m_groups; }
    QVariantList inboxItems() const { return m_inboxItems; }
    QVariantList duplicateExact() const { return m_duplicateExact; }
    QVariantList duplicateNear() const { return m_duplicateNear; }
    QVariantMap healthReport() const { return m_healthReport; }
    QVariantList namingPreview() const { return m_namingPreview; }
    QString downloadsPath() const;

    Q_INVOKABLE void initialize();
    Q_INVOKABLE void ensureInitialized();
    Q_INVOKABLE void scanFolder(const QString &path);
    Q_INVOKABLE void scanDownloadsInbox();
    Q_INVOKABLE QVariantMap timelineForPath(const QString &path) const;
    Q_INVOKABLE QVariantMap groupForPath(const QString &path) const;
    Q_INVOKABLE void setCurrentVersion(const QString &groupId, const QString &path);
    Q_INVOKABLE void setDocumentStatus(const QString &path, const QString &status);
    Q_INVOKABLE void triageInbox(const QString &path, const QString &action);
    Q_INVOKABLE void clearInbox();
    Q_INVOKABLE QVariantList previewCanonicalNames(const QString &rootPath, const QString &nameTemplate);
    Q_INVOKABLE int applyCanonicalNames();
    Q_INVOKABLE int undoLastCanonicalRename();
    Q_INVOKABLE void mergeExactDuplicateGroup(const QString &hashKey, const QString &action);
    Q_INVOKABLE QString defaultNameTemplate() const { return "{base}_{date}_v{version}"; }

signals:
    void enabledChanged();
    void busyChanged();
    void lastScanRootChanged();
    void groupsChanged();
    void inboxItemsChanged();
    void duplicatesChanged();
    void healthReportChanged();
    void namingPreviewChanged();
    void operationNotice(const QString &message, bool isError);

private:
    struct DocItem {
        QString path;
        QString name;
        QString extension;
        QString baseKey;
        QString groupId;
        QString status;
        qlonglong size{0};
        qlonglong mtime{0};
        QString quickHash;
    };

    QString normalizeBaseKey(const QString &name) const;
    QString computeQuickHash(const QString &path, qlonglong size) const;
    QString computeFullHash(const QString &path) const;
    bool isDocumentPath(const QString &path) const;
    QVariantMap docToMap(const DocItem &doc) const;
    QString buildCanonicalName(const DocItem &doc, const QString &nameTemplate, int versionIndex) const;
    void rebuildDerivedData();
    void rebuildGroupsAndDuplicates();
    void rebuildHealthReport();
    void loadState();
    void saveState() const;
    QString statePath() const;
    QString dbPath() const;
    bool ensureDatabase();
    void loadStateFromDb();
    void saveStateToDb() const;
    void markBusy(bool value);
    void attachStatusesToGroups();

    bool m_enabled{true};
    bool m_busy{false};
    QString m_lastScanRoot;
    QList<DocItem> m_docs;
    QVariantList m_groups;
    QVariantList m_inboxItems;
    QVariantList m_duplicateExact;
    QVariantList m_duplicateNear;
    QVariantMap m_healthReport;
    QVariantList m_namingPreview;
    QVariantList m_lastRenameUndo;
    QVariantMap m_statusByPath;
    QVariantMap m_currentByGroup;
    QVariantMap m_seenDownloads;
    QVariantMap m_ignoredDuplicateHashes;
    QFileSystemWatcher m_downloadsWatcher;
    QTimer m_downloadsDebounceTimer;
    QTimer m_healthRefreshTimer;
    bool m_initialized{false};
};
