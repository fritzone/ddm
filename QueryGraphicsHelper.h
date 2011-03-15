#ifndef QUERYGRAPHICSHELPER_H
#define QUERYGRAPHICSHELPER_H

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

class QueryGraphicsHelper
{
public:

    enum ListType
    {
        LIST_TABLES = 1,
        INPUT_TEXT = 2
    };

    QueryGraphicsHelper();
    void addNewHotCell(QueryGraphicsItem*, QRect);
    QueryGraphicsItem* clicked(int x, int y);
    QString presentList(int, int, ListType);
    QString presentList(int, int, QStringList, QList<QIcon>);
    void setScene(QueryGraphicsScene* sc) ;
    void setQuery(Query* q) ;
    void setForm(MainWindow*);
    void resetHighlightedItem();
    void triggerReRender();
    QueryGraphicsScene* getScene()
    {
        return m_scene;
    }

private:

    QMap<QueryGraphicsItem*, QRect> hotCells;
    QueryItemListDialog* m_lstDlg;
    QueryGraphicsScene* m_scene;
    Query* m_query;
    MainWindow* m_form;

};

#endif // QUERYGRAPHICSHELPER_H
