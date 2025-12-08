// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef TABELFMTWG_H
#define TABELFMTWG_H

#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QSet>

#include <widgets/infotablewg.h>
#include <widgets/basefmtwg.h>

#include <model/mediainfotabelmodel.h>

namespace Ui {
class TabelFormatWG;
}

class TabelFormatWG : public BaseFormatWG
{
    Q_OBJECT

public:
    explicit TabelFormatWG(QWidget *parent = nullptr);
    ~TabelFormatWG();

public slots:
    void enableImageContextMenu(const bool &enable);

private:
    Ui::TabelFormatWG *ui;

    InfoWidgets *m_tableFormatWg = nullptr;

    QList<QStringList> m_data_tb;
    QList<QString> m_headers;

    // image menu
    QMenu *m_imageMenu = nullptr;
    QAction *m_previewImageAction = nullptr;
    QAction *m_saveImageAction = nullptr;
    
    // Enable image context menu flag
    bool m_enableImageContextMenu = true;

    QString valueToString(const QJsonValue &value);
    QString extractSideData(const QJsonObject &frameObj, const QString &key);
    
    QStringList getSelectedMediaTypes();
    
    void updateImageMenuVisibility();

private slots:
    void previewImage();
    void saveImage();
    void onContextMenuAboutToShow();

protected:
    bool loadJson(const QByteArray &json);
};

#endif // TABELFMTWG_H
