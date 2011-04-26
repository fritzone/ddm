#include "CellTypeChooser.h"
#include "Workspace.h"
#include "Version.h"
#include "IconFactory.h"

#include <QPen>

CellTypeChooser::CellTypeChooser(CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryGraphicsHelper *c, QueryGraphicsItem* parent, QueryComponent* owner):
        QueryGraphicsItem(parent, c, owner),
        m_defaultType(defaultType), m_allowedTypes(allowedTypes), m_currentType(defaultType), m_rect(0)
{
}

QGraphicsItemGroup* CellTypeChooser::render(int& x, int& y, int& w, int &h)
{
    QRect r(x, y, CELL_SIZE, CELL_SIZE);
    m_helper->addNewHotCell(this, r);
    addToGroup(m_rect = new QGraphicsRectItem(r));
    QGraphicsPixmapItem* typeIcon = 0;

    switch(m_currentType)
    {
    case CELLTYPE_TABLE:
        if(Workspace::getInstance()->currentProjectIsOop())
        {
            typeIcon = new QGraphicsPixmapItem(IconFactory::getTabinstIcon().pixmap(CELL_SIZE,CELL_SIZE), this);
        }
        else
        {
            typeIcon = new QGraphicsPixmapItem(IconFactory::getTablesIcon().pixmap(CELL_SIZE,CELL_SIZE), this);
        }
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
