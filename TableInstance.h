#ifndef TABLEINSTANCE_H
#define TABLEINSTANCE_H

#include "Table.h"
#include "TreeItem.h"

class TableInstance : public TreeItem
{
public:

    TableInstance(Table* tab);

private:

    Table* m_table;

};

#endif // TABLEINSTANCE_H
