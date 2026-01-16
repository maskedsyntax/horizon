#include "ScreenItem.h"
#include <QBrush>
#include <QColor>
#include <QFont>

ScreenItem::ScreenItem(const QString &name, int w, int h, QGraphicsItem *parent)
    : QGraphicsRectItem(0, 0, w / 10.0, h / 10.0, parent), screenName(name), m_width(w), m_height(h) // Scale down for view
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    setBrush(QBrush(QColor(100, 149, 237, 200))); // Cornflower blue
    setPen(QPen(Qt::black, 2));

    textItem = new QGraphicsTextItem(name, this);
    textItem->setDefaultTextColor(Qt::white);
    
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
