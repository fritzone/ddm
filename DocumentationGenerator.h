#ifndef DOCUMENTATIONGENERATOR_H
#define DOCUMENTATIONGENERATOR_H

#include <QString>

class StoredMethod;
class Solution;
class Project;
class QHtmlDocument;
class QHtmlCSSStyleSet;

class DocumentationGenerator
{
public:
    DocumentationGenerator(const Solution*, const QHtmlCSSStyleSet* ss = 0);
    QString getDocumentation();

private:
    void generateHtmlForStoredMethod(QVector<StoredMethod *> *_methods, const QString &header, QHtmlDocument &doc);
private:
    const Solution* m_solution;
    const QHtmlCSSStyleSet* m_styles;
};

#endif // DOCUMENTATIONGENERATOR_H
