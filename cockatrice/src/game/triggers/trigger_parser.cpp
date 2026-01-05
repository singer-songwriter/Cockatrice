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
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever [^,.]+ attacks)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Attackers});

    triggerPatterns.append({QRegularExpression(R"(When [^,.]+ attacks)", QRegularExpression::CaseInsensitiveOption),
                            TriggerPhase::Attackers});

    triggerPatterns.append({QRegularExpression(R"(Whenever you attack with)", QRegularExpression::CaseInsensitiveOption),
                            TriggerPhase::Attackers});

    // Block triggers (declare blockers step)
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever [^,.]+ blocks)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Blockers});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever [^,.]+ becomes blocked)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Blockers});

    // Combat damage triggers
    triggerPatterns.append(
        {QRegularExpression(R"(Whenever [^,.]+ deals combat damage)", QRegularExpression::CaseInsensitiveOption),
         TriggerPhase::Damage});

    triggerPatterns.append(
        {QRegularExpression(R"(Whenever [^,.]+ deals damage to a player)", QRegularExpression::CaseInsensitiveOption),
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
}

bool TriggerParser::isEminenceAbility(const QString &text) const
{
    static QRegularExpression eminencePattern(R"(^Eminence\s*[—–-])", QRegularExpression::CaseInsensitiveOption);
    return eminencePattern.match(text).hasMatch();
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

    // Safely get card info - getCardInfo() returns a reference to empty CardInfo if null
    const CardInfo &info = card->getCardInfo();

    // Early exit if card name is empty (invalid/unloaded card)
    QString cardName = card->getName();
    if (cardName.isEmpty()) {
        return {};
    }

    QString oracleText = info.getText();
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
