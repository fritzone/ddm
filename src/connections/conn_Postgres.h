#ifndef CONN_POSTGRES_H
#define CONN_POSTGRES_H

#include "conn_AuthenticatedConnection.h"
#include "strings.h"

class PostgresConnection : public AuthenticatedConnection
{
public:
    PostgresConnection(const QString& name, const QString& host,
                    const QString& user, const QString& pass, const QString& db,
                    bool savePw, bool autoConnect, int port);

    virtual QString getDbType() const
    {
        return strPostgres;
    }

};

#endif // CONN_POSTGRES_H
