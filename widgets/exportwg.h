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

#include <common/zffprobe.h>
#include <common/flowlayout.h>
#include <common/commandexecutor.h>

#include <widgets/progressdlg.h>

namespace Ui {
class ExportWG;
}

typedef enum {
    BasicInfo = 1,
    MediaInfo = 2,
    BaseAndMediaInfo = 3
} ExportModelType;

class ExportWG : public QWidget
{
    Q_OBJECT

public:
    explicit ExportWG(QWidget *parent = nullptr);
    ~ExportWG();

    // set export model
    void setExportModel(ExportModelType type);

    // Interface for managing search range checkboxes
    void setMediaInfoExportFiledsOptions(const QStringList &options);
    void clearMediaInfoExportFiledsOptions();
    QStringList getMediaInfoSelectedExportFileds() const;
    void setMediaInfoSelectedExportFileds(const QStringList &selectedOptions);

    void setBasicInfoExportFiledsOptions(const QStringList &options);
    void clearBasicInfoExportFiledsOptions();
    QStringList getBasicInfoSelectedExportFileds() const;
    void setBasicInfoSelectedExportFileds(const QStringList &selectedOptions);

    // Input File Name
    void setInputMediaFilePath(const QString& fileName);

signals:
    void exportMediaInfoFiledsSelectionChanged(const QStringList &selectedOptions);
    void exportBasicInfoFiledsSelectionChanged(const QStringList &selectedOptions);
private slots:
    void onMediaInfoSelectAllClicked(bool checked);
    void onMediaInfoSelectNoneClicked();
    void onMediaInfoSearchRangeRBtnToggled();

    void onBasicInfoSelectAllClicked(bool checked);
    void onBasicInfoSelectNoneClicked();
    void onBasicInfoSearchRangeRBtnToggled();

    void on_save_filename_le_textChanged(const QString &arg1);

    void on_export_btn_clicked();
    
    // Context menu slots
    void onSelectSaveDirectory();
    void onOpenSaveDirectory();
    void showExportButtonContextMenu(const QPoint &pos);

private:
    Ui::ExportWG *ui;

    FlowLayout *m_mediaInfoFloatLayout = nullptr;
    FlowLayout *m_basicInfoFloatLayout = nullptr;
    CommandExecutor *m_executor = nullptr;

    // Export Media Info Fileds controls
    QRadioButton *m_selectAllMediaInfoRBtn;
    QRadioButton *m_selectNoneMediaInfoRBtn;
    QList<QCheckBox*> m_exportMediaInfoFiledsCBoxes;

    // Export Media Info Fileds controls
    QRadioButton *m_selectAllBasicInfoRBtn;
    QRadioButton *m_selectNoneBasicInfoRBtn;
    QList<QCheckBox*> m_exportBasicInfoFiledsCBoxes;

    ExportModelType m_exportModel = BasicInfo;
    QString m_save_name;
    QString m_save_dir;
    QString m_input_fileName;
};

#endif // EXPORTWG_H
