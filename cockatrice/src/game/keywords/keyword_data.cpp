#include "keyword_data.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

KeywordData::KeywordData() : loaded(false)
{
    loadKeywords();
}

KeywordData &KeywordData::instance()
{
    static KeywordData instance;
    return instance;
}

void KeywordData::loadKeywords()
{
    if (loaded) {
        return;
    }

    QFile file(":/resources/data/mtg_keywords.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open keywords JSON file";
        loaded = true;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing keywords JSON:" << parseError.errorString();
        loaded = true;
        return;
    }

    QJsonObject root = doc.object();
    QJsonObject keywordsObj = root.value("keywords").toObject();

    for (auto it = keywordsObj.begin(); it != keywordsObj.end(); ++it) {
        QString key = it.key().toLower();
        QJsonObject kwObj = it.value().toObject();

        KeywordDefinition def;
        def.name = kwObj.value("name").toString();
        def.description = kwObj.value("description").toString();
        def.example = kwObj.value("example").toString();
        def.tip = kwObj.value("tip").toString();

        if (!def.name.isEmpty() && !def.description.isEmpty()) {
            keywords.insert(key, def);
        }
    }

    qDebug() << "Loaded" << keywords.size() << "keyword definitions";
    loaded = true;
}

const KeywordDefinition *KeywordData::getDefinition(const QString &keyword) const
{
    QString key = keyword.toLower();
    auto it = keywords.constFind(key);
    if (it != keywords.constEnd()) {
        return &it.value();
    }
    return nullptr;
}

bool KeywordData::keywordMatchesInText(const QString &keyword, const QString &text) const
{
    // Create a pattern that matches the keyword as a whole word
    // Handles variations like "Flying", "has flying", "gains flying", "with flying"
    QString escapedKeyword = QRegularExpression::escape(keyword);

    // Pattern to match keyword at word boundaries (case insensitive)
    QString pattern = QString("\\b%1\\b").arg(escapedKeyword);
    QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);

    return regex.match(text).hasMatch();
}

QList<const KeywordDefinition *> KeywordData::getKeywordsForCard(const QString &oracleText) const
{
    QList<const KeywordDefinition *> result;
    QSet<QString> foundKeywords; // Avoid duplicates

    if (oracleText.isEmpty()) {
        return result;
    }

    QString textLower = oracleText.toLower();

    // Check each known keyword against the text
    for (auto it = keywords.begin(); it != keywords.end(); ++it) {
        const QString &key = it.key();
        const KeywordDefinition &def = it.value();

        // Check if keyword appears in text
        if (keywordMatchesInText(key, oracleText)) {
            if (!foundKeywords.contains(key)) {
                foundKeywords.insert(key);
                result.append(&def);
            }
        }
    }

    // Sort results alphabetically by name
    std::sort(result.begin(), result.end(),
              [](const KeywordDefinition *a, const KeywordDefinition *b) { return a->name < b->name; });

    return result;
}

QStringList KeywordData::getAllKeywordNames() const
{
    QStringList names;
    for (auto it = keywords.begin(); it != keywords.end(); ++it) {
        names.append(it.value().name);
    }
    names.sort();
    return names;
}
