#include "ERGraphicsScene.h"
#include "ForeignKey.h"

#include <qdebug.h>

void ERGraphicsScene::finalizeItem(int x, int y)
{
    if(!itm)
    {
        return; // meaning: we dragged in a table which somehow didn't create its object ... this is so sad
    }

    itm->setX(x);
    itm->setY(y);

    justDropped = false;

    m_onStage.append(itm);

    // now search in the version if there are any other tables that have a foreign key with the table from itm and add
    // the foreign key to the scene
    const QVector<Table*>&  allTables = m_version->getTables();
    for(int i=0; i<allTables.size(); i++)
    {
        if(allTables.at(i) != itm->getTable())
        {
            const QVector<ForeignKey*> & fksI = allTables.at(i)->getFks();
            for(int j=0; j<fksI.size(); j++)
            {
                const QVector<ForeignKey::ColumnAssociation*>&  assocs = fksI.at(j)->getAssociations();
                for(int k=0; k<assocs.size(); k++)
                {
                    if(assocs.at(k)->getForeignTable() == itm->getTable())
                    {
                        DraggableGraphicsViewItem* itmForOtherTable = getItemForTable(allTables.at(i)->getName());
                        if(itmForOtherTable != 0)
                        {
                            qDebug() << "do the stuff here: prepare a new object to be put on the screen between itm and itmForOtherTable";

                            DraggableGraphicsViewItemForForeignKey* difks = fksI.at(j)->getItem();
                            FkRelationDescriptor* fkrd = new FkRelationDescriptor(difks, itm, itmForOtherTable);
                            fkrd->updateContent(true);
                            addItem(difks);
                            addItem(fkrd->firstLine);
                            addItem(fkrd->secondLine);
                            m_fksOnStage.append(fkrd);
                        }
                    }
                }
            }
        }
    }

}

DraggableGraphicsViewItem* ERGraphicsScene::getItemForTable(const QString &tabName)
{
    for(int i=0; i<m_onStage.size(); i++)
    {
        if(m_onStage.at(i)->getTable()->getName() == tabName)
        {
            return m_onStage.at(i);
        }
    }
    return 0;
}

void ERGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent * event)
{
    QString tabName = event->mimeData()->text();
    event->acceptProposedAction();
    Table* tab = m_version->getTable(tabName);
    itm = tab->getDiagramEntity();
    if(!itm)
    {
        return;
    }
    justDropped = true;
    addItem(itm);
}
