#ifndef DIAGRAM_H
#define DIAGRAM_H

#include "TreeItem.h"
#include "NamedItem.h"
#include "DiagramNoteDescriptor.h"
#include "DiagramFKDescriptor.h"
#include "SerializableElement.h"

class Table;
class ERGraphicsScene;
class FkRelationDescriptor;
class DraggableGraphicsViewItem;
class DraggableGraphicsViewItemForText;
class FkRelationDescriptor;
class DiagramForm;
class Version;

/**
 * Class responsible for managing the diagrams of a project
 */
class Diagram : public TreeItem, public NamedItem, public SerializableElement
{
public:

    Diagram(Version*);

    Diagram(Version*, const QString&);

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
    void removeNote(int);

    QVector<const Table*> getTables() const;

    const QVector<DiagramNoteDescriptor*> & getNoteDescriptors();
    const QVector<DiagramObjectDescriptor*> & getTableDescriptors();

    DraggableGraphicsViewItemForText* getNote(int);

    DraggableGraphicsViewItemForText* clone(DiagramNoteDescriptor* src);
    DraggableGraphicsViewItem* clone(DiagramObjectDescriptor* src);

    void addDescriptor(DraggableGraphicsViewItemForText* df);
    void addDescriptor(DraggableGraphicsViewItem* df);

    void reset();

    void recreateFks(ERGraphicsScene*);

    void addNoteItem(DraggableGraphicsViewItemForText*);
    void addTableItem(DraggableGraphicsViewItem*);

    DraggableGraphicsViewItem* getTableItem(const QString& name);

    void updateDescriptors();

    /**
     * Will generate the following XML:
     <Diagram>
     <Tables>
        <Table name="TabName" x="TableX" y="TableY" />
     </Tables>
     <Notes>
        <Note x+"X" y="Y" frame="1/0">
            <Text> CDATA </Text>
        </Note>
     </Notes>
     <Fks>
        <Fk ... everything from the descriptor>
     </Fks>
     </Diagram>
     */
    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

public:

    friend class ERGraphicsScene;
    friend class ERGraphicsView;

private:

    // these table elements are already on the stage
    QVector <DraggableGraphicsViewItem*> m_onStage;

    // these foreign key elements are already on the stage. FkRelationDescriptor objects contain the DiagramFKDescriptor
    // objects that are being saved to the file
    QVector <FkRelationDescriptor*> m_fksOnStage;

    // the diagrams' notes used internally by the scene. These are cleared from time to time, so don't trust them. Use the noteDescriptors below instead
    QVector <DraggableGraphicsViewItemForText*> m_notes;

    // the note descriptors that are saved to the file
    QVector <DiagramNoteDescriptor*> m_noteDescriptors;

    // the table descriptors that are saved to the file
    QVector <DiagramObjectDescriptor*> m_tableDescriptors;

    DiagramForm* m_form;

    bool m_saved;

    Version* m_version;
};

#endif // DIAGRAM_H
