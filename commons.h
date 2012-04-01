#ifndef COMMONS_H
#define COMMONS_H

#include <QString>

#include "Version.h"
#include "NamedItem.h"

typedef NamedItem* (Version::*itemGetter)(const QString&);

#endif // COMMONS_H
