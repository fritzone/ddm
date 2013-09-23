#include "qbr_SelectQueryGraphicsItem.h"
#include "qbr_CellSelectCommand.h"
#include "qbr_CellWhereCommand.h"
#include "qbr_CellFromCommand.h"
#include "qbr_CellGroupByCommand.h"
#include "qbr_TableGraphicsItem.h"
#include "qbr_CellAsCommand.h"
#include "qbr_CellOrderByCommand.h"
#include "qbr_CellQuerySmallOptionsBoxCircle.h"

#include <QPen>

SelectQueryGraphicsItem::SelectQueryGraphicsItem(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) :
    QueryGraphicsItem(level, parent, c, owner), m_select(0), m_from(0), m_where(0), m_groupby(0), m_having(0), m_as(0),m_orderBy(0),
    m_frameRect(0)
{
}

void SelectQueryGraphicsItem::createSelectCell(QueryComponent* owner)
{
    m_select = new CellSelectCommand(m_helper, m_level, this, owner);
}

void SelectQueryGraphicsItem::createGroupByCell(QueryComponent* owner)
{
    m_groupby = new CellGroupByCommand(m_helper, m_level, this, owner);
}

void SelectQueryGraphicsItem::createFromCell(QueryComponent* owner)
{
    m_from = new CellFromCommand(m_helper, m_level, this, owner);
}

void SelectQueryGraphicsItem::createWhereCell(QueryComponent* owner)
{
    m_where = new CellWhereCommand(m_helper, m_level, this, owner, SelectQueryWhereComponent::WHERETYPE_WHERE);
}

void SelectQueryGraphicsItem::createHavingCell(QueryComponent* owner)
{
    m_having = new CellWhereCommand(m_helper, m_level, this, owner, SelectQueryWhereComponent::WHERETYPE_HAVING);
}

void SelectQueryGraphicsItem::createAsCell(QueryComponent* owner)
{
    m_as = new CellAsCommand(m_helper, m_level, this, owner, false);
}

void SelectQueryGraphicsItem::createOrderByCell(QueryComponent* owner)
{
    m_orderBy = new CellOrderByCommand(m_helper, m_level, this, owner);
}


QGraphicsItemGroup* SelectQueryGraphicsItem::render(int& x, int& y, int& w, int &h)
{
    addToGroup(m_select->render(x, y, w, h));
    if(m_from) addToGroup(m_from->render(x, y, w, h));
    if(m_where) addToGroup(m_where->render(x, y, w, h));
    if(m_groupby) addToGroup(m_groupby->render(x, y, w, h));
    if(m_having) addToGroup(m_having->render(x, y, w, h));
    if(m_as)addToGroup(m_as->render(x, y, w, h));
    if(m_orderBy)addToGroup(m_orderBy->render(x, y, w, h));

    y += 2;

    CellQuerySmallOptionsBox* smb = new CellQuerySmallOptionsBoxCircle(m_owner->provideOptions(), m_helper, m_level, m_parent, m_owner);
    int tx = x + 5 + 2; int ty = y-1; int tw = w; int th = h;
    QGraphicsLineItem* lineUp = new QGraphicsLineItem(tx, ty-5, tx , ty + 5, this);
    lineUp->setZValue(-5);
    addToGroup(lineUp);
    y = ty + 5 + 1;
    tx -= 5;
    addToGroup(smb->render(tx, y, tw, th));

    QRectF br = boundingRect();
    br.setWidth(br.width() - 4);
    br.setHeight(br.height() + 4);
    br.setLeft(br.left() + 1 + m_level * CHILDREN_ALIGNMENT);
    br.setTop(br.top() + 1);

//    m_frameRect = new QGraphicsRectItem(br, this);
//    m_frameRect->setZValue(-2);
//    QPen thick;
//    thick.setWidth(1);
//    thick.setColor(Qt::black);
//    m_frameRect->setPen(thick);
//    m_frameRect->setBrush(QBrush(QColor(255,255,255)));

    y = br.bottom() + 2 + 1;

    return this;
}

void SelectQueryGraphicsItem::updateWidth(int newWidth)
{
    m_select->updateWidth(newWidth);
    if(m_from) m_from->updateWidth(newWidth);
    if(m_where) m_where->updateWidth(newWidth);
    if(m_groupby) m_groupby->updateWidth(newWidth);
    if(m_having) m_having->updateWidth(newWidth);
    if(m_as) m_as->updateWidth(newWidth);
    if(m_orderBy) m_orderBy->updateWidth(newWidth);

    //QRect newRect(m_frameRect->boundingRect().left(), m_frameRect->boundingRect().top(), newWidth, m_frameRect->boundingRect().height()-1);
    //m_frameRect->setRect(newRect);
}

void SelectQueryGraphicsItem::addFromGraphicsItem(QueryGraphicsItem* fromsChild)
{
    if(m_from) m_from->addChild(fromsChild);
}

void SelectQueryGraphicsItem::addSelectGraphicsItem(QueryGraphicsItem* selectsChild)
{
    if(m_select) m_select->addChild(selectsChild);
}

void SelectQueryGraphicsItem::addWhereGraphicsItem(QueryGraphicsItem* wheresChild)
{
    if(m_where) m_where->addChild(wheresChild);
}

void SelectQueryGraphicsItem::addGroupByGraphicsItem(QueryGraphicsItem* groupByChild)
{
    if(m_groupby) m_groupby->addChild(groupByChild);
}

void SelectQueryGraphicsItem::addHavingGraphicsItem(QueryGraphicsItem* havingChild)
{
    if(m_having) m_having->addChild(havingChild);
}

void SelectQueryGraphicsItem::addOrderByGraphicsItem(QueryGraphicsItem* orderByChild)
{
    if(m_orderBy) m_orderBy->addChild(orderByChild);
}

void SelectQueryGraphicsItem::onClose()
{

}
