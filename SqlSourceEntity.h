#ifndef SQLSOURCEENTITY_H
#define SQLSOURCEENTITY_H

class AbstractSqlGenerator;

/**
 * This is a very stupid name for a class which can generate SQLs (ie. a Table or a Trigger).
 */
class SqlSourceEntity
{
public:
    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>) const = 0;
};

#endif // SQLSOURCEENTITY_H
