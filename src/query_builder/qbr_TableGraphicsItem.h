#ifndef TABLEGRAPHICSITEM_H
#define TABLEGRAPHICSITEM_H

#include "qbr_QueryGraphicsItem.h"
#include "qbr_QueryGraphicsHelper.h"

class Table;
class QueryComponent;
class CellAsCommand;
class CellJoinCommand;
class CellTable;
class CellWhereCommand;

class TableGraphicsItem : public QueryGraphicsItem
{
public:

    TableGraphicsItem(const QString& tabName, int level, QueryGraphicsHelper* c, QueryGraphicsItem* parent, QueryComponent* owner);

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h);
    virtual void updateWidth(int newWidth);
    virtual void mousePress(int, int){}
    virtual void mouseMove(int, int){}
    virtual void mouseLeft(int, int){}

    virtual void onClose(){}
    void setAs(CellAsCommand* as);
    void addJoin(CellJoinCommand* join, CellWhereCommand* on);

private:
    CellTable* m_tableCell;
    CellAsCommand* m_as;
    QVector<CellJoinCommand*> m_joins;
    QVector<CellWhereCommand*> m_ons;
};

#endif // TABLEGRAPHICSITEM_H
