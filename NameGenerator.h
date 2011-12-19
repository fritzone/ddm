#ifndef NAMEGENERATOR_H
#define NAMEGENERATOR_H

#include <QString>

#include "Version.h"
#include "NamedItem.h"

class NameGenerator
{
public:

    typedef NamedItem* (Version::*itemGetter)(const QString&);

    static QString getUniqueName(Version* v, itemGetter fun, const QString& prefix);

private:

    static const long MAX_IT = 99999999;
};

#endif // NAMEGENERATOR_H
