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

    Workspace::getInstance()->workingVersion()->setupForeignKeyRelationshipsForATable(tab);

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

    Column* col = DeserializationFactory::createColumn(Workspace::getInstance()->currentProjectsEngine(), Workspace::getInstance()->workingVersion(), a, a.documentElement().firstChild().toElement());

    return col;
}
