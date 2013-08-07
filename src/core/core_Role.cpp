#include "core_Role.h"

Role::Role(const QString &name, const QString &uid, const QString &description) : NamedItem(name), m_description(description), m_uid(uid)
{
}
