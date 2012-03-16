#ifndef WIDGETFORSPECIFICPROPERTIES_H
#define WIDGETFORSPECIFICPROPERTIES_H

class SpInstance;

#include <QWidget>
#include <QVector>
#include <QSignalMapper>
#include <QCheckBox>

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
        QWidget* w;
    };

public:
    explicit WidgetForSpecificProperties(const DatabaseEngine* dbe, ObjectWithSpInstances* osp, QWidget *parent = 0);
    ~WidgetForSpecificProperties();

    void feedInSpecificProperties(const QVector<SpInstance*> & spInstances, const QString& dbDestinationUid);

protected:
    void changeEvent(QEvent *e);

private slots:

    void checkBoxToggled(QString);

private:
    QCheckBox* getCheckBoxForObjectUid(const QString&);

private:
    Ui::WidgetForSpecificProperties *ui;
    QVector<UidToWidget*> m_mappings;
    QSignalMapper* m_signalMapper;
    ObjectWithSpInstances* m_osp;
    const DatabaseEngine* m_dbEngine;
};

#endif // WIDGETFORSPECIFICPROPERTIES_H
