#include "SpsTooltipProviderForUid.h"

#include "uids.h"

QString SpsTooltipProviderForUid::provideTooltipForUid(const QString &uid)
{
//    if (uid == uidTemporaryTable)
//    {
//        return "Create a <b>TEMPORARY</b> table.<p>"
//                "A TEMPORARY table is visible only to the current connection, "
//                "and is dropped automatically when the connection is closed.";
//    }
//    if (uid == uidIfDoesNotExistTable)
//    {
//        return "Specify the <b>IF NOT EXISTS</b> option.<p>"
//                "The keywords IF NOT EXISTS prevent an error from occurring if "
//                "the table exists. However, there is no verification that the "
//                "existing table has a structure identical to that indicated by "
//                "the CREATE TABLE statement.";
//    }

//    if(uid == uidMysqlAutoincrementTable)
//    {
//        return "The initial <b>AUTO_INCREMENT</b> value for the table.";
//    }

//    if(uid == uidMysqlAvgRowLengthTable)
//    {
//        return "An approximation of the average row length for your table. <p>"
//                "You need to set this only for large tables with variable-size rows. ";
//    }

    if(uid == uidMysqlChecksumTable)
    {
        return "Set this to 1 if you want MySQL to maintain a live checksum "
                "for all rows (that is, a checksum that MySQL updates "
                "automatically as the table changes). This makes the table a "
                "little slower to update, but also makes it easier to find "
                "corrupted tables. The CHECKSUM TABLE statement reports the "
                "checksum. (MyISAM storage engine only.) ";
    }

    return "";
}
