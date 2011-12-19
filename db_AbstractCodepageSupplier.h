#ifndef ABSTRACTCODEPAGESUPPLIER_H
#define ABSTRACTCODEPAGESUPPLIER_H

#include <QVector>

class Codepage;

/**
 * Abstract class used to supply codepages
 */
class AbstractCodepageSupplier
{
public:
    AbstractCodepageSupplier(){}
    virtual ~AbstractCodepageSupplier() {}

    virtual QVector<Codepage*> getCodepages() = 0;
};

#endif // ABSTRACTCODEPAGESUPPLIER_H
