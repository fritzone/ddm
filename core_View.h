#ifndef CORE_VIEW_H
#define CORE_VIEW_H

#include "SqlSourceEntity.h"

class View : public SqlSourceEntity
{
    virtual QStringList generateSqlSource(AbstractSqlGenerator*, QHash<QString,QString>, const QString& codepage) = 0;
};

#endif // CORE_VIEW_H
