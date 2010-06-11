#ifndef MYSQLMYISAMSTORAGEENGINE_H
#define MYSQLMYISAMSTORAGEENGINE_H

#include "AbstractStorageEngine.h"

class MySQLMyISAMStorageEngine : public AbstractStorageEngine
{
public:
    MySQLMyISAMStorageEngine()
    {
    }

    virtual bool supportsGeospatialDataType()
    {
        return true;
    }

    virtual bool supportsIndexOnGeospatialDataType()
    {
        return true;
    }

    virtual bool supportsIndexType(const QString& type)
    {
        if(type == "HASH") return false;
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
        return "MyISAM";
    }

};

#endif // MYSQLMYISAMSTORAGEENGINE_H
