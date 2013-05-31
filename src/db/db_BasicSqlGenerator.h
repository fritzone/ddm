#ifndef DB_BASICSQLGENERATOR_H
#define DB_BASICSQLGENERATOR_H

#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"

#include <QUuid>

class Table;
class ObjectWithSpInstances;
class DatabaseEngine;

class BasicSqlGenerator : public AbstractSqlGenerator
{
public:

    BasicSqlGenerator(DatabaseEngine* engine) : m_upcase(false),
        m_comments(false),
        m_backticks(false),
        m_engine(engine),
        m_pkpos(Configuration::PkAfterTableDeclaration),
        m_fkpos(Configuration::AfterTable)
    {}

    QString spiResult(const ObjectWithSpInstances *, QUuid) const;

    QString generateTableCreationComments(const Table *t, const QString &tabName) const;

    QString correctCase(QString) const;

    void initForOptions(const QHash<QString, QString> &options) const;

    QString requestedTableName(const QString&) const;

    QString startCreateTableCommand(Table* table) const;

    QString generateColumnList(Table *table) const;

    virtual QString sqlForAColumn(const Column* col) const = 0;

    QString generatePrimaryKeys(const QStringList&) const;

    QString generateForeignKeys(const QStringList&) const;

    QString backtickedName(const QString&) const;

protected:

    bool mutable m_upcase;
    bool mutable m_comments;
    bool mutable m_backticks;
    DatabaseEngine* m_engine;
    Configuration::PrimaryKeyPosition mutable m_pkpos;
    Configuration::ForeignKeyPosition mutable m_fkpos;
};

#endif // DB_BASICSQLGENERATOR_H
