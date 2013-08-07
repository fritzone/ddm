#include "SqlHighlighter.h"
#include "core_Table.h"
#include "core_Column.h"
#include "db_DatabaseEngine.h"
#include "core_UserDataType.h"
#include "db_AbstractDTSupplier.h"
#include "db_DatabaseBuiltinFunction.h"
#include "gui_colors.h"

SqlHighlighter::SqlHighlighter(QTextDocument *parent, const QStringList &keywords, const QList<UserDataType> &numericTypes, const QList<UserDataType> &booleanTypes,
                               const QList<UserDataType> &textTypes, const QList<UserDataType> &blobTypes, const QList<UserDataType> &dateTimeTypes, const QList<UserDataType> &miscTypes,
                               const QStringList &tables) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;  // this is sort of reused

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);

    // feeding in the keywords of the database engine
    QStringList keywordPatterns;

    for(int i=0; i<keywords.size(); i++)
    {
        QString t = QString("\\b") + keywords.at(i).toUpper() + QString("\\b");
        keywordPatterns.append(t);
        t = QString("\\b") + keywords.at(i).toLower() + QString("\\b");
        keywordPatterns.append(t);
    }

    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;

        highlightingRules.append(rule);
    }

    // the types offered by the database
    typesFormat.setForeground(Qt::darkBlue);
    typesFormat.setFontWeight(QFont::Bold);

    QStringList typesPatterns;
    appendTypePattern(typesPatterns, numericTypes);
    appendTypePattern(typesPatterns, booleanTypes);
    appendTypePattern(typesPatterns, textTypes);
    appendTypePattern(typesPatterns, blobTypes);
    appendTypePattern(typesPatterns, dateTimeTypes);
    appendTypePattern(typesPatterns, miscTypes);

    foreach (const QString &pattern, typesPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = typesFormat;
        highlightingRules.append(rule);
    }

    // the funny (??) question mark
    questionMarksFormat.setForeground(Qt::red);
    questionMarksFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("(?:\\s|^)\\(\\?\\?\\)(?:\\s|$)");
    rule.format = questionMarksFormat;
    highlightingRules.append(rule);

    // the table names
    tableNamesFormat.setForeground(Qt::black);
    tableNamesFormat.setFontWeight(QFont::Bold);
    for(int i=0; i< tables.size(); i++)
    {
        QString p = "\\b" + tables.at(i)+ "\\b";
        rule.pattern = QRegExp(p);
        rule.format = tableNamesFormat;
        highlightingRules.append(rule);
    }

    // text in double quote
    quoteFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("\"([^\"']*)\"");
    rule.format = quoteFormat;
    highlightingRules.append(rule);

    // the one line comment (if you move this before the the tables the parser will highlight the table names in the comments too, so leave it here)
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    singleLineCommentFormat.setFontItalic(true);
    rule.pattern = QRegExp("--[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

}

void SqlHighlighter::appendTypePattern(QStringList &ls, const QList<UserDataType> & tp)
{
    for(int i=0; i<tp.size(); i++)
    {
        ls << QString("\\b") + tp.at(i).getName().toUpper() + QString("\\b");
        ls << QString("\\b") + tp.at(i).getName().toLower() + QString("\\b");
    }
}

void SqlHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        //qDebug() << index << " -> " << text << " @ " << rule.pattern.pattern();
        while (index > 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
        if(index == 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}
