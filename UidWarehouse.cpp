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
void UidWarehouse::addElement(ObjectWithUid *o)
{
    WarehouseEntry* whe = new WarehouseEntry();
    whe->obj = o;
    whe->objectId = o->getObjectUid();
    Solution* ss = Workspace::getInstance()->currentSolution();
    if(ss)
    {
        Project* p = ss->currentProject();
        if(p)
        {
            whe->ver = p->getWorkingVersion();
        }
        else
        {
            whe->ver = 0;
        }
    }
    else
    {
        whe->ver = 0;
    }

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
