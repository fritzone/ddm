#ifndef VERSIONUPDATEGENERATOR_H
#define VERSIONUPDATEGENERATOR_H

#include <QStringList>

class Version;

/**
 * @brief The VersionUpdateGenerator class
 * This class generates code that is supposed to be executed on a database which has deployed
 * the version "from" but needs to be updated to the version "to".
 */
class VersionUpdateGenerator
{
public:

    VersionUpdateGenerator(Version* from, Version* to);

    QStringList getCommands() const
    {
        return m_commands;
    }

private:

    void updateTables(Version* from, Version* to);

private:

    QStringList m_commands;
};

#endif // VERSIONUPDATEGENERATOR_H
