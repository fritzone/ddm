CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# Project Info
SET(PROJECT_NAME_SHORT   "DDM")
SET(PROJECT_NAME_LONG    "Database Deployment Manager")
SET(PROJECT_DESCRIPTION  "A tool for managing database deployments")
SET(PROJECT_COPYRIGHT    "Copyright (C) 2010-2013 Ferenc Deak")
SET(PROJECT_CONTACT      "fritzone@gmail.com")
SET(PROJECT_VENDOR       "Ferenc Deak")
SET(ORG_WEBSITE          "http://ddmproject.weebly.com/")

# Configuring build type
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
    MESSAGE("Setting build type to '${CMAKE_BUILD_TYPE}'")
ELSE(NOT CMAKE_BUILD_TYPE)
    MESSAGE("Build type set to '${CMAKE_BUILD_TYPE}'")
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(WIN32)
    # Building in release for WIN32
    SET(CMAKE_BUILD_TYPE "Release")
    SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG)
    # Use mingw's sprintf instead of windows's
    ADD_DEFINITIONS( -D__USE_MINGW_ANSI_STDIO=1 )
ENDIF(WIN32)

# For projects using Git, use git version patch
IF("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    SET(PATCH_OUT "0")
ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "Release")

SET(PROJECT_VERSION_PATCH ${PATCH_OUT})

# Project Version
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1j")
SET(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
SET(CPACK_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR})
SET(PROJECT_VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR})
SET(PROJECT_VERSION ${CPACK_PACKAGE_VERSION})

MESSAGE(STATUS "Version: ${PROJECT_VERSION}")

STRING(TOLOWER ${PROJECT_NAME_SHORT} PROJECT_NAME_SHORT_LOWER)
PROJECT(${PROJECT_NAME_SHORT_LOWER}-${PROJECT_VERSION})

# Codename of the Project
SET(CODENAME "treefrog")

# System on which project is built
SITE_NAME(HOSTNAME)
SET(SYSNAME ${CMAKE_SYSTEM}-${CMAKE_SYSTEM_PROCESSOR})

# CMake Settings
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)

# Set path to additional CMake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Path to contribs
SET(CONTRIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/contribs)

IF (UNIX)
    SET(WITH_PROFILING FALSE CACHE BOOL "Build with profiling support" )
    SET(CMAKE_INSTALL_PREFIX /opt/ddm)
ENDIF(UNIX)

# Search and include Qt4 parameters
# Set Qt min required version and add modules here

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtXml QtSvg QtNetwork QtSql QtWebkit REQUIRED )

IF (NOT WIN32)
    #FindQt4.cmake is so brain-dead when Xcompiling
    SET(DDM_LIBS ${QT_QTCORE_LIBRARY}
                     ${QT_QTGUI_LIBRARY}
                     ${QT_QTSVG_LIBRARY}
                     ${QT_QTNETWORK_LIBRARY}
                     ${QT_QTSQL_LIBRARY}
                     ${QT_QTWEBKIT_LIBRARY}
                     ${QT_QTXML_LIBRARY}) # Add here any other QT module or other library
    INCLUDE(${QT_USE_FILE})
    ADD_DEFINITIONS(${QT_DEFINITIONS})
ELSE (NOT WIN32)
    SET (DDM_LIBS QtNetwork QtSql QtWebkit QtXml QtGui QtCore 
            ws2_32 winmm imm32 libeay32 ssleay32 crypt32 gdi32)
ENDIF(NOT WIN32)

# Copy the sh launcher
IF(NOT WIN32)
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmakeqt.sh" "${CMAKE_CURRENT_BINARY_DIR}/cmakeqt" COPYONLY)
ENDIF(NOT WIN32)

# Layout of CMakeQt installation
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set common dirs for MAC and UNIX
SET(DEFAULT_BIN_SUBDIR     bin)
SET(DEFAULT_LIB_SUBDIR     lib)
SET(DEFAULT_DATA_SUBDIR    share)
SET(DEFAULT_SRC_SUBDIR     src)
SET(DEFAULT_INCLUDE_SUBDIR include)

# Set DDM path params
SET(DDM_BIN_SUBDIR     "${DEFAULT_BIN_SUBDIR}"        CACHE STRING "Subdirectory where executables will be installed")
SET(DDM_LIB_SUBDIR     "${DEFAULT_LIB_SUBDIR}"        CACHE STRING "Subdirectory where libraries will be installed")
SET(DDM_DATA_SUBDIR    "${DEFAULT_DATA_SUBDIR}"       CACHE STRING "Subdirectory where DDM's data will be installed")
SET(DDM_SRC_SUBDIR     "${DEFAULT_SRC_SUBDIR}"        CACHE STRING "Subdirectory where DDM's source is")
SET(DDM_INCLUDE_SUBDIR "${DEFAULT_INCLUDE_SUBDIR}"    CACHE STRING "Subdirectory where header files will be installed")
SET(DDM_DESKTOP_DIR     "/usr/share/applications/")
SET(DDM_PIXMAPS_DIR     "/usr/share/icons/")

# Full paths for the installation
SET(DDM_BIN_DIR     ${CMAKE_INSTALL_PREFIX}/${DDM_BIN_SUBDIR})
SET(DDM_RSRC_DIR    ${CMAKE_INSTALL_PREFIX}/${DDM_BIN_SUBDIR}/rsrc/)
SET(DDM_DOC_DIR     ${CMAKE_INSTALL_PREFIX}/${DDM_BIN_SUBDIR}/doc/)

OPTION(DDM_ENABLE_CUBRID_SUPPORT "Enable CUBRID support")
OPTION(DDM_ENABLE_UNIT_TESTS "Enable Unit tests")

# Recursively add sub dirs
# enable the next one when a translations is done
add_subdirectory(src)
if(DDM_ENABLE_CUBRID_SUPPORT)
  add_subdirectory(cubrid)
endif()

if(DDM_ENABLE_UNIT_TESTS)
  enable_testing()
  add_subdirectory(unit_tests)
endif()

# Install the binary
INSTALL(FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ddm DESTINATION ${DDM_BIN_DIR})

# copy the resources
FILE(GLOB res_files "${CMAKE_CURRENT_SOURCE_DIR}/src/rsrc/*")
INSTALL(FILES ${res_files} DESTINATION ${DDM_RSRC_DIR})

# Copy the help
FILE(GLOB help_files "${CMAKE_CURRENT_SOURCE_DIR}/src/doc/*")
INSTALL(FILES ${help_files} DESTINATION ${DDM_DOC_DIR})

# Copy .desktop file
INSTALL (FILES share/ddm.desktop DESTINATION ${DDM_DESKTOP_DIR})
INSTALL (FILES share/ddm.xml DESTINATION ${DDM_DESKTOP_DIR})

# Copy the ddm pixmap
INSTALL (FILES share/ddm.png DESTINATION ${DDM_PIXMAPS_DIR})

# try to set up the menu system
find_program(XDG-MIME_EXECUTABLE xdg-mime)
find_program(XDG-DESKTOP-MENU_EXECUTABLE xdg-desktop-menu)

INSTALL(CODE "
  execute_process(COMMAND ${XDG-MIME_EXECUTABLE} install --novendor ${DDM_DESKTOP_DIR}/ddm.xml)
  execute_process(COMMAND ${XDG-DESKTOP-MENU_EXECUTABLE} install --novendor ${DDM_DESKTOP_DIR}/ddm.desktop)
  execute_process(COMMAND ${XDG-MIME_EXECUTABLE} default ${DDM_DESKTOP_DIR}/ddm.desktop application/x-ddm-item)
  "
)


#############################################################
# Enable packaging

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
SET(CPACK_PACKAGE_NAME ${PROJECT_NAME_SHORT})
SET(CPACK_PACKAGE_DESCRIPTION "${PROJECT_NAME_LONG}")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME_SHORT} \n ${PROJECT_DESCRIPTION}")
SET(CPACK_PACKAGE_VENDOR ${PROJECT_VENDOR})
SET(CPACK_PACKAGE_CONTACT ${PROJECT_CONTACT})
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

# Force Package Name
SET(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${CPACK_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

# RPM packages
INCLUDE(${CMAKE_MODULE_PATH}/RpmBuild.cmake)
IF(RPMBUILD_FOUND AND NOT WIN32)
    SET(CPACK_GENERATOR "RPM")
    SET(CPACK_RPM_PACKAGE_SUMMARY ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
    SET(CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
    SET(CPACK_RPM_PACKAGE_VERSION ${PROJECT_VERSION})
    SET(CPACK_RPM_PACKAGE_LICENSE "GPLv2+ License")
    SET(CPACK_RPM_PACKAGE_GROUP ${PROJECT_VENDOR})
    SET(CPACK_RPM_PACKAGE_VENDOR ${PROJECT_VENDOR})
    SET(CPACK_RPM_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
    SET(CPACK_RPM_PACKAGE_DEPENDS "qt ( >= 4.6 ), qt-x11 ( >= 4.6 )") # Specify dependencies here
    SET(CPACK_SET_DESTDIR TRUE)
ENDIF(RPMBUILD_FOUND AND NOT WIN32)

# Debian packages
INCLUDE (${CMAKE_MODULE_PATH}/DpkgBuild.cmake)
IF(DPKG_FOUND AND NOT WIN32)
    SET(CPACK_GENERATOR "DEB")
    SET(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
    SET(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
    SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
    SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    SET(CPACK_DEBIAN_PACKAGE_DEBUG ON)
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS ${PACKAGE_REQUIRES})
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt4-sql-mysql, libmysqlclient18, libqt4-dbus, libqt4-network, libqt4-sql, libqt4-sql-mysql, libqt4-xml, libqtcore4, libqtgui4, libqtwebkit4") # Specify dependencies here
    SET(CPACK_SET_DESTDIR TRUE)
ENDIF(DPKG_FOUND AND NOT WIN32)

SET(CPACK_PACKAGE_EXECUTABLES "ddm" "DDM")
INCLUDE(CPack)
