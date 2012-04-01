#ifndef CORE_TRIGGER_H
#define CORE_TRIGGER_H

#include "NamedItem.h"
#include "SqlSourceEntity.h"
#include "TreeItem.h"
#include "SerializableElement.h"
#include "core_ObjectWithUid.h"

class Table;

class Trigger : public SqlSourceEntity, public NamedItem, public TreeItem, public SerializableElement, public ObjectWithUid
{
public:

    Trigger(const QString& name, const QString& uid);

    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

    void setSql(const QString& s)
    {
        m_body = s;
    }

    QString getSql() const
    {
        return m_body;
    }

    QString getTable() const
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

    void setTable(const QString& t)
    {
        m_table = t;
    }

    virtual QUuid getClassUid() const;

private:

    QString m_body;
    QString m_event;
    QString m_ttime;
    QString m_table;
};

#endif // CORE_TRIGGER_H
