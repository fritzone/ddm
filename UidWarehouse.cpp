#include "UidWarehouse.h"

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

    if(m_items.keys().contains(s))
    {
        m_items[s].append(whe);
    }
    else
    {
        QVector<WarehouseEntry*> * e = new QVector<WarehouseEntry*>();
        e->append(whe);
        m_items.insert(s, *e);
    }
}
