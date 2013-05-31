#
# DDM: CMake build system template for Qt based projects
# Author: Rohit Yadav <rohityadav89 AT gmail.com>
#

#Put here CPPs
SET(DDM_GUI_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/AboutBoxDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/BrowseTableForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ConnectionGuiElements.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ContextMenuCollection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ContextMenuDelegate.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CreateTableInstancesDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DataTypesListForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DiagramForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DocumentationForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DynamicActionHandlerforMainWindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/EnterNoteTextDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/FrameForLineNumbers.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GuiElements.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gui_HelpWindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/InjectSqlDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ListWidgetForCodeCompletion.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindowButtonDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/NamedObjectListingForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/NewDataTypeForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/NewProjectDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/NewTableForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/NewViewForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PreferencesDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ProcedureForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ProjectDetailsForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SimpleTextInputDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SqlForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SqlHighlighter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TableComparisonForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TableInstanceForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TextEditWithCodeCompletion.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TreeItem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TriggerForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/VersionGuiElements.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/WidgetForSpecificProperties.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/repo_DatabaseDetailsForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/repo_RoleListingForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/repo_RepositoryElementForm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/RepositoryGuiElements.cpp

)

SET(DDM_GUI_HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/ConnectionGuiElements.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ContextMenuCollection.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ContextMenuEnabledTreeWidget.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DocumentationSourceTreeItem.h
  ${CMAKE_CURRENT_SOURCE_DIR}/GuiElements.h
  ${CMAKE_CURRENT_SOURCE_DIR}/IconFactory.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SimpleTextInputDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SourceCodePresenterWidget.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SqlHighlighter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SqlSourceTreeItem.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TableListWidget.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TreeItem.h
  ${CMAKE_CURRENT_SOURCE_DIR}/VersionGuiElements.h
  
  ${CMAKE_CURRENT_SOURCE_DIR}/RepositoryGuiElements.h
)

SET(DDM_GUI_MOC_HDRS
  ${CMAKE_CURRENT_SOURCE_DIR}/AboutBoxDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/BrowseTableForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/CreateTableInstancesDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DataTypesListForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DiagramForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DocumentationForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DynamicActionHandlerForMainWindow.h
  ${CMAKE_CURRENT_SOURCE_DIR}/EnterNoteTextDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/FrameForLineNumbers.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gui_HelpWindow.h
  ${CMAKE_CURRENT_SOURCE_DIR}/InjectSqlDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ListWidgetForCodeCompletion.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindowButtonDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.h
  ${CMAKE_CURRENT_SOURCE_DIR}/NamedObjectListingForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/NewDataTypeForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/NewProjectDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/NewTableForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/NewViewForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/PreferencesDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ProcedureForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ProjectDetailsForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SimpleTextInputDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SqlForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TableComparisonForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TableInstanceForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TextEditWithCodeCompletion.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TriggerForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/WidgetForSpecificProperties.h
  
  ${CMAKE_CURRENT_SOURCE_DIR}/repo_DatabaseDetailsForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/repo_RoleListingForm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/repo_RepositoryElementForm.h
)

#Put here QRC files to be Rcc-ed
SET(DDM_RCC
)

#Put here UI files
SET(DDM_GUI_UIS
  ${CMAKE_CURRENT_SOURCE_DIR}/AboutBoxDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/BrowseTableForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/CreateTableInstancesDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/DataTypesListForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/DiagramForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/DocumentationForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/EnterNoteTextDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/HelpWindow.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/InjectSqlDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindowButtonDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/NamedObjectListingForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/NewDataTypeForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/NewProjectDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/NewTableForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/NewViewForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/PreferencesDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/ProcedureForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/ProjectDetailsForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/SimpleTextInputDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/SqlForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/TableComparisonForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/TableInstanceForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/TriggerForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/WidgetForSpecificProperties.ui
  
  ${CMAKE_CURRENT_SOURCE_DIR}/repo_DatabaseDetailsForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/repo_RoleListingForm.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/repo_RepositoryElementForm.ui
  
)

LIST( APPEND DDM_SRCS	
             ${DDM_GUI_SRCS} )

LIST( APPEND DDM_HDRS
             ${DDM_GUI_HDRS} )

LIST( APPEND DDM_MOC_HDRS
	     ${DDM_GUI_MOC_HDRS} )

LIST( APPEND DDM_UIS
             ${DDM_GUI_UIS} )

set(DDM_SRCS ${DDM_SRCS} CACHE INTERNAL DDM_SRCS)
set(DDM_HDRS ${DDM_HDRS} CACHE INTERNAL DDM_HDRS)
set(DDM_MOC_HDRS ${DDM_MOC_HDRS} CACHE INTERNAL DDM_MOC_HDRS)
set(DDM_UIS ${DDM_UIS} CACHE INTERNAL DDM_UIS)
