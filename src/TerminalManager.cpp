#include "TerminalManager.hpp"
#include <QDir>
#include <QDebug>

TerminalManager::TerminalManager(QObject *parent)
    : QObject(parent), m_process(new QProcess(this))
{
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &TerminalManager::readOutput);
    connect(m_process, &QProcess::errorOccurred, this, &TerminalManager::handleError);

    m_currentDir = QDir::homePath();
    m_process->setWorkingDirectory(m_currentDir);
    
    // Start a shell
    m_process->start("sh", QStringList() << "-i");
}

TerminalManager::~TerminalManager()
{
    if (m_process->state() == QProcess::Running) {
        m_process->terminate();
        if (!m_process->waitForFinished(1000)) {
            m_process->kill();
            m_process->waitForFinished(500);
        }
    }
}

void TerminalManager::setCurrentDir(const QString &dir)
{
    if (m_currentDir == dir) return;
    m_currentDir = dir;
    if (m_process->state() == QProcess::Running) {
        sendCommand("cd \"" + dir + "\"");
    } else {
        m_process->setWorkingDirectory(dir);
    }
    emit currentDirChanged();
}

void TerminalManager::sendCommand(const QString &cmd)
{
    if (m_process->state() == QProcess::Running) {
        m_process->write((cmd + "\n").toUtf8());
        emit commandExecuted(cmd);
    }
}

void TerminalManager::clear()
{
    m_output.clear();
    emit outputChanged();
}

void TerminalManager::readOutput()
{
    QByteArray data = m_process->readAllStandardOutput();
    appendOutput(QString::fromUtf8(data));
}

void TerminalManager::handleError(QProcess::ProcessError error)
{
    appendOutput("\n[Terminal Error: " + QString::number(error) + "]\n");
}

void TerminalManager::appendOutput(const QString &text)
{
    m_output += text;
    // Limit buffer size
    if (m_output.length() > 50000) {
        m_output = m_output.right(40000);
    }
    emit outputChanged();
}
