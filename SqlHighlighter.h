#ifndef SQLHIGHLIGHTER_H
#define SQLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QList>
#include <DataType.h>

class SqlHighlighter : public QSyntaxHighlighter
{
public:

    SqlHighlighter(QTextDocument* parent = 0);
    ~SqlHighlighter(){}

protected:
    void highlightBlock(const QString &text);

private:

    void appendTypePattern(QStringList&, const QList<DataType>&);

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
