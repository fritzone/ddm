#include "IconFactory.h"

const QIcon& IconFactory::getIconForActionId(const QString& id)
{
    if(id == "QUERY") return getSqlIcon();
    if(id == "DATABASE") return getDatabaseIcon();
    if(id == "DROP") return getDropDatabaseIcon();
    if(id == "RECREATE") return getRecreateDatabaseIcon();
    if(id == "BROWSE") return getBrowseConnectionIcon();
    if(id == "REFRESH") return getRefreshIcon();
    if(id == "NEW") return getNewIcon();
    if(id == "NEWTABLE") return getTableIcon();
    if(id == "DELETE") return getRemoveIcon();
    if(id == "EDIT") return getEditConnectionIcon();

    static const QIcon icon(icons::strEmptyIcon);
    return icon;
}

const QIcon &IconFactory::getRepoEntityIcon()
{
    static const QIcon icon(icons::strRepositoryEntity);
    return icon;
}

const QIcon &IconFactory::getRepoDatabasesIcon()
{
    static const QIcon icon(icons::strRepositoryDatabases);
    return icon;
}

const QIcon &IconFactory::getRefreshIcon()
{
    static const QIcon icon(icons::strRefreshIcon);
    return icon;
}

const QIcon &IconFactory::getRepoRoleIcon()
{
    static const QIcon icon(icons::strRepositoryRole);
    return icon;

}

const QIcon &IconFactory::getKeyIcon()
{
    static const QIcon key(icons::strKeyIcon);
    return key;
}

const QIcon &IconFactory::getSaveIcon()
{
    static const QIcon save(icons::strSaveIcon);
    return save;
}

const QIcon &IconFactory::getNewIcon()
{
    static const QIcon save(icons::strNewIcon);
    return save;
}

const QIcon &IconFactory::getApplyIcon()
{
    static const QIcon apply(icons::strApplyIcon);
    return apply;
}

const QIcon &IconFactory::getAddIcon()
{
    static const QIcon icon(icons::strAddIcon);
    return icon;
}

const QIcon &IconFactory::getEmptyIcon()
{
    static const QIcon icon(icons::strEmptyIcon);
    return icon;
}

const QIcon &IconFactory::getTablesIcon()
{
    static const QIcon icon(icons::strTablesIcon);
    return icon;
}

const QIcon &IconFactory::getLockedTableIcon()
{
    static const QIcon icon(icons::strTableLockIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedTableIcon()
{
    static const QIcon icon(icons::strTableFinalLockIcon);
    return icon;
}

const QIcon &IconFactory::getTableIcon()
{
    static const QIcon icon(icons::strTableIcon);
    return icon;
}

const QIcon &IconFactory::getNotIcon()
{
    static const QIcon icon(icons::strNotIcon);
    return icon;
}

const QIcon &IconFactory::getBigNotIcon()
{
    static const QIcon icon(icons::strBigNotIcon);
    return icon;
}

const QIcon &IconFactory::getNegIcon()
{
    static const QIcon icon(icons::strNegIcon);
    return icon;
}

const QIcon &IconFactory::getMinusIcon()
{
    static const QIcon icon(icons::strMinusIcon);
    return icon;
}

const QIcon &IconFactory::getOpenParanthesesIcon()
{
    static const QIcon icon(icons::strParanthesesOpenIcon);
    return icon;
}

const QIcon &IconFactory::getCloseParanthesesIcon()
{
    static const QIcon icon(icons::strParanthesesCloseIcon);
    return icon;
}

const QIcon &IconFactory::getFunctionIcon()
{
    static const QIcon icon(icons::strFunctionIcon);
    return icon;
}

const QIcon &IconFactory::getBigMinusIcon()
{
    static const QIcon icon(icons::strBigMinusIcon);
    return icon;
}

const QIcon &IconFactory::getBigNegIcon()
{
    static const QIcon icon(icons::strBigNegIcon);
    return icon;
}

const QIcon &IconFactory::getDatabaseIcon()
{
    static const QIcon icon(icons::strDatabaseIcon);
    return icon;
}

const QIcon &IconFactory::getTabinstIcon()
{
    static const QIcon icon(icons::strTableInstIcon);
    return icon;
}

const QIcon &IconFactory::getLockedTabinstIcon()
{
    static const QIcon icon(icons::strLockedTableInstIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedTabinstIcon()
{
    static const QIcon icon(icons::strFinalLockedTableInstIcon);
    return icon;
}

const QIcon &IconFactory::getLiteralIcon()
{
    static const QIcon icon(icons::strTypeHereIcon);
    return icon;
}

const QIcon &IconFactory::getTabinstLockIcon()
{
    static const QIcon icon(icons::strTableInstLockIcon);
    return icon;
}

const QIcon &IconFactory::getDataTypesIcon()
{
    static const QIcon icon(icons::strDataTypesIcon);
    return icon;
}

const QIcon &IconFactory::getStringDataTypesIcon()
{
    static const QIcon icon(icons::strStringsFolder);
    return icon;
}

const QIcon &IconFactory::getStringDataTypeIcon()
{
    static const QIcon icon(icons::strDtTextIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedStringDataTypeIcon()
{
    static const QIcon icon(icons::strDtFinalLockedTextIcon);
    return icon;
}

const QIcon &IconFactory::getLockedStringDataTypeIcon()
{
    static const QIcon icon(icons::strDtLockedTextIcon);
    return icon;
}

const QIcon &IconFactory::getIntDataTypesIcon()
{
    static const QIcon icon(icons::strIntsFolder);
    return icon;
}

const QIcon &IconFactory::getIntDataTypeIcon()
{
    static const QIcon icon(icons::strDtNumericIcon);
    return icon;
}

const QIcon &IconFactory::getLockedIntDataTypeIcon()
{
    static const QIcon icon(icons::strDtLockedNumericIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedIntDataTypeIcon()
{
    static const QIcon icon(icons::strDtFinalLockedNumericIcon);
    return icon;
}

const QIcon &IconFactory::getBlobDataTypesIcon()
{
    static const QIcon icon(icons::strBlobFolder);
    return icon;
}

const QIcon &IconFactory::getLockedBlobDataTypeIcon()
{
    static const QIcon icon(icons::strDtLockedBlobIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedBlobDataTypeIcon()
{
    static const QIcon icon(icons::strDtFinalLockedBlobIcon);
    return icon;
}

const QIcon &IconFactory::getLockedIcon()
{
    static const QIcon icon(icons::strLockedIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedIcon()
{
    static const QIcon icon(icons::strFinalLockedIcon);
    return icon;
}

const QIcon &IconFactory::getUnLockedIcon()
{
    static const QIcon icon(icons::strUnLockedIcon);
    return icon;
}

const QIcon &IconFactory::getBlobDataTypeIcon()
{
    static const QIcon icon(icons::strDtBlobIcon);
    return icon;
}

const QIcon &IconFactory::getBoolDataTypesIcon()
{
    static const QIcon icon(icons::strBoolFolder);
    return icon;
}

const QIcon &IconFactory::getLockedBoolDataTypeIcon()
{
    static const QIcon icon(icons::strDtLockedBoolIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedBoolDataTypeIcon()
{
    static const QIcon icon(icons::strDtFinalLockedBoolIcon);
    return icon;
}

const QIcon &IconFactory::getBoolDataTypeIcon()
{
    static const QIcon icon(icons::strDtBoolIcon);
    return icon;
}

const QIcon &IconFactory::getMiscDataTypesIcon()
{
    static const QIcon icon(icons::strMiscFolder);
    return icon;
}

const QIcon &IconFactory::getMiscDataTypeIcon()
{
    static const QIcon icon(icons::strDtMiscIcon);
    return icon;
}

const QIcon &IconFactory::getLockedMiscDataTypeIcon()
{
    static const QIcon icon(icons::strDtLockedMiscIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedMiscDataTypeIcon()
{
    static const QIcon icon(icons::strDtFinalLockedMiscIcon);
    return icon;
}

const QIcon &IconFactory::getSpatialDataTypesIcon()
{
    static const QIcon icon(icons::strSpatialFolder);
    return icon;
}

const QIcon &IconFactory::getSpatialDataTypeIcon()
{
    static const QIcon icon(icons::strDtSpatialIcon);
    return icon;
}

const QIcon &IconFactory::getGenericDataTypeIcon()
{
    static const QIcon icon(icons::strDtGenericIcon);
    return icon;
}

const QIcon &IconFactory::getVariableDataTypeIcon()
{
    static const QIcon icon(icons::strDtVariableIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedSpatialDataTypeIcon()
{
    static const QIcon icon(icons::strDtFinalLockedSpatialIcon);
    return icon;
}

const QIcon &IconFactory::getLockedSpatialDataTypeIcon()
{
    static const QIcon icon(icons::strDtLockedSpatialIcon);
    return icon;
}

const QIcon &IconFactory::getDateTimeDataTypesIcon()
{
    static const QIcon icon(icons::strDateTimesFolder);
    return icon;
}

const QIcon &IconFactory::getDateTimeDataTypeIcon()
{
    static const QIcon icon(icons::strDtDateTimeIcon);
    return icon;
}

const QIcon &IconFactory::getLockedDateTimeDataTypeIcon()
{
    static const QIcon icon(icons::strDtLockedDateTimeIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedDateTimeDataTypeIcon()
{
    static const QIcon icon(icons::strDtFinalLockedDateTimeIcon);
    return icon;
}

const QIcon &IconFactory::getCodeIcon()
{
    static const QIcon icon(icons::strCodeIcon);
    return icon;
}

const QIcon &IconFactory::getSqlIcon()
{
    static const QIcon icon(icons::strSqlIcon);
    return icon;
}

const QIcon &IconFactory::getVersionIcon()
{
    static const QIcon icon(icons::strMajorVersionIcon);
    return icon;
}

const QIcon &IconFactory::getLockedVersionIcon()
{
    static const QIcon icon(icons::strMajorVersionLockedIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedVersionIcon()
{
    static const QIcon icon(icons::strMajorVersionFinalLockedIcon);
    return icon;
}

const QIcon &IconFactory::getRemoveIcon()
{
    static const QIcon icon(icons::strRemoveIcon);
    return icon;
}

const QIcon &IconFactory::getProjectOpenIcon()
{
    static const QIcon icon(icons::strProjectOpenIcon);
    return icon;
}

const QIcon &IconFactory::getViewsIcon()
{
    static const QIcon icon(icons::strViewsIcon);
    return icon;
}

const QIcon &IconFactory::getTriggersIcon()
{
    static const QIcon icon(icons::strTriggersIcon);
    return icon;
}

const QIcon &IconFactory::getTriggerIcon()
{
    static const QIcon icon(icons::strTriggerIcon);
    return icon;
}

const QIcon &IconFactory::getLockedTriggerIcon()
{
    static const QIcon icon(icons::strLockedTriggerIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedTriggerIcon()
{
    static const QIcon icon(icons::strFinalLockedTriggerIcon);
    return icon;
}

const QIcon &IconFactory::getDiagramIcon()
{
    static const QIcon icon(icons::strDiagramIcon);
    return icon;
}

const QIcon &IconFactory::getLockedDiagramIcon()
{
    static const QIcon icon(icons::strLockedDiagramIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedDiagramIcon()
{
    static const QIcon icon(icons::strFinalLockedDiagramIcon);
    return icon;
}

const QIcon &IconFactory::getIndexIcon()
{
    static const QIcon icon(icons::strIndexIcon);
    return icon;
}

const QIcon &IconFactory::getHelpIcon()
{
    static const QIcon icon(icons::strHelpIcon);
    return icon;
}

const QIcon &IconFactory::getWarningIcon()
{
    static const QIcon icon(icons::strWarningIcon);
    return icon;
}

const QIcon &IconFactory::getRecommendIcon()
{
    static const QIcon icon(icons::strRecommendIcon);
    return icon;
}

const QIcon &IconFactory::getCloseIcon()
{
    static const QIcon icon(icons::strCloseIcon);
    return icon;
}

const QIcon &IconFactory::getXorIcon()
{
    static const QIcon icon(icons::strXorIcon);
    return icon;
}

const QIcon &IconFactory::getBinaryOrIcon()
{
    static const QIcon icon(icons::strBinaryOrIcon);
    return icon;
}

const QIcon &IconFactory::getLeftShiftIcon()
{
    static const QIcon icon(icons::strLeftShiftIcon);
    return icon;
}

const QIcon &IconFactory::getRightShiftIcon()
{
    static const QIcon icon(icons::strRightShiftIcon);
    return icon;
}

const QIcon &IconFactory::getBinaryAndIcon()
{
    static const QIcon icon(icons::strBinaryAndIcon);
    return icon;
}

const QIcon &IconFactory::getPlusIcon()
{
    static const QIcon icon(icons::strPlusIcon);
    return icon;
}

const QIcon &IconFactory::getDivideIcon()
{
    static const QIcon icon(icons::strDivideIcon);
    return icon;
}

const QIcon &IconFactory::getModuloIcon()
{
    static const QIcon icon(icons::strModuloIcon);
    return icon;
}

const QIcon &IconFactory::getMultiplyIcon()
{
    static const QIcon icon(icons::strMultiplyIcon);
    return icon;
}

const QIcon &IconFactory::getNotEqNullSafeIcon()
{
    static const QIcon icon(icons::strNotEqNullSafeIcon);
    return icon;
}

const QIcon &IconFactory::getNotEq1Icon()
{
    static const QIcon icon(icons::strNotEq1Icon);
    return icon;
}

const QIcon &IconFactory::getNotEqIcon()
{
    static const QIcon icon(icons::strNotEqIcon);
    return icon;
}

const QIcon &IconFactory::getLessIcon()
{
    static const QIcon icon(icons::strLessIcon);
    return icon;
}

const QIcon &IconFactory::getGreaterIcon()
{
    static const QIcon icon(icons::strGreaterIcon);
    return icon;
}

const QIcon &IconFactory::getLessOrEqualIcon()
{
    static const QIcon icon(icons::strLessOrEqualIcon);
    return icon;
}

const QIcon &IconFactory::getGreaterOrEqualIcon()
{
    static const QIcon icon(icons::strGreaterOrEqualIcon);
    return icon;
}

const QIcon &IconFactory::getEqualIcon()
{
    static const QIcon icon(icons::strEqualIcon);
    return icon;
}

const QIcon &IconFactory::getAliasIcon()
{
    static const QIcon icon(icons::strAliasIcon);
    return icon;
}

const QIcon &IconFactory::getOrderIcon()
{
    static const QIcon icon(icons::strOrderIcon);
    return icon;
}

const QIcon &IconFactory::getColumnIcon()
{
    static const QIcon icon(icons::strColumnIcon);
    return icon;
}

const QIcon &IconFactory::getViewIcon()
{
    static const QIcon icon(icons::strViewIcon);
    return icon;
}

const QIcon &IconFactory::getLockedViewIcon()
{
    static const QIcon icon(icons::strLockedViewIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedViewIcon()
{
    static const QIcon icon(icons::strFinalLockedViewIcon);
    return icon;
}

const QIcon &IconFactory::getConnectedDatabaseIcon()
{
    static const QIcon icon(icons::strConnectedDatabaseIcon);
    return icon;
}

const QIcon &IconFactory::getConnectConnectionIcon()
{
    static const QIcon icon(icons::strConnectConnectionIcon);
    return icon;
}

const QIcon &IconFactory::getBrowseConnectionIcon()
{
    static const QIcon icon(icons::strBrowseConnectionIcon);
    return icon;
}

const QIcon &IconFactory::getEditConnectionIcon()
{
    static const QIcon icon(icons::strEditConnectionIcon);
    return icon;
}

const QIcon &IconFactory::getDroppedDatabaseIcon()
{
    static const QIcon icon(icons::strDroppedDatabaseIcon);
    return icon;
}

const QIcon &IconFactory::getDropDatabaseIcon()
{
    static const QIcon icon(icons::strDropDatabaseIcon);
    return icon;
}

const QIcon &IconFactory::getUnConnectedDatabaseIcon()
{
    static const QIcon icon(icons::strUnConnectedDatabaseIcon);
    return icon;
}

const QIcon &IconFactory::getForeignKeyIcon()
{
    static const QIcon icon(icons::strForeignKeyIcon);
    return icon;
}

const QIcon &IconFactory::getProcedureIcon()
{
    static const QIcon icon(icons::strProcedureIcon);
    return icon;
}

const QIcon &IconFactory::getLockedProcedureIcon()
{
    static const QIcon icon(icons::strLockedProcedureIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedProcedureIcon()
{
    static const QIcon icon(icons::strFinalLockedProcedureIcon);
    return icon;
}

const QIcon &IconFactory::getProceduresIcon()
{
    static const QIcon icon(icons::strProcedureIcon);
    return icon;
}

const QIcon &IconFactory::getRecreateDatabaseIcon()
{
    static const QIcon icon(icons::strRecreateDatabaseIcon);
    return icon;
}

const QIcon &IconFactory::getFunctionTreeIcon()
{
    static const QIcon icon(icons::strFunctionTreeIcon);
    return icon;
}

const QIcon &IconFactory::getLockedFunctionTreeIcon()
{
    static const QIcon icon(icons::strLockedFunctionTreeIcon);
    return icon;
}

const QIcon &IconFactory::getFinalLockedFunctionTreeIcon()
{
    static const QIcon icon(icons::strFinalLockedFunctionTreeIcon);
    return icon;
}

const QIcon &IconFactory::getFunctionsTreeIcon()
{
    static const QIcon icon(icons::strFunctionTreeIcon);
    return icon;
}

const QIcon &IconFactory::getTriggerUpdateIcon()
{
    static const QIcon icon(icons::strTriggerUpdateIcon);
    return icon;
}

const QIcon &IconFactory::getTriggerDeleteIcon()
{
    static const QIcon icon(icons::strTriggerDeleteIcon);
    return icon;
}

const QIcon &IconFactory::getTriggerInsertIcon()
{
    static const QIcon icon(icons::strTriggerInsertIcon);
    return icon;
}

const QIcon &IconFactory::getInsertRowIcon()
{
    static const QIcon icon(icons::strTriggerInsertRecordIcon);
    return icon;
}

const QIcon &IconFactory::getTriggerAfterIcon()
{
    static const QIcon icon(icons::strTriggerAfterIcon);
    return icon;
}

const QIcon &IconFactory::getTriggerBeforeIcon()
{
    static const QIcon icon(icons::strTriggerBeforeIcon);
    return icon;
}

const QIcon &IconFactory::getOpenFileIcon()
{
    static const QIcon icon(icons::strOpenFileIcon);
    return icon;
}

const QIcon &IconFactory::getBindToDbIcon()
{
    static const QIcon icon(icons::strBindToDbIcon);
    return icon;
}

const QIcon &IconFactory::getRunQueryIcon()
{
    static const QIcon icon(icons::strRunQueryIcon);
    return icon;
}

const QIcon &IconFactory::getReleaseMajorVersionIcon()
{
    static const QIcon icon(icons::strReleaseMajorVersionIcon);
    return icon;
}

const QIcon &IconFactory::getMySqlIcon()
{
    static const QIcon icon(icons::strMySqlIcon);
    return icon;
}

const QIcon &IconFactory::getSqliteIcon()
{
    static const QIcon icon(icons::strSqliteIcon);
    return icon;
}

const QIcon &IconFactory::getCUBRIDIcon()
{
    static const QIcon icon(icons::strCUBRIDIcon);
    return icon;
}

const QIcon &IconFactory::getPostgresIcon()
{
    static const QIcon icon(icons::strPostgresIcon);
    return icon;
}

const QIcon &IconFactory::getIconForDatabase(const QString &db)
{
    if(db.toUpper() == strMySql.toUpper()) return getMySqlIcon();
    if(db.toUpper() == strSqlite.toUpper()) return getSqliteIcon();
    if(db.toUpper() == strCUBRID.toUpper()) return getCUBRIDIcon();
    if(db.toUpper() == strPostgres.toUpper()) return getPostgresIcon();

    return getEmptyIcon();
}

const QIcon &IconFactory::getChangedIcon()
{
    static const QIcon icon(icons::strChangedIcon);
    return icon;
}

const QIcon &IconFactory::getPatchIcon()
{
    static const QIcon icon(icons::strPatchIcon);
    return icon;
}

const QIcon &IconFactory::getSuspendPatchIcon()
{
    static const QIcon icon(icons::strSuspendPatchIcon);
    return icon;
}

const QIcon &IconFactory::getDeployIcon()
{
    static const QIcon icon(icons::strDeployIcon);
    return icon;
}

const QIcon &IconFactory::getSuspendedPatchIcon()
{
    static const QIcon icon(icons::strSuspendedPatchIcon);
    return icon;
}

const QIcon &IconFactory::getAllDataTypes()
{
    static const QIcon icon(icons::strIcon);
    return icon;
}

const QIcon &IconFactory::getUndeleteIcon()
{
    static const QIcon key(icons::strUndeleteIcon);
    return key;
}

const QIcon &IconFactory::getIconForClassUid(const QString &uid)
{
    QString cuid = uid.toUpper();
    if(cuid == uidTable) return getTableIcon();
    if(cuid == uidTableInstance) return getTabinstIcon();
    if(cuid == uidDiagram) return getDiagramIcon();
    if(cuid == uidProcedure) return getProcedureIcon();
    if(cuid == uidFunction) return getFunctionTreeIcon();
    if(cuid == uidView) return getViewIcon();
    if(cuid == uidTrigger) return getTriggerIcon();

    if(cuid == uidNumericDT) return getIntDataTypeIcon();
    if(cuid == uidStringDT) return getStringDataTypeIcon();
    if(cuid == uidDateTimeDT) return getDateTimeDataTypeIcon();
    if(cuid == uidBooleanDT) return getBoolDataTypeIcon();
    if(cuid == uidBlobDT) return getBlobDataTypeIcon();
    if(cuid == uidMiscDT) return getMiscDataTypeIcon();
    if(cuid == uidSpatialDT) return getSpatialDataTypeIcon();

    return getEmptyIcon();
}

QIcon IconFactory::getIconForDataType(DT_TYPE dt)
{
    switch(dt)
    {
    case DT_STRING: return getStringDataTypeIcon();
    case DT_NUMERIC: return getIntDataTypeIcon();
    case DT_BOOLEAN: return getBoolDataTypeIcon();
    case DT_BLOB: return getBlobDataTypeIcon();
    case DT_DATETIME: return getDateTimeDataTypeIcon();
    case DT_MISC: return getMiscDataTypeIcon();
    case DT_SPATIAL: return getSpatialDataTypeIcon();
    case DT_GENERIC: return getGenericDataTypeIcon();
    case DT_VARIABLE: return getVariableDataTypeIcon();
    case DT_INVALID:

    default:
        return getEmptyIcon();
    }

    return getEmptyIcon();
}

QIcon IconFactory::getFinalLockedIconForDataType(DT_TYPE dt)
{
    switch(dt)
    {
    case DT_STRING: return getFinalLockedStringDataTypeIcon();
    case DT_NUMERIC: return getFinalLockedIntDataTypeIcon();
    case DT_BOOLEAN: return getFinalLockedBoolDataTypeIcon();
    case DT_BLOB: return getFinalLockedBlobDataTypeIcon();
    case DT_DATETIME: return getFinalLockedDateTimeDataTypeIcon();
    case DT_MISC: return getFinalLockedMiscDataTypeIcon();
    case DT_SPATIAL: return getFinalLockedSpatialDataTypeIcon();
    case DT_INVALID:
    case DT_GENERIC:
    case DT_VARIABLE:
    default:
        return getEmptyIcon();
    }

    return getEmptyIcon();
}

QIcon IconFactory::getLockedIconForDataType(DT_TYPE dt)
{
    switch(dt)
    {
    case DT_STRING: return getLockedStringDataTypeIcon();
    case DT_NUMERIC: return getLockedIntDataTypeIcon();
    case DT_BOOLEAN: return getLockedBoolDataTypeIcon();
    case DT_BLOB: return getLockedBlobDataTypeIcon();
    case DT_DATETIME: return getLockedDateTimeDataTypeIcon();
    case DT_MISC: return getLockedMiscDataTypeIcon();
    case DT_SPATIAL: return getLockedSpatialDataTypeIcon();
    case DT_INVALID:
    case DT_GENERIC:
    default:
        return getEmptyIcon();
    }

    return getEmptyIcon();
}

QIcon IconFactory::getConnectionStateIcon(ConnectionState c)
{
    switch(c)
    {
    case DID_NOT_TRY:
    case UNDEFINED:
        return (getDatabaseIcon());
        break;
    case FAILED:
        return (getUnConnectedDatabaseIcon());
        break;
    case CONNECTED:
        return (getConnectedDatabaseIcon());
        break;
    case DROPPED:
    case DELETED:
        return (getDroppedDatabaseIcon());
        break;
    }
    return getEmptyIcon();
}

QIcon IconFactory::getIconForUid(const QString &uid)
{
    if(uid == uidFunction) return getFunctionTreeIcon();

    return getEmptyIcon();
}
