#ifndef MYSQLMEMORYSTORAGEENGINE_H
#define MYSQLMEMORYSTORAGEENGINE_H

#include "db_AbstractStorageEngine.h"

class MySQLMemoryStorageEngine : public AbstractStorageEngine
{
public:
    MySQLMemoryStorageEngine()
    {
    }

    virtual bool supportsGeospatialDataType()
    {
        return false;
    }

    virtual bool supportsIndexOnGeospatialDataType()
    {
        return false;
    }

    virtual bool supportsIndexType(const QString& )
    {
        return true;
    }

    virtual bool supportsForeignKeys()
    {
        return false;
    }

    virtual bool supportsTransactions()
    {
        return false;
    }

    virtual QString name()
    {
        return "Memory";
    }

};

#endif // MYSQLMEMORYSTORAGEENGINE_H
