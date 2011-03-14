#include "CellAsCommand.h"
#include "CellQuerySmallOptionsBox.h"
#include "QueryTextInputItem.h"

#include <QPen>

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
    m_textItem->setHtml("A");
    m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
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
    m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
}
