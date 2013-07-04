#ifndef QUERYCOMPONENTS_H
#define QUERYCOMPONENTS_H

#include "qbr_OptionTypes.h"
#include "core_SerializableElement.h"
#include "core_ObjectWithUid.h"
#include "core_CloneableElement.h"
#include "core_VersionElement.h"

#include <QString>
#include <QList>
#include <QSet>
#include <QVector>

class QueryGraphicsItem;
class QueryGraphicsHelper;
class Query;

/**
 * The logic behind a query
 */
class QueryComponent :
        public SerializableElement,
        public VersionElement,
        public CloneableElement
{
public:

    enum LEVELS
    {
        PARAMETER_LEVEL = -2
    };

    QueryComponent(Query* q, QueryComponent* parent, int level, Version *v) :
        VersionElement(QUuid::createUuid().toString(), v),
        m_parent(parent), m_level(level), m_query(q)
    {}

    virtual ~QueryComponent()
    {}

    /**
     * @brief get returns the stringified value of the query component
     * @return
     */
    virtual QString get() const = 0;

    /**
     * @brief getClass is just for tracking in the serialization
     * @return
     */
    virtual QString getClass() const = 0;

    /**
     * @brief creates a graphics item in accordance to this component
     * @param helper
     * @param parent
     * @return
     */
    virtual QueryGraphicsItem* createGraphicsItem(QueryGraphicsHelper* helper,
                                                  QueryGraphicsItem* parent) = 0;

    /**
     * @brief handles a specific action triggered from the gui
     * @param action the action to be executed
     * @param referringObject this object initated the action
     */
    virtual void handleAction(const QString& action,
                              QueryComponent* referringObject) = 0;

    /**
     * This function provides a list of options that are presented in the
     * rect/diamond/circle button in front of
     * the query. These should not be messed up with the types...
     */
    virtual QSet<OptionsType> provideOptions() = 0;

    /**
     * @brief onClose called if this component is closed
     */
    virtual void onClose() = 0;

    /**
     * @brief duplicates the component
     * @return
     */
    virtual QueryComponent* duplicate() = 0;

    /**
     * @brief getClassUid return the class UID of this component
     * @return
     */
    virtual QUuid getClassUid() const = 0;

    /**
     * @brief clones the component
     * @param sourceVersion
     * @param targetVersion
     * @return
     */
    virtual CloneableElement* clone(Version* sourceVersion,
                                    Version* targetVersion) = 0;
public:

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

public:

    void cloneTheChildren(Version* sourceVersion,
                          Version* targetVersion,
                          QueryComponent *targetObject);

    void addChild(QueryComponent* c)
    {
        m_children.append(c);
    }

    void insertChild(int index, QueryComponent* c)
    {
        m_children.insert(index, c);
    }

    void setChild(QueryComponent* c, int i)
    {
        m_children.insert(i,c);
    }

    const QList<QueryComponent*>& getChildren() const
    {
        return m_children;
    }

    QueryComponent* getParent()
    {
        return m_parent;
    }

    void setParent(QueryComponent* p)
    {
        m_parent = p;
    }

    int getLevel() const
    {
        return m_level;
    }

    int nextLevel() const
    {
        return getLevel() + 1;
    }

    bool hasParent() const
    {
        return m_parent != 0;
    }

    bool hasChildren() const
    {
        return !(m_children.empty());
    }

    Query* getQuery() const;

    QString getSpacesForLevel() const;

    void removeChild(QueryComponent* c);

    template <class T> bool is() const
    {
        return dynamic_cast<const T*>(this) != 0;
    }

    template <class T> T* as()
    {
        return dynamic_cast<T*>(this);
    }


private:

    // the children of this component
    QList<QueryComponent*> m_children;

    // the parent of this component
    QueryComponent* m_parent;

    // this component can be found at this level
    int m_level;

    // this component is in this query.
    Query* m_query;
};

#endif // QUERYCOMPONENTS_H
