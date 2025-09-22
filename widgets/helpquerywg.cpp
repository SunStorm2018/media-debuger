#include "helpquerywg.h"
#include "ui_helpquerywg.h"

HelpQueryWg::HelpQueryWg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HelpQueryWg)
{
    ui->setupUi(this);
    ui->category_combx->addItems(HELP_OPTION_FORMATS);

    m_highLighter = new ZTextHighlighter(ui->search_output_ple);

    m_detailSearchWg = new SearchWG(this);
    m_detailSearchWg->setWindowTitle(tr("Detail Search"));

    // Configure to show only the required group boxes for InfoWidgets
    auto requiredBoxes = SearchWG::MatchControl | SearchWG::Operation;
    m_detailSearchWg->setVisibleGroupBoxes(requiredBoxes);

    ui->verticalLayout->addWidget(m_detailSearchWg);
    m_detailSearchWg->setVisible(false);

    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(shortcut, &QShortcut::activated, this, [this]() {
        m_detailSearchWg->setVisible(!m_detailSearchWg->isVisible());
    });

    connect(m_detailSearchWg, &SearchWG::searchReady, this, &HelpQueryWg::on_search);
    connect(m_detailSearchWg, &SearchWG::searchClear, this, [this]() {
        m_highLighter->clearHighlight();
        m_detailSearchWg->setSearchText("");
        m_detailSearchWg->setSearchStatus("");
    });
    connect(m_detailSearchWg, &SearchWG::searchBefore, m_highLighter, &ZTextHighlighter::gotoPreviousHighlight);
    connect(m_detailSearchWg, &SearchWG::searchNext, m_highLighter, &ZTextHighlighter::gotoNextHighlight);

    connect(m_highLighter, &ZTextHighlighter::highlightCountChanged, [=](int count) {
        m_detailSearchWg->setSearchStatus(QString("Found %1 results").arg(count));
    });
    connect(m_highLighter, &ZTextHighlighter::currentHighlightChanged, [=](int index) {
        if (index >= 0) {
            m_detailSearchWg->setSearchStatus(QString("Result %1 of %2").arg(index + 1).arg(m_highLighter->highlightCount()));
        }
    });
    connect(m_highLighter, &ZTextHighlighter::searchTextNotFound, [=](const QString &searchText) {
        m_detailSearchWg->setSearchStatus(QString("Text '%1' not found").arg(searchText));
    });

    emit ui->category_combx->activated(0);
}

HelpQueryWg::~HelpQueryWg()
{
    delete ui;
}

bool HelpQueryWg::setHelpParams(const QString &category, const QString &value)
{
    // 清除高亮状态
    m_highLighter->clearHighlight();
    
    // 清空文本框
    ui->search_output_ple->clear();

    QStringList helpList {
        QString("%1%2%3")
        .arg(category)
            .arg("=")
            .arg(value)
    };

    QString helpText = m_probe.getHelp(helpList);
    if (helpText.isEmpty()) {
        helpText = tr("No help information available for %1=%2").arg(category, value);
    }
    
    ui->search_output_ple->setPlainText(helpText);
    
    // 不再自动重新应用高亮，避免切换内容时背景变黄的问题
    // 用户需要手动重新搜索才会应用高亮
    
    return !helpText.isEmpty();
}

void HelpQueryWg::on_category_combx_activated(int index)
{
    QString currentCategory = ui->category_combx->currentText();
    
    // 根据类别决定是否显示参数下拉框
    bool showParamBox = !QStringList{"full", "long"}.contains(currentCategory);
    ui->param_combox->setVisible(showParamBox);

    ui->param_combox->clear();
    
    // 如果不需要参数，直接查询
    if (!showParamBox) {
        setHelpParams(currentCategory, "");
        return;
    }

    // 根据不同类别加载对应的参数列表
    QStringList items;
    bool success = false;
    
    if (currentCategory == DECODER_FMT) {
        items = m_probe.getCodecsFromLibav(CODEC_TYPE_DECODER);
        success = !items.isEmpty();
    } else if (currentCategory == ENCODER_FMT) {
        items = m_probe.getCodecsFromLibav(CODEC_TYPE_ENCODER);
        success = !items.isEmpty();
    } else if (currentCategory == DEMUXER_FMT) {
        items = m_probe.getMuxersFromLibav(MUXER_TYPE_DEMUXER);
        success = !items.isEmpty();
    } else if (currentCategory == MUXER_FMT) {
        items = m_probe.getMuxersFromLibav(MUXER_TYPE_MUXER);
        success = !items.isEmpty();
    } else if (currentCategory == FILTER_FMT) {
        items = m_probe.getFiltersFromLibav();
        success = !items.isEmpty();
    } else if (currentCategory == BSF_FMT) {
        items = m_probe.getBsfFromLibav();
        success = !items.isEmpty();
    } else if (currentCategory == PROTOCOL_FMT) {
        items = m_probe.getProtocolFromLibav();
        success = !items.isEmpty();
    }

    if (success && !items.isEmpty()) {
        ui->param_combox->addItems(items);
        // 自动选择第一项
        if (items.size() > 0) {
            setHelpParams(currentCategory, items.first());
        }
    } else {
        // 加载失败时显示错误信息
        ui->search_output_ple->setPlainText(tr("Failed to load %1 parameters").arg(currentCategory));
    }
}

void HelpQueryWg::on_search()
{
    QString searchText = m_detailSearchWg->getSearchText().trimmed();
    if (searchText.isEmpty()) {
        m_detailSearchWg->setSearchStatus(tr("Search text is empty"));
        return;
    }

    // 检查是否有内容可以搜索
    if (ui->search_output_ple->toPlainText().isEmpty()) {
        m_detailSearchWg->setSearchStatus(tr("No content to search"));
        return;
    }

    m_highLighter->setCaseSensitive(m_detailSearchWg->isCaseSensitive());
    m_highLighter->setWholeWord(m_detailSearchWg->isMatchWholewords());
    m_highLighter->setUseRegex(m_detailSearchWg->isUseRegularExpression());

    m_highLighter->highlight(searchText);
}

void HelpQueryWg::on_param_combox_activated(int index)
{
    setHelpParams(ui->category_combx->currentText(), ui->param_combox->currentText());
}

void HelpQueryWg::on_keep_last_cbx_stateChanged(int state)
{
    // 当keep_last_cbx状态改变时，如果取消勾选，清除当前内容
    if (state == Qt::Unchecked) {
        m_highLighter->clearHighlight();
        ui->search_output_ple->clear();
    }
}
