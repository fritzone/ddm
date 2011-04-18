#ifndef TABLEGRAPHICSITEM_H
#define TABLEGRAPHICSITEM_H

#include "QueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"

class Table;
class QueryComponent;
class CellAsCommand;
class CellJoinCommand;
class CellTable;
class CellWhereCommand;

class TableGraphicsItem : public QueryGraphicsItem
{
public:

    TableGraphicsItem(Table*, int level, QueryGraphicsHelper* c, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){}
    virtual void mouseMove(int x, int y){}
    virtual void mouseLeft(int x, int y){}
    virtual void onClose(){}
    void setAs(CellAsCommand* as);
    void setJoin(CellJoinCommand* join, CellWhereCommand* on);

private:
    Table* m_table;
    CellTable* m_tableCell;
    CellAsCommand* m_as;
    CellJoinCommand* m_join;
    CellWhereCommand* m_on;
};

#endif // TABLEGRAPHICSITEM_H
