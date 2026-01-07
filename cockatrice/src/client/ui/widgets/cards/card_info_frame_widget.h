#ifndef CARDFRAME_H
#define CARDFRAME_H

#include "../../../../game/cards/card_database.h"

#include <QPushButton>
#include <QTabWidget>

class AbstractCardItem;
class CardInfoKeywordsWidget;
class CardInfoPictureWidget;
class CardInfoRulingsWidget;
class CardInfoTextWidget;
class QVBoxLayout;
class QSplitter;

class CardInfoFrameWidget : public QTabWidget
{
    Q_OBJECT
private:
    CardInfoPtr info;
    CardInfoPictureWidget *pic;
    CardInfoTextWidget *text;
    CardInfoKeywordsWidget *keywords;
    CardInfoRulingsWidget *rulings;
    QPushButton *viewTransformationButton;
    bool cardTextOnly;
    QWidget *tab1, *tab2, *tab3, *tab4, *tab5;
    QVBoxLayout *tab1Layout, *tab2Layout, *tab3Layout, *tab4Layout, *tab5Layout;
    QSplitter *splitter;

    void setViewTransformationButtonVisibility(bool visible);
    void refreshLayout();

public:
    enum ViewMode
    {
        ImageOnlyView,
        TextOnlyView,
        ImageAndTextView,
        KeywordsView,
        RulingsView
    };

    explicit CardInfoFrameWidget(const QString &cardName = QString(), QWidget *parent = nullptr);
    CardInfoPtr getInfo()
    {
        return info;
    }
    void retranslateUi();

public slots:
    void setCard(CardInfoPtr card);
    void setCard(const QString &cardName);
    void setCard(const QString &cardName, const QString &providerId);
    void setCard(AbstractCardItem *card);
    void viewTransformation();
    void clearCard();
    void setViewMode(int mode);
};

#endif
