#ifndef TABLEGRAPHICSITEM_H
#define TABLEGRAPHICSITEM_H

#include "QueryGraphicsItem.h"
#include "QueryGraphicsHelper.h"

class Table;
class QueryComponent;
class CellAsCommand;

class TableGraphicsItem : public QueryGraphicsItem
{
public:

    TableGraphicsItem(Table*, QueryGraphicsHelper* c, QueryGraphicsItem* parent, QueryComponent* owner);
    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int x, int y){}
    virtual void mouseMove(int x, int y){}
    virtual void mouseLeft(int x, int y){}
    virtual void onClose(){}
    void setAs(CellAsCommand* as);

private:
    Table* m_table;
    QueryGraphicsItem* m_tableCell;
    CellAsCommand* m_as;
};

#endif // TABLEGRAPHICSITEM_H
