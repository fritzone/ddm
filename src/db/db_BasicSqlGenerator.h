#ifndef DB_BASICSQLGENERATOR_H
#define DB_BASICSQLGENERATOR_H

#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"

#include <QUuid>

class Table;
class ObjectWithSpInstances;
class DatabaseEngine;
class Index;

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

    QString quotelessString(const QString&) const;

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

    QStringList foreignKeyParticipants(Table* table, const QMap<QString, QString> &fkMappings) const;

    QString basicCreateTableScript(Table *table, const QStringList &foreignKeys, const QString &tabName, bool needfk) const;

    void appendCreateIndexCommands(Table* table, QStringList& toReturn, const QString& tabName) const;

    virtual QStringList generateCreateTableSql(Table *table,
                                               const QHash<QString, QString> &options,
                                               const QString& tabName,
                                               const QMap<QString, QString> &fkMappings,
                                               const Connection* pdest) const;

    QStringList generateDefaultValuesSql(TableInstance* tableInstance, const QHash<QString, QString>& options) const;
    QStringList generateDefaultValuesSql(Table* table, const QHash<QString, QString>& options) const;
    QStringList generateCreateViewSql(View *v, const QHash<QString, QString> &options) const;
    QStringList generateTriggerSql(Trigger* t, const QHash<QString, QString>& options) const;

    // stuff returning empty results. other generators should override if needed
    virtual QString getIndexUsedLength(Index*, const Column*) const;
    virtual QString indexTypeSpecified(Index*) const;
    virtual QString createViewReplaceability(View*) const;
    virtual QString createViewColumnNames(View*) const;

    // default behavior for some queries (return more emptyness)
    QString getAlterTableForChangeColumnOrder(const QString& /*table*/, const Column* /*column*/, const QString& /*afterThis*/);
    QStringList generateAlterTableForForeignKeys(Table* t, const QHash<QString, QString>& options) const;
    QStringList generateCreateStoredMethodSql(StoredMethod* p, const QHash<QString, QString>& options) const;
    QString getAlterTableForColumnChange(const QString& table, const Column* col);
    QString getDropProcedure(const QString&);
    QString getDropFunction(const QString&);
    QString getAlterTableToDropForeignKey(const QString& table, const QString& fkName);
    QString getAlterTableForColumnRename(const QString& table, const Column* column, const QString& oldName);
    QString getAlterTableForColumnDeletion(const QString& table, const QString& column);
    QString getRecreateForeignKeySql(ForeignKey* fkI, const QString& foreignKeysTable, const QString &localTable);


    // common sense behavior for other queries, return SQL standard queries
    QString getInsertsIntoTable(const QString& table, const QStringList& columns, const QStringList& values);
    QString getDeleteFromTable(const QString& table, const QStringList& pkeys, const QStringList& pvalues);
    QString getDropView(const QString& viewName);
    QString getDropTrigger(const QString& trig);
    QString getUpdateTableForColumns(const QString& table, const QStringList& pkeys, const QStringList& pvalues, const QString& destCol, const QString& destValue);
    QString getDropTable(const QString& table);

    virtual QString createTableOnlyScript(Table* table, const QStringList &foreignKeys, const QString &tabName, const Connection *dest) const;
    virtual QString backtickedName(const QString&) const;

protected:

    bool mutable m_upcase;
    bool mutable m_comments;
    bool mutable m_backticks;
    DatabaseEngine* m_engine;
    Configuration::PrimaryKeyPosition mutable m_pkpos;
    Configuration::ForeignKeyPosition mutable m_fkpos;
};

#endif // DB_BASICSQLGENERATOR_H
