/**
 * @file trigger_reminder_dock_widget.h
 * @brief Dock widget wrapper for the trigger reminder panel.
 */

#ifndef TRIGGER_REMINDER_DOCK_WIDGET_H
#define TRIGGER_REMINDER_DOCK_WIDGET_H

#include <QDockWidget>

class TriggerReminderWidget;
class TriggerManager;

class TriggerReminderDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit TriggerReminderDockWidget(TriggerManager *manager, QWidget *parent = nullptr);
    void retranslateUi();

    TriggerReminderWidget *getTriggerWidget() const
    {
        return triggerWidget;
    }

private:
    TriggerReminderWidget *triggerWidget;
};

#endif // TRIGGER_REMINDER_DOCK_WIDGET_H
