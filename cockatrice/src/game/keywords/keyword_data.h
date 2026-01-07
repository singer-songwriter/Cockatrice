#ifndef KEYWORD_DATA_H
#define KEYWORD_DATA_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QString>

struct KeywordDefinition
{
    QString name;        // Display name (e.g., "Double Strike")
    QString description; // Rules description
};

class KeywordData : public QObject
{
    Q_OBJECT

public:
    static KeywordData &instance();

    // Get definition for a specific keyword (case-insensitive lookup)
    const KeywordDefinition *getDefinition(const QString &keyword) const;

    // Extract all keywords found in oracle text and return their definitions
    QList<const KeywordDefinition *> getKeywordsForCard(const QString &oracleText) const;

    // Get list of all keyword names
    QStringList getAllKeywordNames() const;

private:
    KeywordData();
    ~KeywordData() override = default;

    // Disable copy and assignment
    KeywordData(const KeywordData &) = delete;
    KeywordData &operator=(const KeywordData &) = delete;

    void loadKeywords();
    bool keywordMatchesInText(const QString &keyword, const QString &text) const;

    QHash<QString, KeywordDefinition> keywords; // lowercase keyword -> definition
    bool loaded;
};

#endif // KEYWORD_DATA_H
