#ifndef DEFAULTVERSIONIMPLEMENTATION_H
#define DEFAULTVERSIONIMPLEMENTATION_H

#include "Version.h"
#include "VersionData.h"

#include <QTreeWidgetItem>

class Project;

class DefaultVersionImplementation : public Version
{
public:
    DefaultVersionImplementation(QTreeWidget* tree, QTreeWidget* dttree, QTreeWidget* itt, ContextMenuEnabledTreeWidgetItem* projectItem, Project* p, int, int);

    virtual void serialize(QDomDocument &doc, QDomElement &parent) const = 0;

    virtual void addNewDataType(UserDataType*);
    virtual void addDiagram(Diagram*);
    virtual const QVector<UserDataType*>& getDataTypes() const;
    virtual bool hasDataType(const QString& name) const;
    virtual UserDataType* getDataType(const QString& name);
    virtual int getDataTypeIndex(const QString& name);
    virtual void addTable(Table*);
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
    virtual void addTableInstance(TableInstance* inst);
    virtual const QVector<TableInstance*> & getTableInstances() const;
    virtual UserDataType* duplicateDataType(const QString&);
    virtual void deleteDiagram(const QString&);
    virtual VersionGuiElements* getGui();
    virtual QString getVersionText();
    virtual QVector<Table*> getTablesReferencingAColumnThroughForeignKeys(const Column*);
    virtual void setupForeignKeyRelationshipsForATable(Table *tab);
    virtual QList<QString> getSqlScript(const QString& codepage, bool generateDelimiters);
    virtual UserDataType* provideDatatypeForSqlType(const QString& name, const QString& sql, const QString& nullable, const QString& defaultValue, bool relaxed);
    virtual QVector<Issue*> checkIssuesOfNewColumn(Column* inNewColumn, Table* inTable);
    virtual void addIssuse(Issue*);
    virtual Issue* getIssue(const QString&);
    virtual void removeIssue(const QString &);
    virtual QVector<Issue*>& getIssues();
    virtual void validateVersion(bool onRequest);
    virtual void setSpecialValidationFlags(int);
    virtual View* getView(const QString &viewName) const;
    virtual void addView(View* v);
    virtual void deleteView(const QString &v);
    virtual const QVector<View*>& getViews();
    virtual Procedure* getProcedure(const QString &name) const;
    virtual Function* getFunction(const QString &name) const;
    virtual void addProcedure(Procedure* p);
    virtual const QVector<Procedure*>& getProcedures();
    virtual void deleteProcedure(const QString& p);
    virtual void cleanupDataTypes();
    virtual void addTrigger(Trigger *);
    virtual Trigger* getTrigger(const QString &name) const;
    virtual const QVector<Trigger*>& getTriggers();
    virtual SqlSourceEntity* getSqlSourceEntityNamed(const QString& name) const;
    virtual void addFunction(Function* p);
    virtual const QVector<Function*>& getFunctions();

    void createTreeItems(QTreeWidget* tree = 0, QTreeWidget* dtTree = 0, QTreeWidget* issueTree = 0, ContextMenuEnabledTreeWidgetItem* projectIem = 0);

protected:
    // the version as a string representation. Major versions are always of form X.0
    QString version;

    // the data of this version
    VersionData m_data;

    // the project tree
    QTreeWidget* m_tree;
    // the  data types tree
    QTreeWidget* m_dtTree;
    QTreeWidget* m_issueTree;

    ContextMenuEnabledTreeWidgetItem* m_projectItem;
    Project* m_project;

    VersionGuiElements* m_guiElements;
    int m_validationFlags;
};

#endif // DEFAULTVERSIONIMPLEMENTATION_H
