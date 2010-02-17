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

    static const QIcon& getSaveIcon()
    {
        static const QIcon save(strSaveIcon);
        return save;
    }

    static const QIcon& getApplyIcon()
    {
        static const QIcon apply(strApplyIcon);
        return apply;
    }
};

#endif // ICONFACTORY_H
