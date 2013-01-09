#ifndef DOCUMENTATIONSOURCE_H
#define DOCUMENTATIONSOURCE_H

#include "ContextMenuEnabledTreeWidget.h"

class DocumentationSourceTreeItem
{
public:

    void setDocItem(ContextMenuEnabledTreeWidgetItem* sqlItem)
    {
        m_docItem = sqlItem;
    }

    void onDeleteDoc()
    {
        delete m_docItem;
    }

    ContextMenuEnabledTreeWidgetItem* getDocLocation()
    {
        return m_docItem;
    }


private:
    ContextMenuEnabledTreeWidgetItem* m_docItem;
};

#endif // DOCUMENTATIONSOURCE_H
