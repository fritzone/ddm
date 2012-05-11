#include "core_StoredMethod.h"
#include "db_AbstractSQLGenerator.h"
#include "Configuration.h"

void StoredMethod::setSql(const QString &s)
{
    m_sql = s;
}

QStringList StoredMethod::generateSqlSource(AbstractSqlGenerator * gen, QHash<QString, QString> /*opts*/, const Connection* dest)
{
    QStringList r;
    r.append(gen->generateCreateStoredMethodSql(this, Configuration::instance().sqlOpts()));
    return r;
}


void StoredMethod::rename(const QString &n)
{
    setName(n);
    TreeItem::rename(n);
}

QString StoredMethod::getSql() const
{
    return m_sql;
}
