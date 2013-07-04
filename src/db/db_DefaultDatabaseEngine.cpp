#include "db_DefaultDatabaseEngine.h"
#include "core_Table.h"
#include "core_Column.h"
#include "Version.h"
#include "core_ForeignKey.h"
#include "db_SP.h"

#include <QSqlQuery>
#include <QSqlError>

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


bool DefaultDatabaseEngine::injectMetadata(Connection *c, const Version *v)
{
    QDomDocument doc("DBM");
    QDomElement root = doc.createElement("Metadata");
    v->serialize(doc, root);

    doc.appendChild(root);
    QString xml = doc.toString();

    QStringList sqls;
    sqls << "DROP TABLE IF EXISTS DDM_META";
    sqls << "create table DDM_META                                          \
            (                                                               \
                INJECT_TIME timestamp,                                      \
                IDX integer(10) primary key auto_increment ,                \
                METADATA_CHUNK text                                         \
            )";

    QString last;
    bool b = executeSql(c, sqls, QStringList(), last, false);
    if(!b) return false;

    QSqlDatabase db = getQSqlDatabaseForConnection(c);
    if(!db.isOpen()) return false;
    QSqlQuery q(db);
    q.prepare("INSERT INTO DDM_META(inject_time, metadata_chunk)  VALUES(SYSDATE(), :md)");
    QString hexedXml = toHexString(xml);
    QStringList chopped = chopUpString(hexedXml, 200);
    for(int i=0; i<chopped.size(); i++)
    {
        q.bindValue(":md", chopped.at(i));
        if(!q.exec())
        {
            lastError = formatLastError(QObject::tr("Cannot inject metadata"), db.lastError());
//            qDebug() << lastError;
            return false;
        }
    }
    return true;
}

// by default a table does not block foreign key functionality
bool DefaultDatabaseEngine::tableBlocksForeignKeyFunctionality(const Table*) const
{
    return false;
}

Sp* DefaultDatabaseEngine::getSpForSqlRole(const QString& uid) const
{
    const QVector<Sp*>& allsps = getDatabaseSpecificProperties();
    for(int i=0; i<allsps.size(); i++)
    {
        if(allsps.at(i)->getSqlRoleUid() == uid)
        {
            return allsps.at(i);
        }
    }
    return 0;
}

QString DefaultDatabaseEngine::formatLastError(const QString& header, const QSqlError &error)
{
    QString errorText = "<b>" + header + "</b>";
    errorText += "<br><br>";
    errorText += "<b>"+ QObject::tr("DB Error: ")  + "</b><!-- DBE: -->"+ error.databaseText() + "<!-- /DBE --><br>";
    errorText += "<b>" + QObject::tr("Driver Error: ")  + "</b><!-- DRV: -->"+ error.driverText() + "<!-- /DRV --><br>";
    errorText += "<b>" + QObject::tr("Error Number: ")  + "</b><!-- NR: -->"+  QString::number(error.number()) + "<!-- /NR --><br>";
    errorText += "<b>" + QObject::tr("Error Type: ") + "</b>";
    switch(error.type())
    {
    case QSqlError::NoError: errorText += "<!-- TYPE: -->No Error<!-- /TYPE -->";
        break;
    case QSqlError::ConnectionError: errorText += "<!-- TYPE: -->Connection Error<!-- /TYPE -->";
        break;
    case QSqlError::StatementError: errorText += "<!-- TYPE: -->Statement Error<!-- /TYPE -->";
        break;
    case QSqlError::TransactionError: errorText += "<!-- TYPE: -->Transaction Error<!-- /TYPE -->";
        break;
    case QSqlError::UnknownError: errorText += "<!-- TYPE: -->Unknown Error<!-- /TYPE -->";
        break;
    }
    errorText += "<br>";
    return errorText;
}

QStringList DefaultDatabaseEngine::getResultOfQuery(const QString& squery, Connection* conn, const QString &error, int column)
{

    QStringList result;

    QSqlDatabase dbo = getQSqlDatabaseForConnection(conn);

    if(!dbo.isOpen())
    {
        lastError = formatLastError(error, dbo.lastError());
        return result;
    }

    QSqlQuery query(dbo);
    query.exec(squery);

    while(query.next())
    {
        QString tab = query.value(column).toString();
        result.append(tab);
    }
    dbo.close();
    return result;

}
