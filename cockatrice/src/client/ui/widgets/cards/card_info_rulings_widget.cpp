#include "card_info_rulings_widget.h"

#include "../../../../game/cards/card_info.h"

#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <algorithm>

CardInfoRulingsWidget::CardInfoRulingsWidget(QWidget *parent) : QFrame(parent), info(nullptr)
{
    rulingsText = new QTextEdit();
    rulingsText->setReadOnly(true);
    rulingsText->setFrameStyle(QFrame::NoFrame);

    noRulingsLabel = new QLabel();
    noRulingsLabel->setAlignment(Qt::AlignCenter);
    noRulingsLabel->setWordWrap(true);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(rulingsText);
    layout->addWidget(noRulingsLabel);

    retranslateUi();
}

void CardInfoRulingsWidget::setCard(CardInfoPtr card)
{
    info = card;
    updateDisplay();
}

void CardInfoRulingsWidget::updateDisplay()
{
    if (!info) {
        rulingsText->clear();
        rulingsText->hide();
        noRulingsLabel->show();
        return;
    }

    const QList<CardRuling> &rulings = info->getRulings();

    if (rulings.isEmpty()) {
        rulingsText->clear();
        rulingsText->hide();
        noRulingsLabel->show();
        return;
    }

    noRulingsLabel->hide();
    rulingsText->show();

    // Sort rulings by date (newest first)
    QList<CardRuling> sortedRulings = rulings;
    std::sort(sortedRulings.begin(), sortedRulings.end(),
              [](const CardRuling &a, const CardRuling &b) { return a.date > b.date; });

    QString html = "<html><body style=\"margin: 8px;\">";

    for (const CardRuling &ruling : sortedRulings) {
        QString dateStr = ruling.date.isEmpty() ? tr("(No date)") : ruling.date;
        html += QString("<p><i>%1</i><br>%2</p>").arg(dateStr.toHtmlEscaped(), ruling.text.toHtmlEscaped());
    }

    html += "</body></html>";
    rulingsText->setHtml(html);
}

void CardInfoRulingsWidget::retranslateUi()
{
    noRulingsLabel->setText(tr("No rulings available for this card."));
    updateDisplay();
}
