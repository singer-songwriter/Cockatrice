/**
 * @file trigger_type.h
 * @brief Defines trigger phase types for the trigger reminder system.
 */

#ifndef TRIGGER_TYPE_H
#define TRIGGER_TYPE_H

#include <QString>

enum class TriggerPhase {
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

    // Non-phase triggers (for future expansion)
    Other = 100,
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
        case TriggerPhase::Other:
            return QStringLiteral("Other");
        default:
            return QStringLiteral("Unknown");
    }
}

} // namespace TriggerTypeUtils

#endif // TRIGGER_TYPE_H
