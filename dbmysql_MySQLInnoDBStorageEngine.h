#ifndef MYSQLINNODBSTORAGEENGINE_H
#define MYSQLINNODBSTORAGEENGINE_H

#include "AbstractStorageEngine.h"

class MySQLInnoDBStorageEngine : public AbstractStorageEngine
{
public:
    MySQLInnoDBStorageEngine()
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
        return true;
    }

    virtual bool supportsForeignKeys()
    {
        return true;
    }

    virtual bool supportsTransactions()
    {
        return true;
    }

    virtual QString name()
    {
        return "InnoDB";
    }

};

#endif // MYSQLINNODBSTORAGEENGINE_H
