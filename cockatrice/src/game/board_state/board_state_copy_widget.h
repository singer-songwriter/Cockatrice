#ifndef BOARD_STATE_COPY_WIDGET_H
#define BOARD_STATE_COPY_WIDGET_H

#include <QWidget>

class AbstractGame;
class PhasesToolbar;
class CardItem;
class CardZoneLogic;
class QPushButton;

class BoardStateCopyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BoardStateCopyWidget(AbstractGame *game, PhasesToolbar *phasesToolbar, QWidget *parent = nullptr);

private slots:
    void actCopyBoardState();

private:
    AbstractGame *game;
    PhasesToolbar *phasesToolbar;
    QPushButton *copyButton;

    static QString serializeCard(CardItem *card);
    static QString serializeZone(const QString &label, CardZoneLogic *zone, bool showNames);
};

#endif // BOARD_STATE_COPY_WIDGET_H
