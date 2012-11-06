#ifndef ICONFACTORY_H
#define ICONFACTORY_H

#include "enums.h"
#include "uids.h"

#include <QIcon>

namespace icons
{

static const QString strDtTextIcon = ":/images/actions/images/small/datatypes_text.png";
static const QString strDtLockedTextIcon = ":/images/actions/images/small/datatypes_text_locked.png";
static const QString strDtFinalLockedTextIcon = ":/images/actions/images/small/datatypes_text_finallock.png";

static const QString strDtNumericIcon = ":/images/actions/images/small/datatypes_numeric.png";
static const QString strDtLockedNumericIcon = ":/images/actions/images/small/datatypes_numeric_locked.png";
static const QString strDtFinalLockedNumericIcon = ":/images/actions/images/small/datatypes_numeric_finallock.png";

static const QString strDtMiscIcon = ":/images/actions/images/small/datatypes_misc.png";
static const QString strDtLockedMiscIcon = ":/images/actions/images/small/datatypes_misc_locked.png";
static const QString strDtFinalLockedMiscIcon = ":/images/actions/images/small/datatypes_misc_finallock.png";

static const QString strDtDateTimeIcon = ":/images/actions/images/small/datatypes_datetime.png";
static const QString strDtLockedDateTimeIcon = ":/images/actions/images/small/datatypes_datetime_locked.png";
static const QString strDtFinalLockedDateTimeIcon = ":/images/actions/images/small/datatypes_datetime_finallock.png";

static const QString strDtBoolIcon = ":/images/actions/images/small/datatypes_bool.png";
static const QString strDtLockedBoolIcon = ":/images/actions/images/small/datatypes_bool_locked.png";
static const QString strDtFinalLockedBoolIcon = ":/images/actions/images/small/datatypes_bool_finallock.png";

static const QString strDtBlobIcon = ":/images/actions/images/small/datatypes_blob.png";
static const QString strDtLockedBlobIcon = ":/images/actions/images/small/datatypes_blob_locked.png";
static const QString strDtFinalLockedBlobIcon = ":/images/actions/images/small/datatypes_blob_finallock.png";

static const QString strDtSpatialIcon = ":/images/actions/images/small/datatypes_spatial.png";
static const QString strDtLockedSpatialIcon = ":/images/actions/images/small/datatypes_spatial_locked.png";
static const QString strDtFinalLockedSpatialIcon = ":/images/actions/images/small/datatypes_spatial_finallock.png";

static const QString strLockedIcon = ":/images/actions/images/small/locked.png";
static const QString strUnLockedIcon = ":/images/actions/images/small/unlocked.png";
static const QString strFinalLockedIcon = ":/images/actions/images/small/locked_final.png";

static const QString strKeyIcon = ":/images/actions/images/small/key.png";
static const QString strChangedIcon = ":/images/actions/images/small/changed.png";
static const QString strSaveIcon = ":/images/actions/images/actions/filesave.png";
static const QString strApplyIcon = ":/images/actions/images/small/apply.png";
static const QString strAddIcon = ":/images/actions/images/small/add.png";
static const QString strEmptyIcon = ":/images/actions/images/actions/empty_icon.png";
static const QString strMySqlIcon = ":/images/actions/images/small/mysql.png";

static const QString strTableIcon = ":/images/actions/images/actions/table.png";
static const QString strTableLockIcon = ":/images/actions/images/actions/table_lock.png";
static const QString strTableFinalLockIcon = ":/images/actions/images/actions/table_finallock.png";
static const QString strTablesIcon = ":/images/actions/images/actions/table.png";

static const QString strTableInstIcon = ":/images/actions/images/small/tabinst.png";
static const QString strLockedTableInstIcon = ":/images/actions/images/small/locked_tabinst.png";
static const QString strTableInstLockIcon = ":/images/actions/images/small/tabinstlock.png";
static const QString strFinalLockedTableInstIcon = ":/images/actions/images/small/tabinst_finallock.png";

static const QString strProjectOpenIcon = ":/images/actions/images/small/project_open.png";

static const QString strDiagramIcon = ":/images/actions/images/small/diagram.png";
static const QString strLockedDiagramIcon = ":/images/actions/images/small/locked_diagram.png";
static const QString strFinalLockedDiagramIcon = ":/images/actions/images/small/diagram_finallock.png";

static const QString strDataTypesIcon = ":/images/actions/images/small/datatypes.png";
static const QString strCodeIcon = ":/images/actions/images/small/code.png";
static const QString strPatchIcon = ":/images/actions/images/small/patch.png";
static const QString strSuspendedPatchIcon = ":/images/actions/images/small/shared_patch.png";
static const QString strSuspendPatchIcon = ":/images/actions/images/small/share.png";
static const QString strSqlIcon = ":/images/actions/images/small/sql.png";
static const QString strReleaseMajorVersionIcon = ":/images/actions/images/small/arrow-branch.png";

static const QString strMajorVersionIcon = ":/images/actions/images/small/arrow-000-medium.png";
static const QString strMajorVersionLockedIcon = ":/images/actions/images/small/version_locked.png";
static const QString strMajorVersionFinalLockedIcon = ":/images/actions/images/small/version_finallock.png";

static const QString strProcedureIcon = ":/images/actions/images/actions/procedure.png";
static const QString strLockedProcedureIcon = ":/images/actions/images/actions/locked_procedure.png";
static const QString strFinalLockedProcedureIcon = ":/images/actions/images/actions/procedure_finallock.png";

static const QString strFunctionTreeIcon = ":/images/actions/images/actions/function_icon.png";
static const QString strLockedFunctionTreeIcon = ":/images/actions/images/actions/locked_function_icon.png";
static const QString strFinalLockedFunctionTreeIcon = ":/images/actions/images/actions/function_finallock.png";

static const QString strRemoveIcon = ":/images/actions/images/small/remove.png";
static const QString strStringsFolder = ":/images/actions/images/big/folder_red.png";
static const QString strIntsFolder = ":/images/actions/images/big/folder_darkblue.png";
static const QString strDateTimesFolder = ":/images/actions/images/big/folder_green.png";
static const QString strBlobFolder = ":/images/actions/images/big/folder_cyan.png";
static const QString strBoolFolder = ":/images/actions/images/big/folder_yellow.png";
static const QString strMiscFolder = ":/images/actions/images/big/folder_brown.png";
static const QString strSpatialFolder = ":/images/actions/images/big/folder_orange.png";
static const QString strDatabaseIcon = ":/images/actions/images/small/database.png";
static const QString strConnectedDatabaseIcon = ":/images/actions/images/small/database_connected.png";
static const QString strDroppedDatabaseIcon = ":/images/actions/images/small/database_dropped.png";
static const QString strDropDatabaseIcon = ":/images/actions/images/small/skull.png";
static const QString strUnConnectedDatabaseIcon = ":/images/actions/images/small/database_cannot_connect.png";
static const QString strFroggieIcon = ":/bigger/images/unaf.png";
static const QString strIcon = ":/bigger/images/big/icon.png";
static const QString strIndexIcon = ":/images/actions/images/small/index.png";
static const QString strHelpIcon = ":/images/actions/images/small/help_book_3d.png";
static const QString strWarningIcon = ":/images/actions/images/small/warning.png";
static const QString strRecommendIcon = ":/images/actions/images/small/reccomend.png";

static const QString strViewIcon = ":/images/actions/images/small/view.png";
static const QString strLockedViewIcon = ":/images/actions/images/small/locked_view.png";
static const QString strFinalLockedViewIcon = ":/images/actions/images/small/view_finallock.png";
static const QString strViewsIcon = ":/images/actions/images/small/view.png";

static const QString strDeployIcon = ":/images/actions/images/actions/deploy.png";

static const QString strTriggersIcon = ":/images/actions/images/small/trigger.png";
static const QString strTriggerIcon = ":/images/actions/images/small/trigger.png";
static const QString strLockedTriggerIcon = ":/images/actions/images/small/locked_trigger.png";
static const QString strFinalLockedTriggerIcon = ":/images/actions/images/small/trigger_finallock.png";

static const QString strCloseIcon = ":/images/actions/images/actions/application_exit.png";
static const QString strWandIcon = ":/images/actions/images/small/wand.png";    // used for the distinct selection
static const QString strNotIcon = ":/images/actions/images/small/not.png";
static const QString strNegIcon = ":/images/actions/images/small/neg.png";
static const QString strMinusIcon = ":/images/actions/images/small/minus.png";
static const QString strFunctionIcon = ":/images/actions/images/small/function.png";
static const QString strBigNotIcon = ":/images/actions/images/big/big_not_icon.png";
static const QString strBigNegIcon = ":/images/actions/images/big/big_neg_icon.png";
static const QString strBigMinusIcon = ":/images/actions/images/big/big_minus_icon.png";
static const QString strParanthesesOpenIcon = ":/images/actions/images/big/parantheses_open.png";
static const QString strParanthesesCloseIcon = ":/images/actions/images/big/parantheses_close.png";
static const QString strXorIcon = ":/images/actions/images/big/big_xor.png";
static const QString strLeftShiftIcon = ":/images/actions/images/big/big_leftshift.png";
static const QString strRightShiftIcon = ":/images/actions/images/big/big_rightshift.png";
static const QString strBinaryOrIcon = ":/images/actions/images/big/big_binary_or.png";
static const QString strBinaryAndIcon = ":/images/actions/images/big/big_binary_and.png";
static const QString strPlusIcon = ":/images/actions/images/big/big_plus.png";
static const QString strDivideIcon = ":/images/actions/images/big/big_divide.png";
static const QString strModuloIcon = ":/images/actions/images/big/big_modulo.png";
static const QString strMultiplyIcon = ":/images/actions/images/big/big_multiply.png";
static const QString strEqualIcon = ":/images/actions/images/big/big_equal.png";
static const QString strGreaterOrEqualIcon = ":/images/actions/images/big/big_greater_or_equal.png";
static const QString strLessOrEqualIcon = ":/images/actions/images/big/big_less_or_equal.png";
static const QString strGreaterIcon = ":/images/actions/images/big/big_greater.png";
static const QString strLessIcon = ":/images/actions/images/big/big_less.png";
static const QString strNotEqIcon = ":/images/actions/images/big/big_noteq.png";
static const QString strNotEq1Icon = ":/images/actions/images/big/big_noteq_1.png";
static const QString strNotEqNullSafeIcon = ":/images/actions/images/big/big_noteq_nullsafe.png";
static const QString strColumnIcon = ":/images/actions/images/small/table-select-column-icon.png";
static const QString strTypeHereIcon = ":/images/actions/images/small/type_here.png";
static const QString strAliasIcon = ":/images/actions/images/small/alias.png";
static const QString strOrderIcon = ":/images/actions/images/small/edit_list_order.png";
static const QString strForeignKeyIcon = ":/images/actions/images/small/foreign_key.png";
static const QString strTriggerUpdateIcon = ":/images/actions/images/small/trigger_update.png";
static const QString strTriggerDeleteIcon = ":/images/actions/images/small/delete_table_row.png";
static const QString strTriggerInsertIcon = ":/images/actions/images/small/insert_table_row.png";
static const QString strTriggerBeforeIcon = ":/images/actions/images/small/before.png";
static const QString strTriggerAfterIcon = ":/images/actions/images/small/after.png";
static const QString strRecreateDatabaseIcon = ":/images/actions/images/small/agt_reload.png";
static const QString strConnectConnectionIcon = ":/images/actions/images/small/connect.png";
static const QString strBrowseConnectionIcon = ":/images/actions/images/small/folder_explore.png";
static const QString strEditConnectionIcon = ":/images/actions/images/small/pencil3.png";
static const QString strOpenFileIcon = ":/images/actions/images/actions/fileopen.png";
static const QString strRunQueryIcon = ":/images/actions/images/actions/button_play_green.png";
static const QString strBindToDbIcon = ":/images/actions/images/big/folder_database.png";
static const QString strUndeleteIcon = ":/images/actions/images/small/gnome_undelete.png";

}

class IconFactory
{

public:

    static const QIcon& getKeyIcon()
    {
        static const QIcon key(icons::strKeyIcon);
        return key;
    }

    static const QIcon& getSaveIcon()
    {
        static const QIcon save(icons::strSaveIcon);
        return save;
    }

    static const QIcon& getApplyIcon()
    {
        static const QIcon apply(icons::strApplyIcon);
        return apply;
    }

    static const QIcon& getAddIcon()
    {
        static const QIcon icon(icons::strAddIcon);
        return icon;
    }

    static const QIcon& getEmptyIcon()
    {
        static const QIcon icon(icons::strEmptyIcon);
        return icon;
    }

    static const QIcon& getTablesIcon()
    {
        static const QIcon icon(icons::strTablesIcon);
        return icon;
    }

    static const QIcon& getLockedTableIcon()
    {
        static const QIcon icon(icons::strTableLockIcon);
        return icon;
    }

    static const QIcon& getFinalLockedTableIcon()
    {
        static const QIcon icon(icons::strTableFinalLockIcon);
        return icon;
    }

    static const QIcon& getTableIcon()
    {
        static const QIcon icon(icons::strTableIcon);
        return icon;
    }

    static const QIcon& getNotIcon()
    {
        static const QIcon icon(icons::strNotIcon);
        return icon;
    }

    static const QIcon& getBigNotIcon()
    {
        static const QIcon icon(icons::strBigNotIcon);
        return icon;
    }

    static const QIcon& getNegIcon()
    {
        static const QIcon icon(icons::strNegIcon);
        return icon;
    }

    static const QIcon& getMinusIcon()
    {
        static const QIcon icon(icons::strMinusIcon);
        return icon;
    }

    static const QIcon& getOpenParanthesesIcon()
    {
        static const QIcon icon(icons::strParanthesesOpenIcon);
        return icon;
    }

    static const QIcon& getCloseParanthesesIcon()
    {
        static const QIcon icon(icons::strParanthesesCloseIcon);
        return icon;
    }

    static const QIcon& getFunctionIcon()
    {
        static const QIcon icon(icons::strFunctionIcon);
        return icon;
    }

    static const QIcon& getBigMinusIcon()
    {
        static const QIcon icon(icons::strBigMinusIcon);
        return icon;
    }

    static const QIcon& getBigNegIcon()
    {
        static const QIcon icon(icons::strBigNegIcon);
        return icon;
    }

    static const QIcon& getDatabaseIcon()
    {
        static const QIcon icon(icons::strDatabaseIcon);
        return icon;
    }

    static const QIcon& getTabinstIcon()
    {
        static const QIcon icon(icons::strTableInstIcon);
        return icon;
    }

    static const QIcon& getLockedTabinstIcon()
    {
        static const QIcon icon(icons::strLockedTableInstIcon);
        return icon;
    }

    static const QIcon& getFinalLockedTabinstIcon()
    {
        static const QIcon icon(icons::strFinalLockedTableInstIcon);
        return icon;
    }

    static const QIcon& getLiteralIcon()
    {
        static const QIcon icon(icons::strTypeHereIcon);
        return icon;
    }

    static const QIcon& getTabinstLockIcon()
    {
        static const QIcon icon(icons::strTableInstLockIcon);
        return icon;
    }

    static const QIcon& getDataTypesIcon()
    {
        static const QIcon icon(icons::strDataTypesIcon);
        return icon;
    }

    static const QIcon& getStringDataTypesIcon()
    {
        static const QIcon icon(icons::strStringsFolder);
        return icon;
    }

    static const QIcon& getStringDataTypeIcon()
    {
        static const QIcon icon(icons::strDtTextIcon);
        return icon;
    }

    static const QIcon& getFinalLockedStringDataTypeIcon()
    {
        static const QIcon icon(icons::strDtFinalLockedTextIcon);
        return icon;
    }

    static const QIcon& getLockedStringDataTypeIcon()
    {
        static const QIcon icon(icons::strDtLockedTextIcon);
        return icon;
    }

    static const QIcon& getIntDataTypesIcon()
    {
        static const QIcon icon(icons::strIntsFolder);
        return icon;
    }

    static const QIcon& getIntDataTypeIcon()
    {
        static const QIcon icon(icons::strDtNumericIcon);
        return icon;
    }

    static const QIcon& getLockedIntDataTypeIcon()
    {
        static const QIcon icon(icons::strDtLockedNumericIcon);
        return icon;
    }

    static const QIcon& getFinalLockedIntDataTypeIcon()
    {
        static const QIcon icon(icons::strDtFinalLockedNumericIcon);
        return icon;
    }

    static const QIcon& getBlobDataTypesIcon()
    {
        static const QIcon icon(icons::strBlobFolder);
        return icon;
    }

    static const QIcon& getLockedBlobDataTypeIcon()
    {
        static const QIcon icon(icons::strDtLockedBlobIcon);
        return icon;
    }

    static const QIcon& getFinalLockedBlobDataTypeIcon()
    {
        static const QIcon icon(icons::strDtFinalLockedBlobIcon);
        return icon;
    }

    static const QIcon& getLockedIcon()
    {
        static const QIcon icon(icons::strLockedIcon);
        return icon;
    }

    static const QIcon& getFinalLockedIcon()
    {
        static const QIcon icon(icons::strFinalLockedIcon);
        return icon;
    }

    static const QIcon& getUnLockedIcon()
    {
        static const QIcon icon(icons::strUnLockedIcon);
        return icon;
    }

    static const QIcon& getBlobDataTypeIcon()
    {
        static const QIcon icon(icons::strDtLockedBlobIcon);
        return icon;
    }

    static const QIcon& getBoolDataTypesIcon()
    {
        static const QIcon icon(icons::strBoolFolder);
        return icon;
    }

    static const QIcon& getLockedBoolDataTypeIcon()
    {
        static const QIcon icon(icons::strDtLockedBoolIcon);
        return icon;
    }

    static const QIcon& getFinalLockedBoolDataTypeIcon()
    {
        static const QIcon icon(icons::strDtFinalLockedBoolIcon);
        return icon;
    }

    static const QIcon& getBoolDataTypeIcon()
    {
        static const QIcon icon(icons::strDtBoolIcon);
        return icon;
    }

    static const QIcon& getMiscDataTypesIcon()
    {
        static const QIcon icon(icons::strMiscFolder);
        return icon;
    }

    static const QIcon& getMiscDataTypeIcon()
    {
        static const QIcon icon(icons::strDtMiscIcon);
        return icon;
    }

    static const QIcon& getLockedMiscDataTypeIcon()
    {
        static const QIcon icon(icons::strDtLockedMiscIcon);
        return icon;
    }

    static const QIcon& getFinalLockedMiscDataTypeIcon()
    {
        static const QIcon icon(icons::strDtFinalLockedMiscIcon);
        return icon;
    }

    static const QIcon& getSpatialDataTypesIcon()
    {
        static const QIcon icon(icons::strSpatialFolder);
        return icon;
    }

    static const QIcon& getSpatialDataTypeIcon()
    {
        static const QIcon icon(icons::strDtSpatialIcon);
        return icon;
    }

    static const QIcon& getFinalLockedSpatialDataTypeIcon()
    {
        static const QIcon icon(icons::strDtFinalLockedSpatialIcon);
        return icon;
    }

    static const QIcon& getLockedSpatialDataTypeIcon()
    {
        static const QIcon icon(icons::strDtLockedSpatialIcon);
        return icon;
    }

    static const QIcon& getDateTimeDataTypesIcon()
    {
        static const QIcon icon(icons::strDateTimesFolder);
        return icon;
    }

    static const QIcon& getDateTimeDataTypeIcon()
    {
        static const QIcon icon(icons::strDtDateTimeIcon);
        return icon;
    }

    static const QIcon& getLockedDateTimeDataTypeIcon()
    {
        static const QIcon icon(icons::strDtLockedDateTimeIcon);
        return icon;
    }

    static const QIcon& getFinalLockedDateTimeDataTypeIcon()
    {
        static const QIcon icon(icons::strDtFinalLockedDateTimeIcon);
        return icon;
    }

    static const QIcon& getCodeIcon()
    {
        static const QIcon icon(icons::strCodeIcon);
        return icon;
    }

    static const QIcon& getSqlIcon()
    {
        static const QIcon icon(icons::strSqlIcon);
        return icon;
    }

    static const QIcon& getVersionIcon()
    {
        static const QIcon icon(icons::strMajorVersionIcon);
        return icon;
    }

    static const QIcon& getLockedVersionIcon()
    {
        static const QIcon icon(icons::strMajorVersionLockedIcon);
        return icon;
    }

    static const QIcon& getFinalLockedVersionIcon()
    {
        static const QIcon icon(icons::strMajorVersionFinalLockedIcon);
        return icon;
    }

    static const QIcon& getRemoveIcon()
    {
        static const QIcon icon(icons::strRemoveIcon);
        return icon;
    }

    static const QIcon& getProjectOpenIcon()
    {
        static const QIcon icon(icons::strProjectOpenIcon);
        return icon;
    }

    static const QIcon& getViewsIcon()
    {
        static const QIcon icon(icons::strViewsIcon);
        return icon;
    }

    static const QIcon& getTriggersIcon()
    {
        static const QIcon icon(icons::strTriggersIcon);
        return icon;
    }

    static const QIcon& getTriggerIcon()
    {
        static const QIcon icon(icons::strTriggerIcon);
        return icon;
    }

    static const QIcon& getLockedTriggerIcon()
    {
        static const QIcon icon(icons::strLockedTriggerIcon);
        return icon;
    }

    static const QIcon& getFinalLockedTriggerIcon()
    {
        static const QIcon icon(icons::strFinalLockedTriggerIcon);
        return icon;
    }

    static const QIcon& getDiagramIcon()
    {
        static const QIcon icon(icons::strDiagramIcon);
        return icon;
    }

    static const QIcon& getLockedDiagramIcon()
    {
        static const QIcon icon(icons::strLockedDiagramIcon);
        return icon;
    }

    static const QIcon& getFinalLockedDiagramIcon()
    {
        static const QIcon icon(icons::strFinalLockedDiagramIcon);
        return icon;
    }

    static const QIcon& getIndexIcon()
    {
        static const QIcon icon(icons::strIndexIcon);
        return icon;
    }

    static const QIcon& getHelpIcon()
    {
        static const QIcon icon(icons::strHelpIcon);
        return icon;
    }

    static const QIcon& getWarningIcon()
    {
        static const QIcon icon(icons::strWarningIcon);
        return icon;
    }

    static const QIcon& getRecommendIcon()
    {
        static const QIcon icon(icons::strRecommendIcon);
        return icon;
    }

    static const QIcon& getCloseIcon()
    {
        static const QIcon icon(icons::strCloseIcon);
        return icon;
    }

    static const QIcon& getXorIcon()
    {
        static const QIcon icon(icons::strXorIcon);
        return icon;
    }

    static const QIcon& getBinaryOrIcon()
    {
        static const QIcon icon(icons::strBinaryOrIcon);
        return icon;
    }

    static const QIcon& getLeftShiftIcon()
    {
        static const QIcon icon(icons::strLeftShiftIcon);
        return icon;
    }

    static const QIcon& getRightShiftIcon()
    {
        static const QIcon icon(icons::strRightShiftIcon);
        return icon;
    }

    static const QIcon& getBinaryAndIcon()
    {
        static const QIcon icon(icons::strBinaryAndIcon);
        return icon;
    }

    static const QIcon& getPlusIcon()
    {
        static const QIcon icon(icons::strPlusIcon);
        return icon;
    }

    static const QIcon& getDivideIcon()
    {
        static const QIcon icon(icons::strDivideIcon);
        return icon;
    }

    static const QIcon& getModuloIcon()
    {
        static const QIcon icon(icons::strModuloIcon);
        return icon;
    }

    static const QIcon& getMultiplyIcon()
    {
        static const QIcon icon(icons::strMultiplyIcon);
        return icon;
    }

    static const QIcon& getNotEqNullSafeIcon()
    {
        static const QIcon icon(icons::strNotEqNullSafeIcon);
        return icon;
    }

    static const QIcon& getNotEq1Icon()
    {
        static const QIcon icon(icons::strNotEq1Icon);
        return icon;
    }

    static const QIcon& getNotEqIcon()
    {
        static const QIcon icon(icons::strNotEqIcon);
        return icon;
    }

    static const QIcon& getLessIcon()
    {
        static const QIcon icon(icons::strLessIcon);
        return icon;
    }

    static const QIcon& getGreaterIcon()
    {
        static const QIcon icon(icons::strGreaterIcon);
        return icon;
    }

    static const QIcon& getLessOrEqualIcon()
    {
        static const QIcon icon(icons::strLessOrEqualIcon);
        return icon;
    }

    static const QIcon& getGreaterOrEqualIcon()
    {
        static const QIcon icon(icons::strGreaterOrEqualIcon);
        return icon;
    }

    static const QIcon& getEqualIcon()
    {
        static const QIcon icon(icons::strEqualIcon);
        return icon;
    }

    static const QIcon& getAliasIcon()
    {
        static const QIcon icon(icons::strAliasIcon);
        return icon;
    }

    static const QIcon& getOrderIcon()
    {
        static const QIcon icon(icons::strOrderIcon);
        return icon;
    }

    static const QIcon& getColumnIcon()
    {
        static const QIcon icon(icons::strColumnIcon);
        return icon;
    }

    static const QIcon& getViewIcon()
    {
        static const QIcon icon(icons::strViewIcon);
        return icon;
    }

    static const QIcon& getLockedViewIcon()
    {
        static const QIcon icon(icons::strLockedViewIcon);
        return icon;
    }

    static const QIcon& getFinalLockedViewIcon()
    {
        static const QIcon icon(icons::strFinalLockedViewIcon);
        return icon;
    }

    static const QIcon& getConnectedDatabaseIcon()
    {
        static const QIcon icon(icons::strConnectedDatabaseIcon);
        return icon;
    }

    static const QIcon& getConnectConnectionIcon()
    {
        static const QIcon icon(icons::strConnectConnectionIcon);
        return icon;
    }

    static const QIcon& getBrowseConnectionIcon()
    {
        static const QIcon icon(icons::strBrowseConnectionIcon);
        return icon;
    }

    static const QIcon& getEditConnectionIcon()
    {
        static const QIcon icon(icons::strEditConnectionIcon);
        return icon;
    }

    static const QIcon& getDroppedDatabaseIcon()
    {
        static const QIcon icon(icons::strDroppedDatabaseIcon);
        return icon;
    }

    static const QIcon& getDropDatabaseIcon()
    {
        static const QIcon icon(icons::strDropDatabaseIcon);
        return icon;
    }

    static const QIcon& getUnConnectedDatabaseIcon()
    {
        static const QIcon icon(icons::strUnConnectedDatabaseIcon);
        return icon;
    }

    static const QIcon& getForeignKeyIcon()
    {
        static const QIcon icon(icons::strForeignKeyIcon);
        return icon;
    }

    static const QIcon& getProcedureIcon()
    {
        static const QIcon icon(icons::strProcedureIcon);
        return icon;
    }

    static const QIcon& getLockedProcedureIcon()
    {
        static const QIcon icon(icons::strLockedProcedureIcon);
        return icon;
    }

    static const QIcon& getFinalLockedProcedureIcon()
    {
        static const QIcon icon(icons::strFinalLockedProcedureIcon);
        return icon;
    }

    static const QIcon& getProceduresIcon()
    {
        static const QIcon icon(icons::strProcedureIcon);
        return icon;
    }

    static const QIcon& getRecreateDatabaseIcon()
    {
        static const QIcon icon(icons::strRecreateDatabaseIcon);
        return icon;
    }

    static const QIcon& getFunctionTreeIcon()
    {
        static const QIcon icon(icons::strFunctionTreeIcon);
        return icon;
    }

    static const QIcon& getLockedFunctionTreeIcon()
    {
        static const QIcon icon(icons::strLockedFunctionTreeIcon);
        return icon;
    }

    static const QIcon& getFinalLockedFunctionTreeIcon()
    {
        static const QIcon icon(icons::strFinalLockedFunctionTreeIcon);
        return icon;
    }

    static const QIcon& getFunctionsTreeIcon()
    {
        static const QIcon icon(icons::strFunctionTreeIcon);
        return icon;
    }

    static const QIcon& getTriggerUpdateIcon()
    {
        static const QIcon icon(icons::strTriggerUpdateIcon);
        return icon;
    }

    static const QIcon& getTriggerDeleteIcon()
    {
        static const QIcon icon(icons::strTriggerDeleteIcon);
        return icon;
    }

    static const QIcon& getTriggerInsertIcon()
    {
        static const QIcon icon(icons::strTriggerInsertIcon);
        return icon;
    }

    static const QIcon& getTriggerAfterIcon()
    {
        static const QIcon icon(icons::strTriggerAfterIcon);
        return icon;
    }

    static const QIcon& getTriggerBeforeIcon()
    {
        static const QIcon icon(icons::strTriggerBeforeIcon);
        return icon;
    }

    static const QIcon& getOpenFileIcon()
    {
        static const QIcon icon(icons::strOpenFileIcon);
        return icon;
    }

    static const QIcon& getBindToDbIcon()
    {
        static const QIcon icon(icons::strBindToDbIcon);
        return icon;
    }

    static const QIcon& getRunQueryIcon()
    {
        static const QIcon icon(icons::strRunQueryIcon);
        return icon;
    }

    static const QIcon& getReleaseMajorVersionIcon()
    {
        static const QIcon icon(icons::strReleaseMajorVersionIcon);
        return icon;
    }

    static const QIcon& getMySqlIcon()
    {
        static const QIcon icon(icons::strMySqlIcon);
        return icon;
    }

    static const QIcon& getChangedIcon()
    {
        static const QIcon icon(icons::strChangedIcon);
        return icon;
    }

    static const QIcon& getPatchIcon()
    {
        static const QIcon icon(icons::strPatchIcon);
        return icon;
    }

    static const QIcon& getSuspendPatchIcon()
    {
        static const QIcon icon(icons::strSuspendPatchIcon);
        return icon;
    }

    static const QIcon& getDeployIcon()
    {
        static const QIcon icon(icons::strDeployIcon);
        return icon;
    }

    static const QIcon& getSuspendedPatchIcon()
    {
        static const QIcon icon(icons::strSuspendedPatchIcon);
        return icon;
    }

    static const QIcon& getAllDataTypes()
    {
        static const QIcon icon(icons::strIcon);
        return icon;
    }

    static const QIcon& getUndeleteIcon()
    {
        static const QIcon key(icons::strUndeleteIcon);
        return key;
    }

    static const QIcon& getIconForClassUid(const QString& uid)
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
    }

    static QIcon getIconForDataType(DT_TYPE dt)
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
        case DT_INVALID:
        case DT_GENERIC:
        default:
            return getEmptyIcon();
        }

        return getEmptyIcon();
    }

    static QIcon getFinalLockedIconForDataType(DT_TYPE dt)
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
        default:
            return getEmptyIcon();
        }

        return getEmptyIcon();
    }

    static QIcon getLockedIconForDataType(DT_TYPE dt)
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

    static QIcon getConnectionStateIcon(ConnectionState c)
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


};

#endif // ICONFACTORY_H
