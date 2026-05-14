#include "board_state_copy_widget.h"

#include "../abstract_game.h"
#include "../board/abstract_counter.h"
#include "../board/card_item.h"
#include "../game_state.h"
#include "../phases_toolbar.h"
#include "../player/player.h"
#include "../player/player_info.h"
#include "../player/player_manager.h"
#include "../zones/logic/card_zone_logic.h"

#include <QApplication>
#include <QClipboard>
#include <QPushButton>
#include <QTextStream>
#include <QVBoxLayout>

BoardStateCopyWidget::BoardStateCopyWidget(AbstractGame *_game,
                                           PhasesToolbar *_phasesToolbar,
                                           QWidget *parent)
    : QWidget(parent), game(_game), phasesToolbar(_phasesToolbar)
{
    copyButton = new QPushButton(tr("Copy Board State"), this);
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(copyButton);
    layout->addStretch();
    setLayout(layout);
    connect(copyButton, &QPushButton::clicked, this, &BoardStateCopyWidget::actCopyBoardState);
}

QString BoardStateCopyWidget::serializeCard(CardItem *card)
{
    QString line = card->getName();

    const QString cardType = card->getCardInfo().getMainCardType();
    if (!cardType.isEmpty())
        line += " (" + cardType + ")";

    // Only show tapped state for opponents — the AI assumes your own cards are untapped
    Player *owner = card->getOwner();
    if (card->getTapped() && (!owner || !owner->getPlayerInfo()->getLocal()))
        line += " [tapped]";
    if (card->getAttacking())
        line += " [attacking]";

    const QString pt = card->getPT();
    if (!pt.isEmpty())
        line += " [P/T: " + pt + "]";

    int totalCounters = 0;
    for (int v : card->getCounters())
        totalCounters += v;
    if (totalCounters > 0)
        line += " [" + QString::number(totalCounters) + " counter" + (totalCounters == 1 ? "" : "s") + "]";

    const QString annotation = card->getAnnotation();
    if (!annotation.isEmpty())
        line += " [note: \"" + annotation + "\"]";

    return line;
}

QString BoardStateCopyWidget::serializeZone(const QString &label, CardZoneLogic *zone, bool showNames)
{
    if (!zone)
        return label + ":\n  (zone unavailable)\n";

    const int count = zone->getCards().size();
    const QString cardWord = count == 1 ? tr("card") : tr("cards");
    QString out = label + " (" + QString::number(count) + " " + cardWord + "):\n";

    if (count == 0) {
        out += "  (empty)\n";
    } else if (showNames) {
        for (CardItem *card : zone->getCards()) {
            const QString cardType = card->getCardInfo().getMainCardType();
            out += "  " + card->getName();
            if (!cardType.isEmpty())
                out += " (" + cardType + ")";
            out += "\n";
        }
    } else {
        out += "  " + QString::number(count) + " " + cardWord + " (hidden)\n";
    }

    return out;
}

void BoardStateCopyWidget::actCopyBoardState()
{
    QString buffer;
    QTextStream out(&buffer);

    out << tr("You are a Magic: The Gathering EDH/Commander assistant. Here is the current game state:") << "\n\n";

    const int phase = game->getGameState()->getCurrentPhase();
    const QString phaseName = (phase >= 0) ? phasesToolbar->getLongPhaseName(phase) : tr("Unknown");
    out << "=== " << tr("Phase") << ": " << phaseName << " ===\n";

    const int activeId = game->getGameState()->getActivePlayer();
    Player *activePl = game->getPlayerManager()->getPlayer(activeId);
    const QString activeName = activePl ? activePl->getPlayerInfo()->getName() : tr("Unknown");
    out << tr("Active Player") << ": " << activeName << "\n\n";

    // Per-player sections
    for (Player *pl : game->getPlayerManager()->getPlayers()) {
        // Life total
        QString lifeStr = "?";
        for (AbstractCounter *counter : pl->getCounters()) {
            if (counter->getName() == "life") {
                lifeStr = QString::number(counter->getValue());
                break;
            }
        }

        // Player header
        const QString name = pl->getPlayerInfo()->getName();
        out << "--- " << tr("Player") << ": " << name << " (" << tr("Life") << ": " << lifeStr << ")";
        if (pl->getPlayerInfo()->getLocal())
            out << " [YOU]";
        if (pl->getConceded())
            out << " (conceded)";
        out << " ---\n";

        // Hand
        CardZoneLogic *hand = pl->getHandZone();
        if (hand) {
            const int handCount = hand->getCards().size();
            if (hand->contentsKnown()) {
                QStringList names;
                for (CardItem *card : hand->getCards()) {
                    if (!card->getFaceDown())
                        names << card->getName();
                }
                out << tr("Hand") << " (" << handCount << " " << (handCount == 1 ? tr("card") : tr("cards")) << "): "
                    << (names.isEmpty() ? tr("(empty)") : names.join(", ")) << "\n";
            } else {
                out << tr("Hand") << ": " << handCount << " " << (handCount == 1 ? tr("card") : tr("cards"))
                    << " (hidden)\n";
            }
        }

        // Battlefield
        CardZoneLogic *table = pl->getTableZone();
        if (table) {
            QList<CardItem *> visibleCards;
            for (CardItem *card : table->getCards()) {
                if (!card->getFaceDown())
                    visibleCards << card;
            }
            const int count = visibleCards.size();
            out << "\n" << tr("Battlefield") << " (" << count << " "
                << (count == 1 ? tr("permanent") : tr("permanents")) << "):\n";
            if (visibleCards.isEmpty()) {
                out << "  (empty)\n";
            } else {
                for (CardItem *card : visibleCards)
                    out << "  " << serializeCard(card) << "\n";
            }
        }

        // Graveyard
        out << "\n" << serializeZone(tr("Graveyard"), pl->getGraveZone(),
                                     pl->getGraveZone() && pl->getGraveZone()->contentsKnown());

        // Exile
        out << serializeZone(tr("Exile"), pl->getRfgZone(),
                             pl->getRfgZone() && pl->getRfgZone()->contentsKnown());

        // Command Zone (stack zone in Cockatrice represents the command zone in Commander)
        out << serializeZone(tr("Command Zone"), pl->getStackZone(),
                             pl->getStackZone() && pl->getStackZone()->contentsKnown());

        out << "\n";
    }

    QApplication::clipboard()->setText(buffer, QClipboard::Clipboard);
    QApplication::clipboard()->setText(buffer, QClipboard::Selection);
}
