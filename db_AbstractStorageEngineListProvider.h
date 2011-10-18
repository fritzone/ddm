#ifndef ABSTRACTSTORAGEENGINELISTPROVIDER_H
#define ABSTRACTSTORAGEENGINELISTPROVIDER_H

#include "db_AbstractStorageEngine.h"
#include <QVector>

class AbstractStorageEngineListProvider
{
public:
    AbstractStorageEngineListProvider(){}
    virtual ~AbstractStorageEngineListProvider(){}

    virtual QVector<AbstractStorageEngine*> getStorageEngines() = 0;
};

#endif // ABSTRACTSTORAGEENGINELISTPROVIDER_H
