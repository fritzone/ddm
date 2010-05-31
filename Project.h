#ifndef PROJECT_H
#define PROJECT_H

#include "SerializableElement.h"
#include "TreeItem.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVector>

class AbstractDTSupplier;
class MajorVersion;
class Version;
class DatabaseEngine;
class ProjectDetailsForm;


/**
 * Holds the data of a project.
 * TODO: I don't like that the clas is knowing a QTreeWidget item. When there's new time, find a way to detach it from this class.
 */
class Project : public SerializableElement, public TreeItem
{
public:

    // constructor
    Project(const QString& _name, QTreeWidget* _tree);

    Project(const QString& _name);

    // creates a version, updates the GUI
    void createMajorVersion();

    // creates a major version, does not update the gui, it must be done at a later stage
    void addMajorVersion(MajorVersion*);

    // set the SQL Data Type supplier for this project
    void setEngine(DatabaseEngine* eng);

    Version* getWorkingVersion() const;

    DatabaseEngine* getEngine() const;

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

    void setDetailsForm(ProjectDetailsForm* pfrm)
    {
        m_prjDetailsForm = pfrm;
    }


    void createTreeItem(QTreeWidget* _tree);

    /**
     * This should be called only by the deserialization method.
     */
    void populateTreeItem();

private:

    // this is the tree from the main window of the application
    QTreeWidget* m_tree;

    // the name of the project
    QString m_name;

    // the data supplier of this project
    DatabaseEngine* m_engine;

    // the list of major versions
    QVector<MajorVersion*> m_majorVersions;

    ProjectDetailsForm* m_prjDetailsForm;

};

#endif // PROJECT_H
