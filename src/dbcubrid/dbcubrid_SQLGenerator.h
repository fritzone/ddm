#ifndef CUBRIDSQLGENERATOR_H
#define CUBRIDSQLGENERATOR_H

#include <QDebug>

#include "db_AdvancedSqlGenerator.h"

class DatabaseEngine;
class ForeignKey;
class Index;
class CUBRIDConnection;

class CUBRIDSQLGenerator : public AdvancedSqlGenerator
{
public:

    CUBRIDSQLGenerator(DatabaseEngine* engine) : AdvancedSqlGenerator(engine)
    {
    }

    ~CUBRIDSQLGenerator() {}

    virtual QStringList generateCreateTableSql(Table* table, const QHash<QString, QString>& options, const QString& tabName, const QMap<QString, QString> &fkMappings, const Connection* pdest) const;
    virtual QStringList generateAlterTableForForeignKeys(Table* t, const QHash<QString, QString>& options) const;
    virtual QString getDropProcedure(const QString& proc);
    virtual QString getDropFunction(const QString& func);
    virtual QString sqlForAColumn(const Column* col) const;
    virtual QString getRecreateForeignKeySql(ForeignKey* fkI, const QString &foreignKeysTable);
    virtual QString backtickedName(const QString& name) const;

private:

    QString createTableOnlyScript(Table* table, const QStringList &foreignKeys, const QString &tabName, const Connection *pdest) const;
    QString getIndexUsedLength(Index* idx, const Column* c) const;
    QString createViewReplaceability(View* v) const;
    QString createViewColumnNames(View *v) const;
};

#endif // CUBRIDSQLGENERATOR_H
