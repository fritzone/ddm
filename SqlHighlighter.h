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

    SqlHighlighter(QTextDocument *parent, QStringList keywords, QList<UserDataType> numericTypes, QList<UserDataType> booleanTypes,
                   QList<UserDataType> textTypes, QList<UserDataType> blobTypes, QList<UserDataType> dateTimeTypes, QList<UserDataType> miscTypes,
                   QVector<Table*> tables);
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
