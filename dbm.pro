# -------------------------------------------------
# Project created by QtCreator 2009-11-25T12:54:30
# -------------------------------------------------
QT += network \
    sql \
    xml
TARGET = dbm
TEMPLATE = app
SOURCES += main.cpp \
    MainWindow.cpp \
    MainWindowButtonDialog.cpp \
    NewProjectDialog.cpp \
    NewDatatypeForm.cpp \
    DatatypesListForm.cpp \
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
    Configuration.cpp
HEADERS += MainWindow.h \
    MainWindowButtonDialog.h \
    NewProjectDialog.h \
    NewDatatypeForm.h \
    DatatypesListForm.h \
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
    Configuration.h
FORMS += mainwindow.ui \
    mainwindowbuttondialog.ui \
    newprojectdialog.ui \
    newdatatypeform.ui \
    datatypeslistform.ui
RESOURCES += dbm.qrc
