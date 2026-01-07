#ifndef CARD_INFO_RULINGS_WIDGET_H
#define CARD_INFO_RULINGS_WIDGET_H

#include "../../../../game/cards/card_database.h"

#include <QFrame>

class QLabel;
class QTextEdit;
class QVBoxLayout;

class CardInfoRulingsWidget : public QFrame
{
    Q_OBJECT

private:
    CardInfoPtr info;
    QTextEdit *rulingsText;
    QLabel *noRulingsLabel;

    void updateDisplay();

public:
    explicit CardInfoRulingsWidget(QWidget *parent = nullptr);
    void retranslateUi();

public slots:
    void setCard(CardInfoPtr card);
};

#endif // CARD_INFO_RULINGS_WIDGET_H
