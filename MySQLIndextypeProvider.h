#ifndef MYSQLINDEXTYPEPROVIDER_H
#define MYSQLINDEXTYPEPROVIDER_H

#include "AbstractIndextypeProvider.h"

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

private:

    QStringList indexTypes;

};

#endif // MYSQLINDEXTYPEPROVIDER_H
