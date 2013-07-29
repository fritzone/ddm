SOURCES +=      db/db_DatabaseEngine.cpp                       \
                db/db_SP.cpp \
                db/db_BasicSqlGenerator.cpp \
                db/db_DefaultDatabaseEngine.cpp \
    db/db_GenericDTSupplier.cpp

HEADERS += 	db/db_AbstractDTSupplier.h                     \
                db/db_AbstractSQLGenerator.h                   \
                db/db_DatabaseBuiltinFunction.h                \
                db/db_DatabaseBuiltinFunctionsParameter.h      \
                db/db_DatabaseEngine.h                         \
                db/db_Codepage.h \
                db/db_DefaultDatabaseEngine.h \
                db/db_BasicSqlGenerator.h \
    db/db_GenericDTSupplier.h \
    db/db_GenericDatabaseType.h
