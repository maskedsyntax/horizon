#include "ScreenItem.h"
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsScene>
#include <cmath>

ScreenItem::ScreenItem(const ScreenInfo &info, QGraphicsItem *parent)
    : QGraphicsRectItem(0, 0, info.width / 10.0, info.height / 10.0, parent), m_info(info)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    textItem = new QGraphicsTextItem(info.name, this);
    textItem->setDefaultTextColor(QColor("#cccccc")); // Light grey text
    QFont font = textItem->font();
    font.setBold(true);
    textItem->setFont(font);
    
    updateAppearance();
}

void ScreenItem::setPrimary(bool primary) {
    m_info.isPrimary = primary;
    updateAppearance();
}

void ScreenItem::setRate(double rate) {
    m_info.currentRate = rate;
    // Maybe update text to show rate?
}

void ScreenItem::updateAppearance() {
    // VS Code colors
    // Selected/Active monitor: #007acc (Blue)
    // Primary: Gold border or distinct visual
    
    QColor fillColor = QColor("#007acc"); // VS Code Blue
    if (!m_info.active) fillColor = QColor("#333333"); // Dim if inactive (not fully implemented yet)

    setBrush(QBrush(fillColor));

    if (m_info.isPrimary) {
        setPen(QPen(QColor("#e7c030"), 2)); // Gold, thinner border
    } else {
        setPen(QPen(QColor("#1e1e1e"), 1)); // Dark border
    }
    
    // Center text
    QRectF r = rect();
    QRectF tr = textItem->boundingRect();
    textItem->setPos(r.center().x() - tr.width() / 2, r.center().y() - tr.height() / 2);
}

void ScreenItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);
}

void ScreenItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mouseMoveEvent(event);
}

void ScreenItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);
}

void ScreenItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu menu;
    menu.setStyleSheet("QMenu { background-color: #252526; color: #cccccc; border: 1px solid #454545; } QMenu::item:selected { background-color: #007acc; }");
    
    QAction *primaryAction = menu.addAction("Make Primary");
    primaryAction->setCheckable(true);
    primaryAction->setChecked(m_info.isPrimary);

    QMenu *rateMenu = menu.addMenu("Refresh Rate");
    rateMenu->setStyleSheet("QMenu { background-color: #252526; color: #cccccc; border: 1px solid #454545; } QMenu::item:selected { background-color: #007acc; }");
    
    // Find current mode rates
    for (const auto &mode : m_info.modes) {
        if (mode.width == m_info.width && mode.height == m_info.height) {
            for (double r : mode.rates) {
                QString rateStr = QString::number(r, 'f', 2) + " Hz";
                QAction *a = rateMenu->addAction(rateStr);
                a->setData(r);
                a->setCheckable(true);
                if (std::abs(r - m_info.currentRate) < 0.01) a->setChecked(true);
            }
            break; 
        }
    }

    QAction *selected = menu.exec(event->screenPos());
    if (selected) {
        if (selected == primaryAction) {
            bool isPrim = primaryAction->isChecked(); // Logic inverted? exec returns after click.
            // Actually checking happens before trigger usually, but with exec we get the action.
            // We just want to Set Primary.
            
            // Unset others
            if (scene()) {
                for (QGraphicsItem *item : scene()->items()) {
                    ScreenItem *si = dynamic_cast<ScreenItem*>(item);
                    if (si && si != this) {
                        si->setPrimary(false);
                    }
                }
            }
            setPrimary(true); // Always set true if clicked (enforce one primary)
        } else if (selected->parent() == rateMenu) {
            double newRate = selected->data().toDouble();
            setRate(newRate);
        }
    }
}