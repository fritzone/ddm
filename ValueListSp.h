#ifndef VALUELISTSP_H
#define VALUELISTSP_H

#include "db_SP.h"

#include <QStringList>

class ValueListSp : public Sp
{
public:

    ValueListSp(const QString& sqlPrUid, const QString& referringObjectClassUid, const QString &name, const QString& propertyGuiText, const QString& group, const QStringList& values);
    virtual QUuid getClassUid() const;
    virtual SpInstance* instantiate();
    virtual SpInstance* createSpecifiedInstance(const QString& spi_uid);
    const QStringList& getValues() const
    {
        return m_values;
    }

private:

    QStringList m_values;

};

#endif // VALUELISTSP_H
