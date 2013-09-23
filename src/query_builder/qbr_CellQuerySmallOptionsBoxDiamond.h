#ifndef QBR_CELLQUERYSMALLOPTIONSBOXDIAMOND_H
#define QBR_CELLQUERYSMALLOPTIONSBOXDIAMOND_H

#include "qbr_CellQuerySmallOptionsBox.h"

class CellQuerySmallOptionsBoxDiamond : public CellQuerySmallOptionsBox
{
public:
    CellQuerySmallOptionsBoxDiamond(QSet<OptionsType> types, QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    void highlight(const QPen &);
    virtual QAbstractGraphicsShapeItem* getGraphicsItem()
    {
        return m_diamond;
    }

private:
    QGraphicsPolygonItem* m_diamond;
};

#endif // QBR_CELLQUERYSMALLOPTIONSBOXDIAMOND_H
