#ifndef TABLEINSTANCE_H
#define TABLEINSTANCE_H

#include "Table.h"
#include "TreeItem.h"
#include "NamedItem.h"

#include <QHash>
#include <QList>
#include <QString>

class TableInstance : public TreeItem, public NamedItem
{
public:

    TableInstance(Table* tab);

    QList<QString> columns() const;

    Table* table() const
    {
        return m_table;
    }

private:

    Table* m_table;

    QHash < QString, QVector<QString> > m_values;

};

#endif // TABLEINSTANCE_H
