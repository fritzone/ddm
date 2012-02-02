#include "VersionGuiElements.h"
#include "ContextMenuEnabledTreeWidget.h"
#include "IconFactory.h"
#include "Version.h"
#include "UserDataType.h"
#include "ContextMenuCollection.h"
#include "Table.h"
#include "Diagram.h"
#include "TableInstance.h"
#include "Workspace.h"
#include "TableInstance.h"
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

#include <QVector>
#include <QtGui>

// TODO: This file looks funny. See why it seems we have a lot of duplicate code

VersionGuiElements::VersionGuiElements(QTreeWidget* projTree, QTreeWidget* dtTree, QTreeWidget* issueTree, Version* v) :
    tablesItem(0), tableInstancesItem(0),
    versionItem(0), diagramsItem(0), proceduresItem(0), finalSqlItem(0), dtsItem(0), viewsItem(0), triggersItem(0),
    m_tree(projTree), m_dtTree(dtTree), m_issuesTree(issueTree),
    stringsDtItem(0), intsDtItem(0), dateDtItem(0), blobDtItem(0),
    boolDtItem(0), miscDtItem(0), spatialDtItem(0), m_version(v),
    m_newTableForm(0), m_existingTableForm(0), m_procedureForm(0)
{
}

void VersionGuiElements::createGuiElements(ContextMenuEnabledTreeWidgetItem* projectItem)
{

    versionItem = new ContextMenuEnabledTreeWidgetItem(projectItem, QStringList(QString("Ver: ") + m_version->getVersionText())) ;
    versionItem->setIcon(0, IconFactory::getVersionIcon());
    m_tree->addTopLevelItem(versionItem);

    // make the tables sub item coming from the version
    tablesItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Table templates"))) ;
    tablesItem->setIcon(0, IconFactory::getTablesIcon());
    tablesItem->setPopupMenu(ContextMenuCollection::getInstance()->getTablesPopupMenu());
    m_tree->addTopLevelItem(tablesItem);

    // make the views sub item coming from the version
    tableInstancesItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Tables"))) ;
    tableInstancesItem->setIcon(0, IconFactory::getTabinstIcon());
    tableInstancesItem->setPopupMenu(ContextMenuCollection::getInstance()->getTableInstancesPopupMenu());
    m_tree->addTopLevelItem(tableInstancesItem);

    diagramsItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Diagrams"))) ;
    diagramsItem->setIcon(0, IconFactory::getDiagramIcon());
    diagramsItem->setPopupMenu(ContextMenuCollection::getInstance()->getDiagramsPopupMenu());
    m_tree->addTopLevelItem(diagramsItem);


    proceduresItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Procedures"))) ;
    proceduresItem->setIcon(0, IconFactory::getProcedureIcon());
    //proceduresItem->setPopupMenu(ContextMenuCollection::getInstance()->getDiagramsPopupMenu());
    m_tree->addTopLevelItem(proceduresItem);

    viewsItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Views"))) ;
    viewsItem->setIcon(0, IconFactory::getViewsIcon());
    //viewsItem->setPopupMenu(ContextMenuCollection::getInstance()->getDiagramsPopupMenu());
    m_tree->addTopLevelItem(viewsItem);

    triggersItem = new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Triggers"))) ;
    triggersItem->setIcon(0, IconFactory::getTriggersIcon());
    //viewsItem->setPopupMenu(ContextMenuCollection::getInstance()->getDiagramsPopupMenu());
    m_tree->addTopLevelItem(triggersItem);

    // last one: SQLs
    ContextMenuEnabledTreeWidgetItem* codeItem= new ContextMenuEnabledTreeWidgetItem(versionItem, QStringList(QObject::tr("Code"))) ;
    codeItem->setIcon(0, IconFactory::getCodeIcon());
    m_tree->addTopLevelItem(codeItem);

    finalSqlItem= new ContextMenuEnabledTreeWidgetItem(codeItem, QStringList(QObject::tr("SQL"))) ;
    finalSqlItem->setIcon(0, IconFactory::getSqlIcon());
    m_tree->addTopLevelItem(finalSqlItem);

    // make the dts sub item coming from the project
    dtsItem = new ContextMenuEnabledTreeWidgetItem((ContextMenuEnabledTreeWidgetItem*)0, QStringList(QObject::tr("Data Types"))) ;
    dtsItem->setIcon(0, IconFactory::getDataTypesIcon());
    dtsItem->setPopupMenu(ContextMenuCollection::getInstance()->getDatatypesPopupMenu());
    m_dtTree->addTopLevelItem(dtsItem);

}

void VersionGuiElements::populateTreeItems()
{
    // insert the user data types in the tree
    const QVector<UserDataType*>& dataTypes = m_version->getDataTypes();
    for(int i=0; i<dataTypes.size(); i++)
    {
        QStringList a(dataTypes[i]->getName());
        a << dataTypes[i]->sqlAsString();

        ContextMenuEnabledTreeWidgetItem* parent = getDtsItem();
        if(dataTypes[i]->getType() == DataType::DT_STRING) parent = getStringDtsItem();
        if(dataTypes[i]->getType() == DataType::DT_NUMERIC) parent = getIntsDtsItem();
        if(dataTypes[i]->getType() == DataType::DT_DATETIME) parent = getDateDtsItem();
        if(dataTypes[i]->getType() == DataType::DT_BLOB) parent = getBlobDtsItem();
        if(dataTypes[i]->getType() == DataType::DT_BOOLEAN) parent = getBoolDtsItem();
        if(dataTypes[i]->getType() == DataType::DT_MISC) parent = getMiscDtsItem();
        if(dataTypes[i]->getType() == DataType::DT_SPATIAL) parent = getSpatialDtsItem();

        ContextMenuEnabledTreeWidgetItem* newDTItem = new ContextMenuEnabledTreeWidgetItem(parent, a) ;
        QVariant var;
        var.setValue(*dataTypes[i]);
        newDTItem->setData(0, Qt::UserRole, var);
        newDTItem->setPopupMenu(ContextMenuCollection::getInstance()->getDatatypePopupMenu());
        // set the icon, add to the tree
        newDTItem->setIcon(0, dataTypes[i]->getIcon());
        m_dtTree->insertTopLevelItem(0, newDTItem);
        dataTypes[i]->setLocation(newDTItem);
    }
    m_dtTree->expandAll();
    m_dtTree->resizeColumnToContents(0);
    m_dtTree->resizeColumnToContents(1);

    // insert the tables
    const QVector<Table*>& tables = m_version->getTables();
    for(int i=0; i<tables.size(); i++)
    {
        Table* tbl = tables.at(i);
        ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(getTablesItem(), QStringList(tbl->getName())) ;

        QVariant var(tbl->getName());
        newTblsItem->setData(0, Qt::UserRole, var);
        newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getTablePopupMenu());
        // set the icon, add to the tree
        newTblsItem->setIcon(0, IconFactory::getTablesIcon());
        m_tree->insertTopLevelItem(0, newTblsItem);

        // set the link to the tree
        tbl->setLocation(newTblsItem);
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

        QVariant var(dI->getName());
        newDgramItem->setData(0, Qt::UserRole, var);
        // set the icon, add to the tree
        newDgramItem->setIcon(0, IconFactory::getDiagramIcon());
        newDgramItem->setPopupMenu(ContextMenuCollection::getInstance()->getDiagramPopupMenu());
        m_tree->insertTopLevelItem(0, newDgramItem);

        // set the link to the tree
        dI->setLocation(newDgramItem);
    }

    // insert the table instances (but only if this is an OOP project...)
    const QVector<TableInstance*> &  tableInstances = m_version->getTableInstances();
    for(int i=0; i<tableInstances.size(); i++)
    {
        TableInstance* tI = tableInstances.at(i);
        ContextMenuEnabledTreeWidgetItem* newTabInstItem = new ContextMenuEnabledTreeWidgetItem(getTableInstancesItem(), QStringList(tI->getName())) ;

        QVariant var(tI->getName());
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
    }

    // now populate the "Code" tree items by adding the SQLs of the tables or table instances
    if(Workspace::getInstance()->currentProjectIsOop())
    {   // insert each table instance entry, since this is an OOP project
        for(int i=0; i<tableInstances.size(); i++)
        {
            TableInstance* tI = tableInstances[i];
            ContextMenuEnabledTreeWidgetItem* tabInstSqlItem = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(tI->getName())) ;

            QVariant var(tI->getName());
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

            QVariant var(tbl->getName());
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

    if(udt->getType() == DataType::DT_STRING) parent = getStringDtsItem();
    if(udt->getType() == DataType::DT_NUMERIC) parent = getIntsDtsItem();
    if(udt->getType() == DataType::DT_DATETIME) parent = getDateDtsItem();
    if(udt->getType() == DataType::DT_BLOB) parent = getBlobDtsItem();
    if(udt->getType() == DataType::DT_BOOLEAN) parent = getBoolDtsItem();
    if(udt->getType() == DataType::DT_MISC) parent = getMiscDtsItem();
    if(udt->getType() == DataType::DT_SPATIAL) parent = getSpatialDtsItem();

    ContextMenuEnabledTreeWidgetItem* newDTItem = new ContextMenuEnabledTreeWidgetItem(parent, itm) ;

    QVariant var;
    var.setValue(*udt);
    newDTItem->setData(0, Qt::UserRole, var);
    // set the icon, add to the tree
    newDTItem->setIcon(0, udt->getIcon());
    newDTItem->setPopupMenu(ContextMenuCollection::getInstance()->getDatatypePopupMenu());
    m_dtTree->insertTopLevelItem(0,newDTItem);
    m_dtTree->header()->setResizeMode(QHeaderView::ResizeToContents);

    // set the link to the tree
    udt->setLocation(newDTItem);

    return newDTItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createDiagramTreeEntry(Diagram* dgram)
{
    ContextMenuEnabledTreeWidgetItem* newDgramItem = new ContextMenuEnabledTreeWidgetItem(getDiagramsItem(), QStringList(dgram->getName())) ;
    QVariant var(dgram->getName());
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
    ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(getTablesItem(), QStringList(tab->getName())) ;
    QVariant var(tab->getName());
    newTblsItem->setData(0, Qt::UserRole, var);
    newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getTablePopupMenu());
    // set the icon, add to the tree
    newTblsItem->setIcon(0, IconFactory::getTablesIcon());
    m_tree->addTopLevelItem(newTblsItem);
    // set the link to the tree
    tab->setLocation(newTblsItem);
    return newTblsItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createTableTreeEntry(Table* tab, ContextMenuEnabledTreeWidgetItem* p)
{
    ContextMenuEnabledTreeWidgetItem* newTblsItem = new ContextMenuEnabledTreeWidgetItem(p, QStringList(tab->getName())) ;
    QVariant var(tab->getName());
    newTblsItem->setData(0, Qt::UserRole, var);
    newTblsItem->setPopupMenu(ContextMenuCollection::getInstance()->getTablePopupMenu());
    // set the icon, add to the tree
    newTblsItem->setIcon(0, IconFactory::getTablesIcon());
    m_tree->addTopLevelItem(newTblsItem);
    // set the link to the tree
    tab->setLocation(newTblsItem);
    return newTblsItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createViewTreeEntry(View* view)
{
    ContextMenuEnabledTreeWidgetItem* newViewItem = new ContextMenuEnabledTreeWidgetItem(viewsItem, QStringList(view->getName())) ;
    QVariant var(view->getName());
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
    QVariant var(proc->getName());
    newProcItem->setData(0, Qt::UserRole, var);
    newProcItem->setIcon(0, IconFactory::getProcedureIcon());
    newProcItem->setPopupMenu(ContextMenuCollection::getInstance()->getProcedurePopupMenu());
    m_tree->addTopLevelItem(newProcItem);
    proc->setLocation(newProcItem);

    ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(proc->getName()));
    sqlItm->setIcon(0, IconFactory::getProcedureIcon());
    sqlItm->setData(0, Qt::UserRole, var);
    proc->setSqlItem(sqlItm);

    return newProcItem;
}

ContextMenuEnabledTreeWidgetItem* VersionGuiElements::createTriggerTreeEntry(Trigger* trg)
{
    ContextMenuEnabledTreeWidgetItem* newTriggerItem = new ContextMenuEnabledTreeWidgetItem(triggersItem, QStringList(trg->getName())) ;
    QVariant var(trg->getName());
    newTriggerItem->setData(0, Qt::UserRole, var);
    newTriggerItem->setIcon(0, IconFactory::getTriggerIcon());
    //newTriggerItem->setPopupMenu(ContextMenuCollection::getInstance()->getTriggerPopupMenu());
    m_tree->addTopLevelItem(newTriggerItem);
    trg->setLocation(newTriggerItem);

    ContextMenuEnabledTreeWidgetItem* sqlItm = new ContextMenuEnabledTreeWidgetItem(getFinalSqlItem(), QStringList(trg->getName()));
    sqlItm->setIcon(0, IconFactory::getTriggerIcon());
    sqlItm->setData(0, Qt::UserRole, var);
    trg->setSqlItem(sqlItm);

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
    QVariant a(tinst->getName());
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
    m_sqlForm->presentSql(Workspace::getInstance()->currentProject(), Workspace::getInstance()->currentProject()->getCodepage());
    if(dynamic_cast<SqlForm*>(MainWindow::instance()->centralWidget()))
    {
        MainWindow::instance()->setCentralWidget(m_sqlForm);
    }

}

SqlForm* VersionGuiElements::getSqlForm()
{
    return m_sqlForm = new SqlForm(Workspace::getInstance()->currentProjectsEngine(), MainWindow::instance());
}

NewTableForm* VersionGuiElements::getTableFormForNewTable()
{
    return m_newTableForm = new NewTableForm(Workspace::getInstance()->currentProjectsEngine(), Workspace::getInstance()->currentProject(), MainWindow::instance(), true);
}

NewTableForm* VersionGuiElements::getTableFormForExistingTable()
{
    return m_existingTableForm = new NewTableForm(Workspace::getInstance()->currentProjectsEngine(), Workspace::getInstance()->currentProject(), MainWindow::instance(), false);
}

ProcedureForm* VersionGuiElements::getProcedureForm()
{
    return m_procedureForm = new ProcedureForm(MainWindow::instance());
}

TriggerForm* VersionGuiElements::getTriggerForm()
{
    return m_triggerForm = new TriggerForm(MainWindow::instance());
}
