#include "SqlNamesValidator.h"
#include <QString>

SqlNamesValidator::SqlNamesValidator()
{
}

QValidator::State SqlNamesValidator::validate ( QString & input, int & pos ) const
{
    for(int i=0; i<input.length(); i++)
    {
        if(!input.at(i).isLetterOrNumber() && input.at(i) != '_')
        {
            pos = i;
            return QValidator::Invalid;
        }
    }

    if(input.length() > 0)
    {
        if(input.at(0).isDigit())
        {
            pos = 0;
            return QValidator::Invalid;
        }
    }
    return QValidator::Acceptable;
}
