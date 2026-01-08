#include "card_details_dock_widget.h"

#include "../../../../game/cards/card_database_manager.h"
#include "../cards/card_info_keywords_widget.h"
#include "../cards/card_info_rulings_widget.h"

#include <QTabWidget>

CardDetailsDockWidget::CardDetailsDockWidget(QWidget *parent) : QDockWidget(parent)
{
    tabWidget = new QTabWidget(this);
    tabWidget->setObjectName("cardDetailsTabWidget");

    keywordsWidget = new CardInfoKeywordsWidget(this);
    keywordsWidget->setObjectName("keywordsWidget");

    rulingsWidget = new CardInfoRulingsWidget(this);
    rulingsWidget->setObjectName("rulingsWidget");

    tabWidget->addTab(keywordsWidget, QString());
    tabWidget->addTab(rulingsWidget, QString());

    setWidget(tabWidget);
    setObjectName("cardDetailsDock");
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);

    retranslateUi();
}

void CardDetailsDockWidget::retranslateUi()
{
    setWindowTitle(tr("Card Details"));
    tabWidget->setTabText(0, tr("Keywords"));
    tabWidget->setTabText(1, tr("Rulings"));

    keywordsWidget->retranslateUi();
    rulingsWidget->retranslateUi();
}

void CardDetailsDockWidget::setCard(CardInfoPtr card)
{
    keywordsWidget->setCard(card);
    rulingsWidget->setCard(card);
}

void CardDetailsDockWidget::setCard(const QString &cardName)
{
    setCard(CardDatabaseManager::getInstance()->guessCard(cardName));
}
