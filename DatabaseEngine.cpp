#include "DatabaseEngine.h"
#include "AbstractCodepageSupplier.h"
#include "MySQLDTSupplier.h"
#include "MysqlCodepageSupplier.h"
#include "MySQLIndextypeProvider.h"

QMap<QString, AbstractDTSupplier*> DatabaseEngine::dtsuppliers;
QMap<QString, AbstractCodepageSupplier*> DatabaseEngine::cpsuppliers;
QMap<QString, AbstractIndextypeProvider*> DatabaseEngine::indextypeProviders;

bool DatabaseEngine::genericInit = false;

DatabaseEngine::DatabaseEngine(const QString& db):database(db)
{
    if(!genericInit)
    {
	genericInit = true;
	// initialize the DT suppliers
	dtsuppliers.insert("MySQL", new MySQLDTSupplier());

	// initialize the Codepage suppliers
	cpsuppliers.insert("MySQL", new MySQLCodepageSupplier());

        indextypeProviders.insert("MySQL", new MySQLIndexTypeProvider());
    }

}

AbstractDTSupplier* DatabaseEngine::getDTSupplier() const
{
    return dtsuppliers.contains(database)?dtsuppliers[database]:0;
}

AbstractCodepageSupplier* DatabaseEngine::getCodepageSupplier() const
{
    return cpsuppliers.contains(database)?cpsuppliers[database]:0;
}

AbstractIndextypeProvider* DatabaseEngine::getIndextypeProvider() const
{
    return indextypeProviders.contains(database)?indextypeProviders[database]:0;
}
