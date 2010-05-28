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
 */
class Project : virtual public SerializableElement, TreeItem
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
        prjDetailsForm = pfrm;
    }

private:

    void createTreeItem();

private:

    // this is the tree from the main window of the application
    QTreeWidget* tree;

    // the name of the project
    QString name;

    // the data supplier of this project
    DatabaseEngine* engine;

    // the list of major versions
    QVector<MajorVersion*> majorVersions;

    ProjectDetailsForm* prjDetailsForm;

};

#endif // PROJECT_H
