#ifndef MYSQLINDEXTYPEPROVIDER_H
#define MYSQLINDEXTYPEPROVIDER_H

#include "db_AbstractIndextypeProvider.h"

class MySQLIndexTypeProvider : public AbstractIndextypeProvider
{

public:

    MySQLIndexTypeProvider()
    {
        indexTypes << "BTREE" << "HASH" << "RTREE";
    }

    virtual QStringList getIndexTypes()
    {
        return indexTypes;
    }

    virtual QString getDefaultIndextype()
    {
        return "BTREE";
    }

private:

    QStringList indexTypes;

};

#endif // MYSQLINDEXTYPEPROVIDER_H
