#ifndef ICONFACTORY_H
#define ICONFACTORY_H

#include "strings.h"

#include <QIcon>

class IconFactory
{
public:

    /**
     * Returns the "key" icon
     */
    static const QIcon& getKeyIcon()
    {
        static const QIcon key(strKeyIcon);
        return key;
    }
};

#endif // ICONFACTORY_H
