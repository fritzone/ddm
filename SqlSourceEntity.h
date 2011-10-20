#ifndef SQLSOURCEENTITY_H
#define SQLSOURCEENTITY_H

#include <QStringList>

class AbstractSqlGenerator;

/**
 * This is a very stupid name for a class which can generate SQLs (ie. a Table or a Trigger).
 */
class SqlSourceEntity
{
public:
    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const QString& codepage) = 0;
    virtual ~SqlSourceEntity() {}
};

#endif // SQLSOURCEENTITY_H
