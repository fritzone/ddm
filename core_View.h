#ifndef CORE_VIEW_H
#define CORE_VIEW_H

#include "NamedItem.h"
#include "SqlSourceEntity.h"
#include "TreeItem.h"
#include "SerializableElement.h"

class SelectQuery;
class QueryGraphicsHelper;

class View : virtual public SqlSourceEntity, virtual public NamedItem, virtual public TreeItem, virtual public SerializableElement
{
public:
    View(bool manual);

    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const QString& codepage);

    QueryGraphicsHelper* getHelper() const
    {
        return m_helper;
    }

    SelectQuery* getQuery() const
    {
        return m_selectQuery;
    }

    void setColumnNames(const QStringList& list)
    {
        m_columNames = list;
    }

    void setReplace(bool b)
    {
        m_canReplace = b;
    }

    bool getReplace() const
    {
        return m_canReplace;
    }

    bool getManual() const
    {
        return m_manual;
    }

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

private:
    SelectQuery* m_selectQuery;
    QueryGraphicsHelper* m_helper;
    QStringList m_columNames;
    bool m_canReplace;
    QString algorithm;
    // whether SQL for the view was manually typed in or the query builder was used
    bool m_manual;
};

#endif // CORE_VIEW_H
