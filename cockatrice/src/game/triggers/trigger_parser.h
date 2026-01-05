/**
 * @file trigger_parser.h
 * @brief Parses card Oracle text to detect triggered abilities.
 */

#ifndef TRIGGER_PARSER_H
#define TRIGGER_PARSER_H

#include "trigger_info.h"

#include <QList>
#include <QRegularExpression>
#include <QString>
#include <QVector>

class CardItem;
class CardInfo;

class TriggerParser
{
public:
    TriggerParser();

    QList<TriggerInfoPtr> parseCardTriggers(CardItem *card, bool isSideboardZone = false);
    QList<TriggerInfoPtr> parseCardTriggers(const QString &cardName,
                                            const QString &oracleText,
                                            int cardId,
                                            bool isSideboardZone = false);

private:
    struct TriggerPattern
    {
        QRegularExpression regex;
        TriggerPhase phase;
    };

    QVector<TriggerPattern> triggerPatterns;

    void initializePatterns();
    bool isEminenceAbility(const QString &text) const;
    bool isEachPlayerTrigger(const QString &text) const;
    QString extractTriggerSentence(const QString &fullText, int matchStart) const;
};

#endif // TRIGGER_PARSER_H
