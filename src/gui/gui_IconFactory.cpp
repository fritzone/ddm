#include "IconFactory.h"

const QIcon& IconFactory::getIconForActionId(const QString& id)
{
    if(id == "QUERY") return getSqlIcon();
    if(id == "DATABASE") return getDatabaseIcon();
    if(id == "DROP") return getDropDatabaseIcon();
    if(id == "RECREATE") return getRecreateDatabaseIcon();
    if(id == "BROWSE") return getBrowseConnectionIcon();
    if(id == "NEW") return getNewIcon();
    if(id == "NEWTABLE") return getTableIcon();
    if(id == "DELETE") return getRemoveIcon();
    if(id == "EDIT") return getEditConnectionIcon();

    static const QIcon icon(icons::strEmptyIcon);
    return icon;
}

