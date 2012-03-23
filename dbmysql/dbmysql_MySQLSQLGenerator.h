#ifndef MYSQLSQLGENERATOR_H
#define MYSQLSQLGENERATOR_H

#include <QDebug>

#include "db_AbstractSQLGenerator.h"

class DatabaseEngine;

class MySQLSQLGenerator : public AbstractSqlGenerator
{
public:

    MySQLSQLGenerator(DatabaseEngine* engine) : m_engine(engine)
    {
    }

    ~MySQLSQLGenerator() {}

    virtual QStringList generateCreateTableSql(Table* table, const QHash<QString, QString>& options, const QString& tabName) const;
    virtual QStringList generateDefaultValuesSql(TableInstance* tableInstance, const QHash<QString, QString>& options) const;
    virtual QStringList generateDefaultValuesSql(Table* table, const QHash<QString, QString>& options) const;
    virtual QStringList generateCreateViewSql(View *v, const QHash<QString, QString> &options) const;
    virtual QStringList generateAlterTableForForeignKeys(Table* t, const QHash<QString, QString>& options) const;
    virtual QStringList generateCreateStoredMethodSql(StoredMethod* p, const QHash<QString, QString>& options) const;
    virtual QStringList generateTriggerSql(Trigger* t, const QHash<QString, QString>& options) const;

private:

    QString quotelessString(const QString&) const;

private:

    DatabaseEngine* m_engine;

};

#endif // MYSQLSQLGENERATOR_H
