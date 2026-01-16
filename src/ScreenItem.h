#ifndef SCREENITEM_H
#define SCREENITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>
#include "XrandrManager.h" // Need ResolutionMode

class ScreenItem : public QGraphicsRectItem
{
public:
    ScreenItem(const ScreenInfo &info, QGraphicsItem *parent = nullptr);

    // Getters for logical position
    QString getName() const { return m_info.name; }
    int getWidth() const { return m_info.width; }
    int getHeight() const { return m_info.height; }
    double getRate() const { return m_info.currentRate; }
    bool isPrimary() const { return m_info.isPrimary; }
    std::vector<ResolutionMode> getModes() const { return m_info.modes; }

    void setPrimary(bool primary);
    void setRate(double rate);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    void updateAppearance();

    ScreenInfo m_info;
    QGraphicsTextItem *textItem;
};

#endif // SCREENITEM_H
