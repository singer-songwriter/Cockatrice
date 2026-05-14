#include "trigger_widget.h"

#include "../../game/board/card_item.h"
#include "../../game/player/player.h"
#include "../../game/zones/logic/card_zone_logic.h"

#include <QLabel>
#include <QListWidget>
#include <QRegularExpression>
#include <QVBoxLayout>

TriggerWidget::TriggerWidget(QWidget *parent) : QWidget(parent)
{
    auto *label = new QLabel(tr("Triggers reminder"));
    label->setAlignment(Qt::AlignCenter);

    triggerList = new QListWidget;
    triggerList->setMouseTracking(true);

    connect(triggerList, &QListWidget::itemEntered, this, [this](QListWidgetItem *item) {
        const QString cardName = item->data(Qt::UserRole).toString();
        if (!cardName.isEmpty()) {
            emit cardNameHovered(cardName);
        }
    });
    connect(triggerList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        const QString cardName = item->data(Qt::UserRole).toString();
        if (!cardName.isEmpty()) {
            emit cardNameHovered(cardName);
        }
    });

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);
    layout->addWidget(triggerList);

    setLayout(layout);
}

void TriggerWidget::retranslateUi()
{
    // window title is set by TabGame; trigger text comes from card data
}

void TriggerWidget::setLocalPlayer(Player *player)
{
    localPlayer = player;

    auto *zone = player->getTableZone();
    connect(zone, &CardZoneLogic::cardAdded, this, [this](CardItem *) { updateTriggers(); });
    connect(zone, &CardZoneLogic::cardCountChanged, this, &TriggerWidget::updateTriggers);

    updateTriggers();
}

void TriggerWidget::updateTriggers()
{
    triggerList->clear();

    if (!localPlayer) {
        return;
    }

    auto *zone = localPlayer->getTableZone();
    const CardList &cards = zone->getCards();

    // Map from category name to list of card names in that category
    QMap<QString, QStringList> categoryCards;

    for (CardItem *card : cards) {
        if (card->getFaceDown()) {
            continue;
        }

        const QString &text = card->getCardInfo().getText();
        const QStringList lines = text.split(QLatin1Char('\n'));

        for (const QString &line : lines) {
            QString category = categorizeTrigger(line);
            if (!category.isEmpty()) {
                if (!categoryCards[category].contains(card->getName())) {
                    categoryCards[category].append(card->getName());
                }
            }
        }
    }

    if (categoryCards.isEmpty()) {
        triggerList->addItem(tr("No triggers on battlefield"));
        return;
    }

    for (auto it = categoryCards.constBegin(); it != categoryCards.constEnd(); ++it) {
        // Add category header (bold via font)
        auto *headerItem = new QListWidgetItem(it.key());
        QFont headerFont = headerItem->font();
        headerFont.setBold(true);
        headerItem->setFont(headerFont);
        headerItem->setForeground(Qt::red);
        headerItem->setFlags(Qt::NoItemFlags);
        triggerList->addItem(headerItem);

        // Add card names indented under the category
        for (const QString &cardName : it.value()) {
            auto *cardItem = new QListWidgetItem(QStringLiteral("  %1").arg(cardName));
            cardItem->setData(Qt::UserRole, cardName);
            triggerList->addItem(cardItem);
        }
    }
}

QString TriggerWidget::categorizeTrigger(const QString &line)
{
    static const QRegularExpression triggerPattern(QStringLiteral("^(When|Whenever|At)\\b"),
                                                   QRegularExpression::CaseInsensitiveOption);

    if (!triggerPattern.match(line).hasMatch()) {
        return {};
    }

    // ETB / LTB
    static const QRegularExpression etbPattern(QStringLiteral("enters? the battlefield|\\benters?\\b"),
                                               QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression ltbPattern(QStringLiteral("leaves? the battlefield|\\bleaves?\\b"),
                                               QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression diesPattern(QStringLiteral("\\bdies?\\b"),
                                                QRegularExpression::CaseInsensitiveOption);

    // Combat
    static const QRegularExpression attackPattern(QStringLiteral("\\battacks?\\b"),
                                                  QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression blockPattern(QStringLiteral("\\bblocks?\\b|\\bblocked\\b"),
                                                 QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression combatDamagePattern(QStringLiteral("deals? combat damage|dealt combat damage"),
                                                        QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression damagePattern(QStringLiteral("deals? damage|dealt damage|is dealt damage"),
                                                  QRegularExpression::CaseInsensitiveOption);

    // Phase-based
    static const QRegularExpression upkeepPattern(QStringLiteral("beginning of (?:your |each (?:player's |opponent's )?)?upkeep"),
                                                  QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression endStepPattern(QStringLiteral("beginning of (?:your |the (?:next )?|each (?:player's )?)?end step"),
                                                   QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression drawPattern(QStringLiteral("beginning of (?:your |each (?:player's )?)?draw step"),
                                                QRegularExpression::CaseInsensitiveOption);

    // Spells
    static const QRegularExpression castPattern(QStringLiteral("\\bcasts? a spell\\b|\\bcasts? an?\\b"),
                                                QRegularExpression::CaseInsensitiveOption);

    if (etbPattern.match(line).hasMatch())
        return tr("Enters the Battlefield");
    if (ltbPattern.match(line).hasMatch())
        return tr("Leaves the Battlefield");
    if (diesPattern.match(line).hasMatch())
        return tr("Dies");
    if (combatDamagePattern.match(line).hasMatch())
        return tr("Combat Damage");
    if (damagePattern.match(line).hasMatch())
        return tr("Deals Damage");
    if (attackPattern.match(line).hasMatch())
        return tr("Attacks");
    if (blockPattern.match(line).hasMatch())
        return tr("Blocks");
    if (upkeepPattern.match(line).hasMatch())
        return tr("Upkeep");
    if (endStepPattern.match(line).hasMatch())
        return tr("End Step");
    if (drawPattern.match(line).hasMatch())
        return tr("Draw Step");
    if (castPattern.match(line).hasMatch())
        return tr("Cast");

    return tr("Other");
}

bool TriggerWidget::hasTrigger(const QString &oracleText)
{
    static const QRegularExpression triggerPattern(QStringLiteral("^(When|Whenever|At)\\b"),
                                                   QRegularExpression::CaseInsensitiveOption |
                                                       QRegularExpression::MultilineOption);
    return triggerPattern.match(oracleText).hasMatch();
}
