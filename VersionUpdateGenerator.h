#ifndef VERSIONUPDATEGENERATOR_H
#define VERSIONUPDATEGENERATOR_H

#include <QStringList>

class Version;

class VersionUpdateGenerator
{
public:

    VersionUpdateGenerator(Version* from, Version* to);

    QStringList getCommands() const
    {
        return m_commands;
    }

private:

    QStringList m_commands;
};

#endif // VERSIONUPDATEGENERATOR_H
