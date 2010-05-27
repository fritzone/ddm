# -------------------------------------------------
# Project created by QtCreator 2009-11-25T12:54:30
# -------------------------------------------------
QT += network \
    sql \
    xml
TARGET = dbm
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    MainWindowButtonDialog.cpp \
    NewProjectDialog.cpp \
    newdatatypeform.cpp \
    datatypeslistform.cpp \
    AbstractDTSupplier.cpp \
    DataType.cpp \
    MySQLDTSupplier.cpp \
    UserDataType.cpp \
    Project.cpp \
    MajorVersion.cpp \
    Version.cpp \
    AbstractCodepageSupplier.cpp \
    MySQLCodepageSupplier.cpp \
    Codepage.cpp \
    DatabaseEngine.cpp \
    Configuration.cpp \
    Table.cpp \
    TablesListForm.cpp \
    NewTableForm.cpp \
    Column.cpp \
    AbstractIndextypeProvider.cpp \
    Index.cpp \
    DeserializationFactory.cpp \
    ProjectDetailsForm.cpp
HEADERS += MainWindow.h \
    MainWindowButtonDialog.h \
    NewProjectDialog.h \
    NewDatatypeForm.h \
    datatypeslistform.h \
    AbstractDTSupplier.h \
    DataType.h \
    MySQLDTSupplier.h \
    UserDataType.h \
    strings.h \
    Project.h \
    MajorVersion.h \
    Version.h \
    AbstractCodepageSupplier.h \
    MySQLCodepageSupplier.h \
    Codepage.h \
    DatabaseEngine.h \
    Configuration.h \
    Table.h \
    TablesListForm.h \
    NewTableForm.h \
    IconFactory.h \
    TreeItem.h \
    Column.h \
    AbstractIndextypeProvider.h \
    MySQLIndextypeProvider.h \
    Index.h \
    SerializableElement.h \
    NamedItem.h \
    DeserializationFactory.h \
    ProjectDetailsForm.h \
    ForeignKey.h
FORMS += mainwindow.ui \
    mainwindowbuttondialog.ui \
    newprojectdialog.ui \
    newdatatypeform.ui \
    datatypeslistform.ui \
    TablesListForm.ui \
    NewTableForm.ui \
    ProjectDetailsForm.ui
RESOURCES += dbm.qrc
