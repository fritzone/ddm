#ifndef CODEPAGE_H
#define CODEPAGE_H

#include <QString>

class Codepage
{
public:
    Codepage(const QString& in) : desc(in), name (in)  {}

    const QString& getName() const
    {
        return name;
    }

private:
    QString desc;
    QString name;

};

#endif // CODEPAGE_H
