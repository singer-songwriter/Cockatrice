/**
 * @file trigger_manager.cpp
 * @brief Implementation of trigger management and caching.
 */

#include "trigger_manager.h"

#include "../cards/card_item.h"
#include "../player/player.h"
#include "../zones/card_zone.h"
#include "../zones/pile_zone.h"
#include "../zones/table_zone.h"

TriggerManager::TriggerManager(QObject *parent) : QObject(parent), localPlayer(nullptr)
{
}

void TriggerManager::setLocalPlayer(Player *player)
{
    localPlayer = player;
    refreshTriggers();
}

void TriggerManager::clearCache()
{
    cardTriggerCache.clear();
    triggersByPhase.clear();
}

void TriggerManager::refreshTriggers()
{
    clearCache();

    if (!localPlayer) {
        emit triggersChanged();
        return;
    }

    // Scan battlefield (table zone)
    TableZone *tableZone = qobject_cast<TableZone *>(localPlayer->getZones().value("table"));
    if (tableZone) {
        scanZone(tableZone, false);
    }

    // Scan sideboard for eminence triggers
    PileZone *sideboardZone = qobject_cast<PileZone *>(localPlayer->getZones().value("sb"));
    if (sideboardZone) {
        scanZone(sideboardZone, true);
    }

    rebuildTriggerGroups();
    emit triggersChanged();
}

void TriggerManager::scanZone(CardZone *zone, bool isSideboard)
{
    if (!zone) {
        return;
    }

    const CardList &cards = zone->getCards();
    for (CardItem *card : cards) {
        if (!card) {
            continue;
        }

        QList<TriggerInfoPtr> triggers = parser.parseCardTriggers(card, isSideboard);
        if (!triggers.isEmpty()) {
            cardTriggerCache.insert(card->getId(), triggers);
        }
    }
}

void TriggerManager::rebuildTriggerGroups()
{
    triggersByPhase.clear();

    for (auto it = cardTriggerCache.constBegin(); it != cardTriggerCache.constEnd(); ++it) {
        const QList<TriggerInfoPtr> &triggers = it.value();
        for (const TriggerInfoPtr &trigger : triggers) {
            triggersByPhase[trigger->triggerPhase].append(trigger);
        }
    }
}

void TriggerManager::onCardAdded(CardItem *card)
{
    if (!card || !localPlayer) {
        return;
    }

    // Determine if this is a sideboard card
    CardZone *zone = card->getZone();
    bool isSideboard = zone && zone->getName() == "sb";

    // Parse triggers for this card
    QList<TriggerInfoPtr> triggers = parser.parseCardTriggers(card, isSideboard);
    if (!triggers.isEmpty()) {
        cardTriggerCache.insert(card->getId(), triggers);
        rebuildTriggerGroups();
        emit triggersChanged();
    }
}

void TriggerManager::onCardRemoved(CardItem *card)
{
    if (!card) {
        return;
    }

    int cardId = card->getId();
    if (cardTriggerCache.contains(cardId)) {
        cardTriggerCache.remove(cardId);
        rebuildTriggerGroups();
        emit triggersChanged();
    }
}

QMap<TriggerPhase, QList<TriggerInfoPtr>> TriggerManager::getTriggersByPhase() const
{
    return triggersByPhase;
}

QList<TriggerInfoPtr> TriggerManager::getTriggersForPhase(int phaseIndex) const
{
    TriggerPhase phase = TriggerTypeUtils::fromPhaseIndex(phaseIndex);
    return triggersByPhase.value(phase);
}

QList<TriggerInfoPtr> TriggerManager::getAllTriggers() const
{
    QList<TriggerInfoPtr> allTriggers;
    for (auto it = triggersByPhase.constBegin(); it != triggersByPhase.constEnd(); ++it) {
        allTriggers.append(it.value());
    }
    return allTriggers;
}
