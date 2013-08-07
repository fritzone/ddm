#
# DDM: CMake build system template for Qt based projects
# Author: Rohit Yadav <rohityadav89 AT gmail.com>
#

#Put here CPPs
SET(DDM_QUERY_BUILDER_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellAsCommand.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellClose.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellCommand.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellForSingleExpression.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellFromCommand.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellGroupByCommand.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellJoinCommand.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellOrderByCommand.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellQuerySmallOptionsBox.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellSelectCommand.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellTableChooser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellTable.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellTypeChooser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellWhereCommand.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_DatabaseFunctionInstantiationComponent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryAsGenerator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryComponents.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_Query.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryGraphicsHelper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryGraphicsItem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryGraphicsScene.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryGraphicsView.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryItemListDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryTextInputItem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryAsComponent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQuery.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryFromComponent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryGraphicsItem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryGroupByComponent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryJoinComponent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryOrderByComponent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQuerySelectComponent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryWhereComponent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SingleExpressionQueryComponent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_TableGraphicsItem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_TableQueryComponent.cpp
)

SET(DDM_QUERY_BUILDER_HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/ColumnProviderForQuery.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellAsCommand.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellClose.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellCommand.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellForSingleExpression.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellFromCommand.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellGroupByCommand.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellJoinCommand.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellOrderByCommand.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellQuerySmallOptionsBox.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellSelectCommand.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellTableChooser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellTable.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellTypeChooser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_CellWhereCommand.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_DatabaseFunctionInstantiationComponent.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_OptionTypes.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryAsGenerator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryComponents.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryGraphicsHelper.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryGraphicsItem.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryGraphicsScene.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryGraphicsView.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_Query.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryAsComponent.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryFromComponent.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryGraphicsItem.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryGroupByComponent.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQuery.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryJoinComponent.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryOrderByComponent.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQuerySelectComponent.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SelectQueryWhereComponent.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_SingleExpressionQueryComponent.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_TableGraphicsItem.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_TableQueryComponent.h
)

SET(DDM_QUERY_BUILDER_MOC_HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryItemListDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qbr_QueryTextInputItem.h
)

#Put here QRC files to be Rcc-ed
SET(DDM_QUERY_BUILDER_RCC
)

#Put here UI files
SET(DDM_QUERY_BUILDER_UIS
  ${CMAKE_CURRENT_SOURCE_DIR}/QueryItemListDialog.ui
)

LIST( APPEND DDM_SRCS
             ${DDM_QUERY_BUILDER_SRCS} )

LIST( APPEND DDM_HDRS
             ${DDM_QUERY_BUILDER_HDRS} )

LIST( APPEND DDM_MOC_HDRS
	     ${DDM_QUERY_BUILDER_MOC_HDRS} )

LIST( APPEND DDM_UIS
             ${DDM_QUERY_BUILDER_UIS} )

set(DDM_SRCS ${DDM_SRCS} CACHE INTERNAL DDM_SRCS)
set(DDM_HDRS ${DDM_HDRS} CACHE INTERNAL DDM_HDRS)
set(DDM_MOC_HDRS ${DDM_MOC_HDRS} CACHE INTERNAL DDM_MOC_HDRS)
set(DDM_UIS ${DDM_UIS} CACHE INTERNAL DDM_UIS)

