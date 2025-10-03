#include "ffmpegbuildtool.h"
#include "ui_ffmpegbuildtool.h"

FFmpegBuildTool::FFmpegBuildTool(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FFmpegBuildTool)
{
    ui->setupUi(this);

    loadSettings();

    for (const QString &path : m_filePaths) {
        if (!path.isEmpty() && QDir(path).exists()) {
            ui->local_ffmpeg_combx->addItem(path);
        }
    }

    if (ui->local_ffmpeg_combx->count() > 0) {
        ui->local_ffmpeg_combx->setCurrentIndex(0);

        loadFFmpegOptions();
    }

    m_addRecordAction = new QAction("Add Too Selected Options");
    m_RemoveRecordAction = new QAction("Remove Selected");

    ui->ffmpeg_complie_options_tb->addContextAction(m_addRecordAction);
    ui->select_option_tb->addContextAction(m_RemoveRecordAction);

    // action connect
    connect(m_addRecordAction, &QAction::triggered, [=]() {
        ui->select_option_tb->clear_detail_tb();
        ui->select_option_tb->init_header_detail_tb(QStringList{"Option", "Description", "Default Value"});
        ui->select_option_tb->append_data_detail_tb(ui->ffmpeg_complie_options_tb->getSelectLines());

        ui->cmd_ple->setPlainText(getConfigCmd());
    });

    connect(m_RemoveRecordAction, &QAction::triggered, [=]() {
        ui->select_option_tb->remove_selected_row();

        ui->cmd_ple->setPlainText(getConfigCmd());
    });

    connect(ui->select_option_tb, &InfoWidgets::dataChanged, [=](){
        ui->cmd_ple->setPlainText(getConfigCmd());
    });

    emit ui->show_cmd_cbx->toggled(false);
    ui->select_option_tb->setSearchTitleVisiable(false);
}

FFmpegBuildTool::~FFmpegBuildTool()
{
    delete ui;
}

QMap<QString, QList<QStringList>> FFmpegBuildTool::getOptions()
{
    QMap<QString, QList<QStringList>> options;

    QProcess process;
    QString optionText;

    process.setWorkingDirectory(ui->local_ffmpeg_combx->currentText());
    process.start("bash", QStringList() << "./configure" << "-h");

    if (process.waitForFinished(30000)) {
        QByteArray output = process.readAllStandardOutput();
        QByteArray error = process.readAllStandardError();

        if (!output.isEmpty()) {
            optionText = output;
        } else if (!error.isEmpty()) {
            qDebug() << "Error:" << QString::fromLocal8Bit(error);
        }
    } else {
        qDebug() << "Process timeout or error:" << process.errorString();
    }

    QStringList optionsLines = optionText.split("\n", QString::SkipEmptyParts);

    QString option, description, defaultValue, lastkey;
    for (auto line: optionsLines) {
        if (line.contains("Usage:") || line.contains("Options:") || line.contains("NOTE:")) {
            continue;
        }

        line = line.trimmed();
        if (line.endsWith(":")) {
            lastkey = line.replace("options:", "").trimmed();
            options.insert(lastkey.replace(":", ""), QList<QStringList>{});
            continue;
        }

        if (line.startsWith("--")) {
            defaultValue.clear();
            parseOptionLineFast(line, option, description, defaultValue);
            options[lastkey].append(QStringList{option, description, defaultValue});
            continue;
        } else {
            if (options[lastkey].size() != 0) {
                QString combineOption = options[lastkey].last().join(" ").append(" " + line.trimmed());
                defaultValue.clear();
                parseOptionLineFast(combineOption, option, description, defaultValue);
                options[lastkey].last() = QStringList{option, description, defaultValue};
            } else {
                continue;
            }
        }
    }

    // for (const QString& category : options.keys()) {
    //     qDebug() << "=== " << category << " ===";
    //     for (auto it : options[category]) {
    //         qDebug() << it.join(" ");
    //     }
    // }

    return options;
}

QString FFmpegBuildTool::getConfigCmd()
{
    QString cmd = "./configure \\\n";

    for (auto it: *ui->select_option_tb->getTableData()) {
        QStringList optionList = it.at(0).split("=", QString::SkipEmptyParts);

        QString value = it.at(2);

        cmd.append(QString("%1%2%3 \\\n")
                       .arg(optionList.at(0))
                       .arg(value.trimmed().isEmpty() ? "" : "=")
                       .arg(value));
    }

    cmd = cmd.trimmed();
    cmd.remove(cmd.length() - 1, 1);
    cmd = cmd.trimmed();
    return cmd;
}

void FFmpegBuildTool::on_show_cmd_cbx_toggled(bool checked)
{
    ui->cmd_ple->setVisible(checked);
}

void FFmpegBuildTool::on_select_local_path_btn_clicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(
        this,
        tr("Select FFmpeg source folder"),
        m_filePaths.isEmpty() ? QDir::homePath() : m_filePaths.first()
        );

    if (folderPath.isEmpty())
        return;

    folderPath = QDir::toNativeSeparators(folderPath);
    if (!QDir(folderPath).exists()) {
        qWarning() << "Selected folder not exist!";
        return;
    }

    m_filePaths.removeAll(folderPath);
    m_filePaths.prepend(folderPath);

    const int maxHistory = 10;
    if (m_filePaths.size() > maxHistory) {
        m_filePaths = m_filePaths.mid(0, maxHistory);
    }

    QString currentText = ui->local_ffmpeg_combx->currentText();

    ui->local_ffmpeg_combx->clear();
    ui->local_ffmpeg_combx->addItems(m_filePaths);

    int index = ui->local_ffmpeg_combx->findText(currentText);
    if (index != -1) {
        ui->local_ffmpeg_combx->setCurrentIndex(index);
    } else if (ui->local_ffmpeg_combx->count() > 0) {
        ui->local_ffmpeg_combx->setCurrentIndex(0);
    }

    saveSettings();
}

void FFmpegBuildTool::loadSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    settings.beginGroup(FFMPEGBUILDER_SETTINGS_GROUP);
    m_filePaths = settings.value(FFMPEGBUILDER_RECENTFOLDERS_KEY).toStringList();
    settings.endGroup();
}

void FFmpegBuildTool::saveSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    settings.beginGroup(FFMPEGBUILDER_SETTINGS_GROUP);
    settings.setValue(FFMPEGBUILDER_RECENTFOLDERS_KEY, m_filePaths);
    settings.endGroup();

    settings.sync();
}

void FFmpegBuildTool::on_build_btn_clicked()
{
    ui->build_btn->setEnabled(false);
    QProcess *process = new QProcess(this);

    ui->complie_output_ple->clear();

    QString startTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->complie_output_ple->appendHtml(
        QString("<font color='blue'>=== Progress start at: %1 ===</font>").arg(startTime)
        );

    connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
        QByteArray output = process->readAllStandardOutput();
        QString text = QString::fromLocal8Bit(output);

        QString timestamp = QTime::currentTime().toString("hh:mm:ss");
        ui->complie_output_ple->appendPlainText("[" + timestamp + "] " + text);

    });

    connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
        QByteArray error = process->readAllStandardError();
        QString text = QString::fromLocal8Bit(error);

        QString timestamp = QTime::currentTime().toString("hh:mm:ss");
        ui->complie_output_ple->appendHtml(
            "<font color='red'>[" + timestamp + "] " + text + "</font>"
            );

    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
                QString endTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                QString color = (exitCode == 0) ? "green" : "red";
                QString status = (exitStatus == QProcess::NormalExit) ? "Normal exit" : "Crush exit";

                ui->build_btn->setEnabled(true);

                ui->complie_output_ple->appendHtml(
                    QString("<font color='%1'>=== Progress end at: %2 ===</font>").arg(color).arg(endTime)
                    );
                ui->complie_output_ple->appendHtml(
                    QString("<font color='%1'>Exit code: %2, statue: %3</font>").arg(color).arg(exitCode).arg(status)
                    );

                process->deleteLater();
            });

    process->setWorkingDirectory(ui->local_ffmpeg_combx->currentText());

    if (process->workingDirectory() != ui->local_ffmpeg_combx->currentText()) {
        process->start("cd", QStringList{ ui->local_ffmpeg_combx->currentText()});
    }
    QString command = ui->cmd_ple->toPlainText().trimmed();
    QStringList arguments = command.split("\\", Qt::SkipEmptyParts);

    for (auto &it : arguments) {
        it = it.trimmed();
    }

    if (arguments.isEmpty()) return;
    qDebug() << "bash" << arguments;
    qDebug() << process->workingDirectory();
    process->start("bash", arguments);
}

void FFmpegBuildTool::parseOptionLineFast(const QString& line, QString& option, QString& description, QString& defaultValue) {
    option.clear();
    description.clear();
    defaultValue.clear();

    int firstSpace = line.indexOf(' ');
    if (firstSpace == -1) {
        option = line.trimmed();
        return;
    }

    option = line.left(firstSpace).trimmed();

    int lastBracket = line.lastIndexOf('[');
    if (lastBracket > firstSpace) {
        description = line.mid(firstSpace + 1, lastBracket - firstSpace - 1).trimmed();
        defaultValue = line.mid(lastBracket).trimmed().remove("[").remove("]");
    } else {
        description = line.mid(firstSpace + 1).trimmed();
    }
}

void FFmpegBuildTool::loadFFmpegOptions()
{
    auto config = getOptions();
    ui->ffmpeg_complie_options_tb->setupConfigs(QStringList{"Option", "Description", "Default Value"}, config);
}
