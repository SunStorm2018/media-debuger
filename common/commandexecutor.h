// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H

#include <QObject>
#include <QStringList>
#include <QProcess>
#include <QtConcurrent>

class CommandExecutor : public QObject
{
    Q_OBJECT

public:
    explicit CommandExecutor(QObject *parent = nullptr);
    ~CommandExecutor();

    /**
     * @brief Start executing the list of commands concurrently
     * @param commands List of commands to execute
     * @param maxConcurrent Maximum number of concurrent processes (default: 4)
     */
    void executeCommands(const QStringList &commands, int maxConcurrent = 4);

    /**
     * @brief Stop all executing commands
     */
    void stopExecution();

    /**
     * @brief Check if execution is currently running
     * @return true if execution is in progress
     */
    bool isRunning() const;

signals:
    /**
     * @brief Signal emitted when a command starts execution
     * @param command The command that started
     * @param index Index of the command in the list
     */
    void commandStarted(const QString &command, int index);

    /**
     * @brief Signal emitted when a command finishes execution
     * @param command The command that finished
     * @param index Index of the command in the list
     * @param exitCode Exit code of the process
     * @param exitStatus Exit status of the process
     */
    void commandFinished(const QString &command, int index, int exitCode, QProcess::ExitStatus exitStatus);

    /**
     * @brief Signal emitted when a command produces standard output
     * @param command The command that produced output
     * @param output The output text
     * @param index Index of the command in the list
     */
    void commandOutput(const QString &command, const QString &output, int index);

    /**
     * @brief Signal emitted when a command produces error output
     * @param command The command that produced error
     * @param error The error text
     * @param index Index of the command in the list
     */
    void commandError(const QString &command, const QString &error, int index);

    /**
     * @brief Signal emitted for progress updates
     * @param completed Number of completed commands
     * @param total Total number of commands
     * @param message Progress message
     */
    void progressUpdated(int completed, int total, const QString &message);

    /**
     * @brief Signal emitted when all commands have finished execution
     * @param success true if all commands completed successfully (exit code 0)
     */
    void allCommandsFinished(bool success);

    /**
     * @brief Signal emitted when execution is stopped by user
     */
    void executionStopped();

private slots:
    void onCommandFinished(int index);
    void onProcessOutputReady();
    void onProcessErrorReady();

private:
    /**
     * @brief Execute a single command
     * @param command The command to execute
     * @param index Index of the command in the list
     */
    bool executeSingleCommand(const QString &command, int index);

    /**
     * @brief Clean up all processes and resources
     */
    void cleanup();

    QList<QProcess*> m_processes;
    QStringList m_commands;
    QFutureWatcher<void> m_watcher;
    QMutex m_mutex;
    int m_completedCount;
    int m_totalCount;
    bool m_isRunning;
    bool m_stopRequested;
};

#endif // COMMANDEXECUTOR_H
