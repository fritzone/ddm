#include "MySQLDatabaseEngine.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QVariant>
#include <QMessageBox>
#include <QApplication>

#include "Table.h"
#include "Version.h"
#include "Column.h"
#include "Project.h"
#include "VersionGuiElements.h" // TODO: This is bad design. Find a way to remove this from here
#include "UserDataType.h"
#include "Index.h"
#include "TableInstance.h"
#include "AbstractStorageEngineListProvider.h"
#include "ForeignKey.h"
#include "DatabaseBuiltinFunction.h"

QVector<DatabaseBuiltinFunction> MySQLDatabaseEngine::s_builtinFunctions = MySQLDatabaseEngine::buildFunctions();

MySQLDatabaseEngine::MySQLDatabaseEngine() : DatabaseEngine("MySQL"), m_revEngMappings(), m_oneTimeMappings()
{
}

bool MySQLDatabaseEngine::reverseEngineerDatabase(const QString& host, const QString& user, const QString& pass, const QString& dbName, QVector<QString> tables, Project*p, bool relaxed)
{
    Version* v = p->getWorkingVersion();
    m_revEngMappings.clear();
    m_oneTimeMappings.clear();
    for(int i=0; i<tables.size(); i++)
    {
        Table* tab = reverseEngineerTable(host, user, pass, dbName, tables.at(i), p, relaxed);
        v->addTable(tab);
        v->getGui()->createTableTreeEntry(tab);
    }

    // and now try to find some nice names for the user data types
    QList <UserDataType*> rkeys = m_revEngMappings.keys();
    for(int i=0; i<rkeys.size(); i++)
    {
        QList <Column*> colsOfUdt = m_revEngMappings.values(rkeys.at(i));
        for(int j=0; j<colsOfUdt.size(); j++)
        {
            qDebug() << "DATATYPE " << rkeys.at(i)->getName() << " AS COLUMN " << colsOfUdt.at(j)->getName();
        }
    }

    // now populate the foreign keys
    {
        QSqlDatabase dbo = QSqlDatabase::addDatabase("QMYSQL");

        dbo.setHostName(host);
        dbo.setUserName(user);
        dbo.setPassword(pass);
        dbo.setDatabaseName(dbName);

        bool ok = dbo.open();

        if(ok)
        {
            QString s = "SELECT distinct CONCAT( table_name, '.', column_name, ':', referenced_table_name, '.', referenced_column_name ) AS list_of_fks "
                    "FROM information_schema.KEY_COLUMN_USAGE WHERE REFERENCED_TABLE_SCHEMA = '"
                    + dbName +
                    "' AND REFERENCED_TABLE_NAME is not null ORDER BY TABLE_NAME, COLUMN_NAME";
            QSqlQuery query;
            query.exec(s);
            bool foundAtLeastOneForeignKey = false;
            while(query.next())
            {
                QString val = query.value(0).toString();
                QString referencee = val.left(val.indexOf(':'));
                QString referenced = val.mid(val.indexOf(':') + 1);

                QString referenceeTableName = referencee.left(referencee.indexOf('.'));
                QString referenceeColumnName = referencee.mid(referencee.indexOf('.') + 1);

                QString referencedTableName = referenced.left(referenced.indexOf('.'));
                QString referencedColumnName = referenced.mid(referenced.indexOf('.') + 1);

                Table* referenceeTable = v->getTable(referenceeTableName);
                Table* referencedTable = v->getTable(referencedTableName);
                Column* referenceeColumn = referenceeTable->getColumn(referenceeColumnName);
                Column* referencedColumn = referencedTable->getColumn(referencedColumnName);
                // now we should check that the columns have the same data type ...
                if(referencedColumn->getDataType() != referenceeColumn->getDataType())
                {
                    // TODO initially here was nothing ...
                    referencedColumn->setDataType(referenceeColumn->getDataType());
                }

                ForeignKey::ColumnAssociation* fkAssociation = new ForeignKey::ColumnAssociation(referencedTable, referencedColumn, referenceeTable, referenceeColumn);
                ForeignKey* fk = new ForeignKey();
                fk->addAssociation(fkAssociation);
                referenceeTable->addForeignKey(fk);
                foundAtLeastOneForeignKey = true;
            }

            if(!foundAtLeastOneForeignKey)
            {
                QMessageBox::information(0, QApplication::tr("Information"), QApplication::tr("This database does not contain foreign key definitions. You will need to set them up manually."), QMessageBox::Ok);
                v->setSpecialValidationFlags(1);
            }
        }
    }

    return true;
}


QVector<QString> MySQLDatabaseEngine::getAvailableTables(const QString& host, const QString& user, const QString& pass, const QString& db)
{
    QSqlDatabase dbo = QSqlDatabase::addDatabase("QMYSQL");

    dbo.setHostName(host);
    dbo.setUserName(user);
    dbo.setPassword(pass);
    dbo.setDatabaseName(db);

    QVector<QString> result;
    bool ok = dbo.open();

    if(!ok)
    {
        QMessageBox::critical(0, "Error", dbo.lastError().databaseText() + "/" + dbo.lastError().databaseText(), QMessageBox::Ok);
        return result;
    }

    QSqlQuery query;

    query.exec("show tables");

    while(query.next())
    {
        QString tab = query.value(0).toString();
        result.append(tab);
    }

    return result;
}


Table* MySQLDatabaseEngine::reverseEngineerTable(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QString& tableName, Project* p, bool relaxed)
{
    Version* v = p->getWorkingVersion();
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(pass);
    db.setDatabaseName(dbName);

    bool ok = db.open();

    if(!ok)
    {
        QMessageBox::critical(0, "Error", db.lastError().databaseText() + "/" + db.lastError().databaseText(), QMessageBox::Ok);
        return 0;
    }

    Table* tab = new Table(v);
    tab->setName(tableName);

    // fetch all the columns of the table
    {
    QSqlQuery query;
    query.exec("desc " + tableName);

    int fieldNo = query.record().indexOf("Field");
    int typeNo = query.record().indexOf("Type");
    int nulldNo = query.record().indexOf("Null");
    int keyNo = query.record().indexOf("Key");
    int defNo = query.record().indexOf("Default");
    int extraNo = query.record().indexOf("Extra");

    while(query.next())
    {
        QString field_name = query.value(fieldNo).toString();
        QString type = query.value(typeNo).toString().toUpper();
        QString nullable = query.value(nulldNo).toString();
        QString keyness = query.value(keyNo).toString();
        QString defaultValue = query.value(defNo).toString();
        QString extra = query.value(extraNo).toString();

        UserDataType* udt = 0;
        QString oneTimeKey = field_name + type + nullable + defaultValue;
        if(m_oneTimeMappings.contains(oneTimeKey))
        {
            udt = m_oneTimeMappings.value(oneTimeKey);
        }
        else
        {
            udt = v->provideDatatypeForSqlType(field_name, type, nullable, defaultValue, relaxed);
            m_oneTimeMappings.insert(oneTimeKey, udt);
        }
        // now fetch the data type for the given type from the version

        Column* col = new Column(field_name, udt, QString::compare(keyness, "PRI", Qt::CaseInsensitive) == 0, extra == "auto_increment");

        // and add the column to the table
        m_revEngMappings.insert(udt, col);
        tab->addColumn(col);
    }
    }

    // now populate the indices of the table
    {

    QSqlQuery query;
    query.exec("show indexes from " + tableName);

    int tableNo = query.record().indexOf("Table");
    int nonuniqueNo = query.record().indexOf("Non_unique");
    int keynameNo = query.record().indexOf("Key_name");
    int seqinindexNo = query.record().indexOf("Seq_in_index");
    int columnnameNo = query.record().indexOf("Column_name");
    int collationNo = query.record().indexOf("Collation");
    int cardinalityNo = query.record().indexOf("Cardinality");
    int subpartNo = query.record().indexOf("Sub_part");
    int packedNo = query.record().indexOf("Packed");
    int nullNo = query.record().indexOf("Null");
    int indextypeNo = query.record().indexOf("Index_type");
    int commentNo = query.record().indexOf("Comment");

    QMap<QString, Index*> createdIndexes;

    while(query.next())
    {
        QString table = query.value(tableNo).toString();
        QString nonunique = query.value(nonuniqueNo).toString();
        QString keyname = query.value(keynameNo).toString();
        QString seqinindex = query.value(seqinindexNo).toString();
        QString columnname = query.value(columnnameNo).toString();
        QString collation = query.value(collationNo).toString();
        QString cardinality = query.value(cardinalityNo).toString();
        QString subpart = query.value(subpartNo).toString();
        QString packed = query.value(packedNo).toString();
        QString nulld = query.value(nullNo).toString();
        QString indextype = query.value(indextypeNo).toString();
        QString comment = query.value(commentNo).toString();

        QString finalIndexName = keyname;
        if(keyname == "PRIMARY") continue;

        Index* idx = 0;
        if(createdIndexes.keys().contains(keyname))
        {
            idx = createdIndexes[keyname];
        }
        else
        {
            idx = new Index(finalIndexName, indextype, tab);
            createdIndexes.insert(keyname, idx);
        }

        idx->addColumn(tab->getColumn(columnname), seqinindex.toInt());
        if(!tab->hasIndex(finalIndexName))
        {
            tab->addIndex(idx);
        }

    }
    }

    // populate the startup values
    {
        QString s = "select ";
        for(int i=0; i<tab->getColumnCount(); i++)
        {
            s += tab->getColumns().at(i)->getName();
            if(i<tab->getColumnCount() -1) s+=", ";
        }
        s += " from ";
        s += tab->getName();
        QSqlQuery query;
        query.exec(s);
        QVector <QVector <QString> > defaultValues;
        while(query.next())
        {
            QVector <QString> row;
            for(int i=0; i<tab->getColumnCount(); i++)
            {
                QVariant atI = query.value(i);
                if(atI.isNull())
                {
                    row.append("NULL");
                }
                else
                {
                    row.append(atI.toString());
                }
            }

            defaultValues.append(row);
        }

        if(! p->oopProject())
        {
            tab->setDefaultValues(defaultValues);
        }
        else
        {
            // convert the vector above to the required hash
            QHash < QString, QVector<QString> > values;
            for(int i=0; i<defaultValues.size(); i++)
            {
                QVector<QString> defVsI = defaultValues.at(i);
                for(int j=0; j<defVsI.size(); j++)
                {
                    if(values.keys().contains(tab->getColumns().at(j)->getName()))
                    {
                        values[tab->getColumns().at(j)->getName()].append(defVsI.at(j));
                    }
                    else
                    {
                        QVector<QString> newValues;
                        newValues.append(defVsI.at(j));
                        values[tab->getColumns().at(j)->getName()] = newValues;
                    }
                }
            }

            // and create a table instance for it
            TableInstance* inst = v->instantiateTable(tab, false);
            inst->setValues(values);

            v->getGui()->createTableInstanceTreeEntry(inst);
        }
    }

    // find the storage engine
    {
        QString s = "SELECT ENGINE FROM information_schema.TABLES WHERE TABLE_SCHEMA = '"+dbName+"' AND TABLE_NAME = '"+tab->getName()+"'";
        QSqlQuery query;
        query.exec(s);
        QString eng = "";
        while(query.next())
        {
            eng = query.value(0).toString();
        }

        AbstractStorageEngineListProvider* stlist = getStorageEngineListProviders();
        QVector<AbstractStorageEngine*> stengines = stlist->getStorageEngines();
        for(int i=0; i<stengines.size(); i++)
        {
            if(stengines.at(i)->name() == eng)
            {
                tab->setStorageEngine(stengines.at(i));
                break;
            }
        }
    }
    return tab;
}

bool MySQLDatabaseEngine::injectSql(const QString& host, const QString& user, const QString& pass, const QString& dbName, const QStringList& sqls, QString& lastSql, bool rollbackOnError, bool)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(pass);
    db.setDatabaseName(dbName);

    bool ok = db.open();


    if(!ok)
    {
        lastError = db.lastError().databaseText() + "/" + db.lastError().databaseText();
        return false;
    }

    bool transactionSucces = db.transaction();
    QSqlQuery query;

    for(int i=0; i<sqls.size(); i++)
    {
        lastSql = sqls[i].trimmed();
        if(lastSql.length() > 0)
        {
            if(!query.exec(lastSql))
            {
                lastError = query.lastError().databaseText() + "/" + query.lastError().databaseText();
                if(transactionSucces && rollbackOnError)
                {
                    db.rollback();
                }
                return false;
            }
        }
    }

    if(transactionSucces)
    {
        db.commit();
    }
    return true;
}

QString MySQLDatabaseEngine::getDefaultDatatypesLocation()
{
    return "mysql.defaults";
}

QVector<QString> MySQLDatabaseEngine::getAvailableDatabases(const QString& host, const QString& user, const QString& pass)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(pass);

    bool ok = db.open();
    QVector<QString> result;

    if(!ok)
    {
        lastError = QObject::tr("Cannot connect to the database: ") + db.lastError().databaseText() + "/" + db.lastError().driverText();
        return result;
    }

    QSqlQuery query;
    query.exec("show databases");

    while(query.next())
    {
        QString db = query.value(0).toString();
        result.append(db);
    }

    return result;
}

bool MySQLDatabaseEngine::createDatabase(const QString& host, const QString& user, const QString& pass, const QString& name)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(host);
    db.setUserName(user);
    db.setPassword(pass);

    bool ok = db.open();
    if(!ok)
    {
        lastError = QObject::tr("Cannot connect to the database: ") + db.lastError().databaseText() + "/" + db.lastError().driverText();
        return false;
    }

    QSqlQuery query;
    bool t = query.exec("create database "+ name);
    if(!t)
    {
        lastError = QObject::tr("Cannot create a database: ") + db.lastError().databaseText() + "/" + db.lastError().driverText();
        return false;
    }
    return true;
}

QVector<DatabaseBuiltinFunction> MySQLDatabaseEngine::getBuiltinFunctions()
{
    return s_builtinFunctions;
}


QVector<DatabaseBuiltinFunction> MySQLDatabaseEngine::buildFunctions()
{
    static QVector<DatabaseBuiltinFunction> result;
    QString X = QString("X");


#define RET_NUMERIC     UserDataType("return", DataType::DT_NUMERIC)
#define RET_STRING      UserDataType("return", DataType::DT_STRING)

#define PAR_STRING      DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DataType::DT_STRING), true)
#define PAR_NUMERIC     DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DataType::DT_NUMERIC), true)
#define PAR_VARIABLE    DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DataType::DT_VARIABLE), true)

#define OPAR_STRING      DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DataType::DT_STRING), false)
#define OPAR_NUMERIC     DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DataType::DT_NUMERIC), false)

#define FUNC result.append(DatabaseBuiltinFunction(QString


    FUNC("@abs"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "ABS(X) Returns the absolute value of X"));
    FUNC("@acos"),         FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "ACOS(X) Returns the arc cosine of X, that is, the value whose cosine is X. Returns NULL if X is not in the range -1 to 1. "));
    FUNC("@asin"),         FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "ASIN(X) Returns the arc sine of X, that is, the value whose sine is X. Returns NULL if X is not in the range -1 to 1. "));
    FUNC("@atan"),         FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "ATAN(X) Returns the arc tangent of X, that is, the value whose tangent is X."));
    FUNC("@ceiling"),      FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "CEILING(X) Returns the smallest integer value not less than X."));
    FUNC("@conv"),         FT_NUMERIC,   RET_STRING,  PAR_STRING, PAR_NUMERIC, PAR_NUMERIC, "CONV(X,from_base,to_base) Converts numbers between different number bases. Returns a string representation of the number N, converted from base from_base to base to_base."));
    FUNC("@cos"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "COS(X) Returns the cosine of X, where X is given in radians. "));
    FUNC("@cot"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "COT(X) Returns the cotangent of X."));
    FUNC("@crc32"),        FT_NUMERIC,   RET_NUMERIC, PAR_STRING,  "CRC32(str) Computes a cyclic redundancy check value and returns a 32-bit unsigned value."));
    FUNC("@degrees"),      FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "DEGREES(X) Returns the argument X, converted from radians to degrees. "));
    FUNC("@exp"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "EXP(X) Returns the value of e (the base of natural logarithms) raised to the power of X."));
    FUNC("@floor"),        FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "FLOOR(X) Returns the largest integer value not greater than X."));
    FUNC("@ln"),           FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "LN(X) Returns the natural logarithm of X; that is, the base-e logarithm of X."));
    FUNC("@log"),          FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, PAR_NUMERIC, "LOG(B,X) Returns the logarithm of X to the base B"));
    FUNC("@log2"),         FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "LOG2(X) Returns the 2-base logarithm of X"));
    FUNC("@log10"),        FT_NUMERIC,   RET_NUMERIC, PAR_NUMERIC, "LOG10(X) Returns the 10-base logarithm of X"));
    FUNC("@pi"),           FT_NUMERIC,   RET_NUMERIC, "PI() Returns the value of p (pi)."));



    result.append(DatabaseBuiltinFunction(QString("@avg"), FT_AGGREGATE, UserDataType("return", DataType::DT_NUMERIC), DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DataType::DT_NUMERIC), true), "AVG(X) Returns the average value of expr."));
    result.append(DatabaseBuiltinFunction(QString("@bit_and"), FT_AGGREGATE, UserDataType("return", DataType::DT_NUMERIC), DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DataType::DT_NUMERIC), true), "BIT_AND(X) Returns the bitwise AND of all bits in expr. The calculation is performed with 64-bit (BIGINT) precision. "));
    result.append(DatabaseBuiltinFunction(QString("@bit_or"), FT_AGGREGATE, UserDataType("return", DataType::DT_NUMERIC), DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DataType::DT_NUMERIC), true), "BIT_XOR(X) Returns the bitwise OR of all bits in expr. The calculation is performed with 64-bit (BIGINT) precision. "));
    result.append(DatabaseBuiltinFunction(QString("@bit_xor"), FT_AGGREGATE, UserDataType("return", DataType::DT_NUMERIC), DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DataType::DT_NUMERIC), true), "BIT_XOR(X) Returns the bitwise XOR of all bits in expr. The calculation is performed with 64-bit (BIGINT) precision. "));
    result.append(DatabaseBuiltinFunction(QString("@count"), FT_AGGREGATE, UserDataType("return", DataType::DT_NUMERIC), DatabaseBuiltinFunctionsParameter(X, UserDataType(X, DataType::DT_GENERIC), true), "Returns a count of the number of non-NULL values of expr in the rows retrieved by a SELECT statement. The result is a BIGINT value."));

    FUNC("@ascii"),        FT_STRING,   RET_NUMERIC,  PAR_STRING, "ASCII(str) Returns the numeric value of the leftmost character of the string str. Returns 0 if str is the empty string. Returns NULL if str is NULL. ASCII() works for 8-bit characters."));
    FUNC("@bin"),          FT_STRING,   RET_STRING,   PAR_NUMERIC, "BIN(N) Returns a string representation of the binary value of N, where N is a longlong (BIGINT) number. This is equivalent to CONV(N,10,2). Returns NULL if N is NULL."));
    FUNC("@bit_length"),   FT_STRING,   RET_NUMERIC,  PAR_STRING, "BIT_LENGTH(str) Returns the length of the string str in bits."));
    FUNC("@char"),         FT_STRING,   RET_STRING,   PAR_NUMERIC, PAR_VARIABLE,  "CHAR(N,...) interprets each argument N as an integer and returns a string consisting of the characters given by the code values of those integers. NULL values are skipped."));
    FUNC("@char_length"),  FT_STRING,   RET_NUMERIC,  PAR_STRING, "CHAR_LENGTH(str) Returns the length of the string str, measured in characters. A multi-byte character counts as a single character."));
    FUNC("@concat"),       FT_STRING,   RET_STRING,   PAR_VARIABLE, "CONCAT(str1,str2,...) Returns the string that results from concatenating the arguments. May have one or more arguments."));
    FUNC("@concat_ws"),    FT_STRING,   RET_STRING,   PAR_VARIABLE, "CONCAT_WS(separator,str1,str2,...) stands for Concatenate With Separator and is a special form of CONCAT(). The first argument is the separator for the rest of the arguments. The separator is added between the strings to be concatenated."));
    FUNC("@elt"),          FT_STRING,   RET_STRING,   PAR_NUMERIC, PAR_VARIABLE, "ELT(N,str1,str2,str3,...) Returns str1 if N = 1, str2 if N = 2, and so on. Returns NULL if N is less than 1 or greater than the number of arguments."));
    FUNC("@export_set"),   FT_STRING,   RET_STRING,   PAR_NUMERIC, PAR_STRING, PAR_STRING, OPAR_STRING, OPAR_NUMERIC, "EXPORT_SET(bits,on,off[,separator[,number_of_bits]]) Returns a string such that for every bit set in the value bits, you get an on string and for every bit not set in the value, you get an off string. "));
    FUNC("@field"),        FT_STRING,   RET_NUMERIC,  PAR_VARIABLE, "FIELD(str,str1,str2,str3,...) Returns the index (position) of str in the str1, str2, str3, ... list. Returns 0 if str is not found. "));
    FUNC("@find_in_set"),  FT_STRING,   RET_NUMERIC,  PAR_STRING, PAR_STRING, "FIND_IN_SET(str,strlist) Returns a value in the range of 1 to N if the string str is in the string list strlist consisting of N substrings."));
    FUNC("@format"),       FT_STRING,   RET_STRING,   PAR_NUMERIC, PAR_NUMERIC, "FORMAT(X,D) Formats the number X to a format like '#,###,###.##', rounded to D decimal places, and returns the result as a string. If D is 0, the result has no decimal point or fractional part. D should be a constant value. "));
    FUNC("@hex"),          FT_STRING,   RET_STRING,   PAR_NUMERIC, "HEX(N) For a numeric argument N, HEX() returns a hexadecimal string representation of the value of N treated as a longlong (BIGINT) number."));
    FUNC("@hex"),          FT_STRING,   RET_STRING,   PAR_STRING, "HEX(str) For a string argument str, HEX() returns a hexadecimal string representation of str where each character in str is converted to two hexadecimal digits."));
    FUNC("@insert"),       FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, PAR_NUMERIC, PAR_STRING, "INSERT(str,pos,len,newstr) Returns the string str, with the substring beginning at position pos and len characters long replaced by the string newstr."));
    FUNC("@instr"),        FT_STRING,   RET_NUMERIC,  PAR_STRING, PAR_STRING, "INSTR(str,substr) Returns the position of the first occurrence of substring substr in string str."));
    FUNC("@left"),         FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, "LEFT(str,len) Returns the leftmost len characters from the string str, or NULL if any argument is NULL."));
    FUNC("@length"),       FT_STRING,   RET_NUMERIC,  PAR_STRING, "LENGTH(str) Returns the length of the string str, measured in bytes."));
    FUNC("@load_file"),    FT_STRING,   RET_STRING,   PAR_STRING, "LOAD_FILE(file_name) Reads the file and returns the file contents as a string."));
    FUNC("@locate"),       FT_STRING,   RET_STRING,   PAR_STRING, PAR_STRING, "LOCATE(substr,str) Returns the position of the first occurrence of substring substr in string str."));
    FUNC("@locate"),       FT_STRING,   RET_STRING,   PAR_STRING, PAR_STRING, PAR_NUMERIC, "LOCATE(substr,str,pos) Returns the position of the first occurrence of substring substr in string str starting at position pos."));
    FUNC("@lower"),        FT_STRING,   RET_STRING,   PAR_STRING, "LOWER(str) Returns the string str with all characters changed to lowercase according to the current character set mapping."));
    FUNC("@lpad"),         FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, PAR_STRING, "LPAD(str,len,padstr) Returns the string str, left-padded with the string padstr to a length of len characters. If str is longer than len, the return value is shortened to len characters. "));
    FUNC("@ltrim"),        FT_STRING,   RET_STRING,   PAR_STRING, "LTRIM(str) Returns the string str with leading space characters removed. "));
    FUNC("@make_set"),     FT_STRING,   RET_STRING,   PAR_NUMERIC, PAR_VARIABLE, "MAKE_SET(bits,str1,str2,...) Returns a set value (a string containing substrings separated by “,” characters) consisting of the strings that have the corresponding bit in bits set. str1 corresponds to bit 0, str2 to bit 1, and so on."));
    FUNC("@mid"),          FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, PAR_NUMERIC, "MID(str,pos,len) is a synonym for SUBSTRING(str,pos,len). "));
    FUNC("@oct"),          FT_STRING,   RET_STRING,   PAR_NUMERIC, "OCT(N) Returns a string representation of the octal value of N, where N is a longlong (BIGINT) number."));
    FUNC("@ord"),          FT_STRING,   RET_NUMERIC,  PAR_STRING, "ORD(str) returns the same value as the ASCII() function."));
    FUNC("@quote"),        FT_STRING,   RET_STRING,   PAR_STRING, "QUOTE(str) Quotes a string to produce a result that can be used as a properly escaped data value in an SQL statement. "));
    FUNC("@repeat"),       FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, "REPEAT(str,count) Returns a string consisting of the string str repeated count times. If count is less than 1, returns an empty string. Returns NULL if str or count are NULL."));
    FUNC("@replace"),      FT_STRING,   RET_STRING,   PAR_STRING, PAR_STRING, PAR_STRING, "REPLACE(str,from_str,to_str) Returns the string str with all occurrences of the string from_str replaced by the string to_str."));
    FUNC("@reverse"),      FT_STRING,   RET_STRING,   PAR_STRING, "REVERSE(str) Returns the string str with the order of the characters reversed."));
    FUNC("@right"),        FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, "RIGHT(str,len) Returns the rightmost len characters from the string str, or NULL if any argument is NULL."));
    FUNC("@rpad"),         FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, "RPAD(str,len,padstr) Returns the string str, right-padded with the string padstr to a length of len characters. If str is longer than len, the return value is shortened to len characters."));
    FUNC("@rtrim"),        FT_STRING,   RET_STRING,   PAR_STRING, "RTRIM(str) Returns the string str with trailing space characters removed. "));
    FUNC("@soundex"),      FT_STRING,   RET_STRING,   PAR_STRING, "SOUNDEX(str) Returns a soundex string from str. Two strings that sound almost the same should have identical soundex strings. A standard soundex string is four characters long, but the SOUNDEX() function returns an arbitrarily long string."));
    FUNC("@space"),        FT_STRING,   RET_STRING,   PAR_NUMERIC, "SPACE(N) Returns a string consisting of N space characters."));
    FUNC("@substring"),    FT_STRING,   RET_STRING,   PAR_STRING, PAR_NUMERIC, OPAR_NUMERIC, "SUBSTRING(str,pos,len) Return a substring len characters long from string str, starting at position pos."));
    FUNC("@substring_index"), FT_STRING,RET_STRING,   PAR_STRING, PAR_STRING, PAR_NUMERIC, "SUBSTRING_INDEX(str,delim,count) Returns the substring from string str before count occurrences of the delimiter delim."));
    FUNC("@trim"),         FT_STRING,   RET_STRING,   OPAR_STRING, PAR_STRING, "TRIM([remstr FROM] str) Returns the string str with all remstr prefixes or suffixes removed."));
    FUNC("@unhex"),        FT_STRING,   RET_STRING,   PAR_STRING, "UNHEX(str) For a string argument str, UNHEX(str) performs the inverse operation of HEX(str)."));
    FUNC("@upper"),        FT_STRING,   RET_STRING,   PAR_STRING, "UPPER(str) Returns the string str with all characters changed to uppercase according to the current character set mapping."));



    return result;
}

static DatabaseBuiltinFunction no_function;

const DatabaseBuiltinFunction& MySQLDatabaseEngine::getBuiltinFunction(const QString& name)
{
    for(int i=0; i<s_builtinFunctions.size(); i++)
    {
        if(s_builtinFunctions.at(i).getName().mid(1).toUpper() == name.toUpper())   // remove the leading @
        {
            return s_builtinFunctions.at(i);
        }
    }
    return no_function;
}
