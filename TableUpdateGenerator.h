#ifndef TABLEUPDATEGENERATOR_H
#define TABLEUPDATEGENERATOR_H

class Table;
class DatabaseEngine;

#include <QStringList>
#include <QVector>

struct NewColumns
{
    // the new name of the column (from Table 2)
    QString newName;

    // the column aftwer which this should be added (from Table 2)
    QString afterColumn;
};


class TableUpdateGenerator
{

public:
    TableUpdateGenerator(Table* t1, Table* t2, DatabaseEngine *dbEngine);

    const QStringList& commands() const
    {
        return m_commands;
    }

    const QStringList& droppedFksCommands() const
    {
        return m_droppedForeignKeys;
    }

    const QStringList& newFksCommands() const
    {
        return m_newForeignKeys;
    }

    const QVector<NewColumns>& getNewColumns() const
    {
        return m_newColumns;
    }

    bool fkDroppingIsRequired() const
    {
        return m_futurePksDropNeeded;
    }

    QStringList getAffectedPkColumns() const
    {
        return m_pkRenamedColsAffected;
    }

private:
    QStringList m_commands;
    QStringList m_droppedForeignKeys;
    QStringList m_newForeignKeys;
    QVector<NewColumns> m_newColumns;     // contains a lis of columns that are new in T2
    bool m_futurePksDropNeeded;
    QStringList m_pkRenamedColsAffected;  // list of PK columns that were renamed
};

#endif // TABLEUPDATEGENERATOR_H
