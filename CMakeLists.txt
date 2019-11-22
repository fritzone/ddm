CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# Project Info
SET(PROJECT_NAME_SHORT   "DDM")
SET(PROJECT_NAME_LONG    "Database Deployment Manager")
SET(PROJECT_DESCRIPTION  "A tool for managing database deployments")
SET(PROJECT_COPYRIGHT    "Copyright (C) 2010-2019 Ferenc Deak")
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
SET(CPACK_PACKAGE_VERSION_MINOR "1k")
SET(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
SET(CPACK_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR})
SET(PROJECT_VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR})
SET(PROJECT_VERSION ${CPACK_PACKAGE_VERSION})

MESSAGE(STATUS "Version: ${PROJECT_VERSION}")

STRING(TOLOWER ${PROJECT_NAME_SHORT} PROJECT_NAME_SHORT_LOWER)
PROJECT(${PROJECT_NAME_SHORT_LOWER}-${PROJECT_VERSION})

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

find_package(Qt5Core QUIET)
find_package(Qt5Sql QUIET)
find_package(Qt5Widgets QUIET)
find_package(Qt5Gui QUIET)
find_package(Qt5Network QUIET)
find_package(Qt5Svg QUIET)
find_package(Qt5Webkit QUIET)


if (Qt5Core_FOUND)

    find_package(Qt5 COMPONENTS Core Widgets Sql Gui Network Xml PrintSupport WebEngine WebKitWidgets REQUIRED)

    if(Qt5_FOUND AND WIN32 AND TARGET Qt5::qmake AND NOT TARGET Qt5::windeployqt)
      get_target_property(_qt5_qmake_location  Qt5::qmake IMPORTED_LOCATION)

      execute_process(
          COMMAND "${_qt5_qmake_location}" -query QT_INSTALL_PREFIX
          RESULT_VARIABLE return_code
          OUTPUT_VARIABLE qt5_install_prefix
          OUTPUT_STRIP_TRAILING_WHITESPACE
      )

      set(imported_location "${qt5_install_prefix}/bin/windeployqt.exe")

      if(EXISTS ${imported_location})
          add_executable(Qt5::windeployqt IMPORTED)

          set_target_properties(Qt5::windeployqt PROPERTIES
              IMPORTED_LOCATION ${imported_location}
          )
      endif()
    endif()

    message("Found Qt5")
    set(QT_LIBRARIES 
        ${Qt5Core_LIBRARIES} 
        ${Qt5Network_LIBRARIES} 
        ${Qt5Widgets_LIBRARIES} 
        ${Qt5Sql_LIBRARIES} 
        ${Qt5Svg_LIBRARIES} 
        ${Qt5Webkit_LIBRARIES} 
        ${Qt5Xml_LIBRARIES}
        ${Qt5PrintSupport_LIBRARIES}
        ${Qt5PrintWebEngine_LIBRARIES}
        ${Qt5WebKitWidgets_LIBRARIES}
    )

    message(${QT_LIBRARIES})

    include_directories(
        ${Qt5Core_INCLUDE_DIRS} 
        ${Qt5Sql_INCLUDE_DIRS} 
        ${Qt5Svg_INCLUDE_DIRS}
        ${Qt5Xml_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Webkit_INCLUDE_DIRS}
    )


    macro(qt_add_resources)
       qt5_add_resources(${ARGN})
    endmacro()

    macro(qt_wrap_cpp)
        qt5_wrap_cpp(${ARGN})
    endmacro()

    macro(qt_wrap_ui)
        qt5_wrap_ui(${ARGN})
    endmacro()

  get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
  if ("${QT_ROOT}" STREQUAL "")
      set(QT_ROOT ${QT_QTCORE_INCLUDE_DIR}/../..)
  endif()
  include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS})
endif()


# Compile flags
if(NOT WIN32)

  set(COMMON_COMPILER_FLAGS "-std=c++17 -ggdb -pedantic -Wno-old-style-cast -fPIC")
  
  # C++11 for linux common for both gcc and clang
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

    if(CLANG_SANITIZE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILER_FLAGS} -fsanitize=address -fno-omit-frame-pointer -O1 -fno-optimize-sibling-calls")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILER_FLAGS} -fPIC -fsanitize=address -fno-omit-frame-pointer -O1 -fno-optimize-sibling-calls")
    endif()

  else()
    if(COVERAGE_BUILD)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILER_FLAGS} -ftest-coverage -fprofile-arcs -fpermissive -finline-functions -Wno-long-long -fvisibility-inlines-hidden")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILER_FLAGS} -fpermissive -finline-functions -Wno-long-long -fvisibility-inlines-hidden")
    endif()
  endif()

  MESSAGE("CMAKE FLAGS: ${CMAKE_CXX_FLAGS}")

  add_definitions("-D_GLIBCXX_USE_CXX11_ABI")

else() # Windows part of compilation flags

    add_definitions("/D_CRT_SECURE_NO_WARNINGS")

endif()

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


# adding the files
add_subdirectory(src)

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
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt4-sql-mysql, libmysqlclient18, libqt4-network, libqt4-sql, libqt4-sql-sqlite, libqt4-xml, libqtcore4, libqtgui4, libqtwebkit4") # Specify dependencies here
    SET(CPACK_SET_DESTDIR TRUE)
    SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "debian/postinst")
ENDIF(DPKG_FOUND AND NOT WIN32)

SET(CPACK_PACKAGE_EXECUTABLES "ddm" "DDM")
INCLUDE(CPack)
