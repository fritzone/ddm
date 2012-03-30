#ifndef VALUELISTSP_H
#define VALUELISTSP_H

#include "db_SP.h"

#include <QStringList>

class ValueListSp : public Sp
{
public:

    ValueListSp(const QString& sqlPrUid, const QString& referringObjectClassUid,
                const QString &name, const QString& propertyGuiText,
                const QString& group, const QStringList& values,
                int defaultValueIndex, int major, int minor);
    virtual QUuid getClassUid() const;
    virtual SpInstance* instantiate();
    virtual SpInstance* createSpecifiedInstance(const QString& spi_uid);
    const QStringList& getValues() const
    {
        return m_values;
    }
    int getDefaultValuesIndex() const
    {
        return m_defaultValuesIndex;
    }

private:

    QStringList m_values;
    int m_defaultValuesIndex;

};

#endif // VALUELISTSP_H
