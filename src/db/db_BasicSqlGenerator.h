#ifndef DB_BASICSQLGENERATOR_H
#define DB_BASICSQLGENERATOR_H

#include "db_AbstractSQLGenerator.h"

#include <QUuid>

class Table;
class ObjectWithSpInstances;
class DatabaseEngine;

class BasicSqlGenerator : public AbstractSqlGenerator
{
public:

    BasicSqlGenerator(DatabaseEngine* engine) : m_engine(engine) {}

    QString spiResult(ObjectWithSpInstances *, QUuid) const;

    QString generateTableCreationComments(const Table *t, const QString &tabName) const;

    QString keyword(QString) const;

protected:

    bool mutable m_upcase;
    DatabaseEngine* m_engine;
};

#endif // DB_BASICSQLGENERATOR_H
