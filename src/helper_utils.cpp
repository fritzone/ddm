#include "helper_utils.h"
#include "ddm_strings.h"

#include <QFileDialog>
#include <QTextStream>

namespace DDM
{

bool saveTextFileWithType(QWidget* parent, const QString &text, const QString &desc, const QString &extType)
{
    QString name = QFileDialog::getSaveFileName(parent, desc, "", extType);
    QFile file(name);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }
    QTextStream out(&file);
    out << text << strNewline;
    return true;
}

}
