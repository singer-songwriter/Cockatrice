#ifndef CARD_DETAILS_DOCK_WIDGET_H
#define CARD_DETAILS_DOCK_WIDGET_H

#include "../../../../game/cards/card_database.h"

#include <QDockWidget>

class CardInfoKeywordsWidget;
class CardInfoRulingsWidget;
class QTabWidget;

class CardDetailsDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CardDetailsDockWidget(QWidget *parent = nullptr);
    void retranslateUi();

public slots:
    void setCard(CardInfoPtr card);
    void setCard(const QString &cardName);

private:
    QTabWidget *tabWidget;
    CardInfoKeywordsWidget *keywordsWidget;
    CardInfoRulingsWidget *rulingsWidget;
};

#endif // CARD_DETAILS_DOCK_WIDGET_H
