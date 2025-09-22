#include "helpquerywg.h"
#include "ui_helpquerywg.h"

HelpQueryWg::HelpQueryWg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HelpQueryWg)
{
    ui->setupUi(this);
    ui->category_combx->addItems(HELP_OPTION_FORMATS);
    emit ui->category_combx->activated(0);
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
    connect(m_detailSearchWg, &SearchWG::searchClear, m_highLighter, &ZTextHighlighter::clearHighlight);
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
}

HelpQueryWg::~HelpQueryWg()
{
    delete ui;
}

bool HelpQueryWg::setHelpParams(const QString &category, const QString &value)
{
    ui->search_output_ple->clear();

    QStringList helpList {
        QString("%1%2%3")
        .arg(category)
            .arg("=")
            .arg(value)
    };

    ui->search_output_ple->setPlainText(m_probe.getHelp(helpList));
}

void HelpQueryWg::on_search_btn_clicked()
{
    if (!ui->keep_last_cbx->isChecked()) {
        ui->search_output_ple->clear();
    }

    QStringList helpList {
        QString("%1%2%3")
        .arg(ui->category_combx->currentText())
            .arg("=")
            .arg(ui->param_combox->currentText())
    };

    ui->search_output_ple->setPlainText(m_probe.getHelp(helpList));
}


void HelpQueryWg::on_category_combx_activated(int index)
{
    if (QStringList{"full", "long"}.contains( ui->category_combx->currentText())) {
        ui->param_combox->setVisible(false);
    } else {
        ui->param_combox->setVisible(true);
    }

    ui->param_combox->clear();
    if (ui->category_combx->currentText() == DECODER_FMT) {
        ui->param_combox->addItems(m_probe.getCodecsFromLibav(CODEC_TYPE_DECODER));
        return;
    }
    if (ui->category_combx->currentText() == ENCODER_FMT) {
        ui->param_combox->addItems(m_probe.getCodecsFromLibav(CODEC_TYPE_ENCODER));
        return;
    }
    if (ui->category_combx->currentText() == DEMUXER_FMT) {
        ui->param_combox->addItems(m_probe.getMuxersFromLibav(MUXER_TYPE_DEMUXER));
        return;
    }
    if (ui->category_combx->currentText() == MUXER_FMT) {
        ui->param_combox->addItems(m_probe.getMuxersFromLibav(MUXER_TYPE_MUXER));
        return;
    }
    if (ui->category_combx->currentText() == FILTER_FMT) {
        ui->param_combox->addItems(m_probe.getFiltersFromLibav());
        return;
    }
    if (ui->category_combx->currentText() == BSF_FMT) {
        ui->param_combox->addItems(m_probe.getBsfFromLibav());
        return;
    }
    if (ui->category_combx->currentText() == PROTOCOL_FMT) {
        ui->param_combox->addItems(m_probe.getProtocolFromLibav());
        return;
    }
}


void HelpQueryWg::on_search_input_le_editingFinished()
{
    emit ui->search_btn->clicked();
}

void HelpQueryWg::on_search()
{
    QString searchText = ui->search_input_le->text().trimmed();
    if (searchText.isEmpty()) {
        return;
    }

    m_highLighter->setCaseSensitive(m_detailSearchWg->isCaseSensitive());
    m_highLighter->setWholeWord(m_detailSearchWg->isMatchWholewords());
    m_highLighter->setUseRegex(m_detailSearchWg->isUseRegularExpression());

    m_highLighter->highlight(searchText);
}

