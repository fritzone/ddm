#ifndef DB_DEFAULTDATABASEENGINE_H
#define DB_DEFAULTDATABASEENGINE_H

#include "db_DatabaseEngine.h"

class Version;
class ForeignKey;

class DefaultDatabaseEngine : public DatabaseEngine
{
public:

    DefaultDatabaseEngine(const QString& db, const QString& uid) : DatabaseEngine(db, uid)
    {}

    QString toHexString(const QString &x);

    QStringList chopUpString(const QString &x, int size)
    {
        QStringList result;
        QString piece = "";
        int ctr = 0;
        for(int i=0;i <x.length(); i++)
        {
            piece += x.at(i);
            ctr ++;
            if(ctr == size)
            {
                result.push_back(piece);
                piece = "";
                ctr = 0;
            }
        }

        result.push_back(piece);
        return result;
    }

    ForeignKey *createForeignKey(bool& foundAtLeastOneForeignKey,
                                                const QString& referenceeTableName,
                                                Version* v,
                                                const QString& referencedColumnName,
                                                const QString& referenceeColumnName,
                                                const QString& referencedTableName, const QString &name);

    QString getDbMetadata(Connection *c);

};

#endif // DB_DEFAULTDATABASEENGINE_H
