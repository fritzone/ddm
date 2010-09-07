#ifndef NAMEGENERATOR_H
#define NAMEGENERATOR_H

#include <QString>

class Version;

class NameGenerator
{
public:
    static QString getNextTableNameFromVersion(Version* v, QString prefix);
    static QString generateUniqueTableInstanceName(Version* v, QString input);

private:

    static const long MAX_IT = 99999999;

    NameGenerator();
};

#endif // NAMEGENERATOR_H
