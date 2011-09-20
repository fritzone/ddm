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
    QString presentList(int, int, QStringList, QList<QIcon>);
    void setScene(QueryGraphicsScene* sc) ;
    void setQuery(Query* q) ;
    void setForm(MainWindow*);
    void resetHighlightedItem();
    void triggerReRender();

    QueryGraphicsScene* getScene() const
    {
        return m_scene;
    }

    Query* getQuery() const
    {
        return m_query;
    }

    void setColumns(QVector<const Column*> c)
    {
        m_columnsToShow = c;
    }

    void setOrderBy(QStringList oby)
    {
        m_orderByElements = oby ;
    }

private:

    QMap<QueryGraphicsItem*, QRect> hotCells;
    QueryItemListDialog* m_lstDlg;
    QueryGraphicsScene* m_scene;
    Query* m_query;
    MainWindow* m_form;
    QVector<const Column*> m_columnsToShow;
    QStringList m_orderByElements;
};

#endif // QUERYGRAPHICSHELPER_H
