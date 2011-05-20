#ifndef QUERYCOMPONENTS_H
#define QUERYCOMPONENTS_H

#include <OptionsType.h>

#include <QString>
#include <QList>
#include <QSet>

class QueryGraphicsItem;
class QueryGraphicsHelper;

/**
 * The logic behind a query
 */
class QueryComponent
{
public:
    QueryComponent(QueryComponent* parent, int level) : m_parent(parent), m_level(level) {}
    virtual ~QueryComponent(){}

    virtual QString get() = 0;
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper* helper, QueryGraphicsItem* parent) = 0;
    virtual void handleAction(const QString& action, QueryComponent* referringObject) = 0;

    /**
     * This function provides a list of options that are presented in the rect/diamond/circle button in front of
     * the query. These should not be messed up with the types...
     */
    virtual QSet<OptionsType> provideOptions() = 0;

    /**
     * If the query component has any types supported, this will give them back...
     */
    virtual QSet<CellTypeChooserType>  getTypeset() const = 0;

    virtual void onClose() = 0;
    virtual QueryComponent* duplicate() = 0;

    void addChild(QueryComponent* c) {m_children.append(c);}

    QList<QueryComponent*>& getChildren() {return m_children;}
    void removeChild(QueryComponent* c);
    QueryComponent* getParent() {return m_parent; }
    void setParent(QueryComponent* p) {m_parent = p;}
    int getLevel() const {return m_level;}

protected:
    QList<QueryComponent*> m_children;
    QueryComponent* m_parent;
    int m_level;
};

#endif // QUERYCOMPONENTS_H

