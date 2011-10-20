#include "dbmysql_MySQLCodepageSupplier.h"
#include "Codepage.h"

#include <QFile>

MySQLCodepageSupplier::MySQLCodepageSupplier()
{
}

QVector<Codepage*> MySQLCodepageSupplier::getCodepages()
{
     QFile file("codepages.mysql.dat");
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
     {
	 return QVector<Codepage*>();
     }

     QVector<Codepage*> result;
     while (!file.atEnd())
     {
	 QByteArray line = file.readLine();
	 QString t(line);
	 t=t.trimmed();
	 Codepage* cp = new Codepage(t);

	 result.append(cp);
     }

     return result;
}
