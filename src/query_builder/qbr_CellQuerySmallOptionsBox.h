#ifndef CELLQUERYSMALLOPTIONSBOX_H
#define CELLQUERYSMALLOPTIONSBOX_H

#include "qbr_QueryGraphicsItem.h"
#include "qbr_QueryGraphicsHelper.h"
#include "qbr_QueryComponents.h"
#include "qbr_OptionTypes.h"

#include <QSet>

class CellQuerySmallOptionsBox : public QueryGraphicsItem
{
public:

    struct OptionsList
    {
        QStringList text;
        QList<QIcon> icons;
    };

    // TODO: This is a nice example of how not to implement something, it would be a perfect "a la carte" polymorphism example
    enum OptionsBoxShape
    {
        SHAPE_RECT    = 1,
        SHAPE_CIRCLE  = 2,
        SHAPE_DIAMOND = 3
    };

    CellQuerySmallOptionsBox(QSet<OptionsType> types, QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner, OptionsBoxShape shape);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int){}
    virtual void mousePress(int x, int y);
    virtual void mouseMove(int x, int y);
    virtual void mouseLeft(int x, int y);
    virtual void onClose(){}

private:

    OptionsList prepareOptions();

private:
    QGraphicsRectItem* m_box;
    QGraphicsEllipseItem* m_ellipse;
    QGraphicsPolygonItem* m_diamond;
    QSet<OptionsType> m_types;
    OptionsBoxShape m_shape;
};

#endif // CELLQUERYSMALLOPTIONSBOX_H