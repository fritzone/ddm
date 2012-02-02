#ifndef CORE_TRIGGER_H
#define CORE_TRIGGER_H

#include "NamedItem.h"
#include "SqlSourceEntity.h"
#include "TreeItem.h"
#include "SerializableElement.h"

class Table;

class Trigger : virtual public SqlSourceEntity, virtual public NamedItem, virtual public TreeItem, virtual public SerializableElement
{
public:

    Trigger();
    Trigger(const QString&);

    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const QString& codepage);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

    void setSql(const QString& s)
    {
        m_body = s;
    }

    QString getSql() const
    {
        return m_body;
    }

    const Table* getTable() const
    {
        return m_table;
    }

    QString getEvent() const
    {
        return m_event;
    }

    QString getTime() const
    {
        return m_ttime;
    }

    void setEvent(const QString& e)
    {
        m_event = e;
    }

    void setTime(const QString& t)
    {
        m_ttime = t;
    }

    void setTable(Table* t)
    {
        m_table = t;
    }

private:

    QString m_body;
    QString m_event;
    QString m_ttime;
    const Table* m_table;
};

#endif // CORE_TRIGGER_H
