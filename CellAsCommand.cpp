#include "CellAsCommand.h"
#include "CellQuerySmallOptionsBox.h"

CellAsCommand::CellAsCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) :
        CellCommand(c, level, parent, owner)
{
}

QBrush CellAsCommand::getCellBrush()
{
    static QBrush selectBrush = QBrush(QColor(214, 183, 88));
    return selectBrush;
}

CellQuerySmallOptionsBox* CellAsCommand::provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner)
{
    return 0;
}

void CellAsCommand::onClose()
{
    m_owner->onClose();
}

QGraphicsItemGroup* CellAsCommand::render(int& x, int& y, int& w, int &h)
{
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();
    grp->addToGroup(CellCommand::render(x,y,w,h));
    int tw = m_txt->boundingRect().width();
    QRect t (x +tw + 2, y + 2, 10, 14);
    grp->addToGroup(new QGraphicsRectItem(t));
    return grp;
}
