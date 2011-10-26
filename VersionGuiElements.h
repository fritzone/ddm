#ifndef VERSIONGUIELEMENTS_H
#define VERSIONGUIELEMENTS_H

#include <QTreeWidget>

#include "ContextMenuEnabledTreeWidget.h"
#include "IconFactory.h"

class Version;
class UserDataType;
class Diagram;
class Table;
class TableInstance;
class Issue;
class TableInstancesListForm;
class MainWindow;
class TablesListForm;
class SqlForm;
class NewTableForm;
class View;

/**
 * The scope of this class is to have a common collection point for the tree widget items of a version (DT, Sql, Table, etc...)
 * This is a GUI class, ie. has access to the QtGui elements
 */
class VersionGuiElements
{
public:

    VersionGuiElements(QTreeWidget*,QTreeWidget*,QTreeWidget*,Version*);


    ContextMenuEnabledTreeWidgetItem* getTablesItem() const
    {
        return tablesItem;
    }

    ContextMenuEnabledTreeWidgetItem* getTableInstancesItem() const
    {
        return tableInstancesItem;
    }

    ContextMenuEnabledTreeWidgetItem* getFinalSqlItem() const
    {
        return finalSqlItem;
    }


    ContextMenuEnabledTreeWidgetItem* getVersionItem() const
    {
        return versionItem;
    }


    ContextMenuEnabledTreeWidgetItem* getDiagramsItem() const
    {
        return diagramsItem;
    }

    ContextMenuEnabledTreeWidgetItem* getViewsItem() const
    {
        return viewsItem;
    }

    ContextMenuEnabledTreeWidgetItem* getDtsItem() const
    {
        return dtsItem;
    }

    ContextMenuEnabledTreeWidgetItem* getIntsDtsItem()
    {
        if(intsDtItem == 0)
        {
            intsDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem, QStringList(QObject::tr("Numeric"))) ;
            intsDtItem ->setIcon(0, IconFactory::getIntDataTypesIcon());
            m_dtTree->addTopLevelItem(intsDtItem);
        }
        return intsDtItem;
    }

    ContextMenuEnabledTreeWidgetItem* getStringDtsItem()
    {
        if(stringsDtItem == 0)
        {
            stringsDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem, QStringList(QObject::tr("String"))) ;
            stringsDtItem ->setIcon(0, IconFactory::getStringDataTypesIcon());
            m_dtTree->addTopLevelItem(stringsDtItem);
        }
        return stringsDtItem;
    }

    ContextMenuEnabledTreeWidgetItem* getDateDtsItem()
    {
        if(dateDtItem == 0)
        {
            dateDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem, QStringList(QObject::tr("Date/Time"))) ;
            dateDtItem ->setIcon(0, IconFactory::getDateTimeDataTypesIcon());
            m_dtTree->addTopLevelItem(dateDtItem);
        }

        return dateDtItem;
    }
    ContextMenuEnabledTreeWidgetItem* getBlobDtsItem()
    {
        if(blobDtItem == 0)
        {
            blobDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem, QStringList(QObject::tr("Blob/Binary"))) ;
            blobDtItem ->setIcon(0, IconFactory::getBlobDataTypesIcon());
            m_dtTree->addTopLevelItem(blobDtItem);
        }
        return blobDtItem;
    }

    ContextMenuEnabledTreeWidgetItem* getBoolDtsItem()
    {
        if(boolDtItem == 0)
        {
            boolDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem, QStringList(QObject::tr("Boolean"))) ;
            boolDtItem ->setIcon(0, IconFactory::getBoolDataTypesIcon());
            m_dtTree->addTopLevelItem(boolDtItem);
        }
        return boolDtItem;
    }

    ContextMenuEnabledTreeWidgetItem* getMiscDtsItem()
    {
        if(miscDtItem == 0)
        {
            miscDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem, QStringList(QObject::tr("Misc"))) ;
            miscDtItem ->setIcon(0, IconFactory::getMiscDataTypesIcon());
            m_dtTree->addTopLevelItem(boolDtItem);
        }
        return miscDtItem;
    }

    ContextMenuEnabledTreeWidgetItem* getSpatialDtsItem()
    {
        if(spatialDtItem == 0)
        {
            spatialDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem, QStringList(QObject::tr("Spatial"))) ;
            spatialDtItem ->setIcon(0, IconFactory::getSpatialDataTypesIcon());
            m_dtTree->addTopLevelItem(spatialDtItem);
        }
        return spatialDtItem;
    }

    void createGuiElements(ContextMenuEnabledTreeWidgetItem* projectItem);

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

    void setMainWindow(MainWindow* mw)
    {
        m_mw = mw;
    }

    void updateForms();

    TableInstancesListForm* getTableInstancesListForm();
    TablesListForm* getTablesListForm();
    SqlForm* getSqlForm();
    NewTableForm* getTableFormForNewTable();
    NewTableForm* getTableFormForExistingTable();

private:

    // the tree item containing the "Tables"
    ContextMenuEnabledTreeWidgetItem* tablesItem;

    // the tree item containing the "Table Instances"
    ContextMenuEnabledTreeWidgetItem* tableInstancesItem;

    // the tree item holding the version
    ContextMenuEnabledTreeWidgetItem* versionItem;

    // the tree item holding the version
    ContextMenuEnabledTreeWidgetItem* diagramsItem;

    // the tree item holding the version
    ContextMenuEnabledTreeWidgetItem* finalSqlItem;

    // the tree item containing the "DataType"
    ContextMenuEnabledTreeWidgetItem* dtsItem;

    // the tree item containing the "DataType"
    ContextMenuEnabledTreeWidgetItem* viewsItem;

    QTreeWidget* m_tree;

    QTreeWidget* m_dtTree;

    QTreeWidget* m_issuesTree;


    // the tree item containing the various data types
    ContextMenuEnabledTreeWidgetItem* stringsDtItem;
    ContextMenuEnabledTreeWidgetItem* intsDtItem;
    ContextMenuEnabledTreeWidgetItem* dateDtItem;
    ContextMenuEnabledTreeWidgetItem* blobDtItem;
    ContextMenuEnabledTreeWidgetItem* boolDtItem;
    ContextMenuEnabledTreeWidgetItem* miscDtItem;
    ContextMenuEnabledTreeWidgetItem* spatialDtItem;

    TableInstancesListForm* m_tblInstancesListForm;
    Version* m_version;
    MainWindow* m_mw;
    TablesListForm* m_tblsListForm;
    SqlForm* m_sqlForm;
    NewTableForm* m_newTableForm;
    NewTableForm* m_existingTableForm;
};

#endif // VERSIONGUIELEMENTS_H
