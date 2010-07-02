#ifndef DIAGRAM_H
#define DIAGRAM_H

#include "TreeItem.h"
#include "NamedItem.h"

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

    DraggableGraphicsViewItemForText* getNote(const QString& note);

public:

    friend class ERGraphicsScene;
    friend class ERGraphicsView;

private:

    // these table elements are already on the stage
    QVector <DraggableGraphicsViewItem*> m_onStage;

    // these foreign key elements are already on the stage
    QVector <FkRelationDescriptor*> m_fksOnStage;

    // the diagrams' notes
    QVector <DraggableGraphicsViewItemForText*> m_notes;

    DiagramForm* m_form;

    bool m_saved;
};

#endif // DIAGRAM_H
