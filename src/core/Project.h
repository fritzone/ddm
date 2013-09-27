#ifndef PROJECT_H
#define PROJECT_H

#include "core_SerializableElement.h"
#include "TreeItem.h"
#include "core_NamedItem.h"

#include <QVector>
#include <QStringList>

class AbstractDTSupplier;
class MajorVersion;
class Version;
class DatabaseEngine;
class GuiElements;

/**
 * Holds the data of a project.
 * TODO: I don't like that the clas is knowing a QTreeWidget item. When there's new time, find a way to detach it from this class.
 */
class Project : public NamedItem, public SerializableElement, public TreeItem
{
public:

    Project();

    // constructor
    Project(const QString& _name, bool oop);

    // creates a version, updates the GUI
    MajorVersion* createMajorVersion(int major, int  minor);

    // creates a major version, does not update the gui, it must be done at a later stage
    void addMajorVersion(MajorVersion*);

    // set the SQL Data Type supplier for this project
    void setEngine(DatabaseEngine* eng);

    Version* getWorkingVersion() const;

    Version* getVersionNamed(const QString&) const;

    DatabaseEngine* getEngine() const;

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

    void createTreeItem(GuiElements*);

    /**
     * This should be called only by the deserialization method.
     */
    void populateTreeItem(GuiElements*);

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

    /**
     * Releases the current working version, creates the next one, copies over all the objects, and locks the current version.
     */
    Version* releaseMajorVersion();

    void setOop(bool a)
    {
        m_oopIsEnabled = a;
    }

    Version* getVersion(const QString& uid);

    QStringList getVersionNames();

    void setWorkingVersionIndex(int u)
    {
        m_workingVersionIndex = u;
    }

    QVector<MajorVersion*>& getMajorVersions()
    {
        return m_majorVersions;
    }

private:

    // the description of the project
    QString m_description;

    // the data supplier of this project
    DatabaseEngine* m_engine;

    // the list of major versions
    QVector<MajorVersion*> m_majorVersions;

    // if this project supports OOP features
    bool m_oopIsEnabled;

    int m_workingVersionIndex;

    GuiElements* m_gui;

};

#endif // PROJECT_H
