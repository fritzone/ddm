#include "CellQuerySmallOptionsBox.h"
#include "IconFactory.h"
#include "QueryGraphicsScene.h"

#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollBar>

CellQuerySmallOptionsBox::CellQuerySmallOptionsBox(QSet<OptionsType> types, QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner, OptionsBoxShape shape):
        QueryGraphicsItem(parent, c, owner), m_types(types), m_shape(shape), m_box(0), m_ellipse(0), m_diamond(0)

{
}

QGraphicsItemGroup* CellQuerySmallOptionsBox::render(int& x, int& y, int& w, int &h)
{
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();

    if(m_shape == SHAPE_RECT)
    {
        m_box = new QGraphicsRectItem(x, y, 10, 10, grp);
        m_helper->addNewHotCell(this, m_box->rect().toRect());
    }
    else
    if(m_shape == SHAPE_CIRCLE)
    {
        m_ellipse = new QGraphicsEllipseItem(x, y, 10, 10, grp);
        m_helper->addNewHotCell(this, m_ellipse->rect().toRect());
    }
    else
    {
        QPointF p1(x, y+5); QPointF p2(x+5, y + 10); QPointF p3(x+10, y+5); QPointF p4(x+5, y);
        QVector <QPointF> v; v << p1 << p2 << p3 << p4;
        QPolygonF po(v);
        m_diamond = new QGraphicsPolygonItem(po, grp);
        m_helper->addNewHotCell(this, (new QGraphicsRectItem(x, y, 10, 10))->rect().toRect());
    }

    new QGraphicsLineItem(x, y + 5, x + 10, y+5, grp);
    new QGraphicsLineItem(x + 5, y, x + 5, y+10, grp);
    if(m_shape == SHAPE_RECT)
    {
        new QGraphicsLineItem(x + 5, y + 5, x + 5, y+16, grp);  // bottom one
    }

    return grp;
}

void CellQuerySmallOptionsBox::mouseMove(int x, int y)
{
    QPen thick;
    thick.setWidth(2);
    if(m_shape == SHAPE_RECT) m_box->setPen(thick);
    else if(m_shape == SHAPE_CIRCLE) m_ellipse->setPen(thick);
    else m_diamond->setPen(thick);
}

void CellQuerySmallOptionsBox::mouseLeft(int x, int y)
{
    QPen normal;
    normal.setWidth(1);
    if(m_shape == SHAPE_RECT) m_box->setPen(normal);
    else if(m_shape == SHAPE_CIRCLE) m_ellipse->setPen(normal);
    else m_diamond->setPen(normal);
}

void CellQuerySmallOptionsBox::mousePress(int x, int y)
{
    QPen normal;
    normal.setWidth(1);
    if(m_shape == SHAPE_RECT) m_box->setPen(normal);
    else if(m_shape == SHAPE_CIRCLE) m_ellipse->setPen(normal);
    else m_diamond->setPen(normal);

    QStringList text;
    QList<QIcon> icons;

    if(m_types.contains(OPTIONS_NEW_TABLE))
    {
        text.append(NEW_TABLE);
        icons.append(IconFactory::getTabinstIcon());
    }

    if(m_types.contains(OPTIONS_DUPLICATE))
    {
        text.append(DUPLICATE);
        icons.append(IconFactory::getTabinstIcon());
    }

    if(m_types.contains(OPTIONS_ADD_FROM))
    {
        text.append(ADD_FROM);
        icons.append(IconFactory::getEmptyIcon());
    }
    if(m_types.contains(OPTIONS_ADD_WHERE))
    {
        text.append(ADD_WHERE);
        icons.append(IconFactory::getEmptyIcon());
    }
    if(m_types.contains(OPTIONS_NEW_SUBQUERY))
    {
        text.append(NEW_SUBQUERY);
        icons.append(IconFactory::getEmptyIcon());
    }

    if(m_types.contains(OPTIONS_ADD_GROUPBY))
    {
        text.append(ADD_GROUPBY);
        icons.append(IconFactory::getEmptyIcon());
    }

    if(m_types.contains(OPTIONS_ADD_HAVING))
    {
        text.append(ADD_HAVING);
        icons.append(IconFactory::getEmptyIcon());
    }

    if(m_types.contains(OPTIONS_ADD_ORDERBY))
    {
        text.append(ADD_ORDERBY);
        icons.append(IconFactory::getEmptyIcon());
    }

    if(m_types.contains(OPTIONS_ADD_JOIN))
    {
        text.append(ADD_JOIN);
        icons.append(IconFactory::getEmptyIcon());
    }

    if(m_types.contains(OPTIONS_AS))
    {
        text.append(ADD_ALIAS);
        icons.append(IconFactory::getEmptyIcon());
    }

    QString selected = "";
    QAbstractGraphicsShapeItem* item = 0;
    if(m_shape == SHAPE_RECT)item = m_box; else if(m_shape == SHAPE_CIRCLE) item = m_ellipse; else item = m_diamond;

    QGraphicsScene* sc = item->scene();
    QList <QGraphicsView*> views =  sc->views();
    if(views.size() > 0)
    {
        QGraphicsView* v = views.at(0);
        if(v)
        {
            QPoint a = v->mapToGlobal((item->mapToScene(item->boundingRect().bottomLeft().toPoint())).toPoint() ) ;
            selected = m_helper->presentList(a.x() + 2 - v->horizontalScrollBar()->sliderPosition(), a.y() - v->verticalScrollBar()->sliderPosition(), text, icons);
        }
        else
        {
            selected = m_helper->presentList(x, y, text, icons);
        }
    }
    else
    {
        selected = m_helper->presentList(x, y, text, icons);
    }


    if(selected.length() == 0) return;
    QueryComponent* referring = 0;
    if(m_parent) referring = m_parent->getOwner();

    m_owner->handleAction(selected, referring);
}
