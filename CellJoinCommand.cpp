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
    m_txt->setX(m_txt->x() + CELL_SIZE);

    int tw = m_txt->boundingRect().width() + CELL_SIZE * 2;
    if(w<tw) w = tw ;

    QSet<CellTypeChooser::CellTypeChooserType> allowedTypes;
    allowedTypes.insert(CellTypeChooser::CELLTYPE_TABLE);

    m_chooser = new CellTypeChooser(CellTypeChooser::CELLTYPECHOOSER_REGULAR, CellTypeChooser::CELLTYPE_TABLE, allowedTypes, m_helper, this, m_owner);
    m_chooser->render(lx, ly, w, h);
    grp->addToGroup(m_chooser);

    return grp;
}

void CellJoinCommand::updateWidth(int newWidth)
{
    m_level ++;
    CellCommand::updateWidth(newWidth - CELL_SIZE*5/2);
    m_level --;
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

