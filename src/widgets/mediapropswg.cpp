// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "mediapropswg.h"
#include "ui_mediapropswg.h"
#include <QJsonParseError>
#include <QFileInfo>
#include <QDebug>
#include <QMetaObject>
#include <QtConcurrent>
#include <QShortcut>
#include <QKeyEvent>
#include <QCoreApplication>

MediaPropsWG::MediaPropsWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MediaPropsWG)
    , m_formatWidget(nullptr)
    , m_streamsWidget(nullptr)
{
    ui->setupUi(this);
    setupUI();
    
    // Connect signals
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MediaPropsWG::onTabChanged);
}

MediaPropsWG::~MediaPropsWG()
{
    delete ui;
}

void MediaPropsWG::setupUI()
{
    // Create JsonFormatWG for Format and Streams tabs
    m_formatWidget = new JsonFormatWG(this);
    m_streamsWidget = new JsonFormatWG(this);
    
    // Add widgets to tabs
    ui->formatLayout->addWidget(m_formatWidget);
    ui->streamsLayout->addWidget(m_streamsWidget);
}

void MediaPropsWG::setMediaFile(const QString &fileName)
{
    if (m_mediaFile != fileName) {
        m_mediaFile = fileName;
        loadMediaInfoAsync();
    }
}

void MediaPropsWG::loadMediaInfo()
{
    if (m_mediaFile.isEmpty()) {
        qWarning() << "No media file specified";
        return;
    }
    
    QFileInfo fileInfo(m_mediaFile);
    if (!fileInfo.exists()) {
        qWarning() << "Media file does not exist:" << m_mediaFile;
        return;
    }
    
    // Load format and streams info
    loadFormatInfo();
    loadStreamsInfo();
}

void MediaPropsWG::loadMediaInfoAsync()
{
    if (m_mediaFile.isEmpty()) {
        qWarning() << "No media file specified";
        return;
    }
    
    QFileInfo fileInfo(m_mediaFile);
    if (!fileInfo.exists()) {
        qWarning() << "Media file does not exist:" << m_mediaFile;
        return;
    }
    
    // Create and show progress dialog
    ProgressDialog *progressDlg = new ProgressDialog(this);
    progressDlg->setWindowTitle(tr("Loading Media Properties"));
    progressDlg->setProgressMode(ProgressDialog::Indeterminate);
    progressDlg->setMessage(tr("Loading media properties..."));
    progressDlg->setAutoClose(false);
    progressDlg->setCancelButtonVisible(false);
    progressDlg->show();
    
    // Run the loading operation in a separate thread
    QtConcurrent::run([=](){
        // Get format info
        QString formatInfo = m_probe.getMediaInfoJsonFormat(SHOW_FORMAT, m_mediaFile);
        
        // Get streams info
        QString streamsInfo = m_probe.getMediaInfoJsonFormat(SHOW_STREAMS, m_mediaFile);
        
        // Update both widgets in a single main thread invocation
        QMetaObject::invokeMethod(this, [this, formatInfo, streamsInfo, progressDlg]() {
            // Update format widget
            if (m_formatWidget) {
                m_formatWidget->loadData(formatInfo.toUtf8());
            }
            
            // Update streams widget
            if (m_streamsWidget) {
                m_streamsWidget->loadData(streamsInfo.toUtf8());
            }
            
            // Finish progress dialog
            progressDlg->finish();
            // Clean up progress dialog immediately
            progressDlg->deleteLater();
        }, Qt::QueuedConnection);
    });
}

void MediaPropsWG::loadFormatInfo()
{
    QString formatData = m_probe.getMediaInfoJsonFormat(SHOW_FORMAT, m_mediaFile);
    if (m_formatWidget) {
        m_formatWidget->loadData(formatData.toUtf8());
    }
}

void MediaPropsWG::loadStreamsInfo()
{
    QString streamsData = m_probe.getMediaInfoJsonFormat(SHOW_STREAMS, m_mediaFile);
    if (m_streamsWidget) {
        m_streamsWidget->loadData(streamsData.toUtf8());
    }
}


void MediaPropsWG::onTabChanged(int index)
{
    Q_UNUSED(index);
    // Could be used for lazy loading of tabs
}
