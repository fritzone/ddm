#include "core_View.h"
#include "qbr_QueryAsGenerator.h"
#include "qbr_SelectQuery.h"

View::View() : SqlSourceEntity()
{
    c = new QueryGraphicsHelper();
    sq = new SelectQuery(c, 0, this);
    QueryAsGenerator::instance().initNewQuery(sq);
    c->setQuery(sq);
}

QStringList View::generateSqlSource(AbstractSqlGenerator *, QHash<QString, QString>, const QString &codepage)
{
    QStringList res;
    res.append(sq->get());
    return res;
}
