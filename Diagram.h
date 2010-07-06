#ifndef DIAGRAM_H
#define DIAGRAM_H

#include "TreeItem.h"
#include "NamedItem.h"
#include "DiagramNoteDescriptor.h"

class Table;
class ERGraphicsScene;
class FkRelationDescriptor;
class DraggableGraphicsViewItem;
class DraggableGraphicsViewItemForText;
class FkRelationDescriptor;
class DiagramForm;

/**
 * Class responsible for managing the diagrams of a project
 */
class Diagram : public TreeItem, public NamedItem
{
public:

    Diagram();

    Diagram(const QString&);

    void setForm(DiagramForm* form)
    {
        m_form = form;
    }

    bool isSaved()
    {
        return m_saved;
    }

    void setSaved(bool b1)
    {
        m_saved = b1;
    }

    DiagramForm* getDiagramForm()
    {
        return m_form;
    }

    void removeTable(const QString& tabName);
    void removeNote(const QString& note);

    QVector<const Table*> getTables() const;

    const QVector<DiagramNoteDescriptor*> & getNoteDescriptors();

    DraggableGraphicsViewItemForText* getNote(const QString& note);

    static DraggableGraphicsViewItemForText* clone(DiagramNoteDescriptor* src);

    void addDescriptor(DraggableGraphicsViewItemForText* df);
    void addDescriptor(DraggableGraphicsViewItem* df);

    void reset()
    {
        m_notes.clear();
        m_fksOnStage.clear();
        m_onStage.clear();
    }

    void addNoteItem(DraggableGraphicsViewItemForText*);

public:

    friend class ERGraphicsScene;
    friend class ERGraphicsView;

private:

    // these table elements are already on the stage
    QVector <DraggableGraphicsViewItem*> m_onStage;

    // these foreign key elements are already on the stage
    QVector <FkRelationDescriptor*> m_fksOnStage;

    // the diagrams' notes used internally by the scene. These are cleared from time to time, so don't trust them. Use the noteDescriptors below instead
    QVector <DraggableGraphicsViewItemForText*> m_notes;

    // the notes that are saved to the file
    QVector <DiagramNoteDescriptor*> m_noteDescriptors;
    QVector <DiagramObjectDescriptor*> m_tableDescriptors;

    DiagramForm* m_form;

    bool m_saved;
};

#endif // DIAGRAM_H
