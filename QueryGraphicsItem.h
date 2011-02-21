#ifndef QUERYGRAPHICSITEM_H
#define QUERYGRAPHICSITEM_H

#include <QGraphicsItemGroup>

class QueryGraphicsHelper;
class QueryComponent;

/**
 * This is a graphics item representing a query
 */
class QueryGraphicsItem : public QGraphicsItemGroup
{
public:
    static const int CHILDREN_ALIGNMENT = 15;
    static const int CELL_SIZE = 20;
    QueryGraphicsItem(QueryGraphicsItem* parent, QueryGraphicsHelper* comps, QueryComponent* owner);
    virtual ~QueryGraphicsItem() {}

    virtual QGraphicsItemGroup* render(int& x, int& y, int& w, int &h) = 0;
    virtual void updateWidth(int newWidth) = 0;
    virtual void mousePress(int x, int y) = 0;
    virtual void mouseMove(int x, int y) = 0;
    virtual void mouseLeft(int x, int y) = 0;
    virtual void onClose() = 0;
    void addChild(QueryGraphicsItem*);
    QueryGraphicsItem* getParent()
    {
        return m_parent;
    }
    void removeChild(QueryGraphicsItem* child);
    QueryComponent* getOwner() {return m_owner; }

protected:

    QList<QueryGraphicsItem*> m_children;
    QueryGraphicsItem* m_parent;
    QueryGraphicsHelper* m_helper;
    QueryComponent* m_owner;
};

#endif // QUERYGRAPHICSITEM_H
