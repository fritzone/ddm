#ifndef MYSQLSTORAGEENGINELISTPROVIDER_H
#define MYSQLSTORAGEENGINELISTPROVIDER_H

#include "AbstractStorageEngineListProvider.h"

class MySQLStorageEngineListProvider : public AbstractStorageEngineListProvider
{
public:
    MySQLStorageEngineListProvider();

    virtual QVector<AbstractStorageEngine*> getStorageEngines();
};

#endif // MYSQLSTORAGEENGINELISTPROVIDER_H
