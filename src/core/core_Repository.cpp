#include "core_Repository.h"
#include "db_DatabaseEngine.h"
#include "core_Role.h"
#include "core_Entity.h"

#include <QApplication>
#include <QDomDocument>
#include <QFile>
#include <QDomElement>

#include <QDebug>

Repository::Repository()
{
    QDomDocument doc ("xml");
    QString filename = QApplication::applicationDirPath() + "/rsrc/repository.xml";
    qDebug()  << filename;
    QFile file (filename);
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

        if(name == "roles")
        {
            for(int j=0; j<childI.childNodes().size(); j++)
            {
                QDomElement el = childI.childNodes().at(j).toElement();
                if(el.nodeName() == "role")
                {
                    Role* r = new Role(el.attribute("name"), el.attribute("class-uid"), el.attribute("description"));
                    m_roles.append(r);
                }
            }
        }
    }

    // second run to be sure that the roles have been fetched
    for(int i=0; i<docElem.childNodes().size(); i++)
    {
        QDomElement childI = docElem.childNodes().at(i).toElement();
        QString name = childI.nodeName();
        if(name == "entities-serialization")
        {
            for(int j=0; j<childI.childNodes().size(); j++)
            {
                QDomElement el = childI.childNodes().at(j).toElement();
                if(el.nodeName() == "entity")
                {
                    addEntity(el);
                }
            }
        }
    }
}

Repository::~Repository()
{
    // TODO: Save the repo
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

void Repository::addEntity(const QDomElement &el)
{
    QString name = el.attribute("name");
    QUuid class_uid(el.attribute("class-uid"));
    QUuid target_class_uid(el.attribute("target-class-uid"));
    bool hasDesc = el.hasAttribute("hasDescription") && el.attribute("hasDescription").toInt() == 1;
    bool supportSps = el.hasAttribute("supportsSps") && el.attribute("supportsSps").toInt() == 1;

    qDebug() << "CUID:" << class_uid;
    Entity* ent = new Entity(name, class_uid, target_class_uid, hasDesc, supportSps);
    m_entities.append(ent);

    // and the attributes of the entity
    for(int i=0; i<el.childNodes().size(); i++)
    {
        QDomElement chI = el.childNodes().at(i).toElement();
        if(chI.nodeName() == "attribute")
        {
            Entity::Attribute *a = new Entity::Attribute;
            a->name = chI.attribute("name");
            a->roleUid = chI.attribute("role-class-uid");
            ent->addAttribute(a);
        }
        if(chI.nodeName() == "collection")
        {
            Entity::Collection*a = new Entity::Collection;
            a->name = chI.attribute("name");
            a->roleUid = chI.attribute("role-class-uid");
            a->targetUid = chI.attribute("target-class-uid");
            ent->addCollection(a);
        }
    }
}

Repository* Repository::instance()
{
    static Repository repo;
    return &repo;
}
