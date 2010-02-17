#include "Column.h"

#include "UserDataType.h"

Column::Column(const QString& name, const UserDataType* type, bool pk) : m_name(name), m_type(type), m_pk(pk)
{

}
