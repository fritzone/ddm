#ifndef TRUEFALSESP_H
#define TRUEFALSESP_H

#include "db_SP.h"
#include "ddm_strings.h"

/**
 * Class representing a special property with a TRUE/FALSE value
 */
class TrueFalseSp : public Sp
{
public:

    TrueFalseSp(const QString& sqlPrUid, const QString& referringObjectClassUid,
                const QString& name, const QString& propertyGuiText, const QString& group,
                bool defaultValue,
                int major, int minor, Version *v);
    virtual QUuid getClassUid() const;
    virtual SpInstance* instantiate();
    virtual SpInstance* createSpecifiedInstance(const QString& spi_uid);

    virtual QString getDefaultValue() const
    {
        if(m_defaultValue) return strTrue;
        else return strFalse;
    }

private:

    bool m_defaultValue;
};

#endif // TRUEFALSESP_H
