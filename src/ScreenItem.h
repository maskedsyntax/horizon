#ifndef SCREENITEM_H
#define SCREENITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>

class ScreenItem : public QGraphicsRectItem
{
public:
    ScreenItem(const QString &name, int w, int h, QGraphicsItem *parent = nullptr);

    // Getters for logical position
    QString getName() const { return screenName; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString screenName;
    int m_width;
    int m_height;
    QGraphicsTextItem *textItem;
};

#endif // SCREENITEM_H
