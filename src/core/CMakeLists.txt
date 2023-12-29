#
# DDM: CMake build system template for Qt based projects
# Author: Rohit Yadav <rohityadav89 AT gmail.com>
#

#Put here CPPs
SET(DDM_CORE_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Column.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Deployer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_DeployerThread.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Diagram.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Function.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_InjectSqlGenerator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_InjectSqlGeneratorThread.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_ObjectWithUid.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Patch.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Procedure.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_StoredMethod.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Table.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_TableInstance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Trigger.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_UserDataType.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_View.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DefaultVersionImplementation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentInitiator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ForeignKey.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Index.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MajorVersion.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ObjectWithSpInstances.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Project.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Solution.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SpInstance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TrueFalseSp.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TrueFalseSpInstance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ValueListSp.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ValueListSpInstance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ValueSp.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ValueSpInstance.cpp
  
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Repository.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Entity.cpp  
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Role.cpp
)

SET(DDM_CORE_HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/ActionDeleteTable.h
  ${CMAKE_CURRENT_SOURCE_DIR}/CopyableElement.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_CloneableElement.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_ColumnWithValue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Diagram.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Function.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_ItemWithDescription.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_LockableElement.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_ObjectWithUid.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_ParameterAndDescription.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Patch.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Procedure.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_SqlSource.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_StoredMethod.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_TableInstance.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Trigger.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_UserDataType.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DefaultVersionImplementation.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ForeignKey.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MajorVersion.h
  ${CMAKE_CURRENT_SOURCE_DIR}/NamedItem.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ObjectWithSpInstances.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Project.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SerializableElement.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Solution.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SpInstance.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TrueFalseSp.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TrueFalseSpInstance.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ValueListSp.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ValueListSpInstance.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ValueSp.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ValueSpInstance.h
  ${CMAKE_CURRENT_SOURCE_DIR}/VersionData.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Version.h
  
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Repository.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Entity.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Role.h
)

SET(DDM_CORE_MOC_HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Deployer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_DeployerThread.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentInitiator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_InjectSqlGenerator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_InjectSqlGeneratorThread.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Column.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_View.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Table.h
  ${CMAKE_CURRENT_SOURCE_DIR}/core_Index.h
)

#Put here QRC files to be Rcc-ed
SET(DDM_RCC
)

#Put here UI files
SET(DDM_CORE_UIS
)

LIST( APPEND DDM_SRCS ${DDM_CORE_SRCS} )
LIST( APPEND DDM_HDRS ${DDM_CORE_HDRS} )
LIST( APPEND DDM_MOC_HDRS ${DDM_CORE_MOC_HDRS} )
LIST( APPEND DDM_UIS ${DDM_CORE_UIS} )

set(DDM_SRCS ${DDM_SRCS} CACHE INTERNAL DDM_SRCS)
set(DDM_HDRS ${DDM_HDRS} CACHE INTERNAL DDM_HDRS)
set(DDM_MOC_HDRS ${DDM_MOC_HDRS} CACHE INTERNAL DDM_MOC_HDRS)
set(DDM_UIS ${DDM_UIS} CACHE INTERNAL DDM_UIS)
