#ifndef TABLEUPDATEGENERATOR_H
#define TABLEUPDATEGENERATOR_H

class Table;
class DatabaseEngine;

#include <QStringList>

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

private:
    QStringList m_commands;
    QStringList m_droppedForeignKeys;
    QStringList m_newForeignKeys;
};

#endif // TABLEUPDATEGENERATOR_H
