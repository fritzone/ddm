#ifndef TREEITEM_H
#define TREEITEM_H

#include <QTreeWidgetItem>

/**
 * Every object that can be put in the tree (DataType, Table, etc...) must be derived from this class
 * to ensure that the object knows how to update its tree entry. This class gives read/write access
 * to the tree location. On the other end, all the other elements that are put in the trees in the
 * NewTableForm are also using this, since they are also can be found in a tree widget.
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

protected:
    // where the object is to be found
    QTreeWidgetItem* m_location;

};

#endif // TREEITEM_H
