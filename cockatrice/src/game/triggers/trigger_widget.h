#ifndef TRIGGER_WIDGET_H
#define TRIGGER_WIDGET_H

#include <QMap>
#include <QString>
#include <QWidget>

class Player;
class QListWidget;

class TriggerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TriggerWidget(QWidget *parent = nullptr);
    void retranslateUi();

signals:
    void cardNameHovered(const QString &cardName);

public slots:
    void setLocalPlayer(Player *player);
    void updateTriggers();

private:
    QListWidget *triggerList;
    Player *localPlayer = nullptr;

    static QString categorizeTrigger(const QString &line);
    static bool hasTrigger(const QString &oracleText);
};

#endif // TRIGGER_WIDGET_H
