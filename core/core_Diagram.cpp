#include "core_Diagram.h"
#include "core_Table.h"
#include "dgram_DraggableGraphicsItem.h"
#include "dgram_DraggableGraphicsItemForForeignKey.h"
#include "dgram_DraggableGraphicsItemForText.h"
#include "FkRelationDescriptor.h"
#include "ForeignKey.h"
#include "Version.h"
#include "NameGenerator.h"
#include "dgram_DiagramItemFactory.h"
#include "uids.h"

Diagram::Diagram(Version* v, const QString& uid) : TreeItem(),
    NamedItem(NameGenerator::getUniqueName(v, (itemGetter)&Version::getDiagram, QString("Diagram"))),
    ObjectWithUid(uid), m_onStage(), m_fksOnStage(), m_notes(),
    m_noteDescriptors(0), m_tableDescriptors(), m_form(0), m_saved(false), m_version(v), m_removed(false)
{

}

Diagram::Diagram(Version* v, const QString & name, const QString& uid) :
    TreeItem(),
    NamedItem(name),
    ObjectWithUid(uid),
    m_onStage(), m_fksOnStage(), m_notes(), m_form(0), m_saved(false), m_version(v)
{

}

void Diagram::removeTable(const QString &tabName)
{
    int idx = -1;
    for(int i=0; i<m_tableDescriptors.size(); i++)
    {
        if(m_tableDescriptors[i]->getText() == tabName)
        {
            idx = i;
            for(int j=0; j<m_fksOnStage.size(); j++)
            {
                if(m_fksOnStage[j]->getTab1Name() == m_tableDescriptors[i]->getText() || m_fksOnStage[j]->getTab2Name() == m_tableDescriptors[i]->getText())
                {
                    m_fksOnStage[j]->sentence();
                }
            }
            break;
        }
    }
    int ms = m_onStage.size();
    if(idx != -1 && ms > 0)
    {
        DraggableGraphicsViewItem* toDelete = m_onStage[idx];
        m_onStage.remove(idx);
        if(!m_removed) delete toDelete;
    }

    int l = m_tableDescriptors.size();
    if(idx != -1 && l > 0 )
    {
        DiagramObjectDescriptor* descToDel = m_tableDescriptors[idx];
        m_tableDescriptors.remove(idx);
        delete descToDel;
    }

    int i=0;
    while(i<m_fksOnStage.size())
    {
        if(m_fksOnStage[i]->isSentenced())
        {
            FkRelationDescriptor* sentenced = m_fksOnStage[i];
            if(!m_removed) sentenced->eliberate();
            m_fksOnStage.remove(i);
            if(!m_removed) delete sentenced;
        }
        else
        {
            i++;
        }
    }
}

void Diagram::removeNote(int note)
{
    int i=0;
    while(i<m_notes.size())
    {
        if(m_notes.at(i)->getIndex() == note)
        {
            m_notes.at(i)->removeFromScene();
            m_notes.remove(i);
            return;
        }
        i++;
    }
}

void Diagram::forcefullyRemoveForeignKey(ForeignKey* fk)
{
    int i=0;
    while(i<m_fksOnStage.size())
    {
        if(m_fksOnStage.at(i)->descriptor()->getFkName() == fk->getName())
        {
            m_fksOnStage.remove(i);
            return;
        }
        i++;
    }
}


QVector<const Table*> Diagram::getTables() const
{
    QVector<const Table*> result;
    for(int i=0; i<m_onStage.size(); i++)
    {
        result.append(m_onStage[i]->getTable());
    }

    return result;
}

DraggableGraphicsViewItemForText* Diagram::getNote(int noteIdx)
{
    int i=0;
    while(i<m_notes.size())
    {
        if(m_notes.at(i)->getIndex() == noteIdx)
        {
            return m_notes.at(i);
        }
        i++;
    }
    return 0;
}

const QVector<DiagramNoteDescriptor*> & Diagram::getNoteDescriptors()
{
    return m_noteDescriptors;
}

const QVector<DiagramTableDescriptor*> & Diagram::getTableDescriptors()
{
    return m_tableDescriptors;
}

DraggableGraphicsViewItemForText* Diagram::clone(DiagramNoteDescriptor* src)
{
    DraggableGraphicsViewItemForText* result= new DraggableGraphicsViewItemForText(src->getText(), src->isFramed());
    result->pSetX(src->getX());
    result->pSetY(src->getY());
    return result;
}

DraggableGraphicsViewItem* Diagram::clone(DiagramObjectDescriptor* src)
{
    DraggableGraphicsViewItem* result= DiagramItemFactory::getDiagramEntityForTable(m_version->getTable(src->getText()));
    result->pSetX(src->getX());
    result->pSetY(src->getY());
    return result;
}

void Diagram::addDescriptor(DraggableGraphicsViewItemForText* df)
{
    DiagramNoteDescriptor* desc = new DiagramNoteDescriptor(df->getText(), df->getpSX(), df->getpSY(), df->isFramed());
    m_noteDescriptors.append(desc);
}

void Diagram::addDescriptor(DraggableGraphicsViewItem* df)
{
    DiagramTableDescriptor* desc = new DiagramTableDescriptor(df->getTable()->getName(), df->getpSX(), df->getpSY());
    m_tableDescriptors.append(desc);
}


void Diagram::addNoteItem(DraggableGraphicsViewItemForText *itm)
{
    m_notes.append(itm);
}

void Diagram::addTableItem(DraggableGraphicsViewItem *itm)
{
    m_onStage.append(itm);
    m_removed = false;
}


void Diagram::removeFromScene()
{
    m_removed = true;
}

void Diagram::reset(bool byButton )
{
    m_notes.clear();
    m_onStage.clear();

    if(byButton)
    {
        m_fksOnStage.clear();
        m_tableDescriptors.clear();
        m_noteDescriptors.clear();
    }
}

void Diagram::recreateFks(ERGraphicsScene* scene)
{
    for(int i=0; i<m_fksOnStage.size();i ++)
    {
        m_fksOnStage[i]->recreate(this);
        m_fksOnStage[i]->addToScene(scene);
    }
}

DraggableGraphicsViewItem* Diagram::getTableItem(const QString& name)
{
    for(int i=0; i<m_onStage.size(); i++)
    {
        if(m_onStage[i]->getTable()->getName() == name)
        {
            return m_onStage[i];
        }
    }
    return 0;
}

void Diagram::updateDescriptors()
{
    for(int i=0; i<m_onStage.size();i++)
    {
        m_tableDescriptors[i]->setX(m_onStage[i]->x());
        m_tableDescriptors[i]->setY(m_onStage[i]->y());
    }

    for(int i=0; i<m_notes.size();i++)
    {
        m_noteDescriptors[i]->setX(m_notes[i]->x());
        m_noteDescriptors[i]->setY(m_notes[i]->y());
        m_noteDescriptors[i]->setFramed(m_notes[i]->isFramed());
    }
}

void Diagram::serialize(QDomDocument &doc, QDomElement &parent) const
{
    QDomElement diagramElement = doc.createElement("Diagram");      // will hold the data in this element
    diagramElement.setAttribute("Name", m_name);
    diagramElement.setAttribute("uid", getObjectUid());
    diagramElement.setAttribute("class-uid", getClassUid());

    // save the tables
    {
    QDomElement tablesElement = doc.createElement("Tables");
    for(int i=0; i<m_tableDescriptors.size(); i++)
    {
        m_tableDescriptors[i]->serialize(doc, tablesElement);
    }
    diagramElement.appendChild(tablesElement);
    }

    // save the notes
    {
    QDomElement notesElement = doc.createElement("Notes");
    for(int i=0; i<m_noteDescriptors.size(); i++)
    {
        m_noteDescriptors[i]->serialize(doc, notesElement);
    }
    diagramElement.appendChild(notesElement);
    }

    // save the fks
    {
    QDomElement fksElement = doc.createElement("FKs");
    for(int i=0; i<m_fksOnStage.size(); i++)
    {
        m_fksOnStage[i]->descriptor()->serialize(doc, fksElement);
    }
    diagramElement.appendChild(fksElement);
    }

    parent.appendChild(diagramElement);

}

void Diagram::addTableDescriptor(DiagramTableDescriptor* td)
{
    m_tableDescriptors.append(td);
}

void Diagram::addNoteDescriptor(DiagramNoteDescriptor* nd)
{
    m_noteDescriptors.append(nd);
}

void Diagram::addFKDescriptor(DiagramFKDescriptor* nd)
{
    FkRelationDescriptor* fkrd = new FkRelationDescriptor();
    fkrd->setDescriptor(nd);
    m_fksOnStage.append(fkrd);
}

QUuid Diagram::getClassUid() const
{
    return QUuid(uidDiagram);
}
