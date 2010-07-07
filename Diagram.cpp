#include "Diagram.h"
#include "Table.h"
#include "DraggableGraphicsItem.h"
#include "DraggableGraphicsItemForForeignKey.h"
#include "DraggableGraphicsItemForText.h"
#include "FkRelationDescriptor.h"
#include "Version.h"

Diagram::Diagram(Version* v) : TreeItem(), NamedItem("Table diagram"), m_onStage(), m_fksOnStage(), m_notes(), m_form(0), m_saved(false), m_version(v)
{

}

Diagram::Diagram(Version* v, const QString & name) : TreeItem(), NamedItem(name), m_onStage(), m_fksOnStage(), m_notes(), m_form(0), m_saved(false), m_version(v)
{

}

void Diagram::removeTable(const QString &tabName)
{
    int idx = -1;
    for(int i=0; i<m_onStage.size(); i++)
    {
        if(m_onStage[i]->getTable()->getName() == tabName)
        {
            idx = i;
            for(int j=0; j<m_fksOnStage.size(); j++)
            {
                if(m_fksOnStage[j]->getFirst() == m_onStage[i] || m_fksOnStage[j]->getSecond() == m_onStage[i])
                {
                    m_fksOnStage[j]->sentence();
                }
            }
            break;
        }
    }
    if(idx != -1)
    {
        DraggableGraphicsViewItem* toDelete = m_onStage[idx];
        m_onStage.remove(idx);
        DiagramObjectDescriptor* descToDel = m_tableDescriptors[idx];
        m_tableDescriptors.remove(idx);
        delete toDelete;
        delete descToDel;
    }

    int i=0;
    while(i<m_fksOnStage.size())
    {
        if(m_fksOnStage[i]->isSentenced())
        {
            FkRelationDescriptor* sentenced = m_fksOnStage[i];
            sentenced->eliberate();
            m_fksOnStage.remove(i);
            delete sentenced;
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
        if(m_notes.at(i)->index() == note)
        {
            m_notes.at(i)->removeFromScene();
            m_notes.remove(i);
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
    qDebug() << "Note: "<< noteIdx;
    int i=0;
    while(i<m_notes.size())
    {
        qDebug() << "Note " << i << m_notes.at(i)->index();
        if(m_notes.at(i)->index() == noteIdx)
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

const QVector<DiagramObjectDescriptor*> & Diagram::getTableDescriptors()
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
    DraggableGraphicsViewItem* result= m_version->getTable(src->getText())->getDiagramEntity() ;
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
    DiagramObjectDescriptor* desc = new DiagramObjectDescriptor(df->getTable()->getName(), df->getpSX(), df->getpSY());
    m_tableDescriptors.append(desc);
}


void Diagram::addNoteItem(DraggableGraphicsViewItemForText *itm)
{
    m_notes.append(itm);
}

void Diagram::addTableItem(DraggableGraphicsViewItem *itm)
{
    m_onStage.append(itm);
}

void Diagram::reset()
{
    m_notes.clear();
    m_onStage.clear();
}

void Diagram::recreateFks(ERGraphicsScene* scene)
{
    for(int i=0; i<m_fksOnStage.size();i ++)
    {
        m_fksOnStage[i]->recreate(this, scene);
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
