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
                int major, int minor, Version* v);
    virtual QUuid getClassUid() const;
    virtual SpInstance* instantiate();
    virtual SpInstance* createSpecifiedInstance(const QString& spi_uid);
    QString getValue() const
    {
        return m_value;
    }

    virtual QString getDefaultValue() const
    {
        return getValue();
    }

private:

    QString m_value;
};

#endif // VALUESP_H
