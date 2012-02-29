#include "UidWarehouse.h"
#include "Workspace.h"

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
    Solution* s = 0;
    qDebug() << "adding: " <<o->getObjectUid().toString();
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
        qDebug() << m << " looking for " << uid ;
        if(m.contains(uid))
        {
            WarehouseEntry* whe = m[uid];
            if(whe)
            {
                return whe->obj;
            }
        }
    }
    qDebug() << "something's wrong";
    return 0;
}
