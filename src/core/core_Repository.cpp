#include "core_Repository.h"
#include "db_DatabaseEngine.h"

#include <QApplication>
#include <QDomDocument>
#include <QFile>
#include <QDomElement>

#include <QDebug>

Repository::Repository()
{
    QDomDocument doc ("xml");
    QFile file (QApplication::applicationDirPath() + "/rsrc/repository.xml");
    if (file.open(QIODevice::ReadOnly))
    {
        if (!doc.setContent(&file))
        {
            file.close();
            return;
        }
        file.close();
    }

    QDomElement docElem = doc.documentElement();
    for(int i=0; i<docElem.childNodes().size(); i++)
    {
        QDomElement childI = docElem.childNodes().at(i).toElement();
        QString name = childI.nodeName();
        if(name == "databases")
        {
            for(int j=0; j<childI.childNodes().size(); j++)
            {
                addDatabase(childI.childNodes().at(j).toElement());
            }
        }
    }

}


void Repository::addDatabase(const QDomElement & el)
{
    if(el.nodeName() == "database")
    {
        DatabaseEngine* dbe = DatabaseEngine::provideEngineFor(el.attribute("name"));
        if(dbe)
        {
            m_databases.append(dbe);
        }
    }
}
