#ifndef MYSQLDATABASEENGINE_H
#define MYSQLDATABASEENGINE_H

#include "DatabaseEngine.h"

class MySQLDatabaseEngine : public DatabaseEngine
{
public:
    MySQLDatabaseEngine();

    virtual bool supportsEngines()
    {
        return true;
    }

    virtual QString dbname()
    {
        return "MySQL";
    }
};

#endif // MYSQLDATABASEENGINE_H
