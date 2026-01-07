#include "card_info_keywords_widget.h"

#include "../../../../game/keywords/keyword_data.h"

#include <QLabel>
#include <QScrollArea>
#include <QTextEdit>
#include <QVBoxLayout>

CardInfoKeywordsWidget::CardInfoKeywordsWidget(QWidget *parent) : QFrame(parent), info(nullptr)
{
    keywordsText = new QTextEdit();
    keywordsText->setReadOnly(true);
    keywordsText->setFrameStyle(QFrame::NoFrame);

    noKeywordsLabel = new QLabel();
    noKeywordsLabel->setAlignment(Qt::AlignCenter);
    noKeywordsLabel->setWordWrap(true);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(keywordsText);
    layout->addWidget(noKeywordsLabel);

    retranslateUi();
}

void CardInfoKeywordsWidget::setCard(CardInfoPtr card)
{
    info = card;
    updateDisplay();
}

void CardInfoKeywordsWidget::updateDisplay()
{
    if (!info) {
        keywordsText->clear();
        keywordsText->hide();
        noKeywordsLabel->show();
        return;
    }

    QString oracleText = info->getText();
    QList<const KeywordDefinition *> keywords = KeywordData::instance().getKeywordsForCard(oracleText);

    if (keywords.isEmpty()) {
        keywordsText->clear();
        keywordsText->hide();
        noKeywordsLabel->show();
        return;
    }

    noKeywordsLabel->hide();
    keywordsText->show();

    QString html = "<html><body style=\"margin: 8px;\">";

    for (const KeywordDefinition *kw : keywords) {
        html += QString("<p><b>%1</b><br>%2</p>")
                    .arg(kw->name.toHtmlEscaped(), kw->description.toHtmlEscaped());
    }

    html += "</body></html>";
    keywordsText->setHtml(html);
}

void CardInfoKeywordsWidget::retranslateUi()
{
    noKeywordsLabel->setText(tr("No keywords found on this card."));
    updateDisplay();
}
