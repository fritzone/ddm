#include "CellTableChooser.h"
#include "Cell.h"
#include "QueryItemListDialog.h"
#include "QueryGraphicsScene.h"

#include <QBrush>
#include <QFont>
#include <QPen>

#include <QDebug>

CellTableChooser::CellTableChooser(const QString& name, QueryComponents*c) : m_name(name), m_comps(c)
{
}


QGraphicsItemGroup* CellTableChooser::render(int &x, int &y, int &w, int &h)
{
    int lx = x;
    int ly = y;
    QGraphicsTextItem* txt = new QGraphicsTextItem(m_name, this);
    txt->setPos(lx + Cell::CELL_SIZE + 5, ly);

    QRect rct(lx + Cell::CELL_SIZE + 3, ly, txt->boundingRect().width(), 30);
    m_frame = new QGraphicsRectItem(rct, this);
    m_frame->setBrush(QBrush(Qt::white));
    txt->setZValue(1);
    m_frame->setZValue(0);
    txt->setFont(QFont("Arial", 14, 2));
    return this;
}

void CellTableChooser::updateWidth(int newWidth)
{
    QRect newRect(m_frame->boundingRect().left(), m_frame->boundingRect().top(), newWidth, m_frame->boundingRect().height());
    m_frame->setRect(newRect);
    m_comps->addNewHotCell(this, newRect);

}

void CellTableChooser::mousePress(int x, int y)
{
    QueryItemListDialog* dlg = new QueryItemListDialog((QWidget*)this->scene()->parent());
    dlg->setWindowFlags(Qt::FramelessWindowHint);
    dlg->move(x, y);
    dlg->setModal(true);
    dlg->show();
}

void CellTableChooser::mouseMove(int x, int y)
{
    QPen thick;
    thick.setWidth(2);
    m_frame->setPen(thick);
}

void CellTableChooser::mouseLeft(int x, int y)
{
    QPen normal;
    normal.setWidth(1);
    m_frame->setPen(normal);
}
