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

    static const QIcon& getAddIcon()
    {
        static const QIcon icon(strAddIcon);
        return icon;
    }

    static const QIcon& getEmptyIcon()
    {
        static const QIcon icon(strEmptyIcon);
        return icon;
    }

    static const QIcon& getTablesIcon()
    {
        static const QIcon icon(strTablesIcon);
        return icon;
    }

};

#endif // ICONFACTORY_H
