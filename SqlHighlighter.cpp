#include "SqlHighlighter.h"
#include "Workspace.h"
#include "Version.h"
#include "Table.h"
#include "Column.h"
#include "db_DatabaseEngine.h"
#include "DataType.h"
#include "db_AbstractDTSupplier.h"
#include "gui_colors.h"

SqlHighlighter::SqlHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;  // this is sort of reused

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);

    // feeding in the keywords of the database engine
    QStringList keywordPatterns;
    QStringList keywords = Workspace::getInstance()->currentProjectsEngine()->getKeywords();
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
    appendTypePattern(typesPatterns, Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->numericTypes());
    appendTypePattern(typesPatterns, Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->booleanTypes());
    appendTypePattern(typesPatterns, Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->textTypes());
    appendTypePattern(typesPatterns, Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->booleanTypes());
    appendTypePattern(typesPatterns, Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->dateTimeTypes());
    appendTypePattern(typesPatterns, Workspace::getInstance()->currentProjectsEngine()->getDTSupplier()->miscTypes());

    foreach (const QString &pattern, typesPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = typesFormat;
        highlightingRules.append(rule);
    }

    // the funny (??) question mark
    questionMarksFormat.setForeground(Qt::red);
    questionMarksFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("(?:\\s|^)\\(\\?\\?\\)(?:\\s|$)"); /*QRegExp("\\B\\(\\?\\?\\)\\B");*/
    rule.format = questionMarksFormat;
    highlightingRules.append(rule);

    // the table names
    tableNamesFormat.setForeground(Qt::darkGray);
    tableNamesFormat.setFontWeight(QFont::Bold);
    QVector<Table*> tables = Workspace::getInstance()->workingVersion()->getTables();
    for(int i=0; i< tables.size(); i++)
    {
        QString p = "\\b" + tables.at(i)->getName() + "\\b";
        rule.pattern = QRegExp(p);
        rule.format = tableNamesFormat;
        highlightingRules.append(rule);
    }

    // the column names
    columnNamesFormat.setForeground(columnColor);
    columnNamesFormat.setFontWeight(QFont::Bold);
    for(int i=0; i< tables.size(); i++)
    {
        for(int j=0; j<tables.at(i)->fullColumns().size(); j++)
        {
            QString p = "\\b" + tables.at(i)->fullColumns().at(j) + "\\b";
            rule.pattern = QRegExp(p);
            rule.format = columnNamesFormat;
            highlightingRules.append(rule);
        }
    }

    // the one line comment (if you move this before the the tables the parser will highlight the table names in the comments too, so leave it here)
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    singleLineCommentFormat.setFontItalic(true);
    rule.pattern = QRegExp("--[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // TODO: Add the columns from the views too, with the same format as above
}

void SqlHighlighter::appendTypePattern(QStringList &ls, const QList<DataType> & tp)
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
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}
