/**
 * @file trigger_reminder_dock_widget.cpp
 * @brief Implementation of the trigger reminder dock widget.
 */

#include "trigger_reminder_dock_widget.h"

#include "trigger_reminder_widget.h"

TriggerReminderDockWidget::TriggerReminderDockWidget(TriggerManager *manager, QWidget *parent) : QDockWidget(parent)
{
    triggerWidget = new TriggerReminderWidget(manager, this);
    setWidget(triggerWidget);

    setWindowTitle(tr("Triggers"));
    setObjectName("triggerReminderDock");

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);

    // Forward card hover/click signals for card info display integration
    connect(triggerWidget, &TriggerReminderWidget::cardHovered, this, &TriggerReminderDockWidget::cardHovered);
    connect(triggerWidget, &TriggerReminderWidget::cardClicked, this, &TriggerReminderDockWidget::cardClicked);
}

void TriggerReminderDockWidget::retranslateUi()
{
    setWindowTitle(tr("Triggers"));
    if (triggerWidget) {
        triggerWidget->retranslateUi();
    }
}
