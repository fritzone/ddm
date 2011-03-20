#include "CellAsCommand.h"
#include "CellQuerySmallOptionsBox.h"
#include "QueryTextInputItem.h"
#include "QueryGraphicsScene.h"
#include "QueryGraphicsView.h"
#include "SelectQueryAsComponent.h"

#include <QPen>
#include <QFont>
#include <QScrollBar>

CellAsCommand::CellAsCommand(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner) :
        CellCommand(c, level, parent, owner), m_strText("A")
{
    SelectQueryAsComponent* own = dynamic_cast<SelectQueryAsComponent*>(owner);
    if(own)
    {
        m_strText = own->getAs();
    }
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
    int ly = y;
    QGraphicsItemGroup* grp = new QGraphicsItemGroup();
    grp->addToGroup(CellCommand::render(x,y,w,h));
    int tw = m_txt->boundingRect().width();
    QRect t (x +tw + 2, ly + 5, 100, m_txt->boundingRect().height() - 5);
    if(w<100) w = 100;
    m_textInputRect = new QGraphicsRectItem(t);
    m_textInputRect->setBrush(QBrush(QColor(Qt::white)));
    grp->addToGroup(m_textInputRect);

    m_textItem = new QueryTextInputItem();
    grp->addToGroup(m_textItem);
    m_textItem->setPos(x +tw + 2, ly + 5);
    m_textItem->setHtml(m_strText);
    m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    m_textItem->setFont(QFont("Arial", 14, 2));
    return grp;
}

void CellAsCommand::updateWidth(int newWidth)
{
    CellCommand::updateWidth(newWidth);
    QRect t = m_textInputRect->boundingRect().toRect();
    QRect newRect(m_textInputRect->boundingRect().left(), m_textInputRect->boundingRect().top(),
                  newWidth - m_textInputRect->boundingRect().left(), m_textInputRect->boundingRect().height());
    m_textInputRect->setRect(newRect);

    m_helper->addNewHotCell(this, newRect);

}

void CellAsCommand::mouseMove(int x, int y)
{
    QPen thick;
    thick.setWidth(2);
    m_textInputRect->setPen(thick);
}

void CellAsCommand::mouseLeft(int x, int y)
{
    QPen normal;
    normal.setWidth(1);
    m_textInputRect->setPen(normal);
}

void CellAsCommand::mousePress(int x, int y)
{
    QString selected = "";
    QGraphicsScene* sc = m_textInputRect->scene();
    QList <QGraphicsView*> views =  sc->views();
    int h = 0, vv = 0;
    if(views.size() > 0)
    {
        QGraphicsView* v = views.at(0);
        if(v)
        {
            QPoint a = v->mapToGlobal((m_textInputRect->mapToScene(m_textInputRect->boundingRect().bottomLeft().toPoint())).toPoint() ) ;
            selected = m_helper->presentList(a.x() + 2 - (h = v->horizontalScrollBar()->sliderPosition()),
                                             a.y() - (vv = v->verticalScrollBar()->sliderPosition()), m_strText);
        }
        else
        {
            selected = m_helper->presentList(x, y, m_strText);
        }
    }
    else
    {
        selected = m_helper->presentList(x, y, m_strText);
    }

    if(selected.length() == 0) return;

    bool trigger = false;
    if(m_strText.length() < selected.length()) trigger = true;
    m_strText = selected;
    m_textItem->setHtml(m_strText);
    SelectQueryAsComponent* own = dynamic_cast<SelectQueryAsComponent*>(getOwner());
    if(own)
    {
        own->setAs(m_strText);
    }

    if(trigger) m_helper->triggerReRender(h, vv);

}
