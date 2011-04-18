#include "CellJoinCommand.h"
#include "CellQuerySmallOptionsBox.h"
#include "QueryTextInputItem.h"
#include "QueryGraphicsScene.h"
#include "QueryGraphicsView.h"
#include "SelectQueryAsComponent.h"
#include "CellTypeChooser.h"
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

CellQuerySmallOptionsBox* CellJoinCommand::provideOptionsBox(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner)
{
    return 0;
}

void CellJoinCommand::onClose()
{
    m_owner->onClose();
}

QGraphicsItemGroup* CellJoinCommand::render(int& x, int& y, int& w, int &h)
{
    int lx = x + 2;
    int ly = y + 2;

    QGraphicsItemGroup* grp = new QGraphicsItemGroup();
    grp->addToGroup(CellCommand::render(x,y,w,h));
    int tw = m_txt->boundingRect().width();
    m_txt->setX(m_txt->x() + CELL_SIZE);
    if(w<tw + 2) w = tw + 2;

    QSet<CellTypeChooser::CellTypeChooserType> allowedTypes;
    allowedTypes.insert(CellTypeChooser::CELLTYPE_TABLE);

    m_chooser = new CellTypeChooser(CellTypeChooser::CELLTYPE_TABLE, allowedTypes, m_helper, this, m_owner);
    m_chooser->render(lx, ly, w, h);
    grp->addToGroup(m_chooser);

    return grp;
}

void CellJoinCommand::updateWidth(int newWidth)
{
    //CellCommand::updateWidth(newWidth + CELL_SIZE * 3);
    QRect newRect(m_txt->boundingRect().left(), m_txt->boundingRect().top(),
                  max(m_txt->boundingRect().width(), newWidth - m_txt->boundingRect().left()),
                  m_txt->boundingRect().height());


    CellCommand::updateWidth(newRect.width() + newRect.left() - CELL_SIZE);

    if(hasClose())
    {
        m_close->updateWidth(newRect.left() + newRect.width() + 8);
    }

    //m_helper->addNewHotCell(this, m_txt->boundingRect().toRect());
}

void CellJoinCommand::mouseMove(int x, int y)
{
}

void CellJoinCommand::mouseLeft(int x, int y)
{
}

void CellJoinCommand::mousePress(int x, int y)
{
}

