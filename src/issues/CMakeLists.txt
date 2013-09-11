#
# DDM: CMake build system template for Qt based projects
# Author: Rohit Yadav <rohityadav89 AT gmail.com>
#

#Put here CPPs
SET(DDM_ISSUES_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/ConnectionIssue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DatabaseNormalizationIssue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ForeignKeyReccomendationIssue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/IssueOriginator.cpp
)

SET(DDM_ISSUES_HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/ConnectionIssue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DatabaseNormalizationIssue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ForeignKeyReccomendationIssue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Issue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/IssueManager.h
  ${CMAKE_CURRENT_SOURCE_DIR}/IssueOriginator.h
)

SET(DDM_ISSUES_MOC_HDRS
)

#Put here QRC files to be Rcc-ed
SET(DDM_ISSUES_RCC
)

#Put here UI files
SET(DDM_ISSUES_UIS

)

LIST( APPEND DDM_SRCS
             ${DDM_ISSUES_SRCS} )

LIST( APPEND DDM_HDRS
             ${DDM_ISSUES_HDRS} )

LIST( APPEND DDM_MOC_HDRS
	     ${DDM_ISSUES_MOC_HDRS} )

LIST( APPEND DDM_UIS
             ${DDM_ISSUES_UIS} )

set(DDM_SRCS ${DDM_SRCS} CACHE INTERNAL DDM_SRCS)
set(DDM_HDRS ${DDM_HDRS} CACHE INTERNAL DDM_HDRS)
set(DDM_MOC_HDRS ${DDM_MOC_HDRS} CACHE INTERNAL DDM_MOC_HDRS)
set(DDM_UIS ${DDM_UIS} CACHE INTERNAL DDM_UIS)
