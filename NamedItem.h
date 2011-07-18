#ifndef NAMEDITEM_H
#define NAMEDITEM_H

#include <QString>

/**
 * Stupid class for all the items in the system that must have a name associated (such as
 * data type, indices, tables, etc...)
 */
class NamedItem
{
public:

    NamedItem() : m_name("")
    {}

    NamedItem(const QString& name) : m_name(name)
    {}

    NamedItem(const NamedItem& other) : m_name(other.m_name)
    {}

    NamedItem& operator = (const NamedItem& other)
    {
        if(this != &other)
        {
            setName(other.getName());
        }

        return *this;
    }

    const QString& getName() const
    {
        return m_name;
    }

    QString getNiceName() const
    {
        QString a = m_name.mid(1).toUpper();
        return a;
    }

    void setName(const QString& name)
    {
        m_name = name;
    }

protected:

    QString m_name;

};

#endif // NAMEDITEM_H
