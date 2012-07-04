#ifndef VALUESP_H
#define VALUESP_H

#include "db_SP.h"

#include <QString>

class ValueSp : public Sp
{
public:

    ValueSp(const QString& sqlPrUid, const QString& referringObjectClassUid,
                const QString &name, const QString& propertyGuiText,
                const QString& group, const QString& value,
                int major, int minor);
    virtual QUuid getClassUid() const;
    virtual SpInstance* instantiate();
    virtual SpInstance* createSpecifiedInstance(const QString& spi_uid);
    const QString& getValue() const
    {
        return m_value;
    }

private:

    QString m_value;

};

#endif // VALUESP_H
