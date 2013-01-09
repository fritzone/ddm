#include "dgram_ERGraphicsScene.h"
#include "ForeignKey.h"
#include "TableListWidget.h"
#include "core_Diagram.h"
#include "EnterNoteTextDialog.h"
#include "DiagramForm.h"
#include "dgram_DraggableGraphicsItemForText.h"
#include "dgram_DiagramItemFactory.h"

#include <qdebug.h>

ERGraphicsScene::ERGraphicsScene(QWidget* parent, Version* v, Diagram* dgram, TableListWidget *lstTables) :  QGraphicsScene(LEFT, TOP, WIDTH, HEIGHT, parent),
                                                                            itm(0), m_version(v), justDropped(0),
                                                                            m_draggedItem(0), m_lstTables(lstTables), m_diagram(dgram)
{
}

ERGraphicsScene::~ERGraphicsScene()
{
    m_diagram->removeFromScene();
}

void ERGraphicsScene::finalizeItem(int x, int y)
{
    if(justDropped == 2)
    {
        EnterNoteTextDialog* enterText = new EnterNoteTextDialog();
        enterText->setModal(true);
        if(enterText->exec() == QDialog::Accepted)
        {
            DraggableGraphicsViewItemForText* dftext = new DraggableGraphicsViewItemForText(enterText->getText(), enterText->isFramed());
            addItem(dftext);
            dftext->setX(x); dftext->pSetX(x);
            dftext->setY(y); dftext->pSetY(y);
            m_diagram->m_notes.append(dftext);
            m_diagram->addDescriptor(dftext);
        }
        justDropped = 0;
        return;
    }

    if(!itm)
    {
        return;
    }

    m_diagram->getDiagramForm()->doneNote();

    itm->setX(x);
    itm->setY(y);

    itm->pSetX(x);
    itm->pSetY(y);

    m_diagram->addDescriptor(itm);

    if(justDropped == 1)
    {
        QString tabName = itm->getTable()->getName();
        m_lstTables->removeItem(tabName);

        justDropped = 0;

        m_diagram->m_onStage.append(itm);

        // TODO: There is duplication in the code below. Refactor

        // now search in the elements that are in the diagram if there are any other tables that have a foreign key with the table from itm and add
        // the foreign key to the scene.
        const QVector<Table*>&  allTables = m_version->getTables();
        for(int i=0; i<allTables.size(); i++)
        {
            if(allTables.at(i) != itm->getTable())
            {
                const QVector<ForeignKey*> & fksI = allTables.at(i)->getForeignKeys();
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
                                DraggableGraphicsViewItemForForeignKey* difks = DiagramItemFactory::getDiagramEntityForForeignKey(fksI.at(j));
                                FkRelationDescriptor* fkrd = new FkRelationDescriptor(fksI.at(j), difks, itm, itmForOtherTable, difks->version());
                                addItem(difks);
                                addForeignKey(fkrd);
                                break;
                            }
                        }
                    }
                }
            }
        }

        // now we should do: search in the foreign keys of the new item, to see if it has a FK to any table in the scene
        const QVector<ForeignKey*> & cfks = itm->getTable()->getForeignKeys();
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
                        DraggableGraphicsViewItemForForeignKey* difks = DiagramItemFactory::getDiagramEntityForForeignKey(cfks.at(i));
                        FkRelationDescriptor* fkrd = new FkRelationDescriptor(cfks.at(i), difks, itmForOtherTable, itm, difks->version());
                        addItem(difks);
                        addForeignKey(fkrd);
                        break;
                    }
                }
            }
        }
    }
}

void ERGraphicsScene::addForeignKey(FkRelationDescriptor* fkrd )
{
    fkrd->updateContent(true);
    addItem(fkrd->getFirstLine() );
    addItem(fkrd->getSecondLine() );
    addItem(fkrd->m_ellipse );
    addItem(fkrd->m_arrowHead );
    addItem(fkrd->rel2Txt);
    addItem(fkrd->rel1Txt);
    m_diagram->m_fksOnStage.append(fkrd);
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

    if(tabName == "Add Note")
    {
        justDropped = 2;
    }
    else
    {
        Table* tab = m_version->getTable(tabName);
        if(!tab) return;
        itm = DiagramItemFactory::getDiagramEntityForTable(tab);
        if(!itm)
        {
            return;
        }
        justDropped = 1;
        addItem(itm);
    }
}

QRectF ERGraphicsScene::getCoverageRect()
{
    qreal minx = 99999, miny = 99999, maxx = -99999, maxy = -99999;
    for(int i=0; i<m_diagram->m_onStage.size(); i++)
    {
        QPointF a  = m_diagram->m_onStage[i]->mapToScene(m_diagram->m_onStage[i]->boundingRect().topLeft());
        QPointF b  = m_diagram->m_onStage[i]->mapToScene(m_diagram->m_onStage[i]->boundingRect().bottomRight());
        if(a.x() < minx) minx = a.x();
        if(a.y() < miny) miny = a.y();
        if(b.x() > maxx) maxx = b.x();
        if(b.y() > maxy) maxy = b.y();
    }

    for(int i=0; i<m_diagram->m_notes.size(); i++)
    {
        QPointF a  = m_diagram->m_notes[i]->mapToScene(m_diagram->m_notes[i]->boundingRect().topLeft());
        QPointF b  = m_diagram->m_notes[i]->mapToScene(m_diagram->m_notes[i]->boundingRect().bottomRight());
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
    m_diagram->updateDescriptors();
    for(int i=0; i<m_diagram->m_fksOnStage.size(); i++)
    {
        m_diagram->m_fksOnStage[i]->updateContent();
        addItem(m_diagram->m_fksOnStage[i]->getFirstLine() );
        addItem(m_diagram->m_fksOnStage[i]->getSecondLine() );

        addItem(m_diagram->m_fksOnStage[i]->m_ellipse );
        addItem(m_diagram->m_fksOnStage[i]->m_arrowHead);
        addItem(m_diagram->m_fksOnStage[i]->rel2Txt);
        addItem(m_diagram->m_fksOnStage[i]->rel1Txt);
    }
}

void ERGraphicsScene::removeTable(const QString &tabName)
{
    m_diagram->removeTable(tabName);
}

void ERGraphicsScene::removeNote(int note)
{
    m_diagram->removeNote(note);
}
