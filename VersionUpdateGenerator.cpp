#include "VersionUpdateGenerator.h"
#include "Version.h"
#include "core_Table.h"
#include "UidWarehouse.h"
#include "TableUpdateGenerator.h"
#include "Workspace.h"

VersionUpdateGenerator::VersionUpdateGenerator(Version *from, Version *to)
{
    // section: tables

    // check: see if there are new tables in version "to" which are not there in "from".
    // the new tables are being put in a list, and they will be created at the end of this
    // method sicne there might be columns from other tables which are referenced as foreign
    // key from the new table
    QStringList newTabUids;
    const QVector<Table*>& toTables = to->getTables();
    for(int i=0; i<toTables.size(); i++)
    {
        if(toTables[i]->getSourceUid() == nullUid)
        {
            // a new table in the "to" version.
            newTabUids.append(toTables[i]->getObjectUid());
        }
    }

    QStringList fkCommands;

    // check: find all the related tables and generate update scripts.
    // Fetch the new foreign key commands into a separate vector
    // Fetch the dropped foreign key commands into a separate vector
    QStringList tableChanges;
    const QVector<Table*>& fromTables = from->getTables();
    for(int i=0; i<fromTables.size(); i++)
    {
        for(int j=0; j<toTables.size(); j++)
        {
            if(UidWarehouse::instance().related(fromTables[i], toTables[j]))
            {
                tableChanges << "\n-- Update " + fromTables[i]->getName() + " (" +from->getVersionText() + ")  to " + toTables[j]->getName() + " (" +to->getVersionText() + ")";
                TableUpdateGenerator tud(fromTables[i], toTables[j], Workspace::getInstance()->currentProjectsEngine());
                tableChanges << tud.commands();
                fkCommands << tud.droppedFksCommands();
                fkCommands << tud.newFksCommands();
            }
        }
    }

    // task: the later scripts
    m_commands << tableChanges;

    // task: drop all the foreign keys
    m_commands << fkCommands;

    // check: see if there are deleted tables in version "to" which are not there in "from"

    // task: create the tables from newTabUids

    // task: add the foreign key commands after all the tables were created and the
    // update scripts of them were run accordingly

}
