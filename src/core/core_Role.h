#ifndef CORE_ROLE_H
#define CORE_ROLE_H

#include "core_NamedItem.h"

#include <QUuid>

class Role : public NamedItem
{
public:
    Role(const QString& name, const QString& uid, const QString& description);

    QString getDescription() const
    {
        return m_description;
    }

    QUuid getUid() const
    {
        return m_uid;
    }

private:
    QString m_description;
    QUuid m_uid;
};

#endif // CORE_ROLE_H
