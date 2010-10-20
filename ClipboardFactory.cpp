#include "ClipboardFactory.h"
#include "DeserializationFactory.h"
#include "Workspace.h"

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

    if(a.documentElement().firstChild().nodeName() != "CopiedTable")
        return 0;

    Table* tab = DeserializationFactory::createTable(Workspace::getInstance()->currentProjectsEngine(), Workspace::getInstance()->workingVersion(), a,
                                                     a.documentElement().firstChild().toElement());

    return tab;
}
