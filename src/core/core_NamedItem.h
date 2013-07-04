#ifndef _NAMEDITEM_H_
#define _NAMEDITEM_H_

#include <QString>
#include <QDebug>

/**
 * Stupid class for all the items in the system that must have a name associated (such as
 * data type, indices, tables, etc...)
 */
class NamedItem
{
public:

    NamedItem() : m_name("")
    {
    }

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

    QString getName() const
    {
        return m_name;
    }

    void setName(const QString& name)
    {
        onRename(m_name, name);
        m_name = name;
    }

    virtual void onRename(const QString& oldName, const QString& newName)
    {
        Q_UNUSED(oldName);
        Q_UNUSED(newName);
    }

protected:

    QString m_name;

};

#endif // _NAMEDITEM_H_
