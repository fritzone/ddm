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

    static const QIcon& getTableIcon()
    {
        static const QIcon icon(strTableIcon);
        return icon;
    }

    static const QIcon& getNotIcon()
    {
        static const QIcon icon(strNotIcon);
        return icon;
    }

    static const QIcon& getBigNotIcon()
    {
        static const QIcon icon(strBigNotIcon);
        return icon;
    }

    static const QIcon& getNegIcon()
    {
        static const QIcon icon(strNegIcon);
        return icon;
    }

    static const QIcon& getMinusIcon()
    {
        static const QIcon icon(strMinusIcon);
        return icon;
    }

    static const QIcon& getOpenParanthesesIcon()
    {
        static const QIcon icon(strParanthesesOpenIcon);
        return icon;
    }

    static const QIcon& getCloseParanthesesIcon()
    {
        static const QIcon icon(strParanthesesCloseIcon);
        return icon;
    }

    static const QIcon& getFunctionIcon()
    {
        static const QIcon icon(strFunctionIcon);
        return icon;
    }

    static const QIcon& getBigMinusIcon()
    {
        static const QIcon icon(strBigMinusIcon);
        return icon;
    }

    static const QIcon& getBigNegIcon()
    {
        static const QIcon icon(strBigNegIcon);
        return icon;
    }

    static const QIcon& getDatabaseIcon()
    {
        static const QIcon icon(strDatabaseIcon);
        return icon;
    }

    static const QIcon& getTabinstIcon()
    {
        static const QIcon icon(strTableInstIcon);
        return icon;
    }

    static const QIcon& getLiteralIcon()
    {
        static const QIcon icon(strTypeHereIcon);
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

    static const QIcon& getStringDataTypeIcon()
    {
        static const QIcon icon(strDtTextIcon);
        return icon;
    }

    static const QIcon& getIntDataTypesIcon()
    {
        static const QIcon icon(strIntsFolder);
        return icon;
    }

    static const QIcon& getIntDataTypeIcon()
    {
        static const QIcon icon(strDtNumericIcon);
        return icon;
    }

    static const QIcon& getBlobDataTypesIcon()
    {
        static const QIcon icon(strBlobFolder);
        return icon;
    }

    static const QIcon& getBlobDataTypeIcon()
    {
        static const QIcon icon(strDtBlobIcon);
        return icon;
    }

    static const QIcon& getBoolDataTypesIcon()
    {
        static const QIcon icon(strBoolFolder);
        return icon;
    }

    static const QIcon& getBoolDataTypeIcon()
    {
        static const QIcon icon(strDtBoolIcon);
        return icon;
    }

    static const QIcon& getMiscDataTypesIcon()
    {
        static const QIcon icon(strMiscFolder);
        return icon;
    }

    static const QIcon& getMiscDataTypeIcon()
    {
        static const QIcon icon(strDtMiscIcon);
        return icon;
    }

    static const QIcon& getSpatialDataTypesIcon()
    {
        static const QIcon icon(strSpatialFolder);
        return icon;
    }

    static const QIcon& getSpatialDataTypeIcon()
    {
        static const QIcon icon(strDtSpatialIcon);
        return icon;
    }

    static const QIcon& getDateTimeDataTypesIcon()
    {
        static const QIcon icon(strDateTimesFolder);
        return icon;
    }

    static const QIcon& getDateTimeDataTypeIcon()
    {
        static const QIcon icon(strDtDateTimeIcon);
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

    static const QIcon& getViewsIcon()
    {
        static const QIcon icon(strViewsIcon);
        return icon;
    }

    static const QIcon& getTriggersIcon()
    {
        static const QIcon icon(strTriggersIcon);
        return icon;
    }

    static const QIcon& getTriggerIcon()
    {
        static const QIcon icon(strTriggerIcon);
        return icon;
    }

    static const QIcon& getDiagramIcon()
    {
        static const QIcon icon(strDiagramIcon);
        return icon;
    }

    static const QIcon& getIndexIcon()
    {
        static const QIcon icon(strIndexIcon);
        return icon;
    }

    static const QIcon& getWarningIcon()
    {
        static const QIcon icon(strWarningIcon);
        return icon;
    }

    static const QIcon& getRecommendIcon()
    {
        static const QIcon icon(strRecommendIcon);
        return icon;
    }

    static const QIcon& getCloseIcon()
    {
        static const QIcon icon(strCloseIcon);
        return icon;
    }

    static const QIcon& getXorIcon()
    {
        static const QIcon icon(strXorIcon);
        return icon;
    }

    static const QIcon& getBinaryOrIcon()
    {
        static const QIcon icon(strBinaryOrIcon);
        return icon;
    }

    static const QIcon& getLeftShiftIcon()
    {
        static const QIcon icon(strLeftShiftIcon);
        return icon;
    }

    static const QIcon& getRightShiftIcon()
    {
        static const QIcon icon(strRightShiftIcon);
        return icon;
    }

    static const QIcon& getBinaryAndIcon()
    {
        static const QIcon icon(strBinaryAndIcon);
        return icon;
    }

    static const QIcon& getPlusIcon()
    {
        static const QIcon icon(strPlusIcon);
        return icon;
    }

    static const QIcon& getDivideIcon()
    {
        static const QIcon icon(strDivideIcon);
        return icon;
    }

    static const QIcon& getModuloIcon()
    {
        static const QIcon icon(strModuloIcon);
        return icon;
    }

    static const QIcon& getMultiplyIcon()
    {
        static const QIcon icon(strMultiplyIcon);
        return icon;
    }

    static const QIcon& getNotEqNullSafeIcon()
    {
        static const QIcon icon(strNotEqNullSafeIcon);
        return icon;
    }

    static const QIcon& getNotEq1Icon()
    {
        static const QIcon icon(strNotEq1Icon);
        return icon;
    }

    static const QIcon& getNotEqIcon()
    {
        static const QIcon icon(strNotEqIcon);
        return icon;
    }

    static const QIcon& getLessIcon()
    {
        static const QIcon icon(strLessIcon);
        return icon;
    }

    static const QIcon& getGreaterIcon()
    {
        static const QIcon icon(strGreaterIcon);
        return icon;
    }

    static const QIcon& getLessOrEqualIcon()
    {
        static const QIcon icon(strLessOrEqualIcon);
        return icon;
    }

    static const QIcon& getGreaterOrEqualIcon()
    {
        static const QIcon icon(strGreaterOrEqualIcon);
        return icon;
    }

    static const QIcon& getEqualIcon()
    {
        static const QIcon icon(strEqualIcon);
        return icon;
    }

    static const QIcon& getAliasIcon()
    {
        static const QIcon icon(strAliasIcon);
        return icon;
    }

    static const QIcon& getOrderIcon()
    {
        static const QIcon icon(strOrderIcon);
        return icon;
    }

    static const QIcon& getColumnIcon()
    {
        static const QIcon icon(strColumnIcon);
        return icon;
    }

    static const QIcon& getViewIcon()
    {
        static const QIcon icon(strViewsIcon);
        return icon;
    }

    static const QIcon& getConnectedDatabaseIcon()
    {
        static const QIcon icon(strConnectedDatabaseIcon);
        return icon;
    }

    static const QIcon& getDroppedDatabaseIcon()
    {
        static const QIcon icon(strDroppedDatabaseIcon);
        return icon;
    }

    static const QIcon& getDropDatabaseIcon()
    {
        static const QIcon icon(strDropDatabaseIcon);
        return icon;
    }

    static const QIcon& getUnConnectedDatabaseIcon()
    {
        static const QIcon icon(strUnConnectedDatabaseIcon);
        return icon;
    }

    static const QIcon& getForeignKeyIcon()
    {
        static const QIcon icon(strForeignKeyIcon);
        return icon;
    }

    static const QIcon& getProcedureIcon()
    {
        static const QIcon icon(strProcedureIcon);
        return icon;
    }

    static const QIcon& getFunctionTreeIcon()
    {
        static const QIcon icon(strFunctionTreeIcon);
        return icon;
    }

    static const QIcon& getTriggerUpdateIcon()
    {
        static const QIcon icon(strTriggerUpdateIcon);
        return icon;
    }

    static const QIcon& getTriggerDeleteIcon()
    {
        static const QIcon icon(strTriggerDeleteIcon);
        return icon;
    }

    static const QIcon& getTriggerInsertIcon()
    {
        static const QIcon icon(strTriggerInsertIcon);
        return icon;
    }

    static const QIcon& getTriggerAfterIcon()
    {
        static const QIcon icon(strTriggerAfterIcon);
        return icon;
    }

    static const QIcon& getTriggerBeforeIcon()
    {
        static const QIcon icon(strTriggerBeforeIcon);
        return icon;
    }

};

#endif // ICONFACTORY_H
