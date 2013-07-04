#ifndef CORE_ENTITY_H
#define CORE_ENTITY_H

#include "core_NamedItem.h"
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
        bool builtin;
    };

    struct Collection
    {
        QString name;
        QString roleUid;
        QString targetUid;
        bool builtin;
    };

    struct Reference
    {
        QString name;
        QString srcEntityUid;
        QString srcRoleUid;
        bool builtin;
    };

    Entity(const QString& name, const QUuid& classUid, const QUuid& targetClassUid, bool hasDesc, bool supportSps);

    virtual QUuid getClassUid() const;

    QUuid getTargetClassUid() const
    {
        return m_targetClassUid;
    }

    void addAttribute(Attribute* a)
    {
        m_attributes.append(a);
    }

    void addCollection(Collection* c)
    {
        m_collections.append(c);
    }

    void addReference(Reference* r)
    {
        m_references.append(r);
    }

    const QVector<Attribute*> & attributes()
    {
        return m_attributes;
    }

    const QVector<Collection*> & collections()
    {
        return m_collections;
    }

    const QVector<Reference*> & references()
    {
        return m_references;
    }

    void setAttribute(const QString& a, const QString& v);

    Attribute* getAttribute(const QString& attr);

protected:
    QUuid m_classUid;
    QUuid m_targetClassUid;
    bool m_hasDescription;
    bool m_supportsSps;
    QVector<Attribute*> m_attributes;
    QVector<Collection*> m_collections;
    QVector<Reference*> m_references;
};

#endif // CORE_ENTITY_H
