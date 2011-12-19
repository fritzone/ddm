#ifndef ABSTRACTINDEXTYPEPROVIDER_H
#define ABSTRACTINDEXTYPEPROVIDER_H

#include <QStringList>

/**
 * Class to provide a specific index type which will be
 */
class AbstractIndextypeProvider
{
public:
    AbstractIndextypeProvider(){}
    virtual ~AbstractIndextypeProvider(){}
    virtual QStringList getIndexTypes() = 0;
    virtual QString getDefaultIndextype() = 0;

};

#endif // ABSTRACTINDEXTYPEPROVIDER_H
