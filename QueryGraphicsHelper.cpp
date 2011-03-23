#include "QueryGraphicsHelper.h"
#include "QueryItemListDialog.h"
#include "QueryGraphicsScene.h"
#include "QueryGraphicsItem.h"
#include "Query.h"
#include "mainwindow.h"

QueryGraphicsHelper::QueryGraphicsHelper() : hotCells(), m_lstDlg(0), m_scene(0), m_form(0)
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

void QueryGraphicsHelper::setForm(MainWindow* f)
{
    m_form = f;
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
    m_lstDlg = new QueryItemListDialog(lt, 0);
    m_lstDlg->setWindowFlags(Qt::FramelessWindowHint);
    m_lstDlg->move(x, y);
    m_lstDlg->setModal(true);
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
    m_lstDlg = new QueryItemListDialog(INPUT_TEXT, 0);
    m_lstDlg->setWindowFlags(Qt::FramelessWindowHint);
    m_lstDlg->move(x, y);
    m_lstDlg->setText(input);
    m_lstDlg->setModal(true);
    m_lstDlg->exec();
    return m_lstDlg->getSelection();
}


QString QueryGraphicsHelper::presentList(int x, int y, QStringList lst, QList<QIcon> icons)
{
    if(m_lstDlg != 0)
    {
        m_lstDlg->close();
        m_lstDlg = 0;
    }
    m_lstDlg = new QueryItemListDialog(lst, icons, 0);
    m_lstDlg->setWindowFlags(Qt::FramelessWindowHint);
    m_lstDlg->move(x, y);
    m_lstDlg->setModal(true);
    m_lstDlg->exec();
    return m_lstDlg->getSelection();
}


void QueryGraphicsHelper::resetHighlightedItem()
{
    for(int i=0; i<hotCells.keys().size(); i++)
    {
        hotCells.keys().at(i)->mouseLeft(0,0);
    }
}

void QueryGraphicsHelper::triggerReRender(int h, int v)
{
    hotCells.clear();
    m_form->rerenderQuery(m_query, h, v);
}

