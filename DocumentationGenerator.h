#ifndef DOCUMENTATIONGENERATOR_H
#define DOCUMENTATIONGENERATOR_H

#include <QString>

class StoredMethod;
class Solution;
class Project;
class QHtmlDocument;

class DocumentationGenerator
{
public:
    DocumentationGenerator(const Solution*);
    QString getDocumentation();
    QString generateHtmlForProject(const Project*);

private:
    void generateHtmlForStoredMethod(QVector<StoredMethod *> *_methods, const QString &header, QHtmlDocument &doc);
private:
    const Solution* m_solution;
};

#endif // DOCUMENTATIONGENERATOR_H
