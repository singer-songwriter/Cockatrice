/**
 * @file trigger_type.h
 * @brief Defines trigger phase types for the trigger reminder system.
 */

#ifndef TRIGGER_TYPE_H
#define TRIGGER_TYPE_H

#include <QString>

enum class TriggerPhase {
    // Phase-based triggers (0-10)
    Untap = 0,
    Upkeep = 1,
    Draw = 2,
    Main1 = 3,
    CombatStart = 4,
    Attackers = 5,
    Blockers = 6,
    Damage = 7,
    CombatEnd = 8,
    Main2 = 9,
    EndStep = 10,

    // Event-based triggers (100+)
    EntersBattlefield = 100,
    LeavesBattlefield = 101,
    Dies = 102,
    SpellCast = 103,
    DrawCard = 104,      // Whenever a player draws (Smothering Tithe, Nekusar)
    Discard = 105,       // Whenever a player discards (Waste Not, Megrim)
    GainLife = 106,      // Whenever you/a player gains life (Soul Warden triggers)
    LoseLife = 107,      // Whenever damage dealt / life lost
    Sacrifice = 108,     // Whenever a permanent is sacrificed (Mayhem Devil)
    TapUntap = 109,      // Whenever becomes tapped/untapped
    Landfall = 110,      // Whenever a land enters (Lotus Cobra)
    TargetedBy = 111,    // Whenever becomes the target of (Heroic)
    CounterPlaced = 112, // Whenever a counter is placed/removed
    TokenCreated = 113,  // Whenever you create a token
    Mana = 114,          // Whenever you tap for mana
    Other = 199,
    Unknown = -1
};

namespace TriggerTypeUtils {

inline bool isPhaseBasedTrigger(TriggerPhase phase)
{
    int p = static_cast<int>(phase);
    return p >= 0 && p <= 10;
}

inline int toPhaseIndex(TriggerPhase phase)
{
    if (isPhaseBasedTrigger(phase)) {
        return static_cast<int>(phase);
    }
    return -1;
}

inline TriggerPhase fromPhaseIndex(int index)
{
    if (index >= 0 && index <= 10) {
        return static_cast<TriggerPhase>(index);
    }
    return TriggerPhase::Unknown;
}

inline QString getDisplayName(TriggerPhase phase)
{
    switch (phase) {
        case TriggerPhase::Untap:
            return QStringLiteral("Untap");
        case TriggerPhase::Upkeep:
            return QStringLiteral("Upkeep");
        case TriggerPhase::Draw:
            return QStringLiteral("Draw Step");
        case TriggerPhase::Main1:
            return QStringLiteral("First Main");
        case TriggerPhase::CombatStart:
            return QStringLiteral("Combat - Beginning");
        case TriggerPhase::Attackers:
            return QStringLiteral("Combat - Attackers");
        case TriggerPhase::Blockers:
            return QStringLiteral("Combat - Blockers");
        case TriggerPhase::Damage:
            return QStringLiteral("Combat - Damage");
        case TriggerPhase::CombatEnd:
            return QStringLiteral("Combat - End");
        case TriggerPhase::Main2:
            return QStringLiteral("Second Main");
        case TriggerPhase::EndStep:
            return QStringLiteral("End Step");
        case TriggerPhase::EntersBattlefield:
            return QStringLiteral("Enters Battlefield");
        case TriggerPhase::LeavesBattlefield:
            return QStringLiteral("Leaves Battlefield");
        case TriggerPhase::Dies:
            return QStringLiteral("Dies");
        case TriggerPhase::SpellCast:
            return QStringLiteral("Spell Cast");
        case TriggerPhase::DrawCard:
            return QStringLiteral("Card Drawn");
        case TriggerPhase::Discard:
            return QStringLiteral("Discard");
        case TriggerPhase::GainLife:
            return QStringLiteral("Life Gained");
        case TriggerPhase::LoseLife:
            return QStringLiteral("Life Lost / Damage");
        case TriggerPhase::Sacrifice:
            return QStringLiteral("Sacrifice");
        case TriggerPhase::TapUntap:
            return QStringLiteral("Tap/Untap");
        case TriggerPhase::Landfall:
            return QStringLiteral("Landfall");
        case TriggerPhase::TargetedBy:
            return QStringLiteral("Targeted");
        case TriggerPhase::CounterPlaced:
            return QStringLiteral("Counters");
        case TriggerPhase::TokenCreated:
            return QStringLiteral("Token Created");
        case TriggerPhase::Mana:
            return QStringLiteral("Mana");
        case TriggerPhase::Other:
            return QStringLiteral("Other");
        default:
            return QStringLiteral("Unknown");
    }
}

} // namespace TriggerTypeUtils

#endif // TRIGGER_TYPE_H
