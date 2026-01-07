#ifndef CARD_INFO_KEYWORDS_WIDGET_H
#define CARD_INFO_KEYWORDS_WIDGET_H

#include "../../../../game/cards/card_database.h"

#include <QFrame>

class QLabel;
class QScrollArea;
class QTextEdit;
class QVBoxLayout;

class CardInfoKeywordsWidget : public QFrame
{
    Q_OBJECT

private:
    CardInfoPtr info;
    QScrollArea *scrollArea;
    QTextEdit *keywordsText;
    QLabel *noKeywordsLabel;

    void updateDisplay();

public:
    explicit CardInfoKeywordsWidget(QWidget *parent = nullptr);
    void retranslateUi();

public slots:
    void setCard(CardInfoPtr card);
};

#endif // CARD_INFO_KEYWORDS_WIDGET_H
