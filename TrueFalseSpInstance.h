#ifndef TRUEFALSESPINSTANCE_H
#define TRUEFALSESPINSTANCE_H

#include "SpInstance.h"

class TrueFalseSpInstance : public SpInstance
{
public:
    TrueFalseSpInstance();
    virtual QString get()
    {
        return "TRUE";
    }
};

#endif // TRUEFALSESPINSTANCE_H
