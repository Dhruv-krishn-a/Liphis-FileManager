#pragma once

#include <QObject>
#include <QProcess>
#include <QStringList>

class TerminalManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString output READ output NOTIFY outputChanged)
    Q_PROPERTY(QString currentDir READ currentDir WRITE setCurrentDir NOTIFY currentDirChanged)

public:
    explicit TerminalManager(QObject *parent = nullptr);
    ~TerminalManager();

    QString output() const { return m_output; }
    QString currentDir() const { return m_currentDir; }
    void setCurrentDir(const QString &dir);

    Q_INVOKABLE void sendCommand(const QString &cmd);
    Q_INVOKABLE void clear();

signals:
    void outputChanged();
    void currentDirChanged();
    void commandExecuted(const QString &cmd);

private slots:
    void readOutput();
    void handleError(QProcess::ProcessError error);

private:
    QProcess *m_process;
    QString m_output;
    QString m_currentDir;
    void appendOutput(const QString &text);
};
