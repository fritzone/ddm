#ifndef QBR_CELLQUERYSMALLOPTIONSBOXCIRCLE_H
#define QBR_CELLQUERYSMALLOPTIONSBOXCIRCLE_H

#include "qbr_CellQuerySmallOptionsBox.h"

class CellQuerySmallOptionsBoxCircle: public CellQuerySmallOptionsBox
{
public:
    CellQuerySmallOptionsBoxCircle(QSet<OptionsType> types, QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    void highlight(const QPen &);
    virtual QAbstractGraphicsShapeItem* getGraphicsItem()
    {
        return m_ellipse;
    }

private:
    QGraphicsEllipseItem* m_ellipse;

};

#endif // QBR_CELLQUERYSMALLOPTIONSBOXCIRCLE_H
