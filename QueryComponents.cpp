#include "QueryComponents.h"
#include "QueryItemListDialog.h"
#include "QueryGraphicsScene.h"

QueryComponents::QueryComponents(QueryGraphicsScene*sc) : hotCells(), m_lstDlg(0), m_scene(sc)
{
}

QueryComponents::QueryComponents() : hotCells(), m_lstDlg(0), m_scene(0)
{
}

void QueryComponents::setScene(QueryGraphicsScene*sc)
{
    m_scene = sc;
}

void QueryComponents::addNewHotCell(QueryGraphicsItem *itm, QRect where)
{
    hotCells.insert(itm, where);
}

QueryGraphicsItem* QueryComponents::clicked(int x, int y)
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

QString QueryComponents::presentList(int x, int y, ListType lt)
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
