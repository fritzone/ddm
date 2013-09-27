#ifndef QUERYGRAPHICSHELPER_H
#define QUERYGRAPHICSHELPER_H

#include "qbr_OptionTypes.h"

#include <QStringList>
#include <QRect>
#include <QIcon>
#include <QMap>

class QueryItemListDialog;
class QueryGraphicsItem;
class QueryGraphicsScene;
class Query;
class NewViewForm;
class MainWindow;
class Column;
class SelectQueryJoinComponent;
class TableQueryComponent;
class SelectQueryAsComponent;
struct ColumnOfTabWithTabInstance;

class QueryGraphicsHelper
{
public:

    enum ListType
    {
        LIST_TABLES = 0,
        INPUT_TEXT,
        INPUT_SYMBOLS,
        INPUT_COLUMNS,
        INPUT_ORDERBY
    };

    QueryGraphicsHelper();
    void addNewHotCell(QueryGraphicsItem*, QRect);
    QueryGraphicsItem* clicked(int x, int y);
    QString presentList(int, int, ListType);
    QString presentList(int, int, const QString&);
    QString presentList(int, int, const QStringList &, const QList<QIcon> &);
    void setScene(QueryGraphicsScene* sc) ;
    void setQuery(Query* q) ;
    void setForm(MainWindow*);
    void resetHighlightedItem();
    void triggerReRender();
    void resetContent()
    {
        hotCells.clear();
    }

    QueryGraphicsScene* getScene() const
    {
        return m_scene;
    }

    Query* getQuery() const
    {
        return m_query;
    }

    void setColumns(QVector<const ColumnOfTabWithTabInstance*> c)
    {
        m_columnsToShow = c;
    }

    void setOrderBy(QStringList oby)
    {
        m_orderByElements = oby ;
    }

    void setJoin(const SelectQueryJoinComponent* j)
    {
        m_join = j;
    }

    void tableRemoved(const QString& t);

private:

    QMap<QueryGraphicsItem*, QRect> hotCells;
    QueryItemListDialog* m_lstDlg;
    QueryGraphicsScene* m_scene;
    Query* m_query;
    QVector<const ColumnOfTabWithTabInstance*> m_columnsToShow;
    QStringList m_orderByElements;
    const SelectQueryJoinComponent* m_join;
};

#endif // QUERYGRAPHICSHELPER_H
