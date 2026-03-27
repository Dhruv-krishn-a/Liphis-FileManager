#include "CommandManager.hpp"
#include <QJSValue>
#include <QJSEngine>
#include <QDebug>

CommandModel::CommandModel(QObject *parent) : QAbstractListModel(parent) {}

int CommandModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return m_commands.count();
}

QVariant CommandModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_commands.count()) return QVariant();
    const auto &c = m_commands[index.row()];
    switch (role) {
        case IdRole: return c.id;
        case LabelRole: return c.label;
        case CategoryRole: return c.category;
        case IconRole: return c.icon;
        case ShortcutRole: return c.shortcut;
        case FullLabelRole: return QString("%1: %2").arg(c.category, c.label);
    }
    return QVariant();
}

QHash<int, QByteArray> CommandModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "commandId";
    roles[LabelRole] = "label";
    roles[CategoryRole] = "category";
    roles[IconRole] = "icon";
    roles[ShortcutRole] = "shortcut";
    roles[FullLabelRole] = "fullLabel";
    return roles;
}

void CommandModel::setCommands(const QList<CommandEntry> &commands) {
    beginResetModel();
    m_commands = commands;
    endResetModel();
}

const CommandEntry& CommandModel::getCommand(int row) const {
    return m_commands[row];
}

CommandManager* CommandManager::s_instance = nullptr;

CommandManager::CommandManager(QObject *parent) : QObject(parent) {
    s_instance = this;
    m_filteredModel = new CommandModel(this);
}

CommandManager* CommandManager::instance() {
    return s_instance;
}

void CommandManager::setFilter(const QString &f) {
    if (m_filter == f) return;
    m_filter = f;
    updateFilteredModel();
    emit filterChanged();
}

void CommandManager::registerCommand(const QString &id, const QString &category, const QString &label, 
                                     const QString &icon, const QString &shortcut, QJSValue callback) {
    auto cb = [callback]() mutable {
        if (callback.isCallable()) callback.call();
    };
    registerCommandCpp(id, category, label, icon, shortcut, cb);
}

void CommandManager::registerCommandCpp(const QString &id, const QString &category, const QString &label, 
                                       const QString &icon, const QString &shortcut, std::function<void()> action) {
    m_allCommands.append({id, label, category, icon, shortcut, action});
    updateFilteredModel();
}

void CommandManager::updateFilteredModel() {
    if (m_filter.isEmpty()) {
        m_filteredModel->setCommands(m_allCommands);
        return;
    }

    QList<CommandEntry> filtered;
    QString f = m_filter.toLower();
    for (auto c : m_allCommands) {
        QString full = (c.category + ": " + c.label).toLower();
        int score = 0;
        
        if (full.contains(f)) {
            // Priority: Starts with > Exact substring > Fuzzy match
            if (full.startsWith(f)) score = 100;
            else score = 50;
            
            c.score = score;
            filtered.append(c);
        } else {
            // Basic fuzzy: check if characters appear in order
            int lastIdx = 0;
            bool match = true;
            for (int i = 0; i < f.length(); ++i) {
                int idx = full.indexOf(f[i], lastIdx);
                if (idx < 0) { match = false; break; }
                lastIdx = idx + 1;
            }
            if (match) {
                c.score = 10;
                filtered.append(c);
            }
        }
    }
    
    // Sort by score (descending)
    std::sort(filtered.begin(), filtered.end(), [](const CommandEntry &a, const CommandEntry &b) {
        if (a.score != b.score) return a.score > b.score;
        return a.label < b.label;
    });
    
    m_filteredModel->setCommands(filtered);
}

void CommandManager::executeCommand(int index) {
    if (index >= 0 && index < m_filteredModel->rowCount()) {
        const auto &c = m_filteredModel->getCommand(index);
        if (c.action) c.action();
    }
}

void CommandManager::executeCommandById(const QString &id) {
    for (const auto &c : m_allCommands) {
        if (c.id == id) {
            if (c.action) c.action();
            return;
        }
    }
}
