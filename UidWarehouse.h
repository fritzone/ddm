#ifndef UIDWAREHOUSE_H
#define UIDWAREHOUSE_H

#include "Solution.h"
#include "core_ObjectWithUid.h"

#include <QMap>

class Version;

/**
 * Class holding all the object in the system referenced by an Uid
 */
class UidWarehouse
{

    struct WarehouseEntry
    {
        QUuid objectId;
        ObjectWithUid* obj;
        Version* ver;
    };

public:

    void addElement(ObjectWithUid* o, Version *v);
    void replace(const QString& uid, ObjectWithUid* o);
    void addElement(QUuid uid, Version* v);
    ObjectWithUid* getElement(const QString&);
    Version* getVersionForUid(const QString& uid);
    bool related(const ObjectWithUid* obj1, const ObjectWithUid* obj2);

//    /**
//     * Returns the descendant of the element with the given UID from
//     * the given Version
//     */
//    ObjectWithUid* getDescendant(const QString& uid, Version* v);

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
