#include "ClipboardFactory.h"
#include "DeserializationFactory.h"
#include "Workspace.h"
#include "core_Table.h"
#include "core_ForeignKey.h"
#include "core_Column.h"
#include "core_Patch.h"
#include "Version.h"
#include "MainWindow.h"

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QApplication>
#include <QClipboard>

ClipboardFactory::ClipboardFactory()
{
}

Table* ClipboardFactory::pasteTable(Version* v)
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

    QDomElement e = a.documentElement().firstChild().toElement();
    e.setAttribute("uid", QUuid::createUuid().toString());
    Table* tab = DeserializationFactory::createTable(Workspace::getInstance()->currentProjectsEngine(), v, a, e);

    v->setupForeignKeyRelationshipsForATable(tab);

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
