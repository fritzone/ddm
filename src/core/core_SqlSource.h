#ifndef CORE_SQLSOURCE_H
#define CORE_SQLSOURCE_H

#include <QStringList>

class AbstractSqlGenerator;
class Connection;

/**
 * Base class for objects that can generate an SQL source
 */
class SqlSource
{
public:

    /**
     * Generates SQL for an operation that will be executed on the database specified by the connection parameter.
     * The parameter sqlOptions has the options which are to be used while generating the SQL script, such as
     * uppercase, or generate backticks, etc...
     * The parameter generator is the link to the database SQL generator doing the effective work
     *
     * @param generator - the DB SQL generator
     * @param sqlOptions - the SQL generation options
     * @param connection - the connection
     * @return a list of SQL statements that are to be executed
     */
    virtual QStringList generateSqlSource(AbstractSqlGenerator* generator, QHash<QString,QString> sqlOptions, const Connection* connection) = 0;

    /**
     * This method calculates an MD5 hash value of the SQL of this object. It is mainly used
     * for the comparison of elements between versions.
     * @return the MD5 hash of the SQL of the object
     */
    virtual QString getSqlHash() const = 0;

};

#endif // CORE_SQLSOURCE_H
