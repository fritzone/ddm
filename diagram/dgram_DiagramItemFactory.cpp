#include "dgram_DiagramItemFactory.h"
#include "core_Table.h"
#include "ForeignKey.h"
#include "core_Column.h"
#include "dgram_DraggableGraphicsItem.h"
#include "dgram_DraggableGraphicsItemForForeignKey.h"
#include "IconFactory.h"
#include "core_UserDataType.h"
#include "Configuration.h"

#include <QPen>

DiagramItemFactory::DiagramItemFactory()
{
}

DraggableGraphicsViewItem* DiagramItemFactory::getDiagramEntityForTable(Table *t)
{
    DraggableGraphicsViewItem* grp = new DraggableGraphicsViewItem(t);

    QGraphicsTextItem* txtName = new QGraphicsTextItem(t->getName(), grp);
    txtName->setPos(0,0);
    txtName->setToolTip(t->getName());
    qreal py = txtName->boundingRect().height();
    qreal headerHeight = py;
    qreal maxX = txtName->boundingRect().right();
    QStringList fullC = t->fullColumns();
    for(int i=0; i<fullC.size(); i++)
    {
        Column* col = t->getColumn(fullC[i]);
        if(!col)
        {
            col = t->getColumnFromParents(fullC[i]);
            if(!col)
            {
                return 0; // Shouldn't ever happen
            }
        }


        QGraphicsTextItem* txtColName = new QGraphicsTextItem(fullC[i], grp);
        txtColName->setY(py);

        QGraphicsPixmapItem* icon = new QGraphicsPixmapItem(IconFactory::getIconForDataType(col->getDataType()->getType()).pixmap(16,16), grp);
        if(col->isPk())
        {
            QGraphicsPixmapItem* keyicon = new QGraphicsPixmapItem(IconFactory::getKeyIcon().pixmap(16,16), grp);
            keyicon->setY(py + 2);
            keyicon->setX(2);
        }

        txtColName->setX(36);
        icon->setY(py + 2);
        icon->setX(20);
        py+=txtColName->boundingRect().height();
        maxX = txtColName->boundingRect().right() + 20 > maxX ? txtColName->boundingRect().right() + 20:maxX;
    }

    maxX += 30;
    qreal finMaxX = maxX;
    py = headerHeight;

    for(int i=0; i<fullC.size(); i++)
    {
        Column* col = t->getColumn(fullC[i]);
        if(!col)
        {
            col = t->getColumnFromParents(fullC[i]);
            if(!col)
            {
                return 0; // Shouldn't ever happen
            }
        }

        QGraphicsTextItem* txtColName = new QGraphicsTextItem(col->getDataType()->sqlAsString(), grp);
        txtColName->setY(py);
        txtColName->setX(maxX);
        py += txtColName->boundingRect().height();
        finMaxX = txtColName->boundingRect().width() + maxX > finMaxX?txtColName->boundingRect().width() + maxX:finMaxX;
    }

    maxX = finMaxX;

    QGraphicsRectItem* rect = new QGraphicsRectItem(0,0, maxX, py, grp);
    QPen tPen;

    if(Configuration::instance().drawTableTypes()) rect->setPen(tPen);

    grp->setLastX(maxX);
    grp->setLastY(py);
    QGraphicsRectItem* recta = new QGraphicsRectItem(0,0, maxX, headerHeight, grp);
    if(Configuration::instance().drawTableTypes()) recta->setPen(tPen);
    QBrush brush (Qt::lightGray);
    QBrush wbrush (Qt::white);
    recta->setBrush(brush);
    rect->setBrush(wbrush);
    rect->setZValue(-1);
    recta->setZValue(0.5);
    txtName->setZValue(1);
    grp->setToolTip(t->getDescription());
    return grp;
}


/**                                            /\
 *                                            /  \
 * This should prepare a graphics like: __ __/name\__ ____
 *                                           \    /
 *                                            \  /
 *                                             \/
 * and the two lines after and before the space should be movable so that they move when the user moves the tables
 */
DraggableGraphicsViewItemForForeignKey* DiagramItemFactory::getDiagramEntityForForeignKey(ForeignKey* fk)
{
    DraggableGraphicsViewItemForForeignKey* grp = new DraggableGraphicsViewItemForForeignKey();
    QGraphicsTextItem* txtName = new QGraphicsTextItem(fk->getName(), grp);
    QRectF boundingForName = txtName->boundingRect();
    int rombX1 = boundingForName.left() - 5;
    int rombY1 = (boundingForName.bottom() + boundingForName.top()) / 2;
    int rombX2 = (boundingForName.right() + boundingForName.left()) / 2;
    int rombY2 = boundingForName.top() - (boundingForName.right() + boundingForName.left()) / 2;
    int rombX3 = boundingForName.right() + 5;
    int rombY3 = (boundingForName.bottom() + boundingForName.top()) / 2;
    int rombX4 = (boundingForName.right() + boundingForName.left()) / 2;
    int rombY4 = boundingForName.bottom() + (boundingForName.right() + boundingForName.left()) / 2;
    QGraphicsLineItem* line1 = new QGraphicsLineItem(rombX1, rombY1, rombX2, rombY2, grp);
    QGraphicsLineItem* line2 = new QGraphicsLineItem(rombX2, rombY2, rombX3, rombY3, grp);
    QGraphicsLineItem* line3 = new QGraphicsLineItem(rombX3, rombY3, rombX4, rombY4, grp);
    QGraphicsLineItem* line4 = new QGraphicsLineItem(rombX4, rombY4, rombX1, rombY1, grp);


    grp->setLeftPoint(QPointF(rombX1, rombY1));
    grp->setTopPoint(QPointF(rombX2, rombY2));
    grp->setRightPoint(QPointF(rombX3, rombY3));
    grp->setBottomPoint(QPointF(rombX4, rombY4));

    grp->setToolTip(fk->getDescriptiveText());
    txtName->setToolTip(fk->getDescriptiveText());
    line1->setToolTip(fk->getDescriptiveText());
    line2->setToolTip(fk->getDescriptiveText());
    line3->setToolTip(fk->getDescriptiveText());
    line4->setToolTip(fk->getDescriptiveText());

    return grp;
}
