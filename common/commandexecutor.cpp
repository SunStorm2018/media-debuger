// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "commandexecutor.h"
#include <QThreadPool>
#include <QDebug>

CommandExecutor::CommandExecutor(QObject *parent)
    : QObject(parent)
    , m_completedCount(0)
    , m_totalCount(0)
    , m_isRunning(false)
    , m_stopRequested(false)
{
    // Register QProcess::ExitStatus for cross-thread signals
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");

    // Register other custom types if needed
    qRegisterMetaType<QList<int>>("QList<int>");
    qRegisterMetaType<QStringList>("QStringList");

    connect(&m_watcher, &QFutureWatcher<void>::finished, this, [this]() {
        emit allCommandsFinished(!m_stopRequested);
        m_isRunning = false;
        cleanup();
    });
}

CommandExecutor::~CommandExecutor()
{
    stopExecution();
    cleanup();
}

void CommandExecutor::executeCommands(const QStringList &commands, int maxConcurrent)
{
    if (m_isRunning) {
        qWarning() << "Execution already in progress";
        return;
    }

    m_commands = commands;
    m_totalCount = commands.size();
    m_completedCount = 0;
    m_isRunning = true;
    m_stopRequested = false;

    emit progressUpdated(0, m_totalCount, tr("Starting execution of %1 commands...").arg(m_totalCount));

    // Set up thread pool with max concurrency
    // QThreadPool::globalInstance()->setMaxThreadCount(maxConcurrent);

    // Use QtConcurrent with thread pool for concurrent execution
    auto executor = [this](const QString &command) -> bool {
        int index = m_commands.indexOf(command);
        if (index != -1 && !m_stopRequested) {
            return executeSingleCommand(command, index);
        }
        return false;
    };
    
    QFuture<void> future = QtConcurrent::map(m_commands, executor);

    m_watcher.setFuture(future);
}

void CommandExecutor::stopExecution()
{
    QMutexLocker locker(&m_mutex);
    if (!m_isRunning) return;

    m_stopRequested = true;

    // Terminate all running processes
    for (QProcess *process : m_processes) {
        if (process->state() == QProcess::Running) {
            process->terminate();
            if (!process->waitForFinished(1000)) {
                process->kill();
            }
        }
    }

    // Stop the concurrent execution
    if (m_watcher.isRunning()) {
        m_watcher.cancel();
        m_watcher.waitForFinished();
    }

    emit executionStopped();
    emit progressUpdated(m_completedCount, m_totalCount, tr("Execution stopped by user"));
}

bool CommandExecutor::isRunning() const
{
    return m_isRunning;
}

bool CommandExecutor::executeSingleCommand(const QString &command, int index)
{
    if (m_stopRequested) return false;

    QProcess *process = new QProcess();
    process->setProcessChannelMode(QProcess::MergedChannels);

    connect(process, &QProcess::readyReadStandardOutput, this, &CommandExecutor::onProcessOutputReady);
    connect(process, &QProcess::readyReadStandardError, this, &CommandExecutor::onProcessErrorReady);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, index, command](int exitCode, QProcess::ExitStatus exitStatus) {
                onCommandFinished(index);
                emit commandFinished(command, index, exitCode, exitStatus);
            });

    {
        QMutexLocker locker(&m_mutex);
        m_processes.append(process);
    }

    emit commandStarted(command, index);
    emit progressUpdated(m_completedCount, m_totalCount,
                         tr("Executing command %1/%2: %3").arg(index + 1).arg(m_totalCount).arg(command));

    // Start the process
    process->start("bash", QStringList() << "-c" << command);

    if (!process->waitForStarted(3000)) {
        emit commandError(command, tr("Failed to start process"), index);
        onCommandFinished(index);
        return false;
    }

    // Wait for process to finish with timeout
    if (!process->waitForFinished(-1) && !m_stopRequested) {
        emit commandError(command, tr("Process timeout"), index);
    }

    return true;
}

void CommandExecutor::onCommandFinished(int /*index*/)
{
    QMutexLocker locker(&m_mutex);
    m_completedCount++;

    QString progressMessage = tr("Completed %1/%2 commands").arg(m_completedCount).arg(m_totalCount);
    if (m_completedCount == m_totalCount) {
        progressMessage = tr("All commands completed successfully");
    }

    emit progressUpdated(m_completedCount, m_totalCount, progressMessage);
}

void CommandExecutor::onProcessOutputReady()
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    if (!process) return;

    QString output = process->readAllStandardOutput();
    if (output.trimmed().isEmpty()) return;

    int index = m_processes.indexOf(process);
    if (index != -1) {
        QString command = m_commands.value(index);
        emit commandOutput(command, output, index);
    }
}

void CommandExecutor::onProcessErrorReady()
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    if (!process) return;

    QString error = process->readAllStandardError();
    if (error.trimmed().isEmpty()) return;

    int index = m_processes.indexOf(process);
    if (index != -1) {
        QString command = m_commands.value(index);
        emit commandError(command, error, index);
    }
}

void CommandExecutor::cleanup()
{
    QMutexLocker locker(&m_mutex);
    for (QProcess *process : m_processes) {
        if (process->state() == QProcess::Running) {
            process->terminate();
            process->waitForFinished(1000);
        }
        process->deleteLater();
    }
    m_processes.clear();
}
