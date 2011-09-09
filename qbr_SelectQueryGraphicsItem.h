#ifndef SELECTQUERYGRAPHICSITEM_H
#define SELECTQUERYGRAPHICSITEM_H

#include "qbr_QueryGraphicsItem.h"
#include "qbr_QueryGraphicsHelper.h"
#include "qbr_CellClose.h"

class CellSelectCommand;
class CellFromCommand;
class CellWhereCommand;
class CellGroupByCommand;
class CellAsCommand;
class CellOrderByCommand;

class SelectQueryGraphicsItem : public QueryGraphicsItem
{
public:
    SelectQueryGraphicsItem(QueryGraphicsHelper* c, int level, QueryGraphicsItem* parent, QueryComponent* owner);
    ~SelectQueryGraphicsItem() {}

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){}
    virtual void mouseMove(int x, int y){}
    virtual void mouseLeft(int x, int y){}
    virtual void onClose();
    CellSelectCommand* getSelect()
    {
        return m_select;
    }
    CellFromCommand* getFrom()
    {
        return m_from;
    }
    void addFromGraphicsItem(QueryGraphicsItem*);
    void addSelectGraphicsItem(QueryGraphicsItem*);
    void addWhereGraphicsItem(QueryGraphicsItem*);

    void createSelectCell(QueryComponent*);
    void createFromCell(QueryComponent*);
    void createWhereCell(QueryComponent*);
    void createGroupByCell(QueryComponent*);
    void createHavingCell(QueryComponent*);
    void createAsCell(QueryComponent*);
    void createOrderByCell(QueryComponent*);

private:
    CellSelectCommand* m_select;
    CellFromCommand* m_from;
    CellWhereCommand* m_where;
    CellGroupByCommand* m_groupby;
    CellWhereCommand* m_having;
    CellAsCommand* m_as;
    CellOrderByCommand* m_orderBy;

    QGraphicsRectItem* m_frameRect;
};

#endif // SELECTQUERYGRAPHICSITEM_H
