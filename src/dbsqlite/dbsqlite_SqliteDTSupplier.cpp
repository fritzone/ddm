#include "dbsqlite_SqliteDTSupplier.h"
#include "core_UserDataType.h"
#include "strings.h"
#include "uids.h"

SqliteDTSupplier::SqliteDTSupplier()
{
}

QList<UserDataType> SqliteDTSupplier::textTypes()
{
    static UserDataType sqlite_dt_text("TEXT", DT_STRING, nullUid, 0);

    static QList<UserDataType> result;

    result.append(sqlite_dt_text);

    return result;
}

QList<UserDataType> SqliteDTSupplier::numericTypes()
{
    static UserDataType sqlite_dt_integer("INTEGER", DT_NUMERIC, nullUid, 0);

    static QList<UserDataType> result;

    result.append(sqlite_dt_integer);

    return result;
}

QList<UserDataType> SqliteDTSupplier::booleanTypes()
{
    static QList<UserDataType> result;
    return result;
}

QList<UserDataType> SqliteDTSupplier::dateTimeTypes()
{
    static QList<UserDataType> result;
    return result;
}

QList<UserDataType> SqliteDTSupplier::blobTypes()
{
    static UserDataType sqlite_dt_blob("BLOB", DT_BLOB, nullUid, 0);

    static QList<UserDataType> result;
    result.append(sqlite_dt_blob);

    return result;
}

QList<UserDataType> SqliteDTSupplier::miscTypes()
{
    static QList<UserDataType> result;
    return result;
}

QList<UserDataType> SqliteDTSupplier::spatialTypes()
{
    static QList<UserDataType> result;
    return result;
}


/**
 * This functions pecifies if the given SQL type has the "SIZE" allowed,
 * ie, if we can write like: column varchar(256) or not ....
 * Return value:
 * 0   - size is not supported (size txt in GUI will be hidden)
 * > 0 - size is supported (size txt will be shown)
 * -1  - this is an enum type (size txt hidden, values list shown)
 * 1 - the returned value is a number, don't populate the width textfield by def
*/
int SqliteDTSupplier::maximumSize(const QString& sqlType)
{
    if(sqlType == "CHAR") return 255;
    if(sqlType == "VARCHAR") return 65535;
    if(sqlType == "TINYTEXT") return 0;
    if(sqlType == "TEXT") return 0;
    if(sqlType == "MEDIUMTEXT") return 0;
    if(sqlType == "LONGTEXT") return 0;

    if(sqlType == "INTEGER") return 1;
    if(sqlType == "INT") return 1;
    if(sqlType == "TINYINT") return 1;
    if(sqlType == "SMALLINT") return 1;
    if(sqlType == "MEDIUMINT") return 1;
    if(sqlType == "BIGINT") return 1;
    if(sqlType == "DECIMAL") return 1;
    if(sqlType == "NUMERIC") return 1;
    if(sqlType == "FLOAT") return 1;
    if(sqlType == "REAL") return 1;
    if(sqlType == "DOUBLE PRECISION") return 1;
    if(sqlType == "BIT") return 64;

    if(sqlType == "DATE") return 0;
    if(sqlType == "DATETIME") return 0;
    if(sqlType == "TIME") return 0;
    if(sqlType == "TIMESTAMP") return 0;
    if(sqlType == "YEAR") return 0;

    if(sqlType == "BINARY") return 65535;
    if(sqlType == "VARBINARY") return 65535;
    if(sqlType == "TINYBLOB") return 0;
    if(sqlType == "BLOB") return 0;
    if(sqlType == "MEDIUMBLOB") return 0;
    if(sqlType == "LONGBLOB") return 0;

    if(sqlType == "SET") return -1;
    if(sqlType == "ENUM") return -1;

    // the spatial data types don't have anything like this
    return 0;
}


QString SqliteDTSupplier::defaultSqlForGenericType(const QString& genericType)
{
    if(genericType == strTextString) return "TEXT";
    if(genericType == strNumeric) return "INTEGER";
    if(genericType == strBoolean) return "INTEGER";
    if(genericType == strDateTime) return "INTEGER";
    if(genericType == strBlob) return "BLOB";
    if(genericType == strMisc) return "";
    return "";
}

bool SqliteDTSupplier::isValid(const UserDataType* )
{
    return true;
}

QString SqliteDTSupplier::typeForSqlType(const QString& sqlType)
{
    if(QString::compare(sqlType, "CHAR", Qt::CaseInsensitive) == 0) return strTextString;
    if(QString::compare(sqlType, "VARCHAR", Qt::CaseInsensitive) == 0) return strTextString;
    if(QString::compare(sqlType, "TINYTEXT", Qt::CaseInsensitive) == 0) return strTextString;
    if(QString::compare(sqlType, "TEXT", Qt::CaseInsensitive) == 0) return strTextString;
    if(QString::compare(sqlType, "MEDIUMTEXT", Qt::CaseInsensitive) == 0) return strTextString;
    if(QString::compare(sqlType, "LONGTEXT", Qt::CaseInsensitive) == 0) return strTextString;

    if(QString::compare(sqlType, "INT", Qt::CaseInsensitive) == 0) return strNumeric;
    if(QString::compare(sqlType, "INTEGER", Qt::CaseInsensitive) == 0) return strNumeric;
    if(QString::compare(sqlType, "TINYINT", Qt::CaseInsensitive) == 0) return strNumeric;
    if(QString::compare(sqlType, "SMALLINT", Qt::CaseInsensitive) == 0) return strNumeric;
    if(QString::compare(sqlType, "MEDIUMINT", Qt::CaseInsensitive) == 0) return strNumeric;
    if(QString::compare(sqlType, "BIGINT", Qt::CaseInsensitive) == 0) return strNumeric;
    if(QString::compare(sqlType, "DECIMAL", Qt::CaseInsensitive) == 0) return strNumeric;
    if(QString::compare(sqlType, "NUMERIC", Qt::CaseInsensitive) == 0) return strNumeric;
    if(QString::compare(sqlType, "FLOAT", Qt::CaseInsensitive) == 0) return strNumeric;
    if(QString::compare(sqlType, "REAL", Qt::CaseInsensitive) == 0) return strNumeric;
    if(QString::compare(sqlType, "DOUBLE PRECISION", Qt::CaseInsensitive) == 0) return strNumeric;
    if(QString::compare(sqlType, "BIT", Qt::CaseInsensitive) == 0) return strNumeric;

    if(QString::compare(sqlType, "DATE", Qt::CaseInsensitive) == 0) return strDateTime;
    if(QString::compare(sqlType, "DATETIME", Qt::CaseInsensitive) == 0) return strDateTime;
    if(QString::compare(sqlType, "TIME", Qt::CaseInsensitive) == 0) return strDateTime;
    if(QString::compare(sqlType, "TIMESTAMP", Qt::CaseInsensitive) == 0) return strDateTime;
    if(QString::compare(sqlType, "YEAR", Qt::CaseInsensitive) == 0) return strDateTime;

    if(QString::compare(sqlType, "BINARY", Qt::CaseInsensitive) == 0) return strBlob;
    if(QString::compare(sqlType, "VARBINARY", Qt::CaseInsensitive) == 0) return strBlob;
    if(QString::compare(sqlType, "TINYBLOB", Qt::CaseInsensitive) == 0) return strBlob;
    if(QString::compare(sqlType, "BLOB", Qt::CaseInsensitive) == 0) return strBlob;
    if(QString::compare(sqlType, "MEDIUMBLOB", Qt::CaseInsensitive) == 0) return strNumeric;
    if(QString::compare(sqlType, "LONGBLOB", Qt::CaseInsensitive) == 0) return strBlob;

    if(QString::compare(sqlType, "SET", Qt::CaseInsensitive) == 0) return strMisc;
    if(QString::compare(sqlType, "ENUM", Qt::CaseInsensitive) == 0) return strMisc;

    // the spatial data types don't have anything like this
    return "";
}

DT_TYPE SqliteDTSupplier::getDT_TYPE(const QString& usedSql)
{
    if(usedSql.toUpper().startsWith("CHAR")) return DT_STRING;
    if(usedSql.toUpper().startsWith("VARCHAR")) return DT_STRING;
    if(usedSql.toUpper().startsWith("TINYTEXT")) return DT_STRING;
    if(usedSql.toUpper().startsWith("TEXT")) return DT_STRING;
    if(usedSql.toUpper().startsWith("MEDIUMTEXT")) return DT_STRING;
    if(usedSql.toUpper().startsWith("LONGTEXT")) return DT_STRING;

    if(usedSql.toUpper().startsWith("INT")) return DT_NUMERIC;
    if(usedSql.toUpper().startsWith("INTEGER")) return DT_NUMERIC;
    if(usedSql.toUpper().startsWith("TINYINT")) return DT_NUMERIC;
    if(usedSql.toUpper().startsWith("SMALLINT")) return DT_NUMERIC;
    if(usedSql.toUpper().startsWith("MEDIUMINT")) return DT_NUMERIC;
    if(usedSql.toUpper().startsWith("BIGINT")) return DT_NUMERIC;
    if(usedSql.toUpper().startsWith("DECIMAL")) return DT_NUMERIC;
    if(usedSql.toUpper().startsWith("NUMERIC")) return DT_NUMERIC;
    if(usedSql.toUpper().startsWith("FLOAT")) return DT_NUMERIC;
    if(usedSql.toUpper().startsWith("REAL")) return DT_NUMERIC;
    if(usedSql.toUpper().startsWith("DOUBLE PRECISION")) return DT_NUMERIC;
    if(usedSql.toUpper().startsWith("BIT")) return DT_NUMERIC;

    if(usedSql.toUpper().startsWith("DATE")) return DT_DATETIME;
    if(usedSql.toUpper().startsWith("DATETIME")) return DT_DATETIME;
    if(usedSql.toUpper().startsWith("TIME")) return DT_DATETIME;
    if(usedSql.toUpper().startsWith("TIMESTAMP")) return DT_DATETIME;
    if(usedSql.toUpper().startsWith("YEAR")) return DT_DATETIME;

    if(usedSql.toUpper().startsWith("BINARY")) return DT_BLOB;
    if(usedSql.toUpper().startsWith("VARBINARY")) return DT_BLOB;
    if(usedSql.toUpper().startsWith("TINYBLOB")) return DT_BLOB;
    if(usedSql.toUpper().startsWith("BLOB")) return DT_BLOB;
    if(usedSql.toUpper().startsWith("MEDIUMBLOB")) return DT_BLOB;
    if(usedSql.toUpper().startsWith("LONGBLOB")) return DT_BLOB;

    if(usedSql.toUpper().startsWith("SET")) return DT_MISC;
    if(usedSql.toUpper().startsWith("ENUM")) return DT_MISC;

    // the spatial data types don't have anything like this
    return DT_INVALID;
}
