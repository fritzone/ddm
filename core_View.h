#ifndef CORE_VIEW_H
#define CORE_VIEW_H

#include "NamedItem.h"
#include "SqlSourceEntity.h"

class SelectQuery;
class QueryGraphicsHelper;

class View : virtual public SqlSourceEntity, virtual public NamedItem
{
public:
    View();
    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const QString& codepage);

    QueryGraphicsHelper* getHelper() const
    {
        return c;
    }

    SelectQuery* getQuery() const
    {
        return sq;
    }

private:
    SelectQuery* sq;
    QueryGraphicsHelper* c;
};

#endif // CORE_VIEW_H
