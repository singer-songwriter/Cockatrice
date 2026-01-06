/**
 * @file trigger_parser.cpp
 * @brief Implementation of trigger parsing from card Oracle text.
 */

#include "trigger_parser.h"

#include "../cards/card_item.h"
#include "../cards/card_info.h"

TriggerParser::TriggerParser()
{
    initializePatterns();
}

void TriggerParser::initializePatterns()
{
    // Untap step triggers (e.g., Seedborn Muse)
    triggerPatterns.append(
        {QRegularExpression(R"(At the beginning of (?:your|each(?: player's)?|the) untap)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Untap});

    triggerPatterns.append(
        {QRegularExpression(R"(during each (?:other )?player's untap step)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Untap});

    // Upkeep triggers
    triggerPatterns.append(
        {QRegularExpression(R"(At the beginning of (?:your|each(?: player's)?|the) upkeep)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Upkeep});

    // Draw step triggers
    triggerPatterns.append(
        {QRegularExpression(R"(At the beginning of (?:your|each(?: player's)?|the) draw step)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Draw});

    // Combat beginning triggers
    triggerPatterns.append({QRegularExpression(R"(At the beginning of (?:your )?combat)",
                                               QRegularExpression::CaseInsensitiveOption),
                            TriggerPhase::CombatStart});

    triggerPatterns.append({QRegularExpression(R"(Whenever you attack)", QRegularExpression::CaseInsensitiveOption),
                            TriggerPhase::CombatStart});

    // Attack triggers (declare attackers step)
    // Using .+? (non-greedy) instead of [^,.]+ to handle card names with commas
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever .+? attacks)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Attackers});

    triggerPatterns.append({QRegularExpression(R"(When .+? attacks)", QRegularExpression::CaseInsensitiveOption),
                            TriggerPhase::Attackers});

    triggerPatterns.append({QRegularExpression(R"(Whenever you attack with)", QRegularExpression::CaseInsensitiveOption),
                            TriggerPhase::Attackers});

    // Block triggers (declare blockers step)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever .+? blocks)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Blockers});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever .+? becomes blocked)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Blockers});

    // Combat damage triggers
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever .+? deals combat damage)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Damage});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever .+? deals damage to a player)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Damage});

    // End of combat triggers
    triggerPatterns.append(
        {QRegularExpression(R"(At (?:the )?end of combat)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::CombatEnd});

    // End step triggers
    triggerPatterns.append(
        {QRegularExpression(R"(At the beginning of (?:your|each(?: player's)?|the) end step)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::EndStep});

    triggerPatterns.append(
        {QRegularExpression(R"(At the beginning of (?:your|the) end phase)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::EndStep});

    triggerPatterns.append(
        {QRegularExpression(R"(At the beginning of (?:your|each(?: opponent's)?|the next) end step)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::EndStep});

    // ETB triggers (enters the battlefield) - exclude lands to avoid overlap with Landfall
    // Using .+? (non-greedy) to handle card names with commas (e.g., "Aurelia, the Warleader")
    triggerPatterns.append(
        {QRegularExpression(R"(When(?:ever)? (?!a land).+? enters(?: the battlefield)?)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::EntersBattlefield});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a|an|another) (?!land).+? enters the battlefield)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::EntersBattlefield});

    // LTB triggers (leaves the battlefield)
    triggerPatterns.append(
        {QRegularExpression(R"(When(?:ever)? .+? leaves the battlefield)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::LeavesBattlefield});

    // Death triggers
    triggerPatterns.append(
        {QRegularExpression(R"(When(?:ever)? .+? dies)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Dies});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a|an|another) .+? (?:dies|is put into a graveyard))",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Dies});

    // Spell cast triggers (e.g., Rhystic Study, Edgar Markov eminence)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a player|an opponent|you) casts? a spell)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::SpellCast});

    // Note: "another" for self-excluding triggers like Edgar Markov's "cast another Vampire spell"
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a player|an opponent|you) casts? (?:a|an|another) .+? spell)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::SpellCast});

    // Draw card triggers (e.g., Smothering Tithe, Nekusar, Consecrated Sphinx)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a player|an opponent|you) draws? a card)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::DrawCard});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a player|an opponent|you) draws? (?:one or more )?cards)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::DrawCard});

    // Discard triggers (e.g., Waste Not, Megrim, Liliana's Caress)
    // Note: Handles "discards a creature card", "discards a noncreature, nonland card", etc.
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a player|an opponent|you) discards? a .+? card)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Discard});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a player|an opponent|you) discards? (?:one or more )?cards)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Discard});

    // Life gain triggers (e.g., Ajani's Pridemate, Soul Warden effect tracking)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever you gain life)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::GainLife});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a player|an opponent) gains life)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::GainLife});

    // Life loss / Damage triggers (e.g., Wound Reflection, Archfiend of Despair)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a player|an opponent|you) loses? life)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::LoseLife});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever .+? deals (?:damage|noncombat damage))",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::LoseLife});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever damage is dealt to)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::LoseLife});

    // Sacrifice triggers (e.g., Mayhem Devil, Blood Artist for sac)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a player|an opponent|you) sacrifices?)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Sacrifice});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever .+? is sacrificed)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Sacrifice});

    // Tap/Untap triggers (e.g., Opposition Agent for opponents, Cryptolith Rite creatures)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever .+? becomes tapped)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::TapUntap});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever .+? becomes untapped)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::TapUntap});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever you tap)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::TapUntap});

    // Landfall triggers (e.g., Lotus Cobra, Omnath, Avenger of Zendikar)
    triggerPatterns.append(
        {QRegularExpression(R"(Landfall\s*[—–-])", QRegularExpression::CaseInsensitiveOption), TriggerPhase::Landfall});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever a land enters the battlefield under your control)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Landfall});

    // Targeted triggers (e.g., Heroic, Feather targets)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever .+? becomes the target of)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::TargetedBy});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever you cast a spell that targets)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::TargetedBy});

    // Counter triggers (e.g., Hardened Scales, Winding Constrictor, proliferate tracking)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a|one or more) .*? counters? (?:is|are) (?:placed|put) on)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::CounterPlaced});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever you put (?:a|one or more) .*? counters?)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::CounterPlaced});

    // Replacement effects for counters (e.g., Hardened Scales, Doubling Season, Winding Constrictor)
    // These use "If ... would be put ... instead" rather than "Whenever"
    triggerPatterns.append(
        {QRegularExpression(R"(If (?:a|one or more) .*? counters? would be (?:placed|put) on)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::CounterPlaced});

    triggerPatterns.append(
        {QRegularExpression(R"(counters? (?:are|is) (?:placed|put) on .+? instead)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::CounterPlaced});

    // Token creation triggers (e.g., Anointed Procession tracking, Parallel Lives)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever you create (?:a|one or more) tokens?)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::TokenCreated});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a|one or more) tokens? (?:enters|enter) the battlefield)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::TokenCreated});

    // Mana triggers (e.g., Mana Flare, Dictate of Karametra, mana dorks that trigger)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a player|you) taps? a land for mana)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Mana});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever you tap a .+? for mana)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Mana});

    // Exile triggers (e.g., Rest in Peace, Leyline of the Void)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever .+? (?:is|are) exiled)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Exile});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever you exile)", QRegularExpression::CaseInsensitiveOption), TriggerPhase::Exile});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a|one or more) cards? (?:is|are) exiled)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Exile});

    // Search library triggers (e.g., Opposition Agent, Ob Nixilis)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever (?:a player|an opponent|you) searches? (?:a|their|his or her) library)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::SearchLibrary});

    // Magecraft triggers (instant/sorcery cast or copy)
    // Keyword version: "Magecraft —"
    triggerPatterns.append(
        {QRegularExpression(R"(Magecraft\s*[—–-])", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Magecraft});

    // Pattern version: "Whenever you cast or copy an instant or sorcery spell"
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever you cast or copy an instant or sorcery spell)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Magecraft});

    // Constellation triggers (enchantment ETB)
    // Keyword version: "Constellation —"
    triggerPatterns.append(
        {QRegularExpression(R"(Constellation\s*[—–-])", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Constellation});

    // Pattern version: "Whenever an enchantment enters"
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever an enchantment (?:you control )?enters)",
                            QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Constellation});

    // Mill triggers (cards put into graveyard from library)
    triggerPatterns.append(
        {QRegularExpression(
             R"(Whenever (?:a|one or more) cards? (?:is|are) put into (?:your|a|an opponent's) graveyard from .+? library)",
             QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Mill});

    // Copy spell triggers
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever you copy a spell)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::CopySpell});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever a player copies a spell)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::CopySpell});
}

bool TriggerParser::isEminenceAbility(const QString &text) const
{
    static QRegularExpression eminencePattern(R"(^Eminence\s*[—–-])", QRegularExpression::CaseInsensitiveOption);
    return eminencePattern.match(text).hasMatch();
}

bool TriggerParser::isEachPlayerTrigger(const QString &text) const
{
    // Patterns that indicate trigger affects ALL players (including you)
    // "a player" means ANY player, "each player" means everyone
    static QRegularExpression eachPlayerPattern(R"((?:each|a) player|other player)",
                                                QRegularExpression::CaseInsensitiveOption);
    return eachPlayerPattern.match(text).hasMatch();
}

bool TriggerParser::isOpponentOnlyTrigger(const QString &text) const
{
    // Patterns that indicate trigger affects only opponents (not you)
    // "an opponent", "each opponent", "opponents"
    static QRegularExpression opponentPattern(R"((?:a|an|each) opponent|opponents)",
                                              QRegularExpression::CaseInsensitiveOption);
    return opponentPattern.match(text).hasMatch();
}

QString TriggerParser::extractTriggerSentence(const QString &fullText, int matchStart) const
{
    // Find the start of the sentence (look for newline, period, or start of text)
    int sentenceStart = matchStart;
    for (int i = matchStart - 1; i >= 0; --i) {
        QChar c = fullText[i];
        if (c == '\n' || c == '.') {
            sentenceStart = i + 1;
            break;
        }
        if (i == 0) {
            sentenceStart = 0;
        }
    }

    // Find the end of the sentence (look for newline or period)
    int sentenceEnd = fullText.length();
    for (int i = matchStart; i < fullText.length(); ++i) {
        QChar c = fullText[i];
        if (c == '\n') {
            sentenceEnd = i;
            break;
        }
        if (c == '.' && i + 1 < fullText.length() && fullText[i + 1] == ' ') {
            sentenceEnd = i + 1;
            break;
        }
    }

    QString sentence = fullText.mid(sentenceStart, sentenceEnd - sentenceStart).trimmed();

    // Truncate if too long (for display purposes)
    const int maxLength = 100;
    if (sentence.length() > maxLength) {
        sentence = sentence.left(maxLength - 3) + "...";
    }

    return sentence;
}

QList<TriggerInfoPtr> TriggerParser::parseCardTriggers(CardItem *card, bool isSideboardZone)
{
    if (!card) {
        return {};
    }

    // Safely get card info - getInfo() returns a shared pointer (CardInfoPtr)
    CardInfoPtr info = card->getInfo();

    // Early exit if card info is null or card name is empty
    QString cardName = card->getName();
    if (!info || cardName.isEmpty()) {
        return {};
    }

    QString oracleText = info->getText();
    int cardId = card->getId();

    return parseCardTriggers(cardName, oracleText, cardId, isSideboardZone);
}

QList<TriggerInfoPtr> TriggerParser::parseCardTriggers(const QString &cardName,
                                                       const QString &oracleText,
                                                       int cardId,
                                                       bool isSideboardZone)
{
    QList<TriggerInfoPtr> triggers;

    if (oracleText.isEmpty()) {
        return triggers;
    }

    // Split by newlines to handle multiple abilities
    QStringList abilities = oracleText.split('\n', Qt::SkipEmptyParts);

    for (const QString &ability : abilities) {
        bool hasEminence = isEminenceAbility(ability);
        bool eachPlayer = isEachPlayerTrigger(ability);
        bool opponentOnly = isOpponentOnlyTrigger(ability);

        // If this is a sideboard card, only process eminence abilities
        if (isSideboardZone && !hasEminence) {
            continue;
        }

        // Check each pattern
        for (const TriggerPattern &pattern : triggerPatterns) {
            QRegularExpressionMatch match = pattern.regex.match(ability);
            if (match.hasMatch()) {
                auto triggerInfo = QSharedPointer<TriggerInfo>::create();
                triggerInfo->cardName = cardName;
                triggerInfo->triggerText = extractTriggerSentence(ability, match.capturedStart());
                triggerInfo->triggerPhase = pattern.phase;
                triggerInfo->isEminence = hasEminence;
                triggerInfo->isEachPlayer = eachPlayer;
                triggerInfo->isOpponentOnly = opponentOnly;
                triggerInfo->cardId = cardId;

                // Avoid duplicates for the same phase on the same ability line
                bool duplicate = false;
                for (const auto &existing : triggers) {
                    if (existing->triggerPhase == triggerInfo->triggerPhase &&
                        existing->triggerText == triggerInfo->triggerText) {
                        duplicate = true;
                        break;
                    }
                }

                if (!duplicate) {
                    triggers.append(triggerInfo);
                }
            }
        }
    }

    return triggers;
}
