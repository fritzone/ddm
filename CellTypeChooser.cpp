#include "CellTypeChooser.h"
#include "Workspace.h"
#include "Version.h"
#include "IconFactory.h"
#include "Cell.h"

#include <QPen>

CellTypeChooser::CellTypeChooser(CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryComponents *c, QueryGraphicsItem* parent):
        m_defaultType(defaultType), m_allowedTypes(allowedTypes), m_currentType(defaultType), m_comps(c), m_rect(0), m_parent(parent)
{
}

QGraphicsItemGroup* CellTypeChooser::render(int& x, int& y, int& w, int &h)
{
    QRect r(x, y, Cell::CELL_SIZE, Cell::CELL_SIZE);
    m_comps->addNewHotCell(this, r);
    addToGroup(m_rect = new QGraphicsRectItem(r));
    QGraphicsPixmapItem* typeIcon = 0;

    switch(m_currentType)
    {
    case CELLTYPE_TABLE:
        if(Workspace::getInstance()->currentProjectIsOop())
        {
            typeIcon = new QGraphicsPixmapItem(IconFactory::getTabinstIcon().pixmap(16,16), this);
        }
        else
        {
            typeIcon = new QGraphicsPixmapItem(IconFactory::getTablesIcon().pixmap(16,16), this);
        }
        break;
    }

    if(typeIcon)
    {
        typeIcon->setX(x + 2);
        typeIcon->setY(y + 2);
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
