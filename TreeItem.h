#ifndef TREEITEM_H
#define TREEITEM_H

#include <QTreeWidgetItem>

/**
 * Every object that can be put in the tree (DataType, Table, etc...) must be derived from this class
 * to ensure that the object knows how to update its tree entry. This class gives read/write access
 * to the tree location
 */
class TreeItem
{
public:

    TreeItem() : m_location(0){}

    virtual ~TreeItem() {}

    void setLocation(QTreeWidgetItem* loc)
    {
        this->m_location = loc;
    }

    QTreeWidgetItem*& getLocation()
    {
        return m_location;
    }


private:
    // where the object is to be found
    QTreeWidgetItem* m_location;

};

#endif // TREEITEM_H
