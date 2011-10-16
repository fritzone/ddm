#include "dbmysql_MySQLStorageEngineListProvider.h"

#include "dbmysql_MySQLMyISAMStorageEngine.h"
#include "dbmysql_MySQLInnoDBStorageEngine.h"
#include "dbmysql_MySQLMemoryStorageEngine.h"
#include "dbmysql_MySQLArchiveStorageEngine.h"

MySQLStorageEngineListProvider::MySQLStorageEngineListProvider():m_stEngines()
{
    m_stEngines.append(new MySQLMyISAMStorageEngine());
    m_stEngines.append(new MySQLInnoDBStorageEngine());
    m_stEngines.append(new MySQLMemoryStorageEngine());
    m_stEngines.append(new MySQLArchiveStorageEngine());
}

QVector<AbstractStorageEngine*> MySQLStorageEngineListProvider::getStorageEngines()
{
    return m_stEngines;
}
