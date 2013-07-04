#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H

#include "conn_AuthenticatedConnection.h"

class MySqlConnection : public AuthenticatedConnection
{
public:

    MySqlConnection(const QString& name, const QString& host,
                    const QString& user, const QString& pass, const QString& db,
                    bool savePw, bool autoConnect, int port);

    virtual void serialize(QDomDocument& doc, QDomElement& parent) const;
    virtual void saveIntoSettings(QSettings& s);
    virtual QString getDbType() const
    {
        return "MySql";
    }
};

#endif // MYSQLCONNECTION_H
