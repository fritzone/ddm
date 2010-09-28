#include "MySQLDTSupplier.h"
#include "DataType.h"
#include "UserDataType.h"

MySQLDTSupplier::MySQLDTSupplier()
{
}

QList<DataType> MySQLDTSupplier::textTypes()
{
    static DataType mysql_dt_char("CHAR", DataType::DT_STRING);
    static DataType mysql_dt_varchar("VARCHAR", DataType::DT_STRING);
    static DataType mysql_dt_tinytext("TINYTEXT", DataType::DT_STRING);
    static DataType mysql_dt_text("TEXT", DataType::DT_STRING);
    static DataType mysql_dt_mediumtext("MEDIUMTEXT", DataType::DT_STRING);
    static DataType mysql_dt_longtext("LONGTEXT", DataType::DT_STRING);

    QList<DataType> result;
    result.append(mysql_dt_char);
    result.append(mysql_dt_varchar);
    result.append(mysql_dt_text);
    result.append(mysql_dt_tinytext);
    result.append(mysql_dt_mediumtext);
    result.append(mysql_dt_longtext);

    return result;
}


QList<DataType> MySQLDTSupplier::numericTypes()
{
    static DataType mysql_dt_integer("INTEGER", DataType::DT_STRING);
    static DataType mysql_dt_tinyint("TINYINT", DataType::DT_STRING);
    static DataType mysql_dt_smallint("SMALLINT", DataType::DT_STRING);
    static DataType mysql_dt_mediumint("MEDIUMINT", DataType::DT_STRING);
    static DataType mysql_dt_bigint("BIGINT", DataType::DT_STRING);
    static DataType mysql_dt_decimal("DECIMAL", DataType::DT_STRING);
    static DataType mysql_dt_numeric("NUMERIC", DataType::DT_STRING);
    static DataType mysql_dt_float("FLOAT", DataType::DT_STRING);
    static DataType mysql_dt_real("REAL", DataType::DT_STRING);
    static DataType mysql_dt_double_precision("DOUBLE PRECISION", DataType::DT_STRING);
    static DataType mysql_dt_bit("BIT", DataType::DT_STRING);

    QList<DataType> result;

    result.append(mysql_dt_integer);
    result.append(mysql_dt_tinyint);
    result.append(mysql_dt_smallint);
    result.append(mysql_dt_mediumint);
    result.append(mysql_dt_bigint);
    result.append(mysql_dt_decimal);
    result.append(mysql_dt_numeric);
    result.append(mysql_dt_float);
    result.append(mysql_dt_real);
    result.append(mysql_dt_double_precision);
    result.append(mysql_dt_bit);

    return result;
}

QList<DataType> MySQLDTSupplier::booleanTypes()
{
    QList<DataType> result;
    static DataType mysql_dt_bool("BOOL", DataType::DT_BOOLEAN);
    result.append(mysql_dt_bool);
    return result;
}

QList<DataType> MySQLDTSupplier::dateTimeTypes()
{
    QList<DataType> result;
    static DataType mysql_dt_date("DATE", DataType::DT_DATETIME);
    static DataType mysql_dt_datetime("DATETIME", DataType::DT_DATETIME);
    static DataType mysql_dt_timestamp("TIMESTAMP", DataType::DT_DATETIME);
    static DataType mysql_dt_time("TIME", DataType::DT_DATETIME);
    static DataType mysql_dt_year("YEAR", DataType::DT_DATETIME);


    result.append(mysql_dt_date);
    result.append(mysql_dt_datetime);
    result.append(mysql_dt_timestamp);
    result.append(mysql_dt_time);
    result.append(mysql_dt_year);

    return result;
}

QList<DataType> MySQLDTSupplier::blobTypes()
{
    QList<DataType> result;

    static DataType mysql_dt_binary("BINARY", DataType::DT_DATETIME);
    static DataType mysql_dt_varbinary("VARBINARY", DataType::DT_DATETIME);
    static DataType mysql_dt_tinyblob("TINYBLOB", DataType::DT_DATETIME);
    static DataType mysql_dt_blob("BLOB", DataType::DT_DATETIME);
    static DataType mysql_dt_mediumblob("MEDIUMBLOB", DataType::DT_DATETIME);
    static DataType mysql_dt_longblob("LONGBLOB", DataType::DT_DATETIME);

    result.append(mysql_dt_binary);
    result.append(mysql_dt_varbinary);
    result.append(mysql_dt_tinyblob);
    result.append(mysql_dt_blob);
    result.append(mysql_dt_mediumblob);
    result.append(mysql_dt_longblob);    

    return result;
}

QList<DataType> MySQLDTSupplier::miscTypes()
{
    QList<DataType> result;
    static DataType mysql_dt_enum("ENUM", DataType::DT_DATETIME);
    static DataType mysql_dt_set("SET", DataType::DT_DATETIME);
    result.append(mysql_dt_enum);
    result.append(mysql_dt_set);
    return result;
}

QList<DataType> MySQLDTSupplier::spatialTypes()
{
    QList<DataType> result;
    static DataType mysql_dt_geometry("GEOMETRY", DataType::DT_DATETIME);
    static DataType mysql_dt_point("POINT", DataType::DT_DATETIME);
    static DataType mysql_dt_linestring("LINESTRING", DataType::DT_DATETIME);
    static DataType mysql_dt_polygon("POLYGON", DataType::DT_DATETIME);
    static DataType mysql_dt_multipolygon("MULTIPOLYGON", DataType::DT_DATETIME);
    static DataType mysql_dt_multipoint("MULTIPOINT", DataType::DT_DATETIME);
    static DataType mysql_dt_multilinestring("MULTILINESTRING", DataType::DT_DATETIME);
    static DataType mysql_dt_geometrycollection("GEOMETRYCOLLECTION", DataType::DT_DATETIME);



    result.append(mysql_dt_geometry);
    result.append(mysql_dt_point);
    result.append(mysql_dt_linestring);
    result.append(mysql_dt_polygon);
    result.append(mysql_dt_multipolygon);
    result.append(mysql_dt_multipoint);
    result.append(mysql_dt_multilinestring);
    result.append(mysql_dt_geometrycollection);

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
int MySQLDTSupplier::maximumSize(const QString& sqldt)
{
    if(sqldt == "CHAR") return 255;
    if(sqldt == "VARCHAR") return 65535;
    if(sqldt == "TINYTEXT") return 0;
    if(sqldt == "TEXT") return 0;
    if(sqldt == "MEDIUMTEXT") return 0;
    if(sqldt == "LONGTEXT") return 0;

    if(sqldt == "INTEGER") return 1;
    if(sqldt == "TINYINT") return 1;
    if(sqldt == "SMALLINT") return 1;
    if(sqldt == "MEDIUMINT") return 1;
    if(sqldt == "BIGINT") return 1;
    if(sqldt == "DECIMAL") return 1;
    if(sqldt == "NUMERIC") return 1;
    if(sqldt == "FLOAT") return 1;
    if(sqldt == "REAL") return 1;
    if(sqldt == "DOUBLE PRECISION") return 1;
    if(sqldt == "BIT") return 64;

    if(sqldt == "DATE") return 0;
    if(sqldt == "DATETIME") return 0;
    if(sqldt == "TIME") return 0;
    if(sqldt == "TIMESTAMP") return 0;
    if(sqldt == "YEAR") return 0;

    if(sqldt == "BINARY") return 65535;
    if(sqldt == "VARBINARY") return 65535;
    if(sqldt == "TINYBLOB") return 0;
    if(sqldt == "BLOB") return 0;
    if(sqldt == "MEDIUMBLOB") return 0;
    if(sqldt == "LONGBLOB") return 0;

    if(sqldt == "SET") return -1;
    if(sqldt == "ENUM") return -1;

    // the spatial data types don't have anything like this
    return 0;

}


QString MySQLDTSupplier::defaultSqlForGenericType(const QString& genericType)
{
    if(genericType == strTextString) return "VARCHAR";
    return "";
}

bool MySQLDTSupplier::isValid(const UserDataType* )
{
    return true;
}













