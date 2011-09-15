#ifndef STRINGS_H
#define STRINGS_H

#include <QString>

static const QString strTextString = "Text/String";
static const QString strNumeric = "Numeric";
static const QString strBoolean = "Boolean";
static const QString strDateTime = "Date/Time";
static const QString strBlob = "Blob/Binary";
static const QString strMisc = "Misc";
static const QString strSpatial = "Spatial";
static const QString strNA = "N/A";

static const QString strDtTextIcon = ":/images/actions/images/small/datatypes_text.PNG";
static const QString strDtNumericIcon = ":/images/actions/images/small/datatypes_numeric.PNG";
static const QString strDtMiscIcon = ":/images/actions/images/small/datatypes_misc.PNG";
static const QString strDtDateTimeIcon = ":/images/actions/images/small/datatypes_datetime.PNG";
static const QString strDtBoolIcon = ":/images/actions/images/small/datatypes_bool.PNG";
static const QString strDtBlobIcon = ":/images/actions/images/small/datatypes_blob.PNG";
static const QString strDtSpatialIcon = ":/images/actions/images/small/datatypes_spatial.PNG";

static const QString strKeyIcon = ":/images/actions/images/small/key.png";
static const QString strSaveIcon = ":/images/actions/images/actions/filesave.png";
static const QString strApplyIcon = ":/images/actions/images/small/apply.png";
static const QString strAddIcon = ":/images/actions/images/small/add.png";
static const QString strEmptyIcon = ":/images/actions/images/actions/empty_icon.png";
static const QString strTablesIcon = ":/images/actions/images/actions/table.png";
static const QString strTableInstIcon = ":/images/actions/images/small/tabinst.png";
static const QString strTableInstLockIcon = ":/images/actions/images/small/tabinstlock.png";
static const QString strProjectOpenIcon = ":/images/actions/images/small/project_open.png";
static const QString strDiagramIcon = ":/images/actions/images/small/diagram.png";
static const QString strDataTypesIcon = ":/images/actions/images/small/datatypes.PNG";
static const QString strCodeIcon = ":/images/actions/images/small/code.png";
static const QString strSqlIcon = ":/images/actions/images/small/sql.png";
static const QString strVersionIcon = ":/images/actions/images/small/version.PNG";
static const QString strRemoveIcon = ":/images/actions/images/small/remove.png";
static const QString strStringsFolder = ":/images/actions/images/big/folder_red.png";
static const QString strIntsFolder = ":/images/actions/images/big/folder_darkblue.png";
static const QString strDateTimesFolder = ":/images/actions/images/big/folder_green.png";
static const QString strBlobFolder = ":/images/actions/images/big/folder_cyan.png";
static const QString strBoolFolder = ":/images/actions/images/big/folder_yellow.png";
static const QString strMiscFolder = ":/images/actions/images/big/folder_brown.png";
static const QString strSpatialFolder = ":/images/actions/images/big/folder_orange.png";
static const QString strDatabaseIcon = ":/images/actions/images/small/database.png";
static const QString strFroggieIcon = ":/bigger/images/unaf.png";
static const QString strIcon = ":/bigger/images/big/icon.png";
static const QString strIndexIcon = ":/images/actions/images/small/index.png";
static const QString strWarningIcon = ":/images/actions/images/small/warning.png";
static const QString strRecommendIcon = ":/images/actions/images/small/reccomend.png";
static const QString strViewsIcon = ":/images/actions/images/small/view.png";
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

static const QString NEW_TABLE = "New table reference";
static const QString NEW_COLUMN = "New column";
static const QString DUPLICATE = "Duplicate";
static const QString ADD_FROM = "FROM...";
static const QString ADD_WHERE = "WHERE...";
static const QString NEW_SUBQUERY = "New subquery";
static const QString ADD_GROUPBY = "GROUP BY...";
static const QString ADD_HAVING = "HAVING...";
static const QString ADD_ORDERBY = "ORDER BY...";
static const QString ADD_JOIN = "JOIN ...";
static const QString ADD_ALIAS = "AS...";
static const QString ADD_WHERE_EXPRESSION_OR = "OR ...";
static const QString ADD_WHERE_EXPRESSION_AND = "AND ...";
static const QString ADD_WHERE_EXPRESSION = "New expression ...";
static const QString strRemove = "REMOVE";
static const QString strMathMinus = "MINUS";
static const QString strMathPlus = "PLUS";
static const QString strMathMultiply = "MULTPILY";
static const QString strMathDivide = "DIVIDE";
static const QString strMathMod = "MOD";
static const QString strMathDiv = "DIV";

static const QString strLogNot = "NOT";
static const QString strLogNeg = "NEG";
static const QString strLogXor = "XOR";
static const QString strLogLShift = "Left Shift";
static const QString strLogRShift = "Right Shift";
static const QString strLogOr = "OR";
static const QString strLogAnd = "AND";

static const QString strCmpEqual = "Equal";
static const QString strCmpNotEqual = "Not Equal";
static const QString strCmpLess = "Less";
static const QString strCmpGreater = "Greater";
static const QString strCmpLessOrEqual = "Less or Equal";
static const QString strCmpGreaterOrEqual = "Greater or Equal";

static const QString strLike = "LIKE";
static const QString strIn = "IN";
static const QString strIs = "IS";
static const QString strExists = "EXISTS";
static const QString strBetween = "BETWEEN";
static const QString strNotText = "NOT_TEXT";
static const QString strDistinct = "DISTINCT";
static const QString strStar = "*";
static const QString strRollup = "ROLLUP";

static const QString strActionIndexSeparator = "^";

#endif // STRINGS_H
