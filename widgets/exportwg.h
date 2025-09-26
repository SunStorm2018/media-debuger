#ifndef EXPORTWG_H
#define EXPORTWG_H

#include <QWidget>
#include <QRadioButton>
#include <QCheckBox>
#include <QDateTime>
#include <QFileInfo>
#include <QDebug>
#include <QTimer>
#include <QMenu>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>

#include <common/flowlayout.h>
#include <common/commandexecutor.h>

#include <widgets/progressdlg.h>

namespace Ui {
class ExportWG;
}

class ExportWG : public QWidget
{
    Q_OBJECT

public:
    explicit ExportWG(QWidget *parent = nullptr);
    ~ExportWG();

    // Interface for managing search range checkboxes
    void setExportFiledsOptions(const QStringList &options);
    void clearExportFiledsOptions();
    QStringList getSelectedExportFileds() const;
    void setSelectedExportFileds(const QStringList &selectedOptions);

    // Input File Name
    void setInputMediaFilePath(const QString& fileName);

signals:
    void exportFiledsSelectionChanged(const QStringList &selectedOptions);

private slots:
    void onSelectAllClicked(bool checked);
    void onSelectNoneClicked();
    void onSearchRangeCheckboxToggled();

    void on_save_filename_le_textChanged(const QString &arg1);

    void on_export_btn_clicked();
    
    // Context menu slots
    void onSelectSaveDirectory();
    void onOpenSaveDirectory();
    void showExportButtonContextMenu(const QPoint &pos);

private:
    Ui::ExportWG *ui;

    FlowLayout *m_floatLayout = nullptr;
    CommandExecutor *m_executor = nullptr;

    // Export Fileds controls
    QRadioButton *m_selectAllRadioBtn;
    QRadioButton *m_selectNoneRadioBtn;
    QList<QCheckBox*> m_exportFiledsCheckBoxes;

    QString m_save_name;
    QString m_save_dir;
    QString m_input_fileName;
};

#endif // EXPORTWG_H
