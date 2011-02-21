#ifndef SELECTQUERYGRAPHICSITEM_H
#define SELECTQUERYGRAPHICSITEM_H

#include "QueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"
#include "CellClose.h"

class CellSelectCommand;
class CellFromCommand;
class CellWhereCommand;

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
    CellSelectCommand* getSelect()
    {
        return m_select;
    }
    CellFromCommand* getFrom()
    {
        return m_from;
    }
    void addFromGraphicsItem(QueryGraphicsItem*);

    void createSelectCell(QueryComponent*);
    void createFromCell(QueryComponent*);
    void createWhereCell(QueryComponent*);

private:
    CellSelectCommand* m_select;
    CellFromCommand* m_from;
    CellWhereCommand* m_where;
    QGraphicsRectItem* m_frameRect;
    int m_level;
};

#endif // SELECTQUERYGRAPHICSITEM_H
