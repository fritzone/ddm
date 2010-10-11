#ifndef ABSTRACTSTORAGEENGINELISTPROVIDER_H
#define ABSTRACTSTORAGEENGINELISTPROVIDER_H

#include "AbstractStorageEngine.h"
#include <QVector>

class AbstractStorageEngineListProvider
{
public:
    AbstractStorageEngineListProvider();

    virtual QVector<AbstractStorageEngine*> getStorageEngines() = 0;
};

#endif // ABSTRACTSTORAGEENGINELISTPROVIDER_H
