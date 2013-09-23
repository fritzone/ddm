#
# DDM: CMake build system template for Qt based projects
# Author: Rohit Yadav <rohityadav89 AT gmail.com>
#

#Put here CPPs
SET(DDM_DB_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/db_AdvancedSqlGenerator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/db_BasicSqlGenerator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/db_DatabaseEngine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/db_SP.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/db_DefaultDatabaseEngine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/db_GenericDTSupplier.cpp
)

SET(DDM_DB_HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/db_AbstractDTSupplier.h
  ${CMAKE_CURRENT_SOURCE_DIR}/db_AbstractSQLGenerator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/db_AdvancedSqlGenerator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/db_BasicSqlGenerator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/db_Codepage.h
  ${CMAKE_CURRENT_SOURCE_DIR}/db_DatabaseBuiltinFunction.h
  ${CMAKE_CURRENT_SOURCE_DIR}/db_DatabaseBuiltinFunctionsParameter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/db_DatabaseEngine.h
  ${CMAKE_CURRENT_SOURCE_DIR}/db_DefaultDatabaseEngine.h
  ${CMAKE_CURRENT_SOURCE_DIR}/db_SpCollection.h
  ${CMAKE_CURRENT_SOURCE_DIR}/db_GenericDTSupplier.h
  ${CMAKE_CURRENT_SOURCE_DIR}/db_SP.h
)

SET(DDM_DB_MOC_HDRS
)

#Put here QRC files to be Rcc-ed
SET(DDM_DB_RCC
)

#Put here UI files
SET(DDM_DB_UIS

)

LIST( APPEND DDM_SRCS
             ${DDM_DB_SRCS} )

LIST( APPEND DDM_HDRS
             ${DDM_DB_HDRS} )

LIST( APPEND DDM_MOC_HDRS
	     ${DDM_DB_MOC_HDRS} )

LIST( APPEND DDM_UIS
             ${DDM_DB_UIS} )

set(DDM_SRCS ${DDM_SRCS} CACHE INTERNAL DDM_SRCS)
set(DDM_HDRS ${DDM_HDRS} CACHE INTERNAL DDM_HDRS)
set(DDM_MOC_HDRS ${DDM_MOC_HDRS} CACHE INTERNAL DDM_MOC_HDRS)
set(DDM_UIS ${DDM_UIS} CACHE INTERNAL DDM_UIS)

