#ifndef CORE_TRIGGER_H
#define CORE_TRIGGER_H

#include "NamedItem.h"
#include "SqlSourceEntity.h"
#include "TreeItem.h"
#include "SerializableElement.h"

class Trigger : virtual public SqlSourceEntity, virtual public NamedItem, virtual public TreeItem, virtual public SerializableElement
{
public:

    enum TriggerEvent
    {
        TRIGGER_INSERT,
        TRIGGER_UPDATE,
        TRIGGER_DELETE
    };

    enum TriggerTime
    {
        TRIGGER_BEFORE,
        TRIGGER_AFTER
    };

    Trigger();

    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const QString& codepage);
    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;

    void setSql(const QString& s)
    {
        m_body = s;
    }

    void setEvent(const QString&);
    void setTime(const QString&);

private:

    QString m_body;
    TriggerEvent m_event;
    TriggerTime m_ttime;

};

#endif // CORE_TRIGGER_H
