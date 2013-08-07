#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H

#include "conn_AuthenticatedConnection.h"

#include "strings.h"

class MySqlConnection : public AuthenticatedConnection
{
public:

    MySqlConnection(const QString& name, const QString& host,
                    const QString& user, const QString& pass, const QString& db,
                    bool savePw, bool autoConnect, int port);

    virtual QString getDbType() const
    {
        return strCamelMySql;
    }
};

#endif // MYSQLCONNECTION_H
