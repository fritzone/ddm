#ifndef VERSIONGUIELEMENTS_H
#define VERSIONGUIELEMENTS_H

#include <QTreeWidget>

#include "ContextMenuEnabledTreeWidget.h"
#include "IconFactory.h"
#include "core_enums.h"

class Version;
class UserDataType;
class Diagram;
class Table;
class TableInstance;
class Issue;
class SqlForm;
class NewTableForm;
class View;
class ProcedureForm;
class Procedure;
class TriggerForm;
class Trigger;
class Function;
class GuiElements;
class DatabaseEngine;

/**
 * The scope of this class is to have a common collection point for the
 * tree widget items of a version (DT, Sql, Table, etc...)
 * This is a GUI class, ie. has access to the QtGui elements
 */
class VersionGuiElements
{
public:

    VersionGuiElements(GuiElements*, Version*);
    void collapseDTEntries();
    void cleanupDtEntries();

    /** @group project tree items */
    ContextMenuEnabledTreeWidgetItem* getTablesItem() const;
    ContextMenuEnabledTreeWidgetItem* getTableInstancesItem() const;
    ContextMenuEnabledTreeWidgetItem* getFinalSqlItem() const;
    ContextMenuEnabledTreeWidgetItem* getVersionItem() const;
    ContextMenuEnabledTreeWidgetItem* getDiagramsItem() const;
    ContextMenuEnabledTreeWidgetItem* getViewsItem() const;
    ContextMenuEnabledTreeWidgetItem* getProceduresItem() const;
    ContextMenuEnabledTreeWidgetItem* getFunctionsItem() const;
    ContextMenuEnabledTreeWidgetItem* getTriggersItem() const;
    ContextMenuEnabledTreeWidgetItem* getDocumentationItem() const;

    ContextMenuEnabledTreeWidgetItem* treeItemForUid(const QString &uid);

    /** @group data type items */
    ContextMenuEnabledTreeWidgetItem* getDtsItem() const;
    ContextMenuEnabledTreeWidgetItem* getIntsDtsItem();
    ContextMenuEnabledTreeWidgetItem* getStringDtsItem();
    ContextMenuEnabledTreeWidgetItem* getDateDtsItem();
    ContextMenuEnabledTreeWidgetItem* getBlobDtsItem();
    ContextMenuEnabledTreeWidgetItem* getBoolDtsItem();
    ContextMenuEnabledTreeWidgetItem* getMiscDtsItem();
    ContextMenuEnabledTreeWidgetItem* getSpatialDtsItem();

    bool hasIntsDtsItem() { return intsDtItem != 0; }
    bool hasStringDtsItem() { return stringsDtItem != 0; }
    bool hasDateDtsItem() { return dateDtItem != 0; }
    bool hasBlobDtsItem() { return blobDtItem != 0; }
    bool hasBoolDtsItem() { return boolDtItem != 0; }
    bool hasMiscDtsItem() { return miscDtItem != 0; }
    bool hasSpatialDtsItem() { return spatialDtItem != 0; }

    /** @group creators, other methods*/
    void createGuiElements(ContextMenuEnabledTreeWidgetItem* projectItem, int idxAfter, DatabaseEngine *dbEngine);
    void populateTreeItems();
    void cleanupOrphanedIssueTableItems();
    ContextMenuEnabledTreeWidgetItem* createDataTypeTreeEntry(UserDataType*);
    ContextMenuEnabledTreeWidgetItem* createDiagramTreeEntry(Diagram*);
    ContextMenuEnabledTreeWidgetItem* createTableTreeEntry(Table* tab);
    ContextMenuEnabledTreeWidgetItem* createTableTreeEntry(Table* tab, ContextMenuEnabledTreeWidgetItem* p);
    ContextMenuEnabledTreeWidgetItem* createTableInstanceTreeEntry(TableInstance* tab);
    ContextMenuEnabledTreeWidgetItem* createIssueTreeEntry(Issue* issue, ContextMenuEnabledTreeWidgetItem* p);
    ContextMenuEnabledTreeWidgetItem* createTableTreeEntryForIssue(Table* tab);
    ContextMenuEnabledTreeWidgetItem* createViewTreeEntry(View* view);
    ContextMenuEnabledTreeWidgetItem* createProcedureTreeEntry(Procedure* proc);
    ContextMenuEnabledTreeWidgetItem* createFunctionTreeEntry(Function* func);
    ContextMenuEnabledTreeWidgetItem* createTriggerTreeEntry(Trigger* trg);
    void updateForms();

    /** @group form retrievers */
    SqlForm* getSqlForm();
    NewTableForm* getTableFormForNewTable();
    NewTableForm* getTableFormForExistingTable();
    ProcedureForm* getProcedureForm(ProcedureFormMode, bool guided);
    TriggerForm* getTriggerForm();

private:

    /** @group project tree items */
    ContextMenuEnabledTreeWidgetItem* tablesItem;
    ContextMenuEnabledTreeWidgetItem* tableInstancesItem;
    ContextMenuEnabledTreeWidgetItem* versionItem;
    ContextMenuEnabledTreeWidgetItem* diagramsItem;
    ContextMenuEnabledTreeWidgetItem* proceduresItem;
    ContextMenuEnabledTreeWidgetItem* functionsItem;
    ContextMenuEnabledTreeWidgetItem* viewsItem;
    ContextMenuEnabledTreeWidgetItem* triggersItem;
    ContextMenuEnabledTreeWidgetItem* versionItemForDocs;

    /** @group the genTree items */
    ContextMenuEnabledTreeWidgetItem* finalSqlItem;
    ContextMenuEnabledTreeWidgetItem* documentationItem;

    /** @group the different trees for the app */
    QTreeWidget* m_tree;            // the solution objects tree
    QTreeWidget* m_issuesTree;      // the tree for the issues
    QTreeWidget* m_genTree;         // the tree for the generated items (code, documentation)
    QTreeWidget* m_patchesTree;         // the tree for the generated items (code, documentation)

    // the tree item containing the various data types
    ContextMenuEnabledTreeWidgetItem* dtsItem;
    ContextMenuEnabledTreeWidgetItem* stringsDtItem;
    ContextMenuEnabledTreeWidgetItem* intsDtItem;
    ContextMenuEnabledTreeWidgetItem* dateDtItem;
    ContextMenuEnabledTreeWidgetItem* blobDtItem;
    ContextMenuEnabledTreeWidgetItem* boolDtItem;
    ContextMenuEnabledTreeWidgetItem* miscDtItem;
    ContextMenuEnabledTreeWidgetItem* spatialDtItem;

    // the forms
    SqlForm* m_sqlForm;
    NewTableForm* m_newTableForm;
    NewTableForm* m_existingTableForm;
    ProcedureForm* m_procedureForm;
    TriggerForm* m_triggerForm;

    // the version
    Version* m_version;

};

#endif // VERSIONGUIELEMENTS_H
