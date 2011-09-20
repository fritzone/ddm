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
    CellSelectCommand* getSelect() const
    {
        return m_select;
    }
    CellFromCommand* getFrom() const
    {
        return m_from;
    }
    CellWhereCommand* getWhere() const
    {
        return m_where;
    }
    CellGroupByCommand* getGroupBy() const
    {
        return m_groupby;
    }
    CellWhereCommand* getHaving() const
    {
        return m_having;
    }
    CellOrderByCommand* getOrderBy() const
    {
        return m_orderBy;
    }
    void addFromGraphicsItem(QueryGraphicsItem*);
    void addSelectGraphicsItem(QueryGraphicsItem*);
    void addWhereGraphicsItem(QueryGraphicsItem*);
    void addGroupByGraphicsItem(QueryGraphicsItem*);
    void addHavingGraphicsItem(QueryGraphicsItem*);
    void addOrderByGraphicsItem(QueryGraphicsItem*);

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
