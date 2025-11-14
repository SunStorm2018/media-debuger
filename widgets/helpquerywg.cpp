// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "helpquerywg.h"
#include "ui_helpquerywg.h"

HelpQueryWg::HelpQueryWg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HelpQueryWg)
{
    ui->setupUi(this);
    ui->category_combx->addItems(HELP_OPTION_FORMATS);

    m_highLighter = new ZTextHighlighter(ui->search_output_ple);

    m_searchWG = new SearchWG(this);
    m_searchWG->setWindowTitle(tr("Detail Search"));

    // Configure to show only the required group boxes for InfoWidgets
    auto requiredBoxes = SearchWG::MatchControl | SearchWG::Operation;
    m_searchWG->setVisibleGroupBoxes(requiredBoxes);

    ui->verticalLayout->addWidget(m_searchWG);
    m_searchWG->setVisible(false);

    QShortcut *ctrl_f_shortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(ctrl_f_shortcut, &QShortcut::activated, this, [this]() {
        m_searchWG->setVisible(!m_searchWG->isVisible());
    });

    QShortcut *ctrl_d_shortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
    connect(ctrl_d_shortcut, &QShortcut::activated, this, [this]() {
        ui->control_header_wg->setVisible(!ui->control_header_wg->isVisible());
    });

    connect(m_searchWG, &SearchWG::searchReady, this, &HelpQueryWg::on_searchReady);
    connect(m_searchWG, &SearchWG::searchClear, this, [this]() {
        m_highLighter->clearHighlight();
        m_searchWG->setSearchText("");
        m_searchWG->setSearchStatus("");
    });
    connect(m_searchWG, &SearchWG::searchBefore, m_highLighter, &ZTextHighlighter::gotoPreviousHighlight);
    connect(m_searchWG, &SearchWG::searchNext, m_highLighter, &ZTextHighlighter::gotoNextHighlight);

    connect(m_highLighter, &ZTextHighlighter::highlightCountChanged, [=](int count) {
        m_searchWG->setSearchStatus(QString("Found %1 results").arg(count));
    });
    connect(m_highLighter, &ZTextHighlighter::currentHighlightChanged, [=](int index) {
        if (index >= 0) {
            m_searchWG->setSearchStatus(QString("Result %1 of %2").arg(index + 1).arg(m_highLighter->highlightCount()));
        }
    });
    connect(m_highLighter, &ZTextHighlighter::searchTextNotFound, [=](const QString &searchText) {
        m_searchWG->setSearchStatus(QString("Text '%1' not found").arg(searchText));
    });

    emit ui->category_combx->activated(0);

}

HelpQueryWg::~HelpQueryWg()
{
    delete ui;
}

bool HelpQueryWg::setHelpParams(const QString &category, const QString &value)
{
    m_highLighter->clearHighlight();

    ui->search_output_ple->clear();

    QStringList helpList {
        QString("%1%2")
            .arg(category).arg(value.isEmpty() ? "" : "=" + value)
    };

    QString helpText = m_probe.getHelp(helpList);
    if (helpText.isEmpty()) {
        helpText = tr("No help information available for %1=%2").arg(category, value);
    }
    
    ui->search_output_ple->setPlainText(helpText);
        
    return !helpText.isEmpty();
}

void HelpQueryWg::setControlHeaderVisiable(const bool &visiable)
{
    ui->control_header_wg->setVisible(visiable);
}

void HelpQueryWg::on_category_combx_activated(int index)
{
    QString currentCategory = ui->category_combx->currentText();
    
    // Show/hide parameter dropdown based on category
    bool showParamBox = !QStringList{"full", "long"}.contains(currentCategory);
    ui->param_combox->setVisible(showParamBox);

    ui->param_combox->clear();
    
    // Query directly if no parameters needed
    if (!showParamBox) {
        setHelpParams(currentCategory, "");
        return;
    }

    // Load corresponding parameters by category
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
        // Auto-select first item
        if (items.size() > 0) {
            setHelpParams(currentCategory, items.first());
        }
    } else {
        // Show error when loading fails
        ui->search_output_ple->setPlainText(tr("Failed to load %1 parameters").arg(currentCategory));
    }
}

void HelpQueryWg::on_searchReady()
{
    QString searchText = m_searchWG->getSearchText().trimmed();
    if (searchText.isEmpty()) {
        m_searchWG->setSearchStatus(tr("Search text is empty"));
        return;
    }

    // Check if there's content to search
    if (ui->search_output_ple->toPlainText().isEmpty()) {
        m_searchWG->setSearchStatus(tr("No content to search"));
        return;
    }

    m_highLighter->setCaseSensitive(m_searchWG->isCaseSensitive());
    m_highLighter->setWholeWord(m_searchWG->isMatchWholewords());
    m_highLighter->setUseRegex(m_searchWG->isUseRegularExpression());

    m_highLighter->highlight(searchText);
}

void HelpQueryWg::on_param_combox_activated(int index)
{
    setHelpParams(ui->category_combx->currentText(), ui->param_combox->currentText());
}

void HelpQueryWg::on_keep_last_cbx_stateChanged(int state)
{
    // Clear content when unchecking keep_last_cbx
    if (state == Qt::Unchecked) {
        m_highLighter->clearHighlight();
        ui->search_output_ple->clear();
    }
}
