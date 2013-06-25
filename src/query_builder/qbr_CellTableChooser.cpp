#include "qbr_CellTableChooser.h"
#include "qbr_QueryGraphicsScene.h"
#include "qbr_TableQueryComponent.h"
#include "qbr_ColorProvider.h"

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QScrollBar>

#include <QDebug>

CellTableChooser::CellTableChooser(const QString& name, int level, QueryGraphicsHelper*c, QueryGraphicsItem* parent, QueryComponent* owner) :
        QueryGraphicsItem(level, parent, c, owner),
        m_name(name)
{
    m_level = level;
}

QGraphicsItemGroup* CellTableChooser::render(int &x, int &y, int &w, int &h)
{
    int lx = x;
    int ly = y;

    // the name of the table
    m_txt = new QGraphicsTextItem(m_name, this);
    m_txt->setPos(lx + 4, ly + 2);
    m_txt->setFont( ColorProvider::getfontForTableName() );
    m_txt->setZValue(1);

    int textWidth =m_txt->boundingRect().width();

    // the white rect below the table name
    QRect rct(lx + 2,
              ly + 2,
              textWidth,
              m_txt->boundingRect().height());

    m_frame = new QGraphicsRectItem(rct, this);
    m_frame->setBrush( QBrush(Qt::white) );
    m_frame->setZValue(0);

    h = m_frame->boundingRect().height();

    int tw = textWidth + CHILDREN_ALIGNMENT * (m_level+1) + m_txt->boundingRect().left() + 2 * CELL_SIZE;

    w = w<tw ? tw : w;

    return this;
}

void CellTableChooser::updateWidth(int newWidth)
{
    QRect newRect(m_frame->boundingRect().left(), m_frame->boundingRect().top(), newWidth + 2, m_frame->boundingRect().height());
    m_frame->setRect(newRect);
    m_helper->addNewHotCell(this, newRect);

}

void CellTableChooser::mousePress(int /*x*/, int /*y*/)
{
    QPoint a = scene()->views().at(0)->mapToGlobal((m_frame->mapToScene(m_frame->boundingRect().bottomLeft().toPoint())).toPoint() ) ;
    QString selected = m_helper->presentList(a.x() + 2-scene()->views().at(0)->horizontalScrollBar()->sliderPosition(),
                                             a.y() - scene()->views().at(0)->verticalScrollBar()->sliderPosition(),
                                             QueryGraphicsHelper::LIST_TABLES);
    if(selected.length() == 0)
    {
        return;
    }

    QString oldName = m_name;
    m_name = selected;
    m_txt->setPlainText(m_name);
    TableQueryComponent* tc = dynamic_cast<TableQueryComponent*>(m_parent->getParent()->getOwner());
    if(tc != 0)
    {
        tc->setTable(m_name);

        if(oldName != m_name)
        {
            // now call upon the query helper and notify the other components, that
            // a table was removed

            m_helper->tableRemoved(oldName);
        }

        m_helper->triggerReRender();
    }
}

void CellTableChooser::mouseMove(int /*x*/, int /*y*/)
{
    QPen thick;
    thick.setWidth(2);
    m_frame->setPen(thick);
}

void CellTableChooser::mouseLeft(int /*x*/, int /*y*/)
{
    QPen normal;
    normal.setWidth(1);
    m_frame->setPen(normal);
}
