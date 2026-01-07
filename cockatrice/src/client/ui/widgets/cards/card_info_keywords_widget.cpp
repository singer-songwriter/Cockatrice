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
    // Use Text color for all content to ensure readability on both light and dark themes
    QPalette pal = palette();
    QString textColor = pal.color(QPalette::Text).name();
    QString borderColor = pal.color(QPalette::Mid).name();
    QString linkColor = pal.color(QPalette::Link).name();

    QString html = QString("<html><head><style>"
                           "body { margin: 8px; color: %1; }"
                           ".keyword-name { font-size: 14px; font-weight: bold; color: %1; margin-bottom: 6px; }"
                           ".info-line { font-size: 12px; color: %1; margin-bottom: 6px; padding-left: 8px; "
                           "border-left: 3px solid %3; }"
                           ".tip-line { font-size: 12px; color: %1; margin-bottom: 6px; padding-left: 8px; "
                           "border-left: 3px solid %3; font-style: italic; }"
                           ".section-label { font-weight: bold; }"
                           ".divider { border: none; border-top: 1px solid %2; margin: 12px 0; }"
                           "</style></head><body>")
                       .arg(textColor, borderColor, linkColor);

    for (int i = 0; i < keywords.size(); ++i) {
        const KeywordDefinition *kw = keywords.at(i);

        // Keyword name
        html += QString("<div class=\"keyword-name\">%1</div>").arg(kw->name.toHtmlEscaped());

        // Description
        html += QString("<div class=\"info-line\"><span class=\"section-label\">Description:</span> %1</div>")
                    .arg(kw->description.toHtmlEscaped());

        // Example (if available)
        if (!kw->example.isEmpty()) {
            html += QString("<div class=\"info-line\"><span class=\"section-label\">Example:</span> %1</div>")
                        .arg(kw->example.toHtmlEscaped());
        }

        // Tip (if available)
        if (!kw->tip.isEmpty()) {
            html += QString("<div class=\"tip-line\"><span class=\"section-label\">Tip:</span> %1</div>")
                        .arg(kw->tip.toHtmlEscaped());
        }

        // Add divider between keywords (but not after the last one)
        if (i < keywords.size() - 1) {
            html += "<hr class=\"divider\">";
        }
    }

    html += "</body></html>";
    keywordsText->setHtml(html);
}

void CardInfoKeywordsWidget::retranslateUi()
{
    noKeywordsLabel->setText(tr("No keywords found on this card."));
    updateDisplay();
}
