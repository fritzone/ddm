#ifndef DEFAULTVERSIONIMPLEMENTATION_H
#define DEFAULTVERSIONIMPLEMENTATION_H

#include "Version.h"
#include "VersionData.h"

#include <QMap>

class GuiElements;
class Project;
class Connection;
class Patch;
struct TableDeletionAction;

class DefaultVersionImplementation : public Version
{
public:

    DefaultVersionImplementation(Project* p, int, int);

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const = 0;
    virtual QUuid getClassUid() const = 0;
    virtual CloneableElement* clone(Version* sourceVersion, Version* targetVersion);
    virtual void updateGui();

    virtual void addNewDataType(UserDataType*, bool initial);
    virtual void addDiagram(Diagram*, bool initial);
    virtual const QVector<UserDataType*>& getDataTypes() const;
    virtual bool hasDataType(const QString& name) const;
    virtual UserDataType* getDataType(const QString& name);
    virtual int getDataTypeIndex(const QString& name);
    virtual void addTable(Table*t, bool initial);
    virtual bool hasTable(Table*);
    virtual bool hasTable(const QString&);
    virtual bool deleteTable(Table*);
    virtual void deleteTableInstance(TableInstance*);
    virtual void purgeSentencedTableInstances();
    virtual void deleteDataType(const QString&);
    virtual Table* getTable(const QString& name) const;
    virtual const QVector<Table*>& getTables() const;
    virtual const QVector<Diagram*>& getDiagrams() const;
    virtual Diagram* getDiagram(const QString& name);
    virtual Table* duplicateTable(Table*);
    virtual void removeForeignKeyFromDiagrams(ForeignKey*);
    virtual void setupTableParentChildRelationships();
    virtual TableInstance* instantiateTable(Table* tab, bool becauseOfReference);
    virtual TableInstance* getTableInstance(const QString& ) const;
    virtual void addTableInstance(TableInstance* inst, bool initial);
    virtual const QVector<TableInstance*> & getTableInstances() const;
    virtual UserDataType* duplicateDataType(const QString&);
    virtual void deleteDiagram(const QString&);
    virtual VersionGuiElements* getGui();
    virtual QString getVersionText();
    virtual QVector<Table*> getTablesReferencingAColumnThroughForeignKeys(const Column*);
    virtual void setupForeignKeyRelationshipsForATable(Table *tab);
    virtual QList<QString> getSqlScript(bool generateDelimiters, const Connection*);
    virtual UserDataType* provideDatatypeForSqlType(const QString& name, const QString& sql, const QString& nullable, const QString& defaultValue, bool relaxed);
    virtual QVector<Issue*> checkIssuesOfNewColumn(Column* inNewColumn, Table* inTable);
    virtual void addIssuse(Issue*);
    virtual Issue* getIssue(const QString&);
    virtual void removeIssue(const QString &);
    virtual QVector<Issue*>& getIssues();
    virtual void validateVersion(bool onRequest);
    virtual void setSpecialValidationFlags(int);
    virtual View* getView(const QString &viewName) const;
    virtual void addView(View* v, bool initial);
    virtual void deleteView(const QString &v);
    virtual const QVector<View*>& getViews();
    virtual Procedure* getProcedure(const QString &name) const;
    virtual Function* getFunction(const QString &name) const;
    virtual void addProcedure(Procedure* p, bool initial);
    virtual const QVector<Procedure*>& getProcedures();
    virtual void deleteProcedure(const QString& p);
    virtual void deleteFunction(const QString& f);
    virtual void deleteTrigger(const QString& t);
    virtual void cleanupDataTypes();
    virtual void addTrigger(Trigger *, bool initial);
    virtual Trigger* getTrigger(const QString &name) const;
    virtual const QVector<Trigger*>& getTriggers();
    virtual SqlSourceEntity* getSqlSourceEntityWithGuid(const QString& guid) const;
    virtual void addFunction(Function* p, bool initial);
    virtual const QVector<Function*>& getFunctions();
    virtual Table* getTableWithUid(const QString& uid) const;
    virtual TableInstance* getTableInstanceWithUid(const QString& uid) const;
    virtual Procedure* getProcedureWithUid(const QString& uid) const;
    virtual Function* getFunctionWithUid(const QString& uid) const;
    virtual Diagram* getDiagramWithUid(const QString& uid) const;
    virtual View* getViewWithUid(const QString& uid) const;
    virtual Trigger* getTriggerWithUid(const QString& uid) const;
    virtual UserDataType* getUserDataTypeWithUid(const QString& uid) const;
    virtual bool cloneInto(Version* other);
    virtual void patchItem(const QString& uid);
    virtual void replaceTable(const QString& uid, Table* newTab);
    virtual void replaceTableInstance(const QString& uid, TableInstance* newInst);
    virtual void replaceDiagram(const QString& uid, Diagram* withDgram);
    virtual void replaceView(const QString& uid, View* view);
    virtual void replaceTrigger(const QString& uid, Trigger* trg);
    virtual void replaceProcedure(const QString& uid, Procedure* proc);
    virtual void replaceUserDataType(const QString& uid, UserDataType* dt);
    virtual void replaceFunction(const QString& uid, Function* func);
    virtual Patch* getWorkingPatch();
    virtual bool undeleteObject(const QString& uid);

    void createTreeItems(GuiElements* gui, ContextMenuEnabledTreeWidgetItem* projectItem);
    void deleteTableInstance(TableInstance *tinst, TableDeletionAction*& );
    void doDeleteTableInstance(TableInstance *tinst, TableDeletionAction *& );

    void addPatch(Patch* p);

    enum CAN_UNDELETE_STATUS
    {
        CAN_UNDELETE = 0,
        DELETED_OBJECT_WAS_NOT_FOUND_IN_PATCH = 1,
        DEPENDENT_TABLE_WAS_NOT_FOUND_IN_VERSION = 2
    };

    /**
     * @brief canUndeleteTable check if we can undelete the table with the given UID... (if the deleted table instances contain
     * an instantiated table that was already deleted we cannot undelete the table)
     * @param uid
     * @return
     */
    CAN_UNDELETE_STATUS canUndeleteTable(const QString& uid, QString& extra);

    /**
     * @brief canUndeleteDiagram we can undelete a diagram only if the tables it is using are still in the version
     * @param uid
     * @return
     */
    CAN_UNDELETE_STATUS canUndeleteDiagram(const QString& uid, QString& extra);
    CAN_UNDELETE_STATUS canUndeleteProcedure(const QString& uid, QString& extra);
    CAN_UNDELETE_STATUS canUndeleteFunction(const QString& uid, QString& extra);
    CAN_UNDELETE_STATUS canUndeleteTrigger(const QString &uid, QString &extra);
    CAN_UNDELETE_STATUS canUndeleteView(const QString &uid, QString &extra);
    CAN_UNDELETE_STATUS canUndeleteDataType(const QString &uid, QString &extra);

    const QVector<Patch*>& getPatches()
    {
        return m_patches;
    }

protected:
    // the version as a string representation. Major versions are always of form X.0
    QString version;

    // the data of this version
    VersionData m_data;

    VersionGuiElements* m_guiElements;
    int m_validationFlags;

    QVector<Patch*> m_patches;
    int m_currentPatchIndex;
};

#endif // DEFAULTVERSIONIMPLEMENTATION_H


