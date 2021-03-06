#include "qbr_QueryGraphicsHelper.h"
#include "qbr_QueryItemListDialog.h"
#include "qbr_QueryGraphicsScene.h"
#include "qbr_QueryGraphicsItem.h"
#include "qbr_Query.h"
#include "MainWindow.h"
#include "IconFactory.h"
#include "Workspace.h"
#include "db_DatabaseEngine.h"
#include "qbr_TableQueryComponent.h"
#include "db_DatabaseBuiltinFunction.h"
#include "core_TableInstance.h"
#include "core_Table.h"
#include "qbr_SelectQueryAsComponent.h"

QueryGraphicsHelper::QueryGraphicsHelper() : hotCells(), m_lstDlg(0), m_scene(0)
{
}

void QueryGraphicsHelper::setScene(QueryGraphicsScene*sc)
{
    m_scene = sc;
}

void QueryGraphicsHelper::setQuery(Query *q)
{
    m_query = q;
}

void QueryGraphicsHelper::addNewHotCell(QueryGraphicsItem *itm, QRect where)
{
    hotCells.insert(itm, where);
}

QueryGraphicsItem* QueryGraphicsHelper::clicked(int x, int y)
{
    for(int i=0; i<hotCells.keys().size(); i++)
    {
        if(hotCells[hotCells.keys().at(i)].contains(x,y, true))
        {
            return hotCells.keys().at(i);
        }
    }
    return 0;
}

QString QueryGraphicsHelper::presentList(int x, int y, ListType lt)
{
    if(m_lstDlg != 0)
    {
        m_lstDlg->close();
        m_lstDlg = 0;
    }
    m_lstDlg = new QueryItemListDialog(this, lt, 0);
    m_lstDlg->setColumns(m_columnsToShow);  // TODO: this is a little bit ugly ...
    m_lstDlg->setOrderBy(m_orderByElements);

    m_lstDlg->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    m_lstDlg->move(x, y);
    m_lstDlg->setModal(true);
    m_lstDlg->setJoin(m_join);
    if(lt == INPUT_SYMBOLS || lt == INPUT_COLUMNS || lt == INPUT_ORDERBY)
    {
        m_lstDlg->showSymbolPanel();
        if(lt == INPUT_COLUMNS)
        {
            m_lstDlg->setColumnMode();
        }
        if(lt == INPUT_ORDERBY)
        {
            m_lstDlg->setColumnMode();
            m_lstDlg->setOrderByMode();
        }
    }
    m_lstDlg->exec();
    return m_lstDlg->getSelection();
}

QString QueryGraphicsHelper::presentList(int x, int y, const QString& input)
{
    if(m_lstDlg != 0)
    {
        m_lstDlg->close();
        m_lstDlg = 0;
    }
    m_lstDlg = new QueryItemListDialog(this, INPUT_TEXT, 0);
    m_lstDlg->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    m_lstDlg->move(x, y);
    m_lstDlg->setText(input);
    m_lstDlg->setModal(true);
    m_lstDlg->exec();
    return m_lstDlg->getSelection();
}


QString QueryGraphicsHelper::presentList(int x, int y, const QStringList& lst,
                                         const QList<QIcon>& icons)
{
    if(m_lstDlg != 0)
    {
        m_lstDlg->close();
        delete m_lstDlg;
        m_lstDlg = 0;
    }

    m_lstDlg = new QueryItemListDialog(this, lst, icons, false, 0);
    m_lstDlg->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    m_lstDlg->move(x, y);
    m_lstDlg->setModal(true);
    m_lstDlg->exec();
    return m_lstDlg->getSelection();
}


void QueryGraphicsHelper::resetHighlightedItem()
{
    for(int i=0; i<hotCells.keys().size(); i++)
    {
        hotCells.keys().at(i)->mouseLeft(0, 0);
    }
}

void QueryGraphicsHelper::triggerReRender()
{
    hotCells.clear();
    MainWindow::instance()->rerenderQuery(m_query);
}

void QueryGraphicsHelper::tableRemoved(const QString& t)
{
    m_query->tableRemovedFromQuery(t);
}
