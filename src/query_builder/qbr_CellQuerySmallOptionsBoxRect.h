#ifndef QBR_CELLQUERYSMALLOPTIONSBOXRECT_H
#define QBR_CELLQUERYSMALLOPTIONSBOXRECT_H

#include "qbr_CellQuerySmallOptionsBox.h"

class CellQuerySmallOptionsBoxRect : public CellQuerySmallOptionsBox
{
public:
    CellQuerySmallOptionsBoxRect(QSet<OptionsType> types, QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void highlight(const QPen &pen);
    virtual QAbstractGraphicsShapeItem* getGraphicsItem()
    {
        return m_box;
    }

private:
    QGraphicsRectItem* m_box;
};

#endif // QBR_CELLQUERYSMALLOPTIONSBOXRECT_H
