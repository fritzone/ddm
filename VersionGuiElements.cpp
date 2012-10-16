#include "VersionGuiElements.h"
#include "ContextMenuEnabledTreeWidget.h"
#include "IconFactory.h"
#include "Version.h"
#include "core_UserDataType.h"
#include "ContextMenuCollection.h"
#include "core_Table.h"
#include "core_Diagram.h"
#include "core_TableInstance.h"
#include "Workspace.h"
#include "core_TableInstance.h"
#include "Issue.h"
#include "IssueOriginator.h"
#include "MainWindow.h"
#include "SqlForm.h"
#include "Project.h"
#include "NewTableForm.h"
#include "core_View.h"
#include "ProcedureForm.h"
#include "core_Procedure.h"
#include "TriggerForm.h"
#include "core_Trigger.h"
#include "core_Function.h"
#include "GuiElements.h"
#include "UidWarehouse.h"

#include <QVector>
#include <QtGui>

// TODO: This file looks funny. See why it seems we have a lot of duplicate code

VersionGuiElements::VersionGuiElements(GuiElements* guiElements, Version* v) :
    tablesItem(0), tableInstancesItem(0), versionItem(0), diagramsItem(0),
    proceduresItem(0), functionsItem(0), viewsItem(0),
    triggersItem(0), versionItemForDocs(0),
    finalSqlItem(0), documentationItem(0),
    m_tree(guiElements->getProjectTree()),
    m_issuesTree(guiElements->getIssuesTree()),
    m_genTree(guiElements->getProjectTree()),
    dtsItem(0), stringsDtItem(0), intsDtItem(0), dateDtItem(0), blobDtItem(0),
    boolDtItem(0), miscDtItem(0), spatialDtItem(0),
    m_newTableForm(0), m_existingTableForm(0), m_procedureForm(0),
    m_version(v)
{
}

void VersionGuiElements::collapseDTEntries()
{
    if(stringsDtItem != 0) getStringDtsItem()->setExpanded(false);
    if(intsDtItem != 0)    getIntsDtsItem()->setExpanded(false);
    if(dateDtItem != 0)    getDateDtsItem()->setExpanded(false);
    if(blobDtItem != 0)    getBlobDtsItem()->setExpanded(false);
    if(boolDtItem != 0)    getBoolDtsItem()->setExpanded(false);
    if(miscDtItem != 0)    getMiscDtsItem()->setExpanded(false);
    if(spatialDtItem != 0) getSpatialDtsItem()->setExpanded(false);
}

void VersionGuiElements::cleanupDtEntries()
{
    if(stringsDtItem != 0)
    {
        if(stringsDtItem->childCount() == 0) delete stringsDtItem;
    }

    if(intsDtItem != 0)
    {
        if(intsDtItem->childCount() == 0) delete intsDtItem;
    }

    if(dateDtItem != 0)
    {
        if(dateDtItem->childCount() == 0) delete dateDtItem;
    }

    if(blobDtItem != 0)
    {
        if(blobDtItem->childCount() == 0) delete blobDtItem;
    }

    if(boolDtItem != 0)
    {
        if(boolDtItem->childCount() == 0) delete boolDtItem;
    }

    if(miscDtItem != 0)
    {
        if(miscDtItem->childCount() == 0) delete miscDtItem;
    }

    if(spatialDtItem != 0)
    {
        if(spatialDtItem->childCount() == 0) delete spatialDtItem;
    }

}


void VersionGuiElements::createGuiElements(ContextMenuEnabledTreeWidgetItem* projectItem, int idxAfter)
{
    versionItem = new ContextMenuEnabledTreeWidgetItem(0, QStringList(QString("Ver: ") + m_version->getVersionText())) ;

    if(idxAfter == -1)
    {
        m_tree->addTopLevelItem(versionItem);
    }
    else
    {
        m_tree->insertTopLevelItem(idxAfter, versionItem);
    }

    QVariant a;
    a.setValue(m_version->getVersionText());
    versionItem->setData(0, Qt::UserRole, a);
    m_version->setLocation(versionItem);

    // make the tables sub item coming from the version
    tablesItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Table templates"))) ;
    tablesItem->setIcon(0, IconFactory::getTablesIcon());
    tablesItem->setPopupMenu(ContextMenuCollection::getInstance()->getTablesPopupMenu());
    m_tree->addTopLevelItem(tablesItem);
    QUuid tablesUid = QUuid::createUuid();
    tablesItem->setData(0, Qt::UserRole, QVariant(tablesUid));
    UidWarehouse::instance().addElement(tablesUid, m_version);

    // make the views sub item coming from the version
    tableInstancesItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Tables"))) ;
    tableInstancesItem->setIcon(0, IconFactory::getTabinstIcon());
    tableInstancesItem->setPopupMenu(ContextMenuCollection::getInstance()->getTableInstancesPopupMenu());
    m_tree->addTopLevelItem(tableInstancesItem);
    QUuid tablesInstUid = QUuid::createUuid();
    tableInstancesItem->setData(0, Qt::UserRole, QVariant(tablesInstUid ));
    UidWarehouse::instance().addElement(tablesInstUid , m_version);

    // diagrams should come from the version
    diagramsItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Diagrams"))) ;
    diagramsItem->setIcon(0, IconFactory::getDiagramIcon());
    diagramsItem->setPopupMenu(ContextMenuCollection::getInstance()->getDiagramsPopupMenu());
    m_tree->addTopLevelItem(diagramsItem);
    QUuid dgramsUid = QUuid::createUuid();
    diagramsItem->setData(0, Qt::UserRole, QVariant(dgramsUid ));
    UidWarehouse::instance().addElement(dgramsUid , m_version);

    // procedures are still coming from the version
    proceduresItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Procedures"))) ;
    proceduresItem->setIcon(0, IconFactory::getProcedureIcon());
    proceduresItem->setPopupMenu(ContextMenuCollection::getInstance()->getProceduresPopupMenu());
    m_tree->addTopLevelItem(proceduresItem);
    QUuid procsUid = QUuid::createUuid();
    proceduresItem->setData(0, Qt::UserRole, QVariant(procsUid ));
    UidWarehouse::instance().addElement(procsUid , m_version);

    // functions are still coming from the version
    functionsItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Functions"))) ;
    functionsItem->setIcon(0, IconFactory::getFunctionTreeIcon());
    functionsItem->setPopupMenu(ContextMenuCollection::getInstance()->getFunctionsPopupMenu());
    m_tree->addTopLevelItem(functionsItem);
    QUuid funcsUid = QUuid::createUuid();
    functionsItem->setData(0, Qt::UserRole, QVariant(funcsUid  ));
    UidWarehouse::instance().addElement(funcsUid  , m_version);

    // views come from the version item too
    viewsItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Views"))) ;
    viewsItem->setIcon(0, IconFactory::getViewsIcon());
    viewsItem->setPopupMenu(ContextMenuCollection::getInstance()->getViewsPopupMenu());
    m_tree->addTopLevelItem(viewsItem);
    QUuid viewsUid = QUuid::createUuid();
    viewsItem->setData(0, Qt::UserRole, QVariant(viewsUid  ));
    UidWarehouse::instance().addElement(viewsUid  , m_version);

    // triggers
    triggersItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Triggers"))) ;
    triggersItem->setIcon(0, IconFactory::getTriggersIcon());
    triggersItem->setPopupMenu(ContextMenuCollection::getInstance()->getTriggersPopupMenu());
    m_tree->addTopLevelItem(triggersItem);
    QUuid trigsUid = QUuid::createUuid();
    triggersItem->setData(0, Qt::UserRole, QVariant(trigsUid));
    UidWarehouse::instance().addElement(trigsUid, m_version);

    // data types
    dtsItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Data Types"))) ;
    dtsItem->setIcon(0, IconFactory::getDataTypesIcon());
    dtsItem->setPopupMenu(ContextMenuCollection::getInstance()->getDatatypesPopupMenu());
    m_tree->addTopLevelItem(dtsItem);
    QUuid dtsUid = QUuid::createUuid();
    dtsItem->setData(0, Qt::UserRole, QVariant(dtsUid));
    UidWarehouse::instance().addElement(dtsUid, m_version);

    // SQLs
    ContextMenuEnabledTreeWidgetItem* codeItem= new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Code"))) ;
    codeItem->setIcon(0, IconFactory::getCodeIcon());
    m_tree->addTopLevelItem(codeItem);

    finalSqlItem= new ContextMenuEnabledTreeWidgetItem(codeItem, QStringList(QObject::tr("SQL"))) ;
    finalSqlItem->setIcon(0, IconFactory::getSqlIcon());
    m_tree->addTopLevelItem(finalSqlItem);
    QUuid sqlsUid = QUuid::createUuid();
    finalSqlItem->setData(0, Qt::UserRole, QVariant(sqlsUid));
    UidWarehouse::instance().addElement(sqlsUid, m_version);

    // documentation
    documentationItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Documentation"))) ;
    documentationItem->setIcon(0, IconFactory::getHelpIcon());
    //viewsItem->setPopupMenu(ContextMenuCollection::getInstance()->getDiagramsPopupMenu());
    m_tree->addTopLevelItem(documentationItem);
    QUuid docsUid = QUuid::createUuid();
    documentationItem->setData(0, Qt::UserRole, QVariant(docsUid));
    UidWarehouse::instance().addElement(docsUid, m_version);

    m_version->updateGui();
}

void VersionGuiElements::populateTreeItems()
{
    // insert the user data types in the tree
    const QVector<UserDataType*>& dataTypes = m_version->getDataTypes();
    for(int i=0; i<dataTypes.size(); i++)
    {
        QStringList a(dataTypes[i]->getName());

        ContextMenuEnabledTreeWidgetItem* parent = getDtsItem();
        if(dataTypes[i]->getType() == DT_STRING) parent = getStringDtsItem();
        if(dataTypes[i]->getType() == DT_NUMERIC) parent = getIntsDtsItem();
        if(dataTypes[i]->getType() == DT_DATETIME) parent = getDateDtsItem();
        if(dataTypes[i]->getType() == DT_BLOB) parent = getBlobDtsItem();
        if(dataTypes[i]->getType() == DT_BOOLEAN) parent = getBoolDtsItem();
        if(dataTypes[i]->getType() == DT_MISC) parent = getMiscDtsItem();
        if(dataTypes[i]->getType() == DT_SPATIAL) parent = getSpatialDtsItem();

        ContextMenuEnabledTreeWidgetItem* newDTItem = new ContextMenuEnabledTreeWidgetItem(parent, a) ;
        QVariant var;
        var.setValue(dataTypes[i]->getObjectUid().toString());
        newDTItem->setData(0, Qt::UserRole, var);
        newDTItem->setPopupMenu(ContextMenuCollection::getInstance()->getDatatypePopupMenu());
        // set the icon, add to the tree
        newDTItem->setIcon(0, IconFactory::getIconForDataType(dataTypes.at(i)->getType()));
        m_tree->insertTopLevelItem(0, newDTItem);
        dataTypes[i]->setLocation(newDTItem);

        dataTypes[i]->updateGui();
    }

    // insert the tables
    const QVector<Table*>& tables = m_version->getTables();
    for(int i=0; i<tables.size(); i++)
    {
        Table* tab = tables.at(i);
        ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(getTablesItem(), QStringList(tab->getName())) ;

        QVariant var(tab->getObjectUid());
        newTblsItem->setData(0, Qt::UserRole, var);
        newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getTablePopupMenu());
        // set the icon, add to the tree
        newTblsItem->setIcon(0, IconFactory::getTablesIcon());
        m_tree->insertTopLevelItem(0, newTblsItem);

        // set the link to the tree
        tab->setLocation(newTblsItem);

        // the documentation item
        ContextMenuEnabledTreeWidgetItem* docItem = new ContextMenuEnabledTreeWidgetItem(getDocumentationItem(), QStringList(tab->getName()));
        docItem->setIcon(0, IconFactory::getTableIcon());
        docItem->setData(0, Qt::UserRole, var);
        m_tree->addTopLevelItem(docItem);

        tab->setDocItem(docItem);

        tab->updateGui();
    }

    // now update the parents
    for(int i=0; i<tables.size(); i++)
    {
        if(tables[i]->getParent() != 0)
        {
            QTreeWidgetItem* p = tables[i]->getLocation();
            p->parent()->removeChild(p);
            tables[i]->getParent()->getLocation()->addChild(p);
        }
    }

    // insert the diagrams
    const QVector<Diagram*>& diagrams = m_version->getDiagrams();
    for(int i=0; i<diagrams.size(); i++)
    {
        Diagram* dI = diagrams.at(i);
        ContextMenuEnabledTreeWidgetItem* newDgramItem = new ContextMenuEnabledTreeWidgetItem(getDiagramsItem(), QStringList(dI->getName())) ;

        QVariant var(dI->getObjectUid());
        newDgramItem->setData(0, Qt::UserRole, var);
        // set the icon, add to the tree
        newDgramItem->setIcon(0, IconFactory::getDiagramIcon());
        newDgramItem->setPopupMenu(ContextMenuCollection::getInstance()->getDiagramPopupMenu());
        m_tree->insertTopLevelItem(0, newDgramItem);

        // set the link to the tree
        dI->setLocation(newDgramItem);

        dI->updateGui();
    }

    // insert the table instances (but only if this is an OOP project...)
    const QVector<TableInstance*> &  tableInstances = m_version->getTableInstances();
    for(int i=0; i<tableInstances.size(); i++)
    {
        TableInstance* tI = tableInstances.at(i);
        ContextMenuEnabledTreeWidgetItem* newTabInstItem = new ContextMenuEnabledTreeWidgetItem(getTableInstancesItem(), QStringList(tI->getName())) ;

        QVariant var(tI->getObjectUid());
        newTabInstItem->setData(0, Qt::UserRole, var);
        newTabInstItem->setPopupMenu(ContextMenuCollection::getInstance()->getTableInstancePopupMenu());

        // set the icon, add to the tree
        if(tI->instantiatedBecuaseOfRkReference())
        {
            newTabInstItem->setIcon(0, IconFactory::getTabinstLockIcon());
        }
        else
        {
            newTabInstItem->setIcon(0, IconFactory::getTabinstIcon());
        }

        m_tree->insertTopLevelItem(0, newTabInstItem);

        // set the link to the tree
        tI->setLocation(newTabInstItem);
        tI->updateGui();
    }

    // now populate the "Code" tree items by adding the SQLs of the tables or table instances
    if(Workspace::getInstance()->currentProjectIsOop())
    {   // insert each table instance entry, since this is an OOP project
        for(int i=0; i<tableInstances.size(); i++)
        {
            TableInstance* tI = tableInstances[i];
            ContextMenuEnabledTreeWidgetItem* tabInstSqlItem = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(tI->getName())) ;

            QVariant var(tI->getObjectUid());
            tabInstSqlItem->setData(0, Qt::UserRole, var);
            tI->setSqlItem(tabInstSqlItem);

            // set the icon, add to the tree
            if(tI->instantiatedBecuaseOfRkReference())
            {
                tabInstSqlItem->setIcon(0, IconFactory::getTabinstLockIcon());
            }
            else
            {
                tabInstSqlItem->setIcon(0, IconFactory::getTabinstIcon());
            }
            m_tree->insertTopLevelItem(0, tabInstSqlItem);
        }
    }
    else
    {
        // insert the tables
        for(int i=0; i<tables.size(); i++)
        {
            Table* tbl = tables.at(i);
            ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(tbl->getName())) ;

            QVariant var(tbl->getObjectUid());
            newTblsItem->setData(0, Qt::UserRole, var);
            newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getTablePopupMenu());
            // set the icon, add to the tree
            newTblsItem->setIcon(0, IconFactory::getTablesIcon());
            m_tree->insertTopLevelItem(0, newTblsItem);
        }
    }

    // add the views to the main tree
    const QVector<View*> views = m_version->getViews();
    for(int i=0; i<views.size(); i++)
    {
        createViewTreeEntry(views.at(i));
    }

    // add the procedures to the main tree
    const QVector<Procedure*> procs = m_version->getProcedures();
    for(int i=0; i<procs.size(); i++)
    {
        createProcedureTreeEntry(procs.at(i));
    }

    // add the functions to the main tree
    const QVector<Function*> funcs = m_version->getFunctions();
    for(int i=0; i<funcs.size(); i++)
    {
        createFunctionTreeEntry(funcs.at(i));
    }

    // add the triggers to the main tree
    const QVector<Trigger*> triggers = m_version->getTriggers();
    for(int i=0; i<triggers.size(); i++)
    {
        createTriggerTreeEntry(triggers.at(i));
    }
}


ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createDataTypeTreeEntry(UserDataType* udt)
{
    QStringList itm(udt->getName());
    itm << udt->sqlAsString();
    ContextMenuEnabledTreeWidgetItem* parent = getDtsItem();

    if(udt->getType() == DT_STRING) parent = getStringDtsItem();
    if(udt->getType() == DT_NUMERIC) parent = getIntsDtsItem();
    if(udt->getType() == DT_DATETIME) parent = getDateDtsItem();
    if(udt->getType() == DT_BLOB) parent = getBlobDtsItem();
    if(udt->getType() == DT_BOOLEAN) parent = getBoolDtsItem();
    if(udt->getType() == DT_MISC) parent = getMiscDtsItem();
    if(udt->getType() == DT_SPATIAL) parent = getSpatialDtsItem();

    ContextMenuEnabledTreeWidgetItem* newDTItem = new ContextMenuEnabledTreeWidgetItem(parent, itm) ;

    QVariant var;
    var.setValue(udt->getObjectUid().toString());
    newDTItem->setData(0, Qt::UserRole, var);
    // set the icon, add to the tree
    newDTItem->setIcon(0, IconFactory::getIconForDataType(udt->getType()));
    newDTItem->setPopupMenu(ContextMenuCollection::getInstance()->getDatatypePopupMenu());
    m_tree->insertTopLevelItem(0,newDTItem);

    // set the link to the tree
    udt->setLocation(newDTItem);

    return newDTItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createDiagramTreeEntry(Diagram* dgram)
{
    ContextMenuEnabledTreeWidgetItem* newDgramItem = new ContextMenuEnabledTreeWidgetItem(getDiagramsItem(), QStringList(dgram->getName())) ;
    QVariant var(dgram->getObjectUid());
    newDgramItem->setData(0, Qt::UserRole, var);
    newDgramItem->setIcon(0, IconFactory::getDiagramIcon());
    newDgramItem->setPopupMenu(ContextMenuCollection::getInstance()->getDiagramPopupMenu());
    dgram->setLocation(newDgramItem);
    dgram->setSaved(true);
    m_tree->addTopLevelItem(newDgramItem);

    return newDgramItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createTableTreeEntry(Table* tab)
{
    return createTableTreeEntry(tab, getTablesItem());
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createTableTreeEntry(Table* tab, ContextMenuEnabledTreeWidgetItem* p)
{
    ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(p, QStringList(tab->getName())) ;
    QVariant var(tab->getObjectUid());
    newTblsItem->setData(0, Qt::UserRole, var);
    newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getTablePopupMenu());
    newTblsItem->setIcon(0, IconFactory::getTablesIcon());
    m_tree->addTopLevelItem(newTblsItem);
    tab->setLocation(newTblsItem);

    // the documentation item

    ContextMenuEnabledTreeWidgetItem* docItem = new ContextMenuEnabledTreeWidgetItem(getDocumentationItem(), QStringList(tab->getName()));
    docItem->setIcon(0, IconFactory::getHelpIcon());
    docItem->setData(0, Qt::UserRole, var);
    m_tree->addTopLevelItem(docItem);
    tab->setDocItem(docItem);
    return newTblsItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createViewTreeEntry(View* view)
{
    ContextMenuEnabledTreeWidgetItem* newViewItem = new ContextMenuEnabledTreeWidgetItem(viewsItem, QStringList(view->getName())) ;
    QVariant var(view->getObjectUid());
    newViewItem->setData(0, Qt::UserRole, var);
    newViewItem->setPopupMenu(ContextMenuCollection::getInstance()->getViewPopupMenu());
    newViewItem->setIcon(0, IconFactory::getViewIcon());
    m_tree->addTopLevelItem(newViewItem);
    view->setLocation(newViewItem);

    ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(view->getName()));
    sqlItm->setIcon(0, IconFactory::getViewIcon());
    sqlItm->setData(0, Qt::UserRole, var);
    view->setSqlItem(sqlItm);

    return newViewItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createProcedureTreeEntry(Procedure* proc)
{
    ContextMenuEnabledTreeWidgetItem* newProcItem = new ContextMenuEnabledTreeWidgetItem(proceduresItem, QStringList(proc->getName())) ;
    QVariant var(proc->getObjectUid());
    newProcItem->setData(0, Qt::UserRole, var);
    newProcItem->setIcon(0, IconFactory::getProcedureIcon());
    newProcItem->setPopupMenu(ContextMenuCollection::getInstance()->getProcedurePopupMenu());
    m_tree->addTopLevelItem(newProcItem);
    proc->setLocation(newProcItem);

    ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(proc->getName()));
    sqlItm->setIcon(0, IconFactory::getProcedureIcon());
    sqlItm->setData(0, Qt::UserRole, var);
    proc->setSqlItem(sqlItm);

    ContextMenuEnabledTreeWidgetItem* docItem = new ContextMenuEnabledTreeWidgetItem(getDocumentationItem(), QStringList(proc->getName()));
    docItem->setIcon(0, IconFactory::getProcedureIcon());
    docItem->setData(0, Qt::UserRole, var);
    m_tree->addTopLevelItem(docItem);

    proc->updateGui();

    return newProcItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createFunctionTreeEntry(Function* func)
{
    ContextMenuEnabledTreeWidgetItem* newFuncItem = new ContextMenuEnabledTreeWidgetItem(functionsItem, QStringList(func->getName())) ;
    QVariant var(func->getObjectUid());
    newFuncItem->setData(0, Qt::UserRole, var);
    newFuncItem->setIcon(0, IconFactory::getFunctionTreeIcon());
    newFuncItem->setPopupMenu(ContextMenuCollection::getInstance()->getFunctionPopupMenu());
    m_tree->addTopLevelItem(newFuncItem);
    func->setLocation(newFuncItem);

    ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(func->getName()));
    sqlItm->setIcon(0, IconFactory::getFunctionTreeIcon());
    sqlItm->setData(0, Qt::UserRole, var);
    func->setSqlItem(sqlItm);

    ContextMenuEnabledTreeWidgetItem* docItem = new ContextMenuEnabledTreeWidgetItem(getDocumentationItem(), QStringList(func->getName()));
    docItem->setIcon(0, IconFactory::getFunctionTreeIcon());
    docItem->setData(0, Qt::UserRole, var);
    m_tree->addTopLevelItem(docItem);

    func->updateGui();

    return newFuncItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createTriggerTreeEntry(Trigger* trg)
{
    ContextMenuEnabledTreeWidgetItem* newTriggerItem = new ContextMenuEnabledTreeWidgetItem(triggersItem, QStringList(trg->getName())) ;
    QVariant var(trg->getObjectUid());
    newTriggerItem->setData(0, Qt::UserRole, var);
    newTriggerItem->setIcon(0, IconFactory::getTriggerIcon());
    newTriggerItem->setPopupMenu(ContextMenuCollection::getInstance()->getTriggerPopupMenu());
    m_tree->addTopLevelItem(newTriggerItem);
    trg->setLocation(newTriggerItem);

    ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(trg->getName()));
    sqlItm->setIcon(0, IconFactory::getTriggerIcon());
    sqlItm->setData(0, Qt::UserRole, var);
    trg->setSqlItem(sqlItm);

    trg->updateGui();

    return newTriggerItem;
}


ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createTableTreeEntryForIssue(Table* tab)
{
    ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, QStringList(tab->getName())) ;
    QVariant var(tab->getName());
    newTblsItem->setData(0, Qt::UserRole, var);
    newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getIssuesOfATablePopupMenuPopupMenu());
    // set the icon, add to the tree
    newTblsItem->setIcon(0, IconFactory::getTablesIcon());
    m_issuesTree->addTopLevelItem(newTblsItem);
    return newTblsItem;
}


ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createTableInstanceTreeEntry(TableInstance* tinst)
{
    ContextMenuEnabledTreeWidgetItem* itm = new ContextMenuEnabledTreeWidgetItem(getTableInstancesItem(), QStringList(tinst->getName()));
    tinst->setLocation(itm);
    itm->setPopupMenu(ContextMenuCollection::getInstance()->getTableInstancePopupMenu());
    itm->setIcon(0, IconFactory::getTabinstIcon());
    QVariant a(tinst->getObjectUid());
    itm->setData(0, Qt::UserRole, a);

    ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(tinst->getName()));
    sqlItm->setIcon(0, IconFactory::getTabinstIcon());
    sqlItm->setData(0, Qt::UserRole, a);
    tinst->setSqlItem(sqlItm);

    return itm;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createIssueTreeEntry(Issue* issue, ContextMenuEnabledTreeWidgetItem* p)
{
    QStringList a;
    a << "" <<issue->typeAsString() << issue->getOriginator()->getFullLocation() << issue->getDescription();
    ContextMenuEnabledTreeWidgetItem* itm = new ContextMenuEnabledTreeWidgetItem(p, a);
    issue->setLocation(itm);
    itm->setPopupMenu(ContextMenuCollection::getInstance()->getIssuePopupMenu());
    if(issue->getType() == Issue::WARNING)
    {
        itm->setIcon(1, IconFactory::getWarningIcon());
    }
    if(issue->getType() == Issue::RECOMMENDATION)
    {
        itm->setIcon(1, IconFactory::getRecommendIcon());
    }
    if(issue->getType() == Issue::CRITICAL) // used only for the connection stuff
    {
        itm->setIcon(1, IconFactory::getUnConnectedDatabaseIcon());
    }
    QTime now = QTime::currentTime();
    QDate nowd = QDate::currentDate();
    QString t = nowd.toString() + " - " + now.toString();
    itm->setText(0, t);
    m_issuesTree->insertTopLevelItem(m_issuesTree->topLevelItemCount(), itm);
    QVariant b(issue->getName());
    itm->setData(0, Qt::UserRole, b);

    return itm;
}

void VersionGuiElements::cleanupOrphanedIssueTableItems()
{
    QVector<int> toRemove;

    for(int i=0; i<m_issuesTree->topLevelItemCount(); i++)
    {
        if(m_issuesTree->topLevelItem(i)->childCount() == 0)
        {
            toRemove.push_front(i);  // pushing to the front!
        }
    }

    for(int i=0; i<toRemove.size(); i++)
    {
        m_issuesTree->takeTopLevelItem(toRemove.at(i));
    }

    if(m_issuesTree->topLevelItemCount() == 0)
    {
        m_issuesTree->parentWidget()->hide();
    }
}

void VersionGuiElements::updateForms()
{
    //see if we have already created a new table form. Must not be central
    if(dynamic_cast<NewTableForm*>(MainWindow::instance()->centralWidget()))
    {
        // yes, reset the foreign tables combo
        dynamic_cast<NewTableForm*>(MainWindow::instance()->centralWidget())->resetForeignTablesCombo();
    }

    getSqlForm();
    m_sqlForm->setSqlSource(0);
    m_sqlForm->presentSql(Workspace::getInstance()->currentProject(), m_version);
    if(dynamic_cast<SqlForm*>(MainWindow::instance()->centralWidget()))
    {
        MainWindow::instance()->setCentralWidget(m_sqlForm);
    }
}

SqlForm* VersionGuiElements::getSqlForm()
{
    return m_sqlForm = new SqlForm(m_version, Workspace::getInstance()->currentProjectsEngine(), MainWindow::instance());
}

NewTableForm* VersionGuiElements::getTableFormForNewTable()
{
    return m_newTableForm = new NewTableForm(Workspace::getInstance()->currentProjectsEngine(),
                                             Workspace::getInstance()->currentProject(), m_version, MainWindow::instance(), true);
}

NewTableForm* VersionGuiElements::getTableFormForExistingTable()
{
    return m_existingTableForm = new NewTableForm(Workspace::getInstance()->currentProjectsEngine(),
                                                  Workspace::getInstance()->currentProject(), m_version, MainWindow::instance(), false);
}

ProcedureForm* VersionGuiElements::getProcedureForm(ProcedureFormMode m)
{
    return m_procedureForm = new ProcedureForm(m_version, m, false, 0, MainWindow::instance());
}

TriggerForm* VersionGuiElements::getTriggerForm()
{
    return m_triggerForm = new TriggerForm(m_version, false, false, MainWindow::instance());
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getTablesItem() const
{
    return tablesItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getTableInstancesItem() const
{
    return tableInstancesItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getFinalSqlItem() const
{
    return finalSqlItem;
}


ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getVersionItem() const
{
    return versionItem;
}


ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getDiagramsItem() const
{
    return diagramsItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getViewsItem() const
{
    return viewsItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getProceduresItem() const
{
    return proceduresItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getFunctionsItem() const
{
    return functionsItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getTriggersItem() const
{
    return triggersItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getDocumentationItem() const
{
    return documentationItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getDtsItem() const
{
    return dtsItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getIntsDtsItem()
{
    if(intsDtItem == 0)
    {
        intsDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem,
                                     QStringList(QObject::tr("Numeric"))) ;
        intsDtItem ->setIcon(0, IconFactory::getIntDataTypesIcon());
        m_tree->addTopLevelItem(intsDtItem);
    }
    return intsDtItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getStringDtsItem()
{
    if(stringsDtItem == 0)
    {
        stringsDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem,
                                      QStringList(QObject::tr("String"))) ;
        stringsDtItem ->setIcon(0, IconFactory::getStringDataTypesIcon());
        m_tree->addTopLevelItem(stringsDtItem);
    }
    return stringsDtItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getDateDtsItem()
{
    if(dateDtItem == 0)
    {
        dateDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem,
                                   QStringList(QObject::tr("Date/Time"))) ;
        dateDtItem ->setIcon(0, IconFactory::getDateTimeDataTypesIcon());
        m_tree->addTopLevelItem(dateDtItem);
    }

    return dateDtItem;
}
ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getBlobDtsItem()
{
    if(blobDtItem == 0)
    {
        blobDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem,
                                 QStringList(QObject::tr("Blob/Binary"))) ;
        blobDtItem ->setIcon(0, IconFactory::getBlobDataTypesIcon());
        m_tree->addTopLevelItem(blobDtItem);
    }
    return blobDtItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getBoolDtsItem()
{
    if(boolDtItem == 0)
    {
        boolDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem, QStringList(QObject::tr("Boolean"))) ;
        boolDtItem ->setIcon(0, IconFactory::getBoolDataTypesIcon());
        m_tree->addTopLevelItem(boolDtItem);
    }
    return boolDtItem;
}

ContextMenuEnabledTreeWidgetItem*VersionGuiElements:: getMiscDtsItem()
{
    if(miscDtItem == 0)
    {
        miscDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem, QStringList(QObject::tr("Misc"))) ;
        miscDtItem ->setIcon(0, IconFactory::getMiscDataTypesIcon());
        m_tree->addTopLevelItem(boolDtItem);
    }
    return miscDtItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::getSpatialDtsItem()
{
    if(spatialDtItem == 0)
    {
        spatialDtItem = new ContextMenuEnabledTreeWidgetItem(dtsItem, QStringList(QObject::tr("Spatial"))) ;
        spatialDtItem ->setIcon(0, IconFactory::getSpatialDataTypesIcon());
        m_tree->addTopLevelItem(spatialDtItem);
    }
    return spatialDtItem;
}
