// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef TABELFMTWG_H
#define TABELFMTWG_H

#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

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

private:
    Ui::TabelFormatWG *ui;

    InfoWidgets *m_tableFormatWg;

    QList<QStringList> m_data_tb;
    QList<QString> m_headers;

    QAction *m_previewImageAction;

    QString valueToString(const QJsonValue &value);
    QString extractSideData(const QJsonObject &frameObj, const QString &key);

private slots:
    void previewImage();

protected:
    bool loadJson(const QByteArray &json);
};

#endif // TABELFMTWG_H
