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
private:
    QStringList m_commands;
};

#endif // TABLEUPDATEGENERATOR_H
