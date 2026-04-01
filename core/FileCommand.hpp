#pragma once

#include <QString>
#include <QStringList>
#include <memory>
#include <vector>
#include <deque>
#include <QObject>
#include "FileSystemEngine.hpp"
#include <filesystem>

namespace fs = std::filesystem;

class IFileCommand {
public:
    virtual ~IFileCommand() = default;
    virtual bool execute() = 0;
    virtual bool undo() = 0;
    virtual QString description() const = 0;
};

class RenameCommand : public IFileCommand {
    std::string m_oldPath;
    std::string m_newPath;
public:
    RenameCommand(const std::string& oldP, const std::string& newP) 
        : m_oldPath(oldP), m_newPath(newP) {}
    
    bool execute() override {
        return FileSystemEngine::renamePath(m_oldPath, m_newPath).success;
    }
    
    bool undo() override {
        return FileSystemEngine::renamePath(m_newPath, m_oldPath).success;
    }
    
    QString description() const override {
        return QString("Rename %1 to %2").arg(QString::fromStdString(fs::path(m_oldPath).filename().string()))
                                         .arg(QString::fromStdString(fs::path(m_newPath).filename().string()));
    }
};

class CreateFolderCommand : public IFileCommand {
    std::string m_path;
public:
    CreateFolderCommand(const std::string& p) : m_path(p) {}
    
    bool execute() override {
        return FileSystemEngine::createDirectory(m_path).success;
    }
    
    bool undo() override {
        return FileSystemEngine::deletePath(m_path).success;
    }
    
    QString description() const override {
        return QString("Create folder %1").arg(QString::fromStdString(fs::path(m_path).filename().string()));
    }
};

class CreateFileCommand : public IFileCommand {
    std::string m_path;
public:
    CreateFileCommand(const std::string& p) : m_path(p) {}
    
    bool execute() override {
        return FileSystemEngine::createFile(m_path).success;
    }
    
    bool undo() override {
        return FileSystemEngine::deletePath(m_path).success;
    }
    
    QString description() const override {
        return QString("Create file %1").arg(QString::fromStdString(fs::path(m_path).filename().string()));
    }
};

class MoveCommand : public IFileCommand {
    std::vector<std::pair<std::string, std::string>> m_paths;
public:
    MoveCommand(const std::vector<std::pair<std::string, std::string>>& p) : m_paths(p) {}
    
    bool execute() override {
        bool all = true;
        for (const auto& pair : m_paths) {
            if (!FileSystemEngine::movePath(pair.first, pair.second).success) all = false;
        }
        return all;
    }
    
    bool undo() override {
        bool all = true;
        for (const auto& pair : m_paths) {
            if (!FileSystemEngine::movePath(pair.second, pair.first).success) all = false;
        }
        return all;
    }
    
    QString description() const override {
        return QString("Move %1 items").arg(m_paths.size());
    }
};

class FileCommandHistory : public QObject {
    Q_OBJECT
public:
    explicit FileCommandHistory(QObject* parent = nullptr) : QObject(parent) {}

    void push(std::unique_ptr<IFileCommand> command) {
        if (command->execute()) {
            m_undoStack.push_back(std::move(command));
            m_redoStack.clear();
            if (m_undoStack.size() > m_maxHistory) {
                m_undoStack.pop_front();
            }
            emit canUndoChanged();
            emit canRedoChanged();
        }
    }

    bool canUndo() const { return !m_undoStack.empty(); }
    bool canRedo() const { return !m_redoStack.empty(); }

    void undo() {
        if (m_undoStack.empty()) return;
        auto cmd = std::move(m_undoStack.back());
        m_undoStack.pop_back();
        if (cmd->undo()) {
            m_redoStack.push_back(std::move(cmd));
            emit canUndoChanged();
            emit canRedoChanged();
        }
    }

    void redo() {
        if (m_redoStack.empty()) return;
        auto cmd = std::move(m_redoStack.back());
        m_redoStack.pop_back();
        if (cmd->execute()) {
            m_undoStack.push_back(std::move(cmd));
            emit canUndoChanged();
            emit canRedoChanged();
        }
    }

    QString undoDescription() const {
        return m_undoStack.empty() ? "" : m_undoStack.back()->description();
    }

signals:
    void canUndoChanged();
    void canRedoChanged();

private:
    std::deque<std::unique_ptr<IFileCommand>> m_undoStack;
    std::deque<std::unique_ptr<IFileCommand>> m_redoStack;
    const size_t m_maxHistory = 50;
};
