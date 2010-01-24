#ifndef NEWDATATYPEFORM_H
#define NEWDATATYPEFORM_H

#include <QtGui/QWidget>

class DatabaseEngine;

namespace Ui {
    class NewDataTypeForm;
}

class MainWindow;
class UserDataType;

/**
 * This is the window in which the user can create a new DataType
 */
class NewDataTypeForm : public QWidget {
    Q_OBJECT
public:
    /**
     * Constructor
     * @param dts - the database engine
     */
    NewDataTypeForm(DatabaseEngine* dts, QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~NewDataTypeForm();

    /**
     * Sets the focus on the name edit
     */
    void focusOnName();

    /**
     * Sets the main window of this window (so that the calls go upwards when
     * the user has pressed the "Save" button.
     */
    void setMainWindow(MainWindow* mw);

    /**
     * Sets the datatype that will be displayed in this window.
     * We get here after clicking on a data type in the tree.
     * @param udt - the data type which will be displayed here
     */
    void setDataType(UserDataType* udt);

public slots:

    /// called when the selection of the basic data type combo has changed
    void basicDTselected(QString);

    /// calle when the user has changed the underlying SQL type
    void onSqlTypeSelected(QString);

    /// called when the user pressed the "Save" button
    void onSave();

    /// called when the user has pressed the Reset button
    void onReset();

    /// called when the user has added a misc value
    void onAddMiscValue();

    /// called when the user has removed a misc value
    void onRemoveSelectedMiscValue();

    /// when the user changed the character set
    void onCharacterSetCmbChanged(int);

protected:
    void changeEvent(QEvent *e);

private:

    // loads the codepages from the codepages
    void populateCodepageCombo();

    // hides some GUI elements
    void hideSpecialComponents();

    // resets the content of the dialog to default values
    void resetContent();

private:
    Ui::NewDataTypeForm *m_ui;
    MainWindow* m_mw;
    DatabaseEngine* m_dbEngine;
    UserDataType* m_udt;
};

#endif // NEWDATATYPEFORM_H
