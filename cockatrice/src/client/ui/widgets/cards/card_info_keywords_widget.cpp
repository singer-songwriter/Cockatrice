#include "card_info_keywords_widget.h"

#include "../../../../game/keywords/keyword_data.h"

#include <QLabel>
#include <QPalette>
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

    // Get palette colors for theme compatibility
    QPalette pal = palette();
    QString textColor = pal.color(QPalette::Text).name();
    QString dimTextColor = pal.color(QPalette::PlaceholderText).name();
    QString borderColor = pal.color(QPalette::Mid).name();
    QString linkColor = pal.color(QPalette::Link).name();

    QString html = QString("<html><head><style>"
                           "body { margin: 8px; }"
                           ".keyword-block { margin-bottom: 16px; padding-bottom: 12px; border-bottom: 1px solid %1; }"
                           ".keyword-block:last-child { border-bottom: none; margin-bottom: 0; }"
                           ".keyword-name { font-size: 14px; font-weight: bold; color: %2; margin-bottom: 4px; }"
                           ".description { font-size: 12px; color: %2; margin-bottom: 8px; }"
                           ".example { font-size: 12px; color: %2; margin-bottom: 6px; padding-left: 8px; "
                           "border-left: 3px solid %4; }"
                           ".tip { font-size: 12px; color: %3; margin-bottom: 6px; padding-left: 8px; "
                           "border-left: 3px solid %3; font-style: italic; }"
                           ".section-label { font-weight: bold; }"
                           "</style></head><body>")
                       .arg(borderColor, textColor, dimTextColor, linkColor);

    for (const KeywordDefinition *kw : keywords) {
        html += "<div class=\"keyword-block\">";

        // Keyword name
        html += QString("<div class=\"keyword-name\">%1</div>").arg(kw->name.toHtmlEscaped());

        // Description
        html += QString("<div class=\"description\">%1</div>").arg(kw->description.toHtmlEscaped());

        // Example (if available)
        if (!kw->example.isEmpty()) {
            html += QString("<div class=\"example\"><span class=\"section-label\">Example:</span> %1</div>")
                        .arg(kw->example.toHtmlEscaped());
        }

        // Tip (if available)
        if (!kw->tip.isEmpty()) {
            html +=
                QString("<div class=\"tip\"><span class=\"section-label\">Tip:</span> %1</div>").arg(kw->tip.toHtmlEscaped());
        }

        html += "</div>";
    }

    html += "</body></html>";
    keywordsText->setHtml(html);
}

void CardInfoKeywordsWidget::retranslateUi()
{
    noKeywordsLabel->setText(tr("No keywords found on this card."));
    updateDisplay();
}
