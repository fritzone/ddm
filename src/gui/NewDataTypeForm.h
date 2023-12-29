#ifndef NEWDATATYPEFORM_H
#define NEWDATATYPEFORM_H

#include "core_UserDataType.h"

#include <QWidget>
#include <QAbstractButton>
#include <QKeyEvent>

class DatabaseEngine;

namespace Ui {
    class NewDataTypeForm;
}

class UserDataType;
class Version;

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
    NewDataTypeForm(Version *v, DT_TYPE t, DatabaseEngine* dts, QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~NewDataTypeForm();

    /**
     * Sets the focus on the name edit
     */
    void focusOnName();

    /**
     * Sets the datatype that will be displayed in this window.
     * We get here after clicking on a data type in the tree.
     * @param udt - the data type which will be displayed here
     */
    void setDataType(UserDataType* udt);

    void hideDeleteButton();

    // called if we click on a default datatype from the tree
    void setDefaultSelected(bool b);

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

    /// when the user clicked the "Null" is default value
    void onNullClicked();

    /// when the user clicked the cna be null checkbox
    void onCanBeNullClicked();

    void onLockUnlock(bool);
    void onUndelete();
    void onDeleteDataType();
    void onNameEdited(QString);
    void onWidthChanged(QString);
    void onChkUnsignedClick();

protected:

    void changeEvent(QEvent *e);
    virtual void keyPressEvent(QKeyEvent *);

private:


    // hides some GUI elements
    void hideSpecialComponents();

    // resets the content of the dialog to default values
    void resetContent();

    bool onSaveNewDataType(const QString& name, const QString& type,
                             const QString& sqlType, const QString& size,
                             const QString& defaultValue, const QStringList &mvs,
                             const QString &desc, bool unsi,
                             bool canBeNull, UserDataType *&pudt, Version *v);



private:
    Ui::NewDataTypeForm *m_ui;
    DatabaseEngine* m_dbEngine;
    UserDataType* m_udt;
    Version* m_version;
    bool m_init;
    bool m_defaultSelected = false;
};

#endif // NEWDATATYPEFORM_H
