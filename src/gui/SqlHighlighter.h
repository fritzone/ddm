#ifndef SQLHIGHLIGHTER_H
#define SQLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QList>
#include <core_UserDataType.h>
#include <QVector>

class Table;

class SqlHighlighter : public QSyntaxHighlighter
{
public:

    SqlHighlighter(QTextDocument *parent,
                   const QStringList& keywords,
                   const QList<UserDataType>& numericTypes,
                   const QList<UserDataType>& booleanTypes,
                   const QList<UserDataType>& textTypes,
                   const QList<UserDataType>& blobTypes,
                   const QList<UserDataType>& dateTimeTypes,
                   const QList<UserDataType>& miscTypes,
                   const QStringList &tables);
    ~SqlHighlighter(){}

protected:
    void highlightBlock(const QString &text);

private:

    void appendTypePattern(QStringList&, const QList<UserDataType>&);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat typesFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat questionMarksFormat;
    QTextCharFormat tableNamesFormat;
    QTextCharFormat columnNamesFormat;
    QTextCharFormat functionFormat;

};

#endif // SQLHIGHLIGHTER_H
