#ifndef SQLSOURCEENTITY_H
#define SQLSOURCEENTITY_H

#include "ContextMenuEnabledTreeWidget.h"

/**
 * This is a very stupid name for a class which can generate SQLs (ie. a Table or a Trigger).
 */
class SqlSourceTreeItem
{
public:

    virtual ~SqlSourceTreeItem() {}
    void setSqlItem(ContextMenuEnabledTreeWidgetItem* sqlItem)
    {
        m_sqlItem = sqlItem;
    }

    void onDelete()
    {
        delete m_sqlItem;
    }

    ContextMenuEnabledTreeWidgetItem* getSqlLocation()
    {
        return m_sqlItem;
    }

private:

    // the SQL item if this table instance
    ContextMenuEnabledTreeWidgetItem* m_sqlItem;
};

#endif // SQLSOURCEENTITY_H
