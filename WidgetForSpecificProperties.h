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
        QString objectUid;
        QString objectRoleUid;
        QWidget* w;
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
     */
    void taylorToSpecificObject(const ObjectWithSpInstances* dest);

protected:
    void changeEvent(QEvent *e);

private slots:

    void checkBoxToggled(QString);
    void comboBoxSelected(int idx);

private:
    QCheckBox* getCheckBoxForObjectUid(const QString&);
    QWidget* getToolboxPageForText(const QString&);
    QString getObjectUidForWidget(const QWidget* w);
    void populateCodepageCombo(QComboBox* comboBox, const QStringList& cps);

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
