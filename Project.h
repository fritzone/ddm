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

/**
 * Holds the data of a project.
 * TODO: I don't like that the clas is knowing a QTreeWidget item. When there's new time, find a way to detach it from this class.
 */
class Project : public SerializableElement, public TreeItem
{
public:

    // constructor
    Project(const QString& _name, QTreeWidget* _tree, QTreeWidget* _dtTree, QTreeWidget* _issueTree, bool oopIsEnabled);

    Project(const QString& _name, bool);

    // creates a version, updates the GUI
    void createMajorVersion();

    // creates a major version, does not update the gui, it must be done at a later stage
    void addMajorVersion(MajorVersion*);

    // set the SQL Data Type supplier for this project
    void setEngine(DatabaseEngine* eng);

    Version* getWorkingVersion() const;

    DatabaseEngine* getEngine() const;

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

    void createTreeItem(QTreeWidget* _tree, QTreeWidget* _dtt, QTreeWidget* _itt);

    /**
     * This should be called only by the deserialization method.
     */
    void populateTreeItem();

    const QString& getName() const
    {
        return m_name;
    }

    void setName(const QString& nm)
    {
        m_name = nm;
        m_location->setText(0, nm);
    }

    const QString& getDescription() const
    {
        return m_description;
    }

    void setDescription(const QString& desc)
    {
        m_description = desc;
    }

    bool oopProject() const
    {
        return m_oopIsEnabled;
    }

    QString getCodepage() const
    {
        return "latin1";
    }

private:

    // this is the tree from the main window of the application
    QTreeWidget* m_tree;
    QTreeWidget* m_dtTree;
    QTreeWidget* m_issueTree;

    // the name of the project
    QString m_name;

    // the description of the project
    QString m_description;

    // the data supplier of this project
    DatabaseEngine* m_engine;

    // the list of major versions
    QVector<MajorVersion*> m_majorVersions;

    // if this project supports OOP features
    bool m_oopIsEnabled;

};

#endif // PROJECT_H
