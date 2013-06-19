#include "db_DefaultDatabaseEngine.h"
#include "core_Table.h"
#include "core_Column.h"
#include "Version.h"
#include "ForeignKey.h"

#include <QSqlQuery>

ForeignKey* DefaultDatabaseEngine::createForeignKey(bool& foundAtLeastOneForeignKey,
                                            const QString& referenceeTableName,
                                            Version* v,
                                            const QString& referencedColumnName,
                                            const QString& referenceeColumnName,
                                            const QString& referencedTableName,
                                             const QString& name)
{
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
    ForeignKey* fk = new ForeignKey(v, QUuid::createUuid().toString());
    fk->setName(name);
    fk->addAssociation(fkAssociation);
    referenceeTable->addForeignKey(fk);
    foundAtLeastOneForeignKey = true;

    return fk;
}


// TODO: this should go to a common place
QString DefaultDatabaseEngine::getDbMetadata(Connection *c)
{
    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    if(!db.isOpen()) return "";
    {
    QSqlQuery q(db);
    QString g = "SELECT count(*) FROM sqlite_master WHERE name='DDM_META' and type='table'";
    if(!q.exec(g))
    {
        return "";
    }

    bool foundMetaTable = false;
    while(q.next())
    {
        int cnt = q.value(0).toInt();
        if(cnt == 1)
        {
            foundMetaTable = true;
        }
    }
    if(!foundMetaTable) return "";
    }

    QString dq = "SELECT metadata_chunk FROM DDM_META ORDER BY IDX";
    QSqlQuery q(db);
    if(!q.exec(dq))
    {
        return "";
    }

    QString xmd = "";
    QString last;
    bool f = true;
    while(q.next())
    {
        last = "";
        QString chunk = q.value(0).toString();
        for(int i=0; i<chunk.length();)
        {
            QString hex = chunk.at(i);
            hex += chunk.at(i+1);
            i += 2;
            xmd += QChar((char)(hex.toInt(0, 16)));
            last+= QChar((char)(hex.toInt(0, 16)));
        }
        if(f)
        {
//            qDebug() << xmd;
            f = false;
        }

    }
//    qDebug() << last;
    return xmd;
}

QString DefaultDatabaseEngine::toHexString(const QString &x)
{
    QString result = "";
    for(int i=0; i<x.length(); i++)
    {
        QString hex = QString("%1").arg((int)(x.at(i).toAscii()), 0, 16);
        if(hex.length() == 1) hex = "0" + hex;
        result += hex;
    }

    return result;
}
