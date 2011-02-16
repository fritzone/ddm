#include "CellTableChooser.h"
#include "Cell.h"
#include "QueryGraphicsScene.h"

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QGraphicsView>
#include <QGraphicsScene>

#include <QDebug>

CellTableChooser::CellTableChooser(const QString& name, QueryComponents*c) : m_name(name), m_comps(c)
{
}


QGraphicsItemGroup* CellTableChooser::render(int &x, int &y, int &w, int &h)
{
    int lx = x;
    int ly = y;
    m_txt = new QGraphicsTextItem(m_name, this);
    m_txt->setPos(lx + Cell::CELL_SIZE + 5, ly);
    QRect rct(lx + Cell::CELL_SIZE + 3, ly, m_txt->boundingRect().width(), 30);
    m_frame = new QGraphicsRectItem(rct, this);
    m_frame->setBrush(QBrush(Qt::white));
    m_txt->setZValue(1);
    m_frame->setZValue(0);
    m_txt->setFont(QFont("Arial", 14, 2));
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
    QPoint a = scene()->views().at(0)->mapToGlobal((m_frame->mapToScene(m_frame->boundingRect().bottomLeft().toPoint())).toPoint() );
    m_name = m_comps->presentList(a.x() + 2, a.y(), QueryComponents::LIST_TABLES);
    m_txt->setPlainText(m_name);
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
