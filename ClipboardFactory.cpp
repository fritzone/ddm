#include "ClipboardFactory.h"
#include "DeserializationFactory.h"
#include "Workspace.h"
#include "Table.h"
#include "ForeignKey.h"
#include "Column.h"
#include "Version.h"

#include <QDomDocument>
#include <QDomElement>
#include <QApplication>
#include <QClipboard>

ClipboardFactory::ClipboardFactory()
{
}

Table* ClipboardFactory::pasteTable()
{
    QDomDocument a("ClipboardData");
    QString err;
    if(!a.setContent(QApplication::clipboard()->text(), &err))
    {
        return 0;
    }
    QString node = a.documentElement().nodeName();
    if(node != "CopiedTable")
        return 0;

    Table* tab = DeserializationFactory::createTable(Workspace::getInstance()->currentProjectsEngine(), Workspace::getInstance()->workingVersion(), a,
                                                     a.documentElement().firstChild().toElement());

    // TODO: this was pasted from the deserialization factory... except that the version was changed
    for(int j=0; j<tab->getFks().size(); j++)
    {
        ForeignKey* fkJ = tab->getFks()[j];
        for(int k=0; k<fkJ->getAssociations().size(); k++)
        {
            ForeignKey::ColumnAssociation* assK = fkJ->getAssociation(k);
            // first: set the tables
            for(int l=0; l<Workspace::getInstance()->workingVersion()->getTables().size(); l++)
            {
                if(Workspace::getInstance()->workingVersion()->getTables().at(l)->getName() == assK->getSForeignTable())
                {
                    assK->setForeignTable(Workspace::getInstance()->workingVersion()->getTables().at(l));
                }
                if(Workspace::getInstance()->workingVersion()->getTables().at(l)->getName() == assK->getSLocalTable())
                {
                    assK->setLocalTable(Workspace::getInstance()->workingVersion()->getTables().at(l));
                }
            }
            // then: set the columns of those tables
            for(int l=0; l<assK->getLocalTable()->columns().size(); l++)
            {
                Column* colL = assK->getLocalTable()->getColumns().at(l);
                if(colL->getName() == assK->getSLocalColumn())
                {
                    assK->setLocalColumn(colL);
                }
            }
            for(int l=0; l<assK->getForeignTable()->columns().size(); l++)
            {
                Column* colL = assK->getForeignTable()->getColumns().at(l);
                if(colL->getName() == assK->getSForeignColumn())
                {
                    assK->setForeignColumn(colL);
                }
            }
        }
    }


    return tab;
}

bool ClipboardFactory::tableIsAvailableOnClipboard()
{
    QDomDocument a("ClipboardData");
    QString err;
    if(!a.setContent(QApplication::clipboard()->text(), &err))
    {
        return false;
    }
    QString node = a.documentElement().nodeName();
    if(node != "CopiedTable")
        return false;

    return true;
}

bool ClipboardFactory::columnIsAvailableOnClipboard()
{
    QDomDocument a("ClipboardData");
    QString err;
    if(!a.setContent(QApplication::clipboard()->text(), &err))
    {
        return false;
    }
    QString node = a.documentElement().nodeName();
    if(node != "CopiedColumn")
        return false;

    return true;
}

Column* ClipboardFactory::pasteColumn()
{
    QDomDocument a("ClipboardData");
    QString err;
    if(!a.setContent(QApplication::clipboard()->text(), &err))
    {
        return 0;
    }
    QString node = a.documentElement().nodeName();
    if(node != "CopiedColumn")
        return 0;

    Column* col = DeserializationFactory::createColumn(Workspace::getInstance()->workingVersion(), a, a.documentElement().firstChild().toElement());

    return col;
}
