#ifndef SQLHIGHLIGHTER_H
#define SQLHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class SqlHighlighter : public QSyntaxHighlighter
{
public:

    SqlHighlighter(QTextDocument* parent = 0);
    ~SqlHighlighter(){}

protected:
    void highlightBlock(const QString &text);

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

};

#endif // SQLHIGHLIGHTER_H
