#ifndef VERSION_H
#define VERSION_H

#include "SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "core_CloneableElement.h"
#include "core_LockableElement.h"

class UserDataType;
class Table;
class ForeignKey;
class Diagram;
class ContextMenuEnabledTreeWidgetItem;
class TableInstance;
class DatabaseEngine;
class VersionGuiElements;
class Column;
class Issue;
class View;
class Procedure;
class Trigger;
class SqlSourceEntity;
class Function;
class Connection;
class Project;
class Patch;

/**
 * Basic class holding data related to versions
 */
class Version : public SerializableElement, public ObjectWithUid, public CloneableElement, public LockableElement
{
public:

    Version(int major, int minor, Project* p) : ObjectWithUid(QUuid::createUuid(), this), m_major(major), m_minor(minor), m_project(p)
    {}

    virtual ~Version() {}

    /**
     * Returns a constant reference to the data types of the version
     */
    virtual const QVector<UserDataType*>& getDataTypes() const = 0;

    /**
     * Returns the tables stored in this version
     */
    virtual const QVector<Table*>& getTables() const = 0;

    /**
     * Return the table instances
     */
    virtual const QVector<TableInstance*> & getTableInstances() const = 0;

    /**
     * Return the diagrams
     */
    virtual const QVector<Diagram*> & getDiagrams() const = 0;

    /**
     * Adds a new data type to this version
     */
    virtual void addNewDataType(UserDataType*, bool) = 0;

    /**
     * Checks if this version has a data type with the specified name
     */
    virtual bool hasDataType(const QString& name) const = 0;

    /**
     * Returns a r/w reference to the given data type, the caller can modify it.
     */
    virtual UserDataType* getDataType(const QString& name) = 0;

    /**
     * Returns the index of the given data type...
     */
    virtual int getDataTypeIndex(const QString& name) = 0;

    /**
     * @brief addTable Adds a new table to the system
     * @param t - the table to add
     * @param initial - true if this is done while loading a solution, false if a normal addition is happening
     */
    virtual void addTable(Table* t, bool initial) = 0;

    /**
     * Add a diagram to the system
     */
    virtual void addDiagram(Diagram*, bool initial) = 0;


    /**
     * Checks if this version has the given table already. The comparison is done based on
     * the table name which is supposed to be unique.
     */
    virtual bool hasTable(const QString&) = 0;

    /**
     * Checks if this version has the given table already. The comparison is done based on
     * the table name which is supposed to be unique.
     */
    virtual bool hasTable(Table*) = 0;

    /**
     * Return the table with the given name for r/w access
     */
    virtual Table* getTable(const QString& name) const = 0;

    /**
     * Return the table with the given uid for r/w access
     */
    virtual Table* getTableWithUid(const QString& uid) const = 0;

    /**
     * Return the diagram with the given uid for r/w access
     */
    virtual Diagram* getDiagramWithUid(const QString& uid) const = 0;

    /**
     * @brief getProcedureWithUid
     * @param uid
     * @return
     */
    virtual Procedure* getProcedureWithUid(const QString& uid) const = 0;

    /**
     * @brief getFunctionWithUid
     * @param uid
     * @return
     */
    virtual Function* getFunctionWithUid(const QString& uid) const = 0;

    /**
     * @brief getUserDataTypeWithUid
     * @param uid
     * @return
     */
    virtual UserDataType* getUserDataTypeWithUid(const QString& uid) const = 0;

    /**
     * @brief getViewWithUid Returns the View with the given UID
     * @param uid
     * @return
     */
    virtual View* getViewWithUid(const QString& uid) const = 0;

    /**
     * @brief getTriggerWithUid
     * @param uid
     * @return
     */
    virtual Trigger* getTriggerWithUid(const QString& uid) const = 0;

    /**
     * Return the table instance with the given uid for r/w access
     */
    virtual TableInstance* getTableInstanceWithUid(const QString& uid) const = 0;

    /**
     * Serializes this version
     */
    virtual void serialize(QDomDocument &doc, QDomElement &parent) const = 0;

    /**
     * Returns the diagram with the given name
     */
    virtual Diagram* getDiagram(const QString& name) = 0;

    /**
     * Removes the foreign key from teh version
     */
    virtual void removeForeignKeyFromDiagrams(ForeignKey*) = 0;

    /**
     * Deletes a table from the system. Also deletes the instantiated tables,
     * and resolves the Diagrams
     * Does not delete the table if there are foreign keys referencing it.
     * @return true if the table was succesfully deleted, false if not
     */
    virtual bool deleteTable(Table*) = 0;

    /**
     * Deletes the given table instance, and also all the other table instances
     * that were instantiated due to foreign keys referencing this table instance
     * when it was instantiated
     */
    virtual void deleteTableInstance(TableInstance*) = 0;

    /**
     * Duplicates the table
     */
    virtual Table* duplicateTable(Table*) = 0;

    /**
     * Sets up a parent child relationship. Used in the factory methods of creating a version
     */
    virtual void setupTableParentChildRelationships() = 0;

    /**
     * Sets up the foreign key relationships. Used in the factory methods of creating a version
     */
    virtual void setupForeignKeyRelationshipsForATable(Table* tab) = 0;

    /**
     * Instantiates a table from the given table template
     */
    virtual TableInstance* instantiateTable(Table* tab, bool becauseOfReference) = 0;

    /**
     * @brief addTableInstance Adds a table instance to the application
     * @param inst - the instane
     * @param initial - true if this happens while loading a solution/reverse engineering, false otherwise
     */
    virtual void addTableInstance(TableInstance* inst, bool initial) = 0;

    /**
     * Returns a table instance
     */
    virtual TableInstance* getTableInstance(const QString& ) const = 0;

    /**
     * Purges the table instances that were sentenced in a "delete table instance" procedure
     */
    virtual void purgeSentencedTableInstances() = 0;

    /**
     * Deletes the given data type
     */
    virtual void deleteDataType(const QString&) = 0;

    /**
     * Duplicates the given data type
     */
    virtual UserDataType* duplicateDataType(const QString&) = 0;

    /**
     * Deletes a diagram from this version
     */
    virtual void deleteDiagram(const QString&) = 0;

    /**
     * Returns the GUI elements (the Version tree widget item, and the otehrs) of this version
     */
    virtual VersionGuiElements* getGui() = 0;

    /**
     * Returns the version as a human readable string
     */
    virtual QString getVersionText() = 0;

    /**
     * Return a vector of tables that are referencing the given column through a foreign key
     */
    virtual QVector<Table*> getTablesReferencingAColumnThroughForeignKeys(const Column*) = 0;

    virtual QList<QString> getSqlScript(bool generateDelimiters, const Connection*) = 0;

    /**
     * Provides a data type for the given SQL type. Firstly checks if there are data types corresponding to the
     * given name/size pair and and if found one, returns it. If did not find any, creates a new one, adds to the
     * vector of data types and returns it.
     * The scope of relaxed is that if it's false it tries to build data types based on the @param name (allowing
     * each column to have its own datatype, thus enforcing more strict foreign key policies) otherwise
     * it builds the data types from the SQL type, allowing more relaxed foreign keys.
     */
    virtual UserDataType* provideDatatypeForSqlType(const QString& name, const QString& sql, const QString& nullable, const QString& defaultValue, bool relaxed) = 0;

    /**
     * Method called when a new column was created in this version, for the given table. It will report
     * issues that the new column introduces (such as normalization, or foreign key suggestion..)
     */
    virtual QVector<Issue*> checkIssuesOfNewColumn(Column* inNewColumn, Table* inTable) = 0;

    /**
     * Adds an issue to the version
     */
    virtual void addIssuse(Issue*) = 0;

    /**
     * Retrieves the issue with the given name, or 0 if there is none
     */
    virtual Issue* getIssue(const QString&) = 0;

    /**
     * Removes the issue from the version
     */
    virtual void removeIssue(const QString&) = 0;

    /**
     * Retrieves the versions' issues
     */
    virtual QVector<Issue*>& getIssues() = 0;

    /**
     * Validates the version. The flag tells if it's an automatic or on request validation
     */
    virtual void validateVersion(bool onRequest) = 0;

    /**
     * Sets the valiadtion flags for the next validation
     */
    virtual void setSpecialValidationFlags(int) = 0;

    /**
     * Returns the given view
     */
    virtual View* getView(const QString& viewName) const = 0;

    /**
     * Adds a view in the system
     */
    virtual void addView(View* v, bool initial) = 0;

    /**
     * Deletes the view from the system
     */
    virtual void deleteView(const QString& v) = 0;

    /**
     * Return the views of the version
     */
    virtual const QVector<View*>& getViews() = 0;

    /**
    * Returns the given procedure
    */
    virtual Procedure* getProcedure(const QString& procName) const = 0;

    /**
    * Adds a procedure in the system
    */
    virtual void addProcedure(Procedure* p, bool initial) = 0;

    /**
    *  Returns all the procedures of the system
    */
    virtual const QVector<Procedure*>& getProcedures() = 0;

    /**
    * Delete the given procedure
    */
    virtual void deleteProcedure(const QString&) = 0;
    virtual void deleteFunction(const QString&) = 0;
    virtual void deleteTrigger(const QString&) = 0;

    /**
    * Deletes all the unused data types
    */
    virtual void cleanupDataTypes() = 0;

    virtual void addTrigger(Trigger*, bool) = 0;
    virtual Trigger* getTrigger(const QString&) const = 0;
    virtual const QVector<Trigger*>& getTriggers() = 0;

    virtual SqlSourceEntity* getSqlSourceEntityWithGuid(const QString& name) const = 0;

    virtual void addFunction(Function* p, bool) = 0;
    virtual Function* getFunction(const QString& procName) const = 0;
    virtual const QVector<Function*>& getFunctions() = 0;

    /**
     * Clones this version into the other one, ie. it inserts all the elements of this version
     * into the other version.
     * @param other - the destination version
     */
    virtual bool cloneInto(Version* other) = 0;

    virtual void patchItem(const QString& uid) = 0;

    virtual QUuid getClassUid() const = 0;

    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion) = 0;

    virtual void updateGui() = 0;

    void setVersionNumbers(int major, int minor)
    {
        if(major > m_major)
        {
            m_major = major;
            m_minor = minor;
        }
    }

    int getMajor() const
    {
        return m_major;
    }

    int getMinor() const
    {
        return m_minor;
    }

    const Project* getProject() const
    {
        return m_project;
    }

    Project* getProject()
    {
        return m_project;
    }

    void setProject(Project* p)
    {
        m_project = p;
    }

    /**
     * @brief getWorkingPatch
     * @return the patch on which we are currently working. Patches can be
     */
    virtual Patch* getWorkingPatch() = 0;

    /**
     * @brief replaceTable replaces the table with uid with the newTab. Used only at the version handling!
     * @param uid
     * @param newTab
     */
    virtual void replaceTable(const QString& uid, Table* newTab) = 0;

    /**
     * @brief replaceTableInstance replaces the table instance with the gven uid with the newInst
     * @param uid
     * @param newTab
     */
    virtual void replaceTableInstance(const QString& uid, TableInstance* newInst) = 0;

    /**
     * @brief replaceDiagram replaces the diagram with the given UID with the diagram
     * @param uid
     * @param withDgram
     */
    virtual void replaceDiagram(const QString& uid, Diagram* withDgram) = 0;

    /**
     * @brief replaceView replaces the view with the given v
     * @param uid
     */
    virtual void replaceView(const QString& uid, View* v) = 0;

    /**
     * @brief replaceTrigger replaces a trigger
     * @param uid
     * @param t
     */
    virtual void replaceTrigger(const QString& uid, Trigger* t) = 0;

    /**
     * @brief replaceProcedure replace a procedure
     * @param uid
     * @param p
     */
    virtual void replaceProcedure(const QString& uid, Procedure* p) = 0;

    /**
     * @brief replaceFunction
     * @param uid
     * @param f
     */
    virtual void replaceFunction(const QString& uid, Function* f) = 0;

    /**
     * @brief replaceUserDataType
     * @param uid
     * @param dt
     */
    virtual void replaceUserDataType(const QString& uid, UserDataType* dt) = 0;

    /**
     * @brief undeleteObject Undeletes the object with given UID. The object was supposed to be deleted in a patch.
     * @param uid
     */
    virtual bool undeleteObject(const QString& uid, bool suspend) = 0;

    /**
     * @brief removePatch
     * @param p
     */
    virtual void removePatch(const Patch* p) = 0;

protected:
    int m_major;
    int m_minor;
    Project* m_project;
};

#endif // VERSION_H
