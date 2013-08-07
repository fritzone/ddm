#
# DDM: CMake build system template for Qt based projects
# Author: Rohit Yadav <rohityadav89 AT gmail.com>
#

#Put here CPPs
SET(DDM_DIAGRAM_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DiagramFKDescriptor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DiagramItemFactory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DiagramNoteDescriptor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DiagramTableDescriptor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DraggableGraphicsItem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DraggableGraphicsViewItemForForeignKey.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DraggableGraphicsViewItemForText.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_ERGraphicsScene.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_ERGraphicsView.cpp
)

SET(DDM_DIAGRAM_HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DiagramFKDescriptor.h
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DiagramItemFactory.h
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DiagramNoteDescriptor.h
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DiagramObjectDescriptor.h
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DiagramTableDescriptor.h
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DraggableGraphicsItemForForeignKey.h
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DraggableGraphicsItemForText.h
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_DraggableGraphicsItem.h
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_ERGraphicsScene.h
  ${CMAKE_CURRENT_SOURCE_DIR}/dgram_ERGraphicsView.h
)

SET(DDM_DIAGRAM_MOC_HDRS
)

#Put here QRC files to be Rcc-ed
SET(DDM_DIAGRAM_RCC
)

#Put here UI files
SET(DDM_DIAGRAM_UIS

)

LIST( APPEND DDM_SRCS
             ${DDM_DIAGRAM_SRCS} )

LIST( APPEND DDM_HDRS
             ${DDM_DIAGRAM_HDRS} )

LIST( APPEND DDM_MOC_HDRS
	     ${DDM_DIAGRAM_MOC_HDRS} )

LIST( APPEND DDM_UIS
             ${DDM_DIAGRAM_UIS} )

set(DDM_SRCS ${DDM_SRCS} CACHE INTERNAL DDM_SRCS)
set(DDM_HDRS ${DDM_HDRS} CACHE INTERNAL DDM_HDRS)
set(DDM_MOC_HDRS ${DDM_MOC_HDRS} CACHE INTERNAL DDM_MOC_HDRS)
set(DDM_UIS ${DDM_UIS} CACHE INTERNAL DDM_UIS)

