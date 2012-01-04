#include "qbr_CellJoinCommand.h"
#include "qbr_CellQuerySmallOptionsBox.h"
#include "qbr_QueryTextInputItem.h"
#include "qbr_QueryGraphicsScene.h"
#include "qbr_QueryGraphicsView.h"
#include "qbr_SelectQueryAsComponent.h"
#include "qbr_CellTypeChooser.h"
#include "utils.h"

#include <QPen>
#include <QFont>
#include <QScrollBar>

#include <QDebug>

CellJoinCommand::CellJoinCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) :
        CellCommand(c, level, parent, owner)
{
}

QBrush CellJoinCommand::getCellBrush()
{
    static QBrush selectBrush = QBrush(QColor(224, 163, 188));
    return selectBrush;
}

void CellJoinCommand::onClose()
{
    m_owner->onClose();
}

QGraphicsItemGroup* CellJoinCommand::render(int& x, int& y, int& w, int &h)
{
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();
    grp->addToGroup(CellCommand::render(x,y,w,h));
    m_txt->setX(m_txt->x() + CELL_SIZE);

    int tw = m_txt->boundingRect().width() + CELL_SIZE * 2;
    if(w<tw) w = tw ;

    return grp;
}

void CellJoinCommand::updateWidth(int newWidth)
{
    m_level ++;
    CellCommand::updateWidth(newWidth);
    m_level --;
}

void CellJoinCommand::mouseMove(int /*x*/, int /*y*/)
{
}

void CellJoinCommand::mouseLeft(int /*x*/, int /*y*/)
{
}

void CellJoinCommand::mousePress(int /*x*/, int /*y*/)
{
}

