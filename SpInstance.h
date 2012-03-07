#ifndef SPINSTANCE_H
#define SPINSTANCE_H

#include <QString>

class Sp;

/**
 * Represents an instance of an SP object.
 */
class SpInstance
{
public:
    SpInstance();

    virtual QString get() = 0;
};

#endif // SPINSTANCE_H
