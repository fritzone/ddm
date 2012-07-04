#ifndef TRUEFALSESP_H
#define TRUEFALSESP_H

#include "db_SP.h"

/**
 * Class representing a special property with a TRUE/FALSE value
 */
class TrueFalseSp : public Sp
{
public:

    TrueFalseSp(const QString& sqlPrUid, const QString& referringObjectClassUid,
                const QString& name, const QString& propertyGuiText, const QString& group,
                int major, int minor);
    virtual QUuid getClassUid() const;
    virtual SpInstance* instantiate();
    virtual SpInstance* createSpecifiedInstance(const QString& spi_uid);
};

#endif // TRUEFALSESP_H
