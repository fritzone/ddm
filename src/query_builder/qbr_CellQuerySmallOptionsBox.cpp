#include "qbr_CellQuerySmallOptionsBox.h"
#include "IconFactory.h"
#include "qbr_QueryGraphicsScene.h"
#include "Workspace.h"
#include "Version.h"
#include "strings.h"

#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollBar>

CellQuerySmallOptionsBox::CellQuerySmallOptionsBox(QSet<OptionsType> types,
                                                   QueryGraphicsHelper* c,
                                                   int level,
                                                   QueryGraphicsItem* parent,
                                                   QueryComponent* owner):
        QueryGraphicsItem(level, parent, c, owner),
        m_types(types)
{
}

void CellQuerySmallOptionsBox::mouseMove(int /*x*/, int /*y*/)
{
    CellQuerySmallOptionsBox::OptionsList op = prepareOptions();
    if(op.text.size() == 0)
    {
        return;
    }

    QPen thickPen;
    thickPen.setWidth(2);
    highlight(thickPen);
}

void CellQuerySmallOptionsBox::mouseLeft(int /*x*/, int /*y*/)
{
    QPen normalPen;
    normalPen.setWidth(1);
    highlight(normalPen);
}

CellQuerySmallOptionsBox::OptionsList CellQuerySmallOptionsBox::prepareOptions()
{
    QStringList text;
    QList<QIcon> icons;

    if(m_types.contains(OPTIONS_NEW_TABLE))
    {
        bool haveTables = false;
        if(Workspace::getInstance()->currentProjectIsOop())
        {
            haveTables = Workspace::getInstance()->workingVersion()->getTableInstances().size() > 0;
        }
        else
        {
            haveTables = Workspace::getInstance()->workingVersion()->getTables().size() > 0;
        }

        if(haveTables)
        {
            text.append(NEW_TABLE);
            icons.append(IconFactory::getTabinstIcon());
        }
    }

    if(m_types.contains(OPTIONS_NEW_COLUMN))
    {
        text.append(NEW_COLUMN);
        icons.append(QIcon(IconFactory::getColumnIcon().pixmap(16, 16)));
    }

    if(m_types.contains(OPTIONS_DUPLICATE))
    {
        text.append(STR_DUPLICATE);
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

    if(m_types.contains(OPTIONS_NEW_WHERE_EXPR))
    {
        text.append(ADD_WHERE_EXPRESSION);
        icons.append(IconFactory::getEmptyIcon());
    }
    if(m_types.contains(OPTIONS_NEW_WHERE_EXPR_OR))
    {
        text.append(ADD_WHERE_EXPRESSION_OR);
        icons.append(IconFactory::getEmptyIcon());
    }
    if(m_types.contains(OPTIONS_NEW_WHERE_EXPR_AND))
    {
        text.append(ADD_WHERE_EXPRESSION_AND);
        icons.append(IconFactory::getEmptyIcon());
    }

    CellQuerySmallOptionsBox::OptionsList op;
    op.text = text;
    op.icons = icons;

    return op;

}

void CellQuerySmallOptionsBox::mousePress(int x, int y)
{
    QPen normal;
    normal.setWidth(1);
    highlight(normal);

    CellQuerySmallOptionsBox::OptionsList op = prepareOptions();
    if(op.text.size() == 0)
    {
        return;
    }

    QString selected = "";
    QAbstractGraphicsShapeItem* item = getGraphicsItem();
    QGraphicsScene* sc = item->scene();
    QList <QGraphicsView*> views =  sc->views();
    if(views.size() > 0)
    {
        QGraphicsView* v = views.at(0);
        if(v)
        {
            QPoint a = v->mapToGlobal((item->mapToScene(item->boundingRect().bottomLeft().toPoint())).toPoint() ) ;
            selected = m_helper->presentList(a.x() + 2 - v->horizontalScrollBar()->sliderPosition(), a.y() - v->verticalScrollBar()->sliderPosition(), op.text, op.icons);
        }
        else
        {
            selected = m_helper->presentList(x, y, op.text, op.icons);
        }
    }
    else
    {
        selected = m_helper->presentList(x, y, op.text, op.icons);
    }


    if(selected.length() == 0) return;
    QueryComponent* referring = 0;
    if(m_parent) referring = m_parent->getOwner();

    m_owner->handleAction(selected, referring);
}
