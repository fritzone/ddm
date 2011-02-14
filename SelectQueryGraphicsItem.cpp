#include "SelectQueryGraphicsItem.h"
#include "CellCommand.h"
#include "TableGraphicsItem.h"

#include "Workspace.h"
#include "Version.h"

#include <QPen>

SelectQueryGraphicsItem::SelectQueryGraphicsItem(QueryComponents* c) : m_select(new CellCommand(CellCommand::CELL_SELECT, c)),
    m_from(new CellCommand(CellCommand::CELL_FROM, c)), m_where(new CellCommand(CellCommand::CELL_WHERE, c)),
    m_frameRect(0), m_comps(c)
{
}

QGraphicsItemGroup* SelectQueryGraphicsItem::render(int& x, int& y, int& w, int &h)
{
    int lx = x;
    int ly = y;
    addToGroup(m_select->render(x, y, w, h));
    addToGroup(m_from->render(x, y, w, h));
    addToGroup(m_where->render(x, y, w, h));
    y += 2;
    QRectF br = boundingRect();
    br.setWidth(br.width() + 2);
    br.setHeight(br.height() + 2);
    m_frameRect = new QGraphicsRectItem(br, this);
    m_frameRect->setZValue(-2);
    QPen thick;
    thick.setWidth(1);
    thick.setColor(Qt::gray);
    m_frameRect->setPen(thick);
    return this;
}


void SelectQueryGraphicsItem::test()
{
    m_from->addChild(new TableGraphicsItem(Workspace::getInstance()->workingVersion()->getTables().at(0), m_comps));
    m_from->addChild(new TableGraphicsItem(Workspace::getInstance()->workingVersion()->getTables().at(0), m_comps));
    m_from->addChild(new SelectQueryGraphicsItem(m_comps));
    m_from->addChild(new TableGraphicsItem(Workspace::getInstance()->workingVersion()->getTables().at(0), m_comps));

}

void SelectQueryGraphicsItem::updateWidth(int newWidth)
{
    m_select->updateWidth(newWidth);
    m_from->updateWidth(newWidth);
    m_where->updateWidth(newWidth);
    QRect newRect(m_frameRect->boundingRect().left(), m_frameRect->boundingRect().top(), newWidth + 2, m_frameRect->boundingRect().height()-1);
    m_frameRect->setRect(newRect);
}
