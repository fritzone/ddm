#ifndef MYSQLARCHIVESTORAGEENGINE_H
#define MYSQLARCHIVESTORAGEENGINE_H

#include "AbstractStorageEngine.h"

class MySQLArchiveStorageEngine : public AbstractStorageEngine
{
public:
    MySQLArchiveStorageEngine()
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
        if(type=="HASH") return false;
        if(type=="BTREE") return false;
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
        return "Archive";
    }

};

#endif // MYSQLARCHIVESTORAGEENGINE_H
