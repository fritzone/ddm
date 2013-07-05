#ifndef CONN_CUBRID_H
#define CONN_CUBRID_H

#include "conn_AuthenticatedConnection.h"
#include "strings.h"

class CUBRIDConnection : public AuthenticatedConnection
{
public:
    CUBRIDConnection(const QString& name, const QString& host,
                    const QString& user, const QString& pass, const QString& db,
                    bool savePw, bool autoConnect, int port);

    virtual QString getDbType() const
    {
        return strCUBRID;
    }

};

#endif // CONN_CUBRID_H
