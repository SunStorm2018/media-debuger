#include "cmdinfowg.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QApplication>

CmdInfoWG::CmdInfoWG(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnections();
    
    // Initialize info type map
    m_infoTypeMap["version"] = "版本信息";
    m_infoTypeMap["buildconf"] = "构建配置";
    m_infoTypeMap["formats"] = "支持的格式";
    m_infoTypeMap["muxers"] = "复用器";
    m_infoTypeMap["demuxers"] = "解复用器";
    m_infoTypeMap["devices"] = "设备信息";
    m_infoTypeMap["codecs"] = "编解码器";
    m_infoTypeMap["decoders"] = "解码器";
    m_infoTypeMap["encoders"] = "编码器";
    m_infoTypeMap["bsfs"] = "比特流过滤器";
    m_infoTypeMap["protocols"] = "协议支持";
    m_infoTypeMap["filters"] = "滤镜";
    m_infoTypeMap["pixfmts"] = "像素格式";
    m_infoTypeMap["layouts"] = "声道布局";
    m_infoTypeMap["samplefmts"] = "采样格式";
    m_infoTypeMap["colors"] = "颜色名称";
    m_infoTypeMap["license"] = "许可证信息";
    
    // Populate combo box
    for (auto it = m_infoTypeMap.constBegin(); it != m_infoTypeMap.constEnd(); ++it) {
        m_infoTypeCombo->addItem(it.value(), it.key());
    }
    
    // Load default info (version)
    displayBasicInfo("version");
}

CmdInfoWG::~CmdInfoWG()
{
}

void CmdInfoWG::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Title label
    m_titleLabel = new QLabel("基础信息查看器", this);
    m_titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    
    // Control panel
    QWidget *controlPanel = new QWidget(this);
    QHBoxLayout *controlLayout = new QHBoxLayout(controlPanel);
    
    m_infoTypeCombo = new QComboBox(this);
    m_infoTypeCombo->setMinimumWidth(150);
    
    m_refreshBtn = new QPushButton("刷新", this);
    m_saveBtn = new QPushButton("保存", this);
    
    controlLayout->addWidget(new QLabel("信息类型:", this));
    controlLayout->addWidget(m_infoTypeCombo);
    controlLayout->addStretch();
    controlLayout->addWidget(m_refreshBtn);
    controlLayout->addWidget(m_saveBtn);
    
    // Info display
    m_infoDisplay = new QTextEdit(this);
    m_infoDisplay->setReadOnly(true);
    m_infoDisplay->setFont(QFont("Consolas", 9));
    
    // Add to main layout
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(m_infoDisplay);
    
    // Set layout margins
    mainLayout->setContentsMargins(10, 10, 10, 10);
}

void CmdInfoWG::setupConnections()
{
    connect(m_infoTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CmdInfoWG::onInfoTypeChanged);
    connect(m_refreshBtn, &QPushButton::clicked,
            this, &CmdInfoWG::onRefreshClicked);
    connect(m_saveBtn, &QPushButton::clicked,
            this, &CmdInfoWG::onSaveClicked);
}

void CmdInfoWG::displayBasicInfo(const QString &infoType)
{
    QString result;
    QString displayName = getInfoTypeName(infoType);
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    if (infoType == "version") {
        result = m_ffprobe.getVersion();
    } else if (infoType == "buildconf") {
        result = m_ffprobe.getBuildconf();
    } else if (infoType == "formats") {
        result = m_ffprobe.getFormats();
    } else if (infoType == "muxers") {
        result = m_ffprobe.getMuxers();
    } else if (infoType == "demuxers") {
        result = m_ffprobe.getDemuxers();
    } else if (infoType == "devices") {
        result = m_ffprobe.getDevices();
    } else if (infoType == "codecs") {
        result = m_ffprobe.getCodecs();
    } else if (infoType == "decoders") {
        result = m_ffprobe.getDecoders();
    } else if (infoType == "encoders") {
        result = m_ffprobe.getEncoders();
    } else if (infoType == "bsfs") {
        result = m_ffprobe.getBsfs();
    } else if (infoType == "protocols") {
        result = m_ffprobe.getProtocols();
    } else if (infoType == "filters") {
        result = m_ffprobe.getFilters();
    } else if (infoType == "pixfmts") {
        result = m_ffprobe.getPixfmts();
    } else if (infoType == "layouts") {
        result = m_ffprobe.getLayouts();
    } else if (infoType == "samplefmts") {
        result = m_ffprobe.getSamplefmts();
    } else if (infoType == "colors") {
        result = m_ffprobe.getColors();
    } else if (infoType == "license") {
        result = m_ffprobe.getL();
    } else {
        result = "不支持的信息类型: " + infoType;
    }
    
    QApplication::restoreOverrideCursor();
    
    // Update display
    m_titleLabel->setText(QString("基础信息查看器 - %1").arg(displayName));
    m_infoDisplay->setText(result);
}

void CmdInfoWG::displayMediaInfo(const QString &filePath)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    // Get media info in JSON format
    QString mediaInfo = m_ffprobe.getMediaInfoJsonFormat(SHOW_FORMAT, filePath);
    
    QApplication::restoreOverrideCursor();
    
    if (mediaInfo.isEmpty()) {
        m_infoDisplay->setText("无法获取媒体文件信息，请检查文件路径是否正确。");
        return;
    }
    
    // Update display
    m_titleLabel->setText(QString("媒体文件信息 - %1").arg(QFileInfo(filePath).fileName()));
    m_infoDisplay->setText(mediaInfo);
}

void CmdInfoWG::onInfoTypeChanged(int index)
{
    Q_UNUSED(index);
    QString infoType = m_infoTypeCombo->currentData().toString();
    displayBasicInfo(infoType);
}

void CmdInfoWG::onRefreshClicked()
{
    QString infoType = m_infoTypeCombo->currentData().toString();
    displayBasicInfo(infoType);
}

void CmdInfoWG::onSaveClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                   "保存信息",
                                                   QString("%1_%2.txt")
                                                   .arg(m_infoTypeCombo->currentText())
                                                   .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
                                                   "文本文件 (*.txt);;所有文件 (*)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << m_titleLabel->text() << "\n\n";
        out << m_infoDisplay->toPlainText();
        file.close();
        
        QMessageBox::information(this, "保存成功", "信息已成功保存到文件:\n" + fileName);
    } else {
        QMessageBox::warning(this, "保存失败", "无法保存文件:\n" + fileName);
    }
}

QString CmdInfoWG::getInfoTypeName(const QString &type)
{
    return m_infoTypeMap.value(type, type);
}
