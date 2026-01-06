/**
 * @file trigger_info.h
 * @brief Data structure for parsed trigger information.
 */

#ifndef TRIGGER_INFO_H
#define TRIGGER_INFO_H

#include "trigger_type.h"

#include <QSharedPointer>
#include <QString>

class CardItem;

struct TriggerInfo
{
    QString cardName;
    QString triggerText;
    TriggerPhase triggerPhase;
    bool isEminence;
    bool isEachPlayer;    // Trigger affects each player ("a player", "each player")
    bool isOpponentOnly;  // Trigger affects only opponents ("an opponent", "each opponent")
    int cardId;

    TriggerInfo()
        : triggerPhase(TriggerPhase::Unknown), isEminence(false), isEachPlayer(false), isOpponentOnly(false),
          cardId(-1)
    {
    }

    TriggerInfo(const QString &_cardName,
                const QString &_triggerText,
                TriggerPhase _phase,
                bool _isEminence = false,
                bool _isEachPlayer = false,
                bool _isOpponentOnly = false,
                int _cardId = -1)
        : cardName(_cardName), triggerText(_triggerText), triggerPhase(_phase), isEminence(_isEminence),
          isEachPlayer(_isEachPlayer), isOpponentOnly(_isOpponentOnly), cardId(_cardId)
    {
    }
};

typedef QSharedPointer<TriggerInfo> TriggerInfoPtr;

#endif // TRIGGER_INFO_H
