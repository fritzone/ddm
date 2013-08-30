#include "db_GenericDTSupplier.h"
#include "db_GenericDatabaseType.h"
#include "core_UserDataType.h"
#include "uids.h"

GenericDTSupplier::GenericDTSupplier(const QVector<GenericDatabaseType *> dbtypes)
{
    for(int i=0; i<dbtypes.size(); i++)
    {
        UserDataType* dt = new UserDataType(dbtypes[i]->getName(), dbtypes[i]->getDT_TYPE() , nullUid, 0);

        // adding it to the appropriate vector
        switch(dt->getType())
        {
        case DT_STRING: m_textTypes.append(*dt);
            break;
        case DT_NUMERIC: m_numericTypes.append(*dt);
            break;
        case DT_BOOLEAN: m_booleanTypes.append(*dt);
            break;
        case DT_DATETIME: m_dateTypes.append(*dt);
            break;
        case DT_BLOB: m_blobTypes.append(*dt);
            break;
        case DT_SPATIAL: m_spatialTypes.append(*dt);
            break;
        default:
            break;
        }

        // the maximum size
        if(dbtypes[i]->getMaxSize() != 0)
        {
            m_maximumSizes[dbtypes[i]->getName()] = dbtypes[i]->getMaxSize();
        }

        // the DT_TYPE map
        m_dtTypes[dbtypes[i]->getName()] = dbtypes[i]->getDT_TYPE();

        // the defaultness of the data type
        if(dbtypes[i]->isDefault())
        {
            m_defaultTypesSql[UserDataType::getTypeStringFromDT_TYPE(dbtypes[i]->getDT_TYPE())] = dbtypes[i]->getName();
            m_defaultTypes[dbtypes[i]->getDT_TYPE()] = dt;
        }

        // the ddm type of it
        m_dataTypesOfType[dbtypes[i]->getName()] = UserDataType::getTypeStringFromDT_TYPE(dbtypes[i]->getDT_TYPE());
    }
}
