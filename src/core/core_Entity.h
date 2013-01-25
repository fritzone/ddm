#ifndef CORE_ENTITY_H
#define CORE_ENTITY_H

#include "NamedItem.h"
#include "core_ObjectWithUid.h"

#include <QString>
#include <QUuid>

class Entity : public NamedItem, public ObjectWithUid
{
public:

    struct Attribute
    {
        QString name;
        QString roleUid;
    };

    struct Collection
    {
        QString name;
        QString roleUid;
        QString targetUid;

    };

    Entity(const QString& name, const QUuid& classUid, const QUuid& targetClassUid, bool hasDesc, bool supportSps);
    virtual QUuid getClassUid() const;
    QUuid getTargetClassUid() const
    {
        return m_targetClassUid;
    }

    void addAttribute(Attribute* a)
    {
        m_attr.append(a);
    }

    void addCollection(Collection* c)
    {
        m_coll.append(c);
    }

    const QVector<Attribute*> & attributes()
    {
        return m_attr;
    }

    const QVector<Collection*> & collections()
    {
        return m_coll;
    }

protected:
    QUuid m_classUid;
    QUuid m_targetClassUid;
    bool m_hasDescription;
    bool m_supportsSps;
    QVector<Attribute*> m_attr;
    QVector<Collection*> m_coll;
};

#endif // CORE_ENTITY_H
