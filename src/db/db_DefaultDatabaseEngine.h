#ifndef DB_DEFAULTDATABASEENGINE_H
#define DB_DEFAULTDATABASEENGINE_H

#include "db_DatabaseEngine.h"

class DefaultDatabaseEngine : public DatabaseEngine
{
public:

    DefaultDatabaseEngine(const QString& db, const QString& uid) : DatabaseEngine(db, uid)
    {}

    QString toHexString(const QString &x)
    {
        QString result = "";
        for(int i=0; i<x.length(); i++)
        {
            QString hex = QString("%1").arg((int)(x.at(i).toLatin1()), 0, 16);
            if(hex.length() == 1) hex = "0" + hex;
            result += hex;
        }

        return result;
    }

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

};

#endif // DB_DEFAULTDATABASEENGINE_H
