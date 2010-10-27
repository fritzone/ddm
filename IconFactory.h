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

    static const QIcon& getTabinstIcon()
    {
        static const QIcon icon(strTableInstIcon);
        return icon;
    }

    static const QIcon& getTabinstLockIcon()
    {
        static const QIcon icon(strTableInstLockIcon);
        return icon;
    }

    static const QIcon& getDataTypesIcon()
    {
        static const QIcon icon(strDataTypesIcon);
        return icon;
    }

    static const QIcon& getStringDataTypesIcon()
    {
        static const QIcon icon(strStringsFolder);
        return icon;
    }

    static const QIcon& getCodeIcon()
    {
        static const QIcon icon(strCodeIcon);
        return icon;
    }

    static const QIcon& getSqlIcon()
    {
        static const QIcon icon(strSqlIcon);
        return icon;
    }

    static const QIcon& getVersionIcon()
    {
        static const QIcon icon(strVersionIcon);
        return icon;
    }

    static const QIcon& getRemoveIcon()
    {
        static const QIcon icon(strRemoveIcon);
        return icon;
    }

    static const QIcon& getProjectOpenIcon()
    {
        static const QIcon icon(strProjectOpenIcon);
        return icon;
    }

    static const QIcon& getDiagramIcon()
    {
        static const QIcon icon(strDiagramIcon);
        return icon;
    }

};

#endif // ICONFACTORY_H
