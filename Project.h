#ifndef PROJECT_H
#define PROJECT_H

#include "SerializableElement.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVector>

class AbstractDTSupplier;
class MajorVersion;
class Version;
class DatabaseEngine;

/**
 * Holds the data of a project.
 */
class Project : virtual public SerializableElement
{
public:

    // constructor
    Project(const QString& _name, QTreeWidget* _tree);

    // creates a version, updates the GUI
    void createMajorVersion();

    // set the SQL Data Type supplier for this project
    void setEngine(DatabaseEngine* eng);

    Version* getWorkingVersion() const;

    DatabaseEngine* getEngine() const;

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

private:

    // this is the tree from the main window of the application
    QTreeWidget* tree;

    // the tree item containing the "Project" name
    QTreeWidgetItem* projectItem;

    // the name of the project
    QString name;

    // the data supplier of this project
    DatabaseEngine* engine;

    // the list of major versions
    QVector<MajorVersion*> majorVersions;

};

#endif // PROJECT_H
