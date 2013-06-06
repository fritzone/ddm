#ifndef WIDGETFORSPECIFICPROPERTIES_H
#define WIDGETFORSPECIFICPROPERTIES_H

class SpInstance;

#include <QWidget>
#include <QVector>
#include <QSignalMapper>
#include <QCheckBox>
#include <QFormLayout>
#include <QToolBox>
#include <QMap>
#include <QComboBox>

namespace Ui {
    class WidgetForSpecificProperties;
}

class ObjectWithSpInstances;
class DatabaseEngine;

class WidgetForSpecificProperties : public QWidget
{
    Q_OBJECT

    struct UidToWidget
    {
        UidToWidget(SpInstance* s) : m_spi(s) {}

        QString objectUid;
        QString objectRoleUid;
        QWidget* w;
        SpInstance* m_spi;
    };

public:
    explicit WidgetForSpecificProperties(const DatabaseEngine* dbe, ObjectWithSpInstances* osp, QWidget *parent = 0);
    ~WidgetForSpecificProperties();

    void feedInSpecificProperties(const QVector<SpInstance*> & spInstances, const QString& dbDestinationUid);

    /**
     * This method will re-work the controls in a way that the SPs of the obhect passed
     * in will restrict the controls to contain a possible subset of the values they
     * might contain by default. For example: for MySQL engine InnoDB the index types
     * differ.
     * This method is alloved to change the object it is passed in.
     */
    void taylorToSpecificObject(ObjectWithSpInstances* dest);

    /**
     * In case this object was create with a NULL m_osp it is mandatory to call at the end this
     * to feed back the data to a new newly created obejct
     * @param dest - the target object
     */
    void repopulateSpsOfObject(ObjectWithSpInstances* dest);

protected:
    void changeEvent(QEvent *e);

private slots:

    void checkBoxToggled(QString);
    void comboBoxSelected(int idx);
    void editTextEdited(const QString&s);

private:
    template <class T>
    T* getWidgetForObjectUid(const QString&);

    QWidget* getToolboxPageForText(const QString&);
    QString getObjectUidForWidget(const QWidget* w);

    SpInstance* getSpInstanceForWidget(const QWidget*);
    void populateCodepageCombo(QComboBox* comboBox, const QStringList& cps, const QString &current);

private:
    Ui::WidgetForSpecificProperties *ui;
    QVector<UidToWidget*> m_mappings;
    QSignalMapper* m_signalMapper;
    ObjectWithSpInstances* m_osp;
    const DatabaseEngine* m_dbEngine;
    QMap<QString,int> m_GroupToIndex;
    QFormLayout* formLayout;
    QToolBox *toolBox;

};

#endif // WIDGETFORSPECIFICPROPERTIES_H
