SET(DDM_SRCS "" CACHE INTERNAL DDM_SRCS)
SET(DDM_HDRS "" CACHE INTERNAL DDM_HDRS)
SET(DDM_RCC "" CACHE INTERNAL DDM_RCC)
SET(DDM_UIS "" CACHE INTERNAL DDM_UIS)

# add the subdirectories
add_subdirectory(core)
add_subdirectory(connections)
add_subdirectory(gui)
add_subdirectory(issues)
add_subdirectory(db)
add_subdirectory(diagram)
add_subdirectory(query_builder)
add_subdirectory(qhtml)
add_subdirectory(dbmysql)
add_subdirectory(dbsqlite)
add_subdirectory(dbcubrid)
add_subdirectory(reverse_eng)

#Put here CPPs
LIST( APPEND DDM_SRCS
  db_DatabaseEngineManager.cpp
  ClipboardFactory.cpp
  Configuration.cpp
  DeserializationFactory.cpp
  DocumentationGenerator.cpp
  FkRelationDescriptor.cpp
  helper_MostRecentlyUsedFiles.cpp
  helper_utils.cpp
  IssueManager.cpp
  main.cpp
  NameGenerator.cpp
  SqlNamesValidator.cpp
  StartupValuesHelper.cpp
  TableUpdateGenerator.cpp
  UidWarehouse.cpp
  VersionUpdateGenerator.cpp
  Workspace.cpp
)

LIST( APPEND DDM_HDRS
  db_DatabaseEngineManager.h
  ClipboardFactory.h
  commons.h
  Configuration.h
  core_enums.h
  DeserializationFactory.h
  DocumentationGenerator.h
  FkRelationDescriptor.h
  gui_colors.h
  helper_MostRecentlyUsedFiles.h
  helper_utils.h
  NameGenerator.h
  SqlNamesValidator.h
  StartupValuesHelper.h
  strings.h
  TableUpdateGenerator.h
  uids.h
  UidWarehouse.h
  utils.h
  VersionUpdateGenerator.h
  Workspace.h
)

LIST( APPEND DDM_MOC_HDRS
  Workspace.h
)


#Put here QRC files to be Rcc-ed
set(DDM_RCC
  dbm.qrc
  docstyles.qrc
  help_resources.qrc
  dbm.rc    
)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/gui
  ${CMAKE_CURRENT_SOURCE_DIR}/core
  ${CMAKE_CURRENT_SOURCE_DIR}/connections
  ${CMAKE_CURRENT_SOURCE_DIR}/issues
  ${CMAKE_CURRENT_SOURCE_DIR}/db
  ${CMAKE_CURRENT_SOURCE_DIR}/diagram
  ${CMAKE_CURRENT_SOURCE_DIR}/query_builder
  ${CMAKE_CURRENT_SOURCE_DIR}/qhtml
  ${CMAKE_CURRENT_SOURCE_DIR}/dbmysql
  ${CMAKE_CURRENT_SOURCE_DIR}/dbsqlite
  ${CMAKE_CURRENT_SOURCE_DIR}/dbcubrid
  ${CMAKE_CURRENT_SOURCE_DIR}/reverse_eng
)

# CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

#Put here UI files
#LIST( APPEND DDM_UIS
#)

set(DDM_SRCS ${DDM_SRCS} CACHE INTERNAL DDM_SRCS)
set(DDM_HDRS ${DDM_HDRS} CACHE INTERNAL DDM_HDRS)
set(DDM_MOC_HDRS ${DDM_MOC_HDRS} CACHE INTERNAL DDM_MOC_HDRS)
set(DDM_UIS ${DDM_UIS} CACHE INTERNAL DDM_UIS)

QT4_WRAP_UI(DDM_UIS_H ${DDM_UIS})
QT4_ADD_RESOURCES(DDM_RCC_SRCS ${DDM_RCC})
QT4_WRAP_CPP(DDM_MOC_SRCS ${DDM_MOC_HDRS})

ADD_EXECUTABLE(ddm ${DDM_SRCS} ${DDM_RCC_SRCS} ${DDM_UIS_H} ${DDM_MOC_SRCS}
)

# # Add translations
ADD_DEPENDENCIES(ddm translations)

# Link the target against the Qt libraries
TARGET_LINK_LIBRARIES(ddm ${DDM_LIBS})

# Now we are done.

INSTALL(TARGETS ddm
        BUNDLE  DESTINATION ${DDM_BIN_DIR}
        RUNTIME DESTINATION ${DDM_BIN_DIR})

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_SOURCE_DIR}/bin/ddm
    COMMAND ${CMAKE_COMMAND} copy ${CMAKE_CURRENT_SOURCE_DIR}/ddm ${CMAKE_SOURCE_DIR}/bin/ddm
    DEPENDS ${CMAKE_BINARY_DIR}/src/ddm
)

ADD_DEFINITIONS(-W -Wall)
