#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap>
#include <QAbstractListModel>
#include <QJSValue>
#include <functional>

struct CommandEntry {
    QString id;
    QString label;
    QString category;
    QString icon;
    QString shortcut;
    std::function<void()> action;
    int score = 0; // For fuzzy matching
};

class CommandModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        LabelRole,
        CategoryRole,
        IconRole,
        ShortcutRole,
        FullLabelRole
    };

    explicit CommandModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setCommands(const QList<CommandEntry> &commands);
    const CommandEntry& getCommand(int row) const;

private:
    QList<CommandEntry> m_commands;
};

class CommandManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QAbstractListModel* model READ model CONSTANT)
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)

public:
    explicit CommandManager(QObject *parent = nullptr);
    static CommandManager* instance();

    QAbstractListModel* model() const { return m_filteredModel; }
    
    QString filter() const { return m_filter; }
    void setFilter(const QString &f);

    Q_INVOKABLE void registerCommand(const QString &id, const QString &category, const QString &label, 
                                    const QString &icon, const QString &shortcut, QJSValue callback);
    
    // Internal C++ registration
    void registerCommandCpp(const QString &id, const QString &category, const QString &label, 
                          const QString &icon, const QString &shortcut, std::function<void()> action);

    Q_INVOKABLE void executeCommand(int index);
    Q_INVOKABLE void executeCommandById(const QString &id);

signals:
    void filterChanged();

private:
    void updateFilteredModel();
    
    static CommandManager* s_instance;
    QString m_filter;
    QList<CommandEntry> m_allCommands;
    CommandModel* m_filteredModel;
};
