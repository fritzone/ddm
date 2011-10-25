#include "core_View.h"
#include "qbr_QueryAsGenerator.h"
#include "qbr_SelectQuery.h"
#include "Workspace.h"
#include "Version.h"
#include "NameGenerator.h"

View::View() : SqlSourceEntity(), NamedItem(NameGenerator::getUniqueName(Workspace::getInstance()->workingVersion(), (NameGenerator::itemGetter)&Version::getView, QString("v")))
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
