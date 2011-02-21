#include "CellQuerySmallOptionsBox.h"
#include "IconFactory.h"
#include "QueryGraphicsScene.h"

#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>

CellQuerySmallOptionsBox::CellQuerySmallOptionsBox(QSet<OptionsType> types, QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner):
        QueryGraphicsItem(parent, c, owner), m_types(types)

{
}

QGraphicsItemGroup* CellQuerySmallOptionsBox::render(int& x, int& y, int& w, int &h)
{
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    m_box = new QGraphicsRectItem(x, y, 10, 10, grp);
    m_helper->addNewHotCell(this, m_box->rect().toRect());
    return grp;
}

void CellQuerySmallOptionsBox::mouseMove(int x, int y)
{
    QPen thick;
    thick.setWidth(2);
    m_box->setPen(thick);
}

void CellQuerySmallOptionsBox::mouseLeft(int x, int y)
{
    QPen normal;
    normal.setWidth(1);
    m_box->setPen(normal);
}

void CellQuerySmallOptionsBox::mousePress(int x, int y)
{
    QPen normal;
    normal.setWidth(1);
    m_box->setPen(normal);

    QStringList text;
    QList<QIcon> icons;

    if(m_types.contains(OPTIONS_NEW_TABLE))
    {
        text.append(NEW_TABLE);
        icons.append(IconFactory::getTabinstIcon());
    }

    QueryGraphicsScene* sc = dynamic_cast<QueryGraphicsScene*>(m_box->scene());
    QPoint a = sc->views().at(0)->mapToGlobal((m_box->mapToScene(m_box->boundingRect().bottomLeft().toPoint())).toPoint() );
    QString selected = m_helper->presentList(a.x() + 2, a.y() + 2, text, icons);
    if(selected.length() == 0) return;
    m_owner->handleAction(selected);
}
