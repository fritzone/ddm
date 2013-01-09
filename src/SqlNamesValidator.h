#ifndef SQLNAMESVALIDATOR_H
#define SQLNAMESVALIDATOR_H

#include <QValidator>

class SqlNamesValidator : public QValidator
{
public:
    SqlNamesValidator();

    QValidator::State validate ( QString & input, int & pos ) const;
};

#endif // SQLNAMESVALIDATOR_H
