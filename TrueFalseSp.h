#ifndef TRUEFALSESP_H
#define TRUEFALSESP_H

#include "db_SP.h"

/**
 * Class representing a special property with a TRUE/FALSE value
 */
class TrueFalseSp : public Sp
{
public:

    TrueFalseSp(const QString& sqlPrUid, const QString& referringObjectClassUid, const QString& name, const QString& propertyGuiText);
    virtual QUuid getClassUid() const;
    virtual SpInstance* instantiate(const Sp*);
};

#endif // TRUEFALSESP_H
