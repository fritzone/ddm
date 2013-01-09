#ifndef CORE_ITEMWITHDESCRIPTION_H
#define CORE_ITEMWITHDESCRIPTION_H

#include <QString>

class ItemWithDescription
{

public:
    ItemWithDescription(QString& desc) : m_description(desc){}
    ItemWithDescription() : m_description() {}

    void setDescription(const QString& desc)
    {
        m_description = desc;
    }

    QString getDescription() const
    {
        return m_description;
    }

private:

    QString m_description;

};

#endif // CORE_ITEMWITHDESCRIPTION_H
