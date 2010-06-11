#ifndef ABSTRACTSTORAGEENGINE_H
#define ABSTRACTSTORAGEENGINE_H

#include <QString>

class AbstractStorageEngine
{
public:
    AbstractStorageEngine();

    virtual bool supportsGeospatialDataType() = 0;

    virtual bool supportsIndexOnGeospatialDataType() = 0;

    virtual bool supportsIndexType(const QString&) = 0;

    virtual bool supportsForeignKeys() = 0;

    virtual bool supportsTransactions() = 0;

    virtual QString name() = 0;
};

#endif // ABSTRACTSTORAGEENGINE_H
