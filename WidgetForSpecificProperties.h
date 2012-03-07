#ifndef WIDGETFORSPECIFICPROPERTIES_H
#define WIDGETFORSPECIFICPROPERTIES_H

#include "db_SP.h"

#include <QWidget>
#include <QVector>

namespace Ui {
    class WidgetForSpecificProperties;
}

class WidgetForSpecificProperties : public QWidget
{
    Q_OBJECT

    struct UidToWidget
    {
        QString objectUid;
        QWidget* w;
    };

public:
    explicit WidgetForSpecificProperties(QWidget *parent = 0);
    ~WidgetForSpecificProperties();

    void feedInSpecificProperties(const QVector<Sp*> & sps, const QString& dbDestinationUid);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetForSpecificProperties *ui;
    QVector<UidToWidget*> m_mappings;
};

#endif // WIDGETFORSPECIFICPROPERTIES_H
