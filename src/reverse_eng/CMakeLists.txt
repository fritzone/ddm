#
# DDM: CMake build system template for Qt based projects
# Author: Rohit Yadav <rohityadav89 AT gmail.com>
#

#Put here CPPs
SET(DDM_REVENG_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineerer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineererThread.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineerWizard.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineerWizardDatabasesForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineerWizardObjectListForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineerWizardOptionsForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineerWizardWelcomeForm.cpp
)

SET(DDM_REVENG_HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineerWizard.h
)

SET(DDM_REVENG_MOC_HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineerer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineererThread.h
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineerWizardDatabasesForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineerWizardObjectListForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineerWizardOptionsForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/reveng_ReverseEngineerWizardWelcomeForm.h
)

#Put here QRC files to be Rcc-ed
SET(DDM_REVENG_RCC
)

#Put here UI files
SET(DDM_REVENG_UIS
  ${CMAKE_CURRENT_SOURCE_DIR}/ReverseEngineerWizardDatabasesForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/ReverseEngineerWizardObjectListForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/ReverseEngineerWizardOptionsForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/ReverseEngineerWizardWelcomeForm.ui
)

LIST( APPEND DDM_SRCS
             ${DDM_REVENG_SRCS} )

LIST( APPEND DDM_HDRS
             ${DDM_REVENG_HDRS} )

LIST( APPEND DDM_MOC_HDRS
	     ${DDM_REVENG_MOC_HDRS} )

LIST( APPEND DDM_UIS
             ${DDM_REVENG_UIS} )

set(DDM_SRCS ${DDM_SRCS} CACHE INTERNAL DDM_SRCS)
set(DDM_HDRS ${DDM_HDRS} CACHE INTERNAL DDM_HDRS)
set(DDM_MOC_HDRS ${DDM_MOC_HDRS} CACHE INTERNAL DDM_MOC_HDRS)
set(DDM_UIS ${DDM_UIS} CACHE INTERNAL DDM_UIS)

