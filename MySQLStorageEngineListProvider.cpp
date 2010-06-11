#include "MySQLStorageEngineListProvider.h"

#include "MySQLMyISAMStorageEngine.h"
#include "MySQLInnoDBStorageEngine.h"
#include "MySQLMemoryStorageEngine.h"

MySQLStorageEngineListProvider::MySQLStorageEngineListProvider():m_stEngines()
{
    m_stEngines.append(new MySQLMyISAMStorageEngine());
    m_stEngines.append(new MySQLInnoDBStorageEngine());
    m_stEngines.append(new MySQLMemoryStorageEngine());
}

QVector<AbstractStorageEngine*> MySQLStorageEngineListProvider::getStorageEngines()
{
    return m_stEngines;
}
