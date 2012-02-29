#ifndef UIDWAREHOUSE_H
#define UIDWAREHOUSE_H

#include "Solution.h"
#include "core_ObjectWithUid.h"

#include <QMap>


/**
 * Class holding all the object in the system referenced by an Uid
 */
class UidWarehouse
{

    struct WarehouseEntry
    {
        QUuid objectId;
        ObjectWithUid* obj;
    };

public:

    void addElement(ObjectWithUid* o);
    ObjectWithUid* getElement(const QString&);

public:

    static UidWarehouse& instance();

private:

    UidWarehouse() : m_items() {}

    static void create();
    static UidWarehouse* m_instance;

private:

    QMap<Solution*, QMap<QString, WarehouseEntry*> > m_items;
};

#endif // UIDWAREHOUSE_H
