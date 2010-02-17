#ifndef COLUMN_H
#define COLUMN_H

#include <QString>

class UserDataType;

/**
 * This class holds the necessary data to define a column
 */
class Column
{
public:
    Column(const QString& name, const UserDataType* type, bool pk);

    const QString& getName() const
    {
        return m_name;
    }

    const UserDataType* getdataType() const
    {
        return m_type;
    }

    bool isPk() const
    {
        return m_pk;
    }

private:
    QString m_name;
    const UserDataType* m_type;
    bool m_pk;
};

#endif // COLUMN_H
