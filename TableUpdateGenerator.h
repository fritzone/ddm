#ifndef TABLEUPDATEGENERATOR_H
#define TABLEUPDATEGENERATOR_H

class Table;

#include <QStringList>

class TableUpdateGenerator
{
public:
    TableUpdateGenerator(Table* t1, Table* t2);

private:
    QStringList m_commands;
};

#endif // TABLEUPDATEGENERATOR_H
