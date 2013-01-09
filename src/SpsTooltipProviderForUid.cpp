#include "SpsTooltipProviderForUid.h"

#include "uids.h"

QString SpsTooltipProviderForUid::provideTooltipForUid(const QString &uid)
{
    if(uid == uidMysqlTemporaryTable) return "Create a <b>TEMPORARY</b> table.<p>A TEMPORARY table is visible only to the current connection, and is dropped automatically when the connection is closed.";
    if(uid == uidMysqlIfNotExistsTable) return "Specify the <b>IF NOT EXISTS</b> option.<p>The keywords IF NOT EXISTS prevent an error from occurring if the table exists. However, there is no verification that the existing table has a structure identical to that indicated by the CREATE TABLE statement.";
    return "";
}
