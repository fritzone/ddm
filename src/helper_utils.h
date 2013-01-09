#ifndef HELPER_UTILS_H
#define HELPER_UTILS_H

#include <QString>
#include <QWidget>

namespace DDM
{

bool saveTextFileWithType(QWidget* parent, const QString& text, const QString& desc, const QString& extType);

}

#endif // HELPER_UTILS_H
