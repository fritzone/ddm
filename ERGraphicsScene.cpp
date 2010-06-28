#include "ERGraphicsScene.h"
#include "ForeignKey.h"
#include "TableListWidget.h"
#include "Diagram.h"

#include <qdebug.h>

ERGraphicsScene::ERGraphicsScene(QWidget* parent, Version* v, Diagram* dgram, TableListWidget *lstTables) :  QGraphicsScene(LEFT, TOP, WIDTH, HEIGHT, parent),
                                                                            itm(0), m_version(v), justDropped(false), m_draggedItem(0), m_lstTables(lstTables), m_diagram(dgram)
{
}


void ERGraphicsScene::finalizeItem(int x, int y)
{
    if(!itm)
    {
        return; // meaning: we dragged in a table which somehow didn't create its object ... this is so sad
    }

    itm->setX(x);
    itm->setY(y);

    QString tabName = itm->getTable()->getName();
    m_lstTables->removeItem(tabName);

    justDropped = false;

    m_diagram->m_onStage.append(itm);

    // now search in the elements that are in the diagram if there are any other tables that have a foreign key with the table from itm and add
    // the foreign key to the scene.
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
                        DraggableGraphicsViewItem* itmForOtherTable = getItemForTable(allTables.at(i)->getName());  // this returns a table only if the table is in the scene already
                        if(itmForOtherTable != 0)
                        {
                            qDebug() << "do the stuff here: prepare a new object to be put on the screen between itm and itmForOtherTable";

                            DraggableGraphicsViewItemForForeignKey* difks = fksI.at(j)->getItem();
                            FkRelationDescriptor* fkrd = new FkRelationDescriptor(difks, itm, itmForOtherTable);
                            fkrd->updateContent(true);
                            addItem(difks);
                            addItem(fkrd->getFirstLine() );
                            addItem(fkrd->getSecondLine() );
                            addItem(fkrd->m_ellipse );
                            addItem(fkrd->rel1Txt);
                            addItem(fkrd->rel2Txt);
                            m_diagram->m_fksOnStage.append(fkrd);
                            break;
                        }
                    }
                }
            }
        }
    }

    // now we should do: search in the foreign keys of the new item, to see if it has a FK to any table in the scene
    const QVector<ForeignKey*> & cfks = itm->getTable()->getFks();
    for(int i=0; i<cfks.size(); i++)
    {
        const QVector<ForeignKey::ColumnAssociation*>&  assocs = cfks.at(i)->getAssociations();
        for(int j=0; j<assocs.size(); j++)
        {
            if(assocs.at(j)->getLocalTable() == itm->getTable())
            {
                DraggableGraphicsViewItem* itmForOtherTable = getItemForTable(assocs.at(j)->getForeignTable()->getName());
                if(itmForOtherTable != 0)
                {
                    DraggableGraphicsViewItemForForeignKey* difks = cfks.at(i)->getItem();
                    FkRelationDescriptor* fkrd = new FkRelationDescriptor(difks, itmForOtherTable, itm);
                    fkrd->updateContent(true);
                    addItem(difks);
                    addItem(fkrd->getFirstLine() );
                    addItem(fkrd->getSecondLine() );
                    addItem(fkrd->m_ellipse );
                    addItem(fkrd->rel1Txt);
                    addItem(fkrd->rel2Txt);
                    m_diagram->m_fksOnStage.append(fkrd);
                    break;
                }
            }
        }
    }
}

DraggableGraphicsViewItem* ERGraphicsScene::getItemForTable(const QString &tabName)
{
    for(int i=0; i<m_diagram->m_onStage.size(); i++)
    {
        if(m_diagram->m_onStage.at(i)->getTable()->getName() == tabName)
        {
            return m_diagram->m_onStage.at(i);
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

QRectF ERGraphicsScene::getCoverageRect()
{
    qreal minx = 99999999999, miny = 99999999999, maxx = -99999999999, maxy = -99999999999;
    for(int i=0; i<m_diagram->m_onStage.size(); i++)
    {
        QPointF a  = m_diagram->m_onStage[i]->mapToScene(m_diagram->m_onStage[i]->boundingRect().topLeft());
        QPointF b  = m_diagram->m_onStage[i]->mapToScene(m_diagram->m_onStage[i]->boundingRect().bottomRight());
        if(a.x() < minx) minx = a.x();
        if(a.y() < miny) miny = a.y();
        if(b.x() > maxx) maxx = b.x();
        if(b.y() > maxy) maxy = b.y();
    }

    for(int i=0; i<m_diagram->m_fksOnStage.size(); i++)
    {
        QPointF a  = m_diagram->m_fksOnStage[i]->getItem()->mapToScene(m_diagram->m_fksOnStage[i]->getItem()->boundingRect().topLeft());
        QPointF b  = m_diagram->m_fksOnStage[i]->getItem()->mapToScene(m_diagram->m_fksOnStage[i]->getItem()->boundingRect().bottomRight());
        if(a.x() < minx) minx = a.x();
        if(a.y() < miny) miny = a.y();
        if(b.x() > maxx) maxx = b.x();
        if(b.y() > maxy) maxy = b.y();
    }

    return QRectF(minx, miny, maxx - minx, maxy - miny);
}

void ERGraphicsScene::upadteFkrds()
{
    for(int i=0; i<m_diagram->m_fksOnStage.size(); i++)
    {
        m_diagram->m_fksOnStage[i]->updateContent();
        addItem(m_diagram->m_fksOnStage[i]->getFirstLine() );
        addItem(m_diagram->m_fksOnStage[i]->getSecondLine() );

        addItem(m_diagram->m_fksOnStage[i]->m_ellipse );
        addItem(m_diagram->m_fksOnStage[i]->rel1Txt);
        addItem(m_diagram->m_fksOnStage[i]->rel2Txt);
    }
}

void ERGraphicsScene::removeTable(const QString &tabName)
{
    m_diagram->removeTable(tabName);
}
