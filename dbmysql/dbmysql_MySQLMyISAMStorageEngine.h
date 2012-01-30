#ifndef MYSQLMYISAMSTORAGEENGINE_H
#define MYSQLMYISAMSTORAGEENGINE_H

#include "db_AbstractStorageEngine.h"

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
        return false;
    }

    virtual bool supportsIndexType(const QString& type)
    {
        if(type == "HASH") return false;
        if(type == "BTREE") return false;
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
