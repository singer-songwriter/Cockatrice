/**
 * @file trigger_reminder_widget.h
 * @brief Widget displaying triggered abilities grouped by game phase.
 */

#ifndef TRIGGER_REMINDER_WIDGET_H
#define TRIGGER_REMINDER_WIDGET_H

#include "../../../../game/triggers/trigger_info.h"
#include "../../../../game/triggers/trigger_type.h"

#include <QMap>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

class TriggerManager;
class QLabel;

class TriggerReminderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TriggerReminderWidget(TriggerManager *manager, QWidget *parent = nullptr);
    void retranslateUi();

signals:
    void cardHovered(const QString &cardName);
    void cardClicked(const QString &cardName);

public slots:
    void refreshDisplay();
    void setCurrentPhase(int phaseIndex);

private slots:
    void onItemEntered(QTreeWidgetItem *item, int column);
    void onItemClicked(QTreeWidgetItem *item, int column);

private:
    TriggerManager *triggerManager;
    QTreeWidget *triggerTree;
    QLabel *headerLabel;
    QVBoxLayout *mainLayout;

    int currentPhaseIndex;
    bool isUpdating;

    QMap<TriggerPhase, QTreeWidgetItem *> categoryItems;

    static const int CardNameRole = Qt::UserRole + 1;

    void setupUi();
    void populateTriggers();
    void updatePhaseHighlighting();
    QTreeWidgetItem *getOrCreateCategoryItem(TriggerPhase phase);
    void clearCategoryItems();
    QString getCardNameFromItem(QTreeWidgetItem *item) const;
};

#endif // TRIGGER_REMINDER_WIDGET_H
