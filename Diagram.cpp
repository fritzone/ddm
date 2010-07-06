#include "Diagram.h"
#include "Table.h"
#include "DraggableGraphicsItem.h"
#include "DraggableGraphicsItemForForeignKey.h"
#include "DraggableGraphicsItemForText.h"
#include "FkRelationDescriptor.h"

Diagram::Diagram() : TreeItem(), NamedItem("Table diagram"), m_onStage(), m_fksOnStage(), m_notes(), m_form(0), m_saved(false)
{

}

Diagram::Diagram(const QString & name) : TreeItem(), NamedItem(name), m_onStage(), m_fksOnStage(), m_notes(), m_form(0), m_saved(false)
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
        delete toDelete;
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

void Diagram::removeNote(const QString &note)
{
    int i=0;
    while(i<m_notes.size())
    {
        if(m_notes.at(i)->getText() == note)
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

DraggableGraphicsViewItemForText* Diagram::getNote(const QString& note)
{
    int i=0;
    while(i<m_notes.size())
    {
        if(m_notes.at(i)->getText() == note)
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

DraggableGraphicsViewItemForText* Diagram::clone(DiagramNoteDescriptor* src)
{
    DraggableGraphicsViewItemForText* result= new DraggableGraphicsViewItemForText(src->getText(), src->isFramed());
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
