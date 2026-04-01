#include "CommandManager.hpp"
#include <QJSValue>
#include <QJSEngine>
#include <QDebug>
#include <QKeySequence>

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
        QList<CommandEntry> sorted = m_allCommands;
        std::sort(sorted.begin(), sorted.end(), [](const CommandEntry &a, const CommandEntry &b) {
            if (a.category != b.category) return a.category < b.category;
            return a.label < b.label;
        });
        m_filteredModel->setCommands(sorted);
        return;
    }

    QList<CommandEntry> filtered;
    QString f = m_filter.toLower();
    
    // Check for category prefix '@'
    QString categoryFilter;
    QString termFilter = f;
    if (f.startsWith("@")) {
        int spaceIdx = f.indexOf(" ");
        if (spaceIdx > 0) {
            categoryFilter = f.mid(1, spaceIdx - 1);
            termFilter = f.mid(spaceIdx + 1);
        } else {
            categoryFilter = f.mid(1);
            termFilter = "";
        }
    }

    for (auto c : m_allCommands) {
        if (!categoryFilter.isEmpty() && !c.category.toLower().startsWith(categoryFilter)) continue;
        
        if (termFilter.isEmpty()) {
            c.score = 100;
            filtered.append(c);
            continue;
        }

        QString full = (c.category + ": " + c.label).toLower();
        QString label = c.label.toLower();
        int score = 0;
        
        if (label.contains(termFilter)) {
            if (label.startsWith(termFilter)) score = 200;
            else score = 150;
        } else if (full.contains(termFilter)) {
            if (full.startsWith(termFilter)) score = 100;
            else score = 50;
        } else {
            // Basic fuzzy match
            int lastIdx = 0;
            bool match = true;
            for (int i = 0; i < termFilter.length(); ++i) {
                int idx = full.indexOf(termFilter[i], lastIdx);
                if (idx < 0) { match = false; break; }
                lastIdx = idx + 1;
            }
            if (match) score = 10;
        }
        
        if (score > 0) {
            c.score = score;
            filtered.append(c);
        }
    }
    
    // Sort by score (descending)
    std::sort(filtered.begin(), filtered.end(), [](const CommandEntry &a, const CommandEntry &b) {
        if (a.score != b.score) return a.score > b.score;
        if (a.category != b.category) return a.category < b.category;
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

void CommandManager::updateCommandShortcut(const QString &id, const QString &shortcut)
{
    bool changed = false;
    for (auto &c : m_allCommands) {
        if (c.id == id) {
            if (c.shortcut != shortcut) {
                c.shortcut = shortcut;
                changed = true;
            }
            break;
        }
    }
    if (changed) updateFilteredModel();
}

void CommandManager::clearCommands()
{
    m_allCommands.clear();
    updateFilteredModel();
}

bool CommandManager::isValidShortcut(const QString &shortcut) const
{
    const QString s = shortcut.trimmed();
    if (s.isEmpty()) return true;
    QKeySequence ks(s, QKeySequence::PortableText);
    return !ks.isEmpty() && !ks.toString(QKeySequence::PortableText).trimmed().isEmpty();
}

QString CommandManager::normalizeShortcut(const QString &shortcut) const
{
    const QString s = shortcut.trimmed();
    if (s.isEmpty()) return "";
    QKeySequence ks(s, QKeySequence::PortableText);
    if (ks.isEmpty()) return "";
    return ks.toString(QKeySequence::PortableText);
}
