/**
 * @file trigger_reminder_widget.cpp
 * @brief Implementation of the trigger reminder display widget.
 */

#include "trigger_reminder_widget.h"

#include "../../../../game/triggers/trigger_manager.h"

#include <QFont>
#include <QHeaderView>
#include <QLabel>

TriggerReminderWidget::TriggerReminderWidget(TriggerManager *manager, QWidget *parent)
    : QWidget(parent), triggerManager(manager), currentPhaseIndex(-1), isUpdating(false)
{
    setupUi();

    if (triggerManager) {
        connect(triggerManager, &TriggerManager::triggersChanged, this, &TriggerReminderWidget::refreshDisplay,
                Qt::QueuedConnection);
    }
}

void TriggerReminderWidget::setupUi()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    headerLabel = new QLabel(tr("Trigger Reminders"), this);
    QFont headerFont = headerLabel->font();
    headerFont.setBold(true);
    headerLabel->setFont(headerFont);
    mainLayout->addWidget(headerLabel);

    triggerTree = new QTreeWidget(this);
    triggerTree->setHeaderHidden(true);
    triggerTree->setRootIsDecorated(true);
    triggerTree->setIndentation(12);
    triggerTree->setAnimated(true);
    triggerTree->setWordWrap(true);
    triggerTree->setMouseTracking(true);
    mainLayout->addWidget(triggerTree);

    // Connect hover and click signals for card info display
    connect(triggerTree, &QTreeWidget::itemEntered, this, &TriggerReminderWidget::onItemEntered);
    connect(triggerTree, &QTreeWidget::itemClicked, this, &TriggerReminderWidget::onItemClicked);

    setLayout(mainLayout);
}

void TriggerReminderWidget::retranslateUi()
{
    headerLabel->setText(tr("Trigger Reminders"));
    refreshDisplay();
}

void TriggerReminderWidget::refreshDisplay()
{
    if (!triggerTree || !triggerManager || isUpdating) {
        return;
    }
    isUpdating = true;
    populateTriggers();
    updatePhaseHighlighting();
    isUpdating = false;
}

void TriggerReminderWidget::setCurrentPhase(int phaseIndex)
{
    if (!triggerManager || isUpdating) {
        return;
    }
    currentPhaseIndex = phaseIndex;
    isUpdating = true;
    updatePhaseHighlighting();
    isUpdating = false;
}

void TriggerReminderWidget::clearCategoryItems()
{
    triggerTree->clear();
    categoryItems.clear();
}

QTreeWidgetItem *TriggerReminderWidget::getOrCreateCategoryItem(TriggerPhase phase)
{
    if (categoryItems.contains(phase)) {
        return categoryItems.value(phase);
    }

    auto *item = new QTreeWidgetItem(triggerTree);
    QString displayName = TriggerTypeUtils::getDisplayName(phase);
    item->setText(0, displayName);
    item->setData(0, Qt::UserRole, static_cast<int>(phase));

    categoryItems.insert(phase, item);
    return item;
}

void TriggerReminderWidget::populateTriggers()
{
    clearCategoryItems();

    if (!triggerManager) {
        return;
    }

    QMap<TriggerPhase, QList<TriggerInfoPtr>> triggersByPhase = triggerManager->getTriggersByPhase();

    // Define the order of phases to display (phase-based first, then event-based)
    QList<TriggerPhase> phaseOrder = {// Phase-based triggers
                                      TriggerPhase::Untap,
                                      TriggerPhase::Upkeep,
                                      TriggerPhase::Draw,
                                      TriggerPhase::Main1,
                                      TriggerPhase::CombatStart,
                                      TriggerPhase::Attackers,
                                      TriggerPhase::Blockers,
                                      TriggerPhase::Damage,
                                      TriggerPhase::CombatEnd,
                                      TriggerPhase::Main2,
                                      TriggerPhase::EndStep,
                                      // Event-based triggers
                                      TriggerPhase::EntersBattlefield,
                                      TriggerPhase::LeavesBattlefield,
                                      TriggerPhase::Dies,
                                      TriggerPhase::SpellCast,
                                      TriggerPhase::DrawCard,
                                      TriggerPhase::Discard,
                                      TriggerPhase::GainLife,
                                      TriggerPhase::LoseLife,
                                      TriggerPhase::Sacrifice,
                                      TriggerPhase::TapUntap,
                                      TriggerPhase::Landfall,
                                      TriggerPhase::TargetedBy,
                                      TriggerPhase::CounterPlaced,
                                      TriggerPhase::TokenCreated,
                                      TriggerPhase::Mana,
                                      TriggerPhase::Other};

    for (TriggerPhase phase : phaseOrder) {
        if (!triggersByPhase.contains(phase) || triggersByPhase.value(phase).isEmpty()) {
            continue;
        }

        QTreeWidgetItem *categoryItem = getOrCreateCategoryItem(phase);
        const QList<TriggerInfoPtr> &triggers = triggersByPhase.value(phase);

        // Update category label with count
        QString displayName = TriggerTypeUtils::getDisplayName(phase);
        categoryItem->setText(0, QString("%1 (%2)").arg(displayName).arg(triggers.size()));

        for (const TriggerInfoPtr &trigger : triggers) {
            auto *triggerItem = new QTreeWidgetItem(categoryItem);

            QString displayText = trigger->cardName;
            if (trigger->isEminence) {
                displayText += " [Eminence]";
            }
            if (trigger->isEachPlayer) {
                displayText += " [All Players]";
                // Use a subtle blue-gray background for "each player" triggers
                triggerItem->setBackground(0, QColor(220, 235, 245)); // Soft blue-gray
                triggerItem->setForeground(0, QColor(0, 0, 0));       // Black text for contrast
            } else if (trigger->isOpponentOnly) {
                displayText += " [Opponents]";
                // Use a subtle orange/peach background for opponent-only triggers
                triggerItem->setBackground(0, QColor(255, 235, 220)); // Soft peach
                triggerItem->setForeground(0, QColor(0, 0, 0));       // Black text for contrast
            }
            triggerItem->setText(0, displayText);
            triggerItem->setToolTip(0, trigger->triggerText);

            // Store card ID and card name for card info display
            triggerItem->setData(0, Qt::UserRole, trigger->cardId);
            triggerItem->setData(0, CardNameRole, trigger->cardName);
        }

        // Expand categories with triggers
        categoryItem->setExpanded(true);
    }
}

void TriggerReminderWidget::updatePhaseHighlighting()
{
    if (currentPhaseIndex < 0 || !triggerTree) {
        return;
    }

    TriggerPhase currentPhase = TriggerTypeUtils::fromPhaseIndex(currentPhaseIndex);

    for (auto it = categoryItems.constBegin(); it != categoryItems.constEnd(); ++it) {
        QTreeWidgetItem *item = it.value();
        if (!item) {
            continue;
        }

        TriggerPhase itemPhase = it.key();

        QFont font = item->font(0);
        if (itemPhase == currentPhase) {
            // Highlight current phase
            font.setBold(true);
            item->setFont(0, font);
            item->setBackground(0, QColor(255, 200, 200)); // Light red highlight
            item->setForeground(0, QColor(0, 0, 0));       // Black text for contrast
            item->setExpanded(true);
        } else {
            // Normal styling
            font.setBold(false);
            item->setFont(0, font);
            item->setBackground(0, QBrush());   // Clear background
            item->setForeground(0, QBrush());   // Reset to default text color
        }
    }
}

QString TriggerReminderWidget::getCardNameFromItem(QTreeWidgetItem *item) const
{
    if (!item) {
        return QString();
    }
    return item->data(0, CardNameRole).toString();
}

void TriggerReminderWidget::onItemEntered(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    QString cardName = getCardNameFromItem(item);
    if (!cardName.isEmpty()) {
        emit cardHovered(cardName);
    }
}

void TriggerReminderWidget::onItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    QString cardName = getCardNameFromItem(item);
    if (!cardName.isEmpty()) {
        emit cardClicked(cardName);
    }
}
