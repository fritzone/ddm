#ifndef NAMEGENERATOR_H
#define NAMEGENERATOR_H

#include <QString>

#include "Version.h"
#include "NamedItem.h"
#include "commons.h"

class NameGenerator
{
public:

    static QString getUniqueName(Version* v, itemGetter fun, const QString& prefix);

private:

    static const long MAX_IT = 99999999;
};

#endif // NAMEGENERATOR_H
