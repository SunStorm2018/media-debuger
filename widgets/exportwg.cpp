#include "exportwg.h"
#include "ui_exportwg.h"
#include <QDir>

ExportWG::ExportWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExportWG)
{
    ui->setupUi(this);

    QDateTime currentTime = QDateTime::currentDateTime();
    // Initialize the flow layout for search range
    m_floatLayout = new FlowLayout();
    ui->export_fileds_gbox->setLayout(m_floatLayout);

    m_save_name = currentTime.toString("yyyy-MM-dd_HH-mm-ss");

    // Create select all and select none checkboxes
    m_selectAllRadioBtn = new QRadioButton(tr("Select All"), this);
    m_selectNoneRadioBtn = new QRadioButton(tr("Select None"), this);

    // Style the control checkboxes differently
    m_selectAllRadioBtn->setObjectName("selectAllCheckBox");
    m_selectNoneRadioBtn->setObjectName("selectNoneCheckBox");

    // Add them to the layout first
    m_floatLayout->addWidget(m_selectAllRadioBtn);
    m_floatLayout->addWidget(m_selectNoneRadioBtn);

    // Connect signals
    connect(m_selectAllRadioBtn, &QCheckBox::toggled, this, &ExportWG::onSelectAllClicked);
    connect(m_selectNoneRadioBtn, &QCheckBox::clicked, this, &ExportWG::onSelectNoneClicked);

    m_executor = new CommandExecutor(this);
    
    // Setup context menu for export button
    ui->export_btn->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->export_btn, &QPushButton::customContextMenuRequested,
            this, &ExportWG::showExportButtonContextMenu);
}

ExportWG::~ExportWG()
{
    delete ui;
}

void ExportWG::setExportFiledsOptions(const QStringList &options)
{
    // Clear existing checkboxes (except select all/none)
    clearExportFiledsOptions();

    // Create new checkboxes for each option
    for (const QString &option : options) {
        auto *checkBox = new QCheckBox(option, this);
        checkBox->setChecked(true);
        m_exportFiledsCheckBoxes.append(checkBox);
        m_floatLayout->addWidget(checkBox);

        // Connect to the toggle signal
        connect(checkBox, &QCheckBox::toggled, this, &ExportWG::onSearchRangeCheckboxToggled);
    }

    // Update the layout
    m_floatLayout->update();
}

void ExportWG::clearExportFiledsOptions()
{
    // Remove and delete all search range checkboxes
    for (auto *checkBox : m_exportFiledsCheckBoxes) {
        m_floatLayout->removeWidget(checkBox);
        checkBox->deleteLater();
    }
    m_exportFiledsCheckBoxes.clear();
}

QStringList ExportWG::getSelectedExportFileds() const
{
    QStringList selectedOptions;
    for (const auto *checkBox : m_exportFiledsCheckBoxes) {
        if (checkBox->isChecked()) {
            selectedOptions.append(checkBox->text());
        }
    }
    return selectedOptions;
}

void ExportWG::setSelectedExportFileds(const QStringList &selectedOptions)
{
    for (auto *checkBox : m_exportFiledsCheckBoxes) {
        checkBox->setChecked(selectedOptions.contains(checkBox->text()));
    }

    // Update select all checkbox state
    bool allSelected = !m_exportFiledsCheckBoxes.isEmpty() &&
                       m_exportFiledsCheckBoxes.size() == getSelectedExportFileds().size();
    m_selectAllRadioBtn->setChecked(allSelected);

    emit exportFiledsSelectionChanged(getSelectedExportFileds());
}

void ExportWG::setInputMediaFilePath(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    if (fileInfo.exists()) {
        m_input_fileName = fileName;
        m_save_name = fileInfo.fileName() + "_media_info";
        m_save_dir = fileInfo.absolutePath();
        ui->save_filename_le->setText(m_save_name);
        ui->save_dir_le->setText(fileInfo.absolutePath());
    } else {
        qWarning() << fileName << tr("not exists!");
    }
}

void ExportWG::onSelectAllClicked(bool checked)
{
    // Block signals temporarily to avoid recursive calls
    const bool blocked = blockSignals(true);

    for (auto *checkBox : m_exportFiledsCheckBoxes) {
        checkBox->setChecked(checked);
    }

    blockSignals(blocked);
    emit exportFiledsSelectionChanged(getSelectedExportFileds());
}

void ExportWG::onSelectNoneClicked()
{
    // Block signals temporarily to avoid recursive calls
    const bool blocked = blockSignals(true);

    for (auto *checkBox : m_exportFiledsCheckBoxes) {
        checkBox->setChecked(false);
    }

    blockSignals(blocked);
    emit exportFiledsSelectionChanged(getSelectedExportFileds());
}

void ExportWG::onSearchRangeCheckboxToggled()
{
    // Update select all/none states based on current selection
    const QStringList selected = getSelectedExportFileds();
    const bool allSelected = !m_exportFiledsCheckBoxes.isEmpty() &&
                             selected.size() == m_exportFiledsCheckBoxes.size();
    const bool noneSelected = selected.isEmpty();

    // Block signals to avoid recursive calls
    const bool blocked = blockSignals(true);

    if (!allSelected && !allSelected) {
        m_selectAllRadioBtn->setAutoExclusive(false);
        m_selectNoneRadioBtn->setAutoExclusive(false);

        m_selectAllRadioBtn->setChecked(false);
        m_selectNoneRadioBtn->setChecked(false);
    } else {
        m_selectAllRadioBtn->setAutoExclusive(true);
        m_selectNoneRadioBtn->setAutoExclusive(true);
        m_selectAllRadioBtn->setChecked(allSelected);
        m_selectNoneRadioBtn->setChecked(noneSelected);
    }
    blockSignals(blocked);

    emit exportFiledsSelectionChanged(selected);
}

void ExportWG::on_save_filename_le_textChanged(const QString &arg1)
{
    m_save_name = arg1;
}


void ExportWG::on_export_btn_clicked()
{
    QStringList cmds;

    // 1. default
    if (ui->default_cbox->isChecked()) {
        QString deault_Cmd = QString("ffprobe -loglevel quiet %1 -i %2 -of default=nk=%3:nw=%4 > %5")
        .arg(getSelectedExportFileds().join(" "))
            .arg(m_input_fileName)
            .arg(ui->default_nokey_cbox->isChecked() ? "1" : "0")
            .arg(ui->default_noprint_wrappers_cbox->isChecked() ? "1" : "0")
            .arg(QDir(m_save_dir).filePath(m_save_name + "_default_" + ui->default_suffix_le->text().trimmed()));

        cmds << deault_Cmd;
    }

    // 2. json
    if (ui->json_cbox->isChecked()) {
        QString json_Cmd = QString("ffprobe -loglevel quiet %1 -i %2 -of json=c=%3 > %4")
        .arg(getSelectedExportFileds().join(" "))
            .arg(m_input_fileName)
            .arg(ui->json_compact_cbox->isChecked() ? "1" : "0")
            .arg(QDir(m_save_dir).filePath(m_save_name + "_json_" + ui->json_suffix_le->text().trimmed()));

        cmds << json_Cmd;
    }

    // 3. ini
    if (ui->ini_cbox->isChecked()) {
        QString ini_Cmd = QString("ffprobe -loglevel quiet %1 -i %2 -of ini=h=%3 > %4")
        .arg(getSelectedExportFileds().join(" "))
            .arg(m_input_fileName)
            .arg(ui->ini_hierarchical_cbox->isChecked() ? "1" : "0")
            .arg(QDir(m_save_dir).filePath(m_save_name + "_ini_"  + ui->ini_suffix_le->text().trimmed()));

        cmds << ini_Cmd;
    }

    // 4. xml
    if (ui->xml_cbox->isChecked()) {
        QString xml_Cmd = QString("ffprobe -loglevel quiet %1 -i %2 -of xml=q=%3:x=%4 > %5")
        .arg(getSelectedExportFileds().join(" "))
            .arg(m_input_fileName)
            .arg(ui->xml_fully_qualified_cbox->isChecked() ? "1" : "0")
            .arg(ui->xml_xsd_strict_cbox->isChecked() ? "1" : "0")
            .arg(QDir(m_save_dir).filePath(m_save_name  + "_xml_" + ui->xml_suffix_le->text().trimmed()));

        cmds << xml_Cmd;
    }

    // 5. flat
    if (ui->flat_cbox->isChecked()) {
        QString flat_Cmd = QString("ffprobe -loglevel quiet %1 -i %2 -of flat=s=%3:h=%4 > %5")
            .arg(getSelectedExportFileds().join(" "))
            .arg(m_input_fileName)
            .arg(ui->flat_sep_char_le->text().trimmed())
            .arg(ui->flat_hierarchical_cbox->isChecked() ? "1" : "0")
            .arg(QDir(m_save_dir).filePath(m_save_name + "_flat_"  + ui->flat_suffix_le->text().trimmed()));

        cmds << flat_Cmd;
    }

    // 6. compact, csv
    if (ui->compact_cbox->isChecked()) {
        QString compact_Cmd = QString("ffprobe -loglevel quiet %1 -i %2 -of compact=s=%3:nk=%4:e=%5:p=%6 > %7")
        .arg(getSelectedExportFileds().join(" "))
            .arg(m_input_fileName)
            .arg(ui->compact_item_sep_le->text().trimmed())
            .arg(ui->compact_nokey_cbox->isChecked() ? "1" : "0")
            .arg(ui->compact_escape_combox->currentText().trimmed())
            .arg(ui->compact_print_section_cbox->isChecked() ? "1" : "0")
            .arg(QDir(m_save_dir).filePath(m_save_name + "_csv_"  + ui->compact_suffix_le->text().trimmed()));

        cmds << compact_Cmd;
    }

    // log out
    for (auto it : cmds) {
        qDebug() << "export cmd: " << it;
    }

    ProgressDialog *progressDlg = new ProgressDialog;
    progressDlg->setWindowTitle(tr("Export Files"));
    progressDlg->setProgressMode(ProgressDialog::Determinate);
    progressDlg->setMessage("Export...");
    progressDlg->setAutoClose(true);

    progressDlg->start();
    connect(m_executor, &CommandExecutor::progressUpdated,
            [=](int completed, int total, const QString &message){
                progressDlg->setRange(1, total);
                progressDlg->setValue(completed);
                progressDlg->setMessage(message);
            });

    connect(m_executor, &CommandExecutor::commandStarted,
            [=](const QString &command, int index){
                progressDlg->setMessage(tr("Started: %1 %2").arg(index).arg(command));
            });

    connect(m_executor, &CommandExecutor::commandFinished,
            [=](const QString &command, int index, int exitCode, QProcess::ExitStatus exitStatus){
                QString status = (exitCode == 0 && exitStatus == QProcess::NormalExit) ? "Success" : "Failed";
                qDebug() << index << command << status;
            });

    connect(m_executor, &CommandExecutor::allCommandsFinished,
            [=](bool success){
                if (success) {
                    qDebug() << tr("All commands completed successfully");
                } else {
                    qDebug() << tr("Some commands failed or were stopped");
                }

                if (success) {
                    progressDlg->setMessage("Finsh parse");
                    progressDlg->finish();
                    progressDlg->deleteLater();
                }
            });

    connect(m_executor, &CommandExecutor::commandOutput,
            [=](const QString &command, const QString &output, int index)
            {
                qDebug() << tr("Output from index %1 %2: %3").arg(index).arg(command).arg(output.trimmed());
            });

    connect(m_executor, &CommandExecutor::commandError,
            [=](const QString &command, const QString &error, int index)
            {
                qDebug() << tr("Error from index %1 %2: %3").arg(index).arg(command).arg(error.trimmed());
            });

    m_executor->executeCommands(cmds);
    progressDlg->exec();
}

void ExportWG::onSelectSaveDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, 
        tr("Select Save Directory"), 
        m_save_dir.isEmpty() ? QDir::homePath() : m_save_dir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    
    if (!dir.isEmpty()) {
        m_save_dir = dir;
        ui->save_dir_le->setText(dir);
    }
}

void ExportWG::onOpenSaveDirectory()
{
    QString dir = m_save_dir.isEmpty() ? QDir::homePath() : m_save_dir;
    QDir checkDir(dir);
    
    if (!checkDir.exists()) {
        // If directory doesn't exist, try to create it
        if (!checkDir.mkpath(dir)) {
            qWarning() << tr("Cannot create directory: %1").arg(dir);
            return;
        }
    }
    
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

void ExportWG::showExportButtonContextMenu(const QPoint &pos)
{
    QMenu *contextMenu = new QMenu(this);
    contextMenu->setAttribute(Qt::WA_DeleteOnClose);
    
    QAction *selectDirAction = contextMenu->addAction(tr("Select Save Directory"));
    QAction *openDirAction = contextMenu->addAction(tr("Open Save Directory"));
    
    connect(selectDirAction, &QAction::triggered, this, &ExportWG::onSelectSaveDirectory);
    connect(openDirAction, &QAction::triggered, this, &ExportWG::onOpenSaveDirectory);
    
    contextMenu->exec(ui->export_btn->mapToGlobal(pos));
}

