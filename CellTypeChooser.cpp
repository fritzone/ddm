#include "CellTypeChooser.h"
#include "Workspace.h"
#include "Version.h"
#include "IconFactory.h"

#include <QPen>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QScrollBar>
#include <QueryComponents.h>

CellTypeChooser::CellTypeChooser(int level, CellTypeChooserSize size, CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryGraphicsHelper *c, QueryGraphicsItem* parent, QueryComponent* owner):
        QueryGraphicsItem(level, parent, c, owner),
        m_defaultType(defaultType), m_allowedTypes(allowedTypes), m_currentType(defaultType), m_rect(0), m_size (size), m_index(-1)
{
}

CellTypeChooser::CellTypeChooser(int level, CellTypeChooserSize size, CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryGraphicsHelper *c, QueryGraphicsItem* parent, QueryComponent* owner, int index):
        QueryGraphicsItem(level, parent, c, owner),
        m_defaultType(defaultType), m_allowedTypes(allowedTypes), m_currentType(defaultType), m_rect(0), m_size (size), m_index(index)
{
}

QGraphicsItemGroup* CellTypeChooser::render(int& x, int& y, int& w, int &h)
{
    int size = (m_size == CELLTYPECHOOSER_REGULAR)?(CELL_SIZE+1):(CELL_SIZE*2+1);
    QRect r(x, y, size-1, size-1);
    m_helper->addNewHotCell(this, r);
    addToGroup(m_rect = new QGraphicsRectItem(r));
    QGraphicsPixmapItem* typeIcon = 0;

    switch(m_currentType)
    {
    case CELLTYPE_TABLE:
        if(Workspace::getInstance()->currentProjectIsOop())
        {
            typeIcon = new QGraphicsPixmapItem(IconFactory::getTabinstIcon().pixmap(size,size), this);
        }
        else
        {
            typeIcon = new QGraphicsPixmapItem(IconFactory::getTablesIcon().pixmap(size,size), this);
        }
        break;

    case CELLTYPE_NOTHING:
        typeIcon = new QGraphicsPixmapItem(IconFactory::getEmptyIcon().pixmap(size,size), this);
        break;
    case CELLTYPE_NOT:
        if(m_size == CELLTYPECHOOSER_REGULAR)typeIcon = new QGraphicsPixmapItem(IconFactory::getNotIcon().pixmap(size,size), this);
        else typeIcon = new QGraphicsPixmapItem(IconFactory::getBigNotIcon().pixmap(size,size), this);
        break;
    case CELLTYPE_NEGATE:
        if(m_size == CELLTYPECHOOSER_REGULAR)typeIcon = new QGraphicsPixmapItem(IconFactory::getNegIcon().pixmap(size,size), this);
        else typeIcon = new QGraphicsPixmapItem(IconFactory::getBigNegIcon().pixmap(size,size), this);
        break;
    case CELLTYPE_MINUS:
        if(m_size == CELLTYPECHOOSER_REGULAR)typeIcon = new QGraphicsPixmapItem(IconFactory::getMinusIcon().pixmap(size,size), this);
        else typeIcon = new QGraphicsPixmapItem(IconFactory::getBigMinusIcon().pixmap(size,size), this);
        break;
    case CELLTYPE_FUNCTION:
        typeIcon = new QGraphicsPixmapItem(IconFactory::getFunctionIcon().pixmap(CELL_SIZE+1,CELL_SIZE+1), this);
        break;

    }

    if(typeIcon)
    {
        typeIcon->setX(x);
        typeIcon->setY(y);
    }

    return this;
}

void CellTypeChooser::updateWidth(int newWidth)
{
}

void CellTypeChooser::mouseMove(int x, int y)
{
    QPen thick;
    thick.setWidth(2);
    m_rect->setPen(thick);
}

void CellTypeChooser::mouseLeft(int x, int y)
{
    QPen normal;
    normal.setWidth(1);
    m_rect->setPen(normal);
}

void CellTypeChooser::mousePress(int x, int y)
{
    QPoint a = scene()->views().at(0)->mapToGlobal((m_rect->mapToScene(m_rect->boundingRect().bottomLeft().toPoint())).toPoint() ) ;
    QString selected = m_helper->presentList(a.x() + 2-scene()->views().at(0)->horizontalScrollBar()->sliderPosition()
                                             , a.y() - scene()->views().at(0)->verticalScrollBar()->sliderPosition(), m_allowedTypes);
    if(selected.length() == 0) return;

    m_owner->handleAction(selected + (selected[0]=='@'?"":(m_index>=0?QString::number(m_index):"")), m_owner);
}
