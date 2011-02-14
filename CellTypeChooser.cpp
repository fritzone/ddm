#include "CellTypeChooser.h"
#include "Workspace.h"
#include "Version.h"
#include "IconFactory.h"
#include "Cell.h"

CellTypeChooser::CellTypeChooser(CellTypeChooserType defaultType, QSet<CellTypeChooserType> allowedTypes, QueryComponents *c):
        m_defaultType(defaultType), m_allowedTypes(allowedTypes), m_currentType(defaultType), m_comps(c)
{
}

QGraphicsItemGroup* CellTypeChooser::render(int& x, int& y, int& w, int &h)
{
    QRect r(x, y, Cell::CELL_SIZE, Cell::CELL_SIZE);
    m_comps->addNewHotCell(this, r);
    addToGroup(new QGraphicsRectItem(r));
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
