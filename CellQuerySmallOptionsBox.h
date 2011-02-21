#ifndef CELLQUERYSMALLOPTIONSBOX_H
#define CELLQUERYSMALLOPTIONSBOX_H

#include "QueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"
#include "QueryComponents.h"

#include <QSet>

class CellQuerySmallOptionsBox : public QueryGraphicsItem
{
public:
    enum OptionsType
    {
        OPTIONS_DUPLICATE = 0,
        OPTIONS_NEW_TABLE = 1,
        OPTIONS_NEW_SUBQUERY = 2,

    };

    CellQuerySmallOptionsBox(QSet<OptionsType> types, QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth){}
    virtual void mousePress(int x, int y);
    virtual void mouseMove(int x, int y);
    virtual void mouseLeft(int x, int y);
    virtual void onClose(){}
private:
    QGraphicsRectItem* m_box;
    QSet<OptionsType> m_types;
};

#endif // CELLQUERYSMALLOPTIONSBOX_H
