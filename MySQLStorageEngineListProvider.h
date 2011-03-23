#ifndef MYSQLSTORAGEENGINELISTPROVIDER_H
#define MYSQLSTORAGEENGINELISTPROVIDER_H

#include "AbstractStorageEngineListProvider.h"

class MySQLStorageEngineListProvider : public AbstractStorageEngineListProvider
{
public:
    MySQLStorageEngineListProvider();

    virtual QVector<AbstractStorageEngine*> getStorageEngines();

private:
    QVector<AbstractStorageEngine*> m_stEngines;
};

#endif // MYSQLSTORAGEENGINELISTPROVIDER_H
