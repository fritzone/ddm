#ifndef QUERYCOMPONENTS_H
#define QUERYCOMPONENTS_H

#include <QRect>
#include <QMap>

class QueryItemListDialog;
class QueryGraphicsItem;
class QueryGraphicsScene;

class QueryComponents
{
public:

    enum ListType
    {
        LIST_TABLES = 1
    };

    QueryComponents(QueryGraphicsScene*);
    QueryComponents();
    void addNewHotCell(QueryGraphicsItem*, QRect);
    QueryGraphicsItem* clicked(int x, int y);
    QString presentList(int,int,ListType);
    void setScene(QueryGraphicsScene*sc) ;

private:

    QMap<QueryGraphicsItem*, QRect> hotCells;
    QueryItemListDialog* m_lstDlg;
    QueryGraphicsScene* m_scene;
};

#endif // QUERYCOMPONENTS_H
