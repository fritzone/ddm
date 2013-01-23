#include "core_Role.h"

Role::Role(const QString &name, const QString &uid, const QString &description) : NamedItem(name), m_uid(uid), m_description(description)
{
}
