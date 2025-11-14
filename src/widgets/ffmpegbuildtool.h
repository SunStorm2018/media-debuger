// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef FFMPEGBUILDTOOL_H
#define FFMPEGBUILDTOOL_H

#include <QWidget>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QProcess>
#include <QDateTime>
#include <QTextCursor>


#include <common/qtcompat.h>

namespace Ui {
class FFmpegBuildTool;
}

class FFmpegBuildTool : public QWidget
{
    Q_OBJECT

public:
    explicit FFmpegBuildTool(QWidget *parent = nullptr);
    ~FFmpegBuildTool();

    QMap<QString, QList<QStringList> > getOptions();

    QString getConfigCmd();
private slots:
    void on_show_cmd_cbx_toggled(bool checked);

    void on_select_local_path_btn_clicked();

    void on_build_btn_clicked();

    void on_configure_btn_clicked();

    void on_install_btn_clicked();

    void on_clean_btn_clicked();

private:
    void loadSettings();
    void saveSettings();
    void parseOptionLineFast(const QString &line, QString &option, QString &description, QString &defaultValue);

    void loadFFmpegOptions();
private:
    Ui::FFmpegBuildTool *ui;
    QAction *m_addRecordAction;
    QAction *m_RemoveRecordAction;

    QStringList m_filePaths;
    QProcess *m_process = nullptr;

    QMap<QString, QStringList> m_optionMaps;
};

#endif // FFMPEGBUILDTOOL_H
