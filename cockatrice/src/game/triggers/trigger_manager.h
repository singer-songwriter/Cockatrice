/**
 * @file trigger_manager.h
 * @brief Manages trigger detection and caching for the trigger reminder panel.
 */

#ifndef TRIGGER_MANAGER_H
#define TRIGGER_MANAGER_H

#include "trigger_info.h"
#include "trigger_parser.h"

#include <QList>
#include <QMap>
#include <QObject>

class Player;
class CardItem;
class CardZone;

class TriggerManager : public QObject
{
    Q_OBJECT

public:
    explicit TriggerManager(QObject *parent = nullptr);

    void setLocalPlayer(Player *player);
    Player *getLocalPlayer() const
    {
        return localPlayer;
    }

    QMap<TriggerPhase, QList<TriggerInfoPtr>> getTriggersByPhase() const;
    QList<TriggerInfoPtr> getTriggersForPhase(int phaseIndex) const;
    QList<TriggerInfoPtr> getAllTriggers() const;

signals:
    void triggersChanged();

public slots:
    void onCardAdded(CardItem *card);
    void onCardRemoved(CardItem *card);
    void refreshTriggers();

private:
    TriggerParser parser;
    Player *localPlayer;

    // Cache: cardId -> list of triggers
    QMap<int, QList<TriggerInfoPtr>> cardTriggerCache;

    // Grouped triggers by phase
    QMap<TriggerPhase, QList<TriggerInfoPtr>> triggersByPhase;

    void scanZone(CardZone *zone, bool isSideboard = false);
    void rebuildTriggerGroups();
    void clearCache();
};

#endif // TRIGGER_MANAGER_H
