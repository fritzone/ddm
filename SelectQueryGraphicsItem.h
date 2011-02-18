#ifndef SELECTQUERYGRAPHICSITEM_H
#define SELECTQUERYGRAPHICSITEM_H

#include "QueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"
#include "CellClose.h"

class SelectQueryGraphicsItem : public QueryGraphicsItem
{
public:
    SelectQueryGraphicsItem(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){}
    virtual void mouseMove(int x, int y){}
    virtual void mouseLeft(int x, int y){}
    virtual void onClose(){}
    QueryGraphicsItem* getSelect()
    {
        return m_select;
    }
    QueryGraphicsItem* getFrom()
    {
        return m_from;
    }
    void addFromGraphicsItem(QueryGraphicsItem*);

private:
    QueryGraphicsItem* m_select;
    QueryGraphicsItem* m_from;
    QueryGraphicsItem* m_where;
    QGraphicsRectItem* m_frameRect;
    int m_level;
};

#endif // SELECTQUERYGRAPHICSITEM_H
