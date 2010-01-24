#ifndef MYSQLCODEPAGESUPPLIER_H
#define MYSQLCODEPAGESUPPLIER_H

#include "AbstractCodepageSupplier.h"

class MySQLCodepageSupplier : public AbstractCodepageSupplier
{
public:
    MySQLCodepageSupplier();
    virtual QVector<Codepage*> getCodepages();
};

#endif // MYSQLCODEPAGESUPPLIER_H
