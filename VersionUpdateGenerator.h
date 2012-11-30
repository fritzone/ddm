#ifndef VERSIONUPDATEGENERATOR_H
#define VERSIONUPDATEGENERATOR_H

#include "core_ColumnWithValue.h"

#include <QStringList>
#include <QMap>

class Version;
class TableUpdateGenerator;
class Column;

/**
 * @brief The VersionUpdateGenerator class
 * This class generates code that is supposed to be executed on a database which has deployed
 * the version "from" but needs to be updated to the version "to".
 */
class VersionUpdateGenerator
{

public:

    VersionUpdateGenerator(Version* from, Version* to);

    QStringList getCommands() const
    {
        return m_commands;
    }
    void generateDefaultValuesUpdateData(const QVector<QVector<ColumnWithValue *> > &pksTo, const QVector<QVector<ColumnWithValue *> > &pksFrom, const QVector<QVector<ColumnWithValue *> > &allTo, const QVector<QVector<ColumnWithValue *> > &allFrom, Version *to, const QString &tabName, bool delayedCommands);

private:

    /**
     * This method generates code to drop tables, create tables or alter table structure
     * @brief updateTables
     * @param from
     * @param to
     */
    void updateTables(Version* from, Version* to);

    /**
     * This method generates code to update the default values of the tables
     * @brief updateTableInstances
     * @param from
     * @param to
     */
    void updateTableInstances(Version* from, Version* to);

    /**
     * Generate code to update the views
     * @brief updateViews
     * @param from
     * @param to
     */
    void updateViews(Version* from, Version* to);
    void updateProcedures(Version* from, Version* to);


private:

    QStringList m_commands;
    QStringList m_generatedTabinstUids;                                           // contains the UIDS of TabInsts for which we generated SQL already
    QMap<QString, TableUpdateGenerator*> m_tableUpdates;
    QMap<QString, QStringList> m_tablesReferencedWithFkFromOtherTables;           // the table -> columns which are referenced by other tables with foreign keys
                                                                                  // and must have the foreign keys dropped because of a name change in the PK
    QStringList m_delayedCommands;
};

#endif // VERSIONUPDATEGENERATOR_H
