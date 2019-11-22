#include "MySqlConnection.h"
#include "db_DatabaseEngine.h"
#include "ddm_strings.h"
#include "db_DatabaseEngineManager.h"

MySqlConnection::MySqlConnection(const QString& name,
                                 const QString& host, const QString& user, const QString& pass, const QString& db,
                                 bool savePw, bool autoConnect, int port):
    AuthenticatedConnection(name, host, port, db, user, pass, savePw, autoConnect)
{
    m_engine = DatabaseEngineManager::instance().engine(strMySql);
}
