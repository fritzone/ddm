#include "UidWarehouse.h"
#include "Workspace.h"
#include "Project.h"
#include "uids.h"
#include "core_VersionElement.h"

UidWarehouse* UidWarehouse::m_instance = 0;

UidWarehouse& UidWarehouse::instance()
{
    if(!m_instance)
    {
        create();
    }

    return *m_instance;
}

void UidWarehouse::create()
{
    static UidWarehouse tinstance;
    m_instance = &tinstance;
}

// TODO: right now the Solution is just a fake
void UidWarehouse::addElement(ObjectWithUid *o, Version *v)
{
    WarehouseEntry* whe = new WarehouseEntry();
    whe->obj = o;
    whe->objectId = o->getObjectUid();
    whe->ver = v;

    Solution* s = 0;
    if(m_items.keys().contains(s))
    {
        m_items[s].insert(o->getObjectUid().toString(), whe);
    }
    else
    {
        QMap<QString, WarehouseEntry*> * e = new QMap<QString, WarehouseEntry*>();
        e->insert(o->getObjectUid().toString(), whe);
        m_items.insert(s, *e);
    }
}

void UidWarehouse::replace(const QString &uid, ObjectWithUid *o)
{
    WarehouseEntry* whe = new WarehouseEntry();
    whe->obj = o;
    whe->objectId = uid;
    whe->ver = dynamic_cast<VersionElement*>(o)->version();


    Solution* s = 0;
    if(m_items.keys().contains(s))
    {
        if(m_items[s].contains(uid))
        {
            m_items[s][uid] = whe;

//            qDebug() << "1 replace" << uid << " obj=" << o;
        }
    }
    else
    {
//        qDebug() << "1 replace" << uid << " obj=" << o;
        QMap<QString, WarehouseEntry*> * e = new QMap<QString, WarehouseEntry*>();
        e->insert(uid, whe);
        m_items.insert(s, *e);
    }
}

void UidWarehouse::addElement(QUuid uid, Version *v)
{
    WarehouseEntry* whe = new WarehouseEntry();
    whe->obj = 0;
    whe->objectId = uid;
    whe->ver = v;

    Solution* s = 0;
    if(m_items.keys().contains(s))
    {
        m_items[s].insert(uid.toString(), whe);
    }
    else
    {
        QMap<QString, WarehouseEntry*> * e = new QMap<QString, WarehouseEntry*>();
        e->insert(uid.toString(), whe);
        m_items.insert(s, *e);
    }
}

// maps a class uid to an entity .. or at least it should
void UidWarehouse::addElement(QUuid uid, ObjectWithUid *obj)
{
    WarehouseEntry* whe = new WarehouseEntry();
    whe->obj = obj;
    whe->objectId = uid;
    whe->ver = 0;

    Solution* s = 0;
    if(m_items.keys().contains(s))
    {
        m_items[s].insert(uid.toString(), whe);
    }
    else
    {
        QMap<QString, WarehouseEntry*> * e = new QMap<QString, WarehouseEntry*>();
        e->insert(uid.toString(), whe);
        m_items.insert(s, *e);
    }
}

void UidWarehouse::setForcedVersionForUid(const QString &uid, Version* v)
{
    Solution* s = 0;

    if(! m_items.keys().contains(s))
    {
        return;
    }
    else
    {
        const QMap<QString, WarehouseEntry*>& m = m_items[s];
        if(m.contains(uid))
        {
            WarehouseEntry* whe = m[uid];
            if(whe)
            {
                whe->ver = v;
            }
        }
    }
    return;
}

Version* UidWarehouse::getVersionForUid(const QString &uid)
{
    Solution* s = 0;

    if(! m_items.keys().contains(s))
    {
        return 0;
    }
    else
    {
        const QMap<QString, WarehouseEntry*>& m = m_items[s];
        if(m.contains(uid))
        {
            WarehouseEntry* whe = m[uid];
            if(whe)
            {
                return whe->ver;
            }
        }
    }
    return 0;
}

ObjectWithUid* UidWarehouse::getElement(const QString &uid)
{
    Solution* s = 0;

    if(! m_items.keys().contains(s))
    {
        return 0;
    }
    else
    {
        const QMap<QString, WarehouseEntry*>& m = m_items[s];
        if(m.contains(uid))
        {
            WarehouseEntry* whe = m[uid];
            if(whe)
            {
                return whe->obj;
            }
        }
    }
    return 0;
}

bool UidWarehouse::related(const ObjectWithUid* obj1, const ObjectWithUid* obj2)
{
    // now see if they have any common source uid ...
    QStringList obj1Ancestry = obj1->sourceUids();
    QStringList obj2Ancestry = obj2->sourceUids();

    for(int i=0; i<obj1Ancestry.size(); i++)
    {
        if(obj2Ancestry.indexOf(obj1Ancestry[i]) != -1)
        {
            return true;
        }
    }

    for(int i=0; i<obj2Ancestry.size(); i++)
    {
        if(obj1Ancestry.indexOf(obj2Ancestry[i]) != -1)
        {
            return true;
        }
    }

    return false;
}
