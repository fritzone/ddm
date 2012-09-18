#include "UidWarehouse.h"
#include "Workspace.h"
#include "Project.h"

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
