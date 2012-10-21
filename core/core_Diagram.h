#ifndef DIAGRAM_H
#define DIAGRAM_H

#include "TreeItem.h"
#include "NamedItem.h"
#include "dgram_DiagramNoteDescriptor.h"
#include "dgram_DiagramTableDescriptor.h"
#include "dgram_DiagramFKDescriptor.h"
#include "SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "core_CloneableElement.h"

class Table;
class ERGraphicsScene;
class FkRelationDescriptor;
class DraggableGraphicsViewItem;
class DraggableGraphicsViewItemForText;
class FkRelationDescriptor;
class DiagramForm;
class Version;
class ForeignKey;

/**
 * Class responsible for managing the diagrams of a project
 */
class Diagram : public TreeItem, public NamedItem, public SerializableElement, public ObjectWithUid, public CloneableElement
{
public:

    Diagram(Version* v, const QString& uid);

    Diagram(Version* v, const QString& name, const QString& uid);

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

    void forcefullyRemoveForeignKey(ForeignKey* fk);

    QVector<const Table*> getTables() const;
    QStringList getTableNames() const;

    const QVector<DiagramNoteDescriptor*> & getNoteDescriptors();
    const QVector<DiagramTableDescriptor*> & getTableDescriptors();

    DraggableGraphicsViewItemForText* getNote(int);

    DraggableGraphicsViewItemForText* clone(DiagramNoteDescriptor* src);
    DraggableGraphicsViewItem* clone(DiagramObjectDescriptor* src);

    /**
     * These two below will add a descriptor for the newly created item. Called when the df was just created after dropping something on the scene.
     */

    void addDescriptor(DraggableGraphicsViewItemForText* df);
    void addDescriptor(DraggableGraphicsViewItem* df);

    void reset(bool byButton = false);
    void removeFromScene();

    void recreateFks(ERGraphicsScene*);

    void addNoteItem(DraggableGraphicsViewItemForText*);
    void addTableItem(DraggableGraphicsViewItem*);

    DraggableGraphicsViewItem* getTableItem(const QString& name);

    void updateDescriptors();

    Version* getVesion() const
    {
        return m_version;
    }

    /**
     * These below are called from the deserializer
     */
    void addTableDescriptor(DiagramTableDescriptor*);
    void addNoteDescriptor(DiagramNoteDescriptor* nd);
    void addFKDescriptor(DiagramFKDescriptor* nd);

    /**
     * Will generate the following XML:
     <Diagram name="Name">
     <Tables>
        <Table name="TabName" x="TableX" y="TableY" />
     </Tables>
     <Notes>
        <Note x="X" y="Y" frame="1/0">
            <Text> CDATA </Text>
        </Note>
     </Notes>
     <Fks>
        <Fk ... everything from the descriptor>
     </Fks>
     </Diagram>
     */
    virtual void serialize(QDomDocument &doc, QDomElement &parent) const;

    virtual QUuid getClassUid() const;

    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);

    virtual void updateGui();

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
    QVector <DiagramTableDescriptor*> m_tableDescriptors;

    DiagramForm* m_form;

    // a little bit confusing, means that the diagram was saved on the GUI (ie. a new tree entry has appeared for it)
    bool m_saved;

    Version* m_version;

    // the scope of this is to provide a workaround for the QT scene handling (destroying the scene whenever he wants) till I find a better solution
    bool m_removed;
};

#endif // DIAGRAM_H
