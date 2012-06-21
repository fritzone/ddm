#ifndef DOCUMENTATIONGENERATOR_H
#define DOCUMENTATIONGENERATOR_H

#include <QString>

class Project;
class Solution;

class DocumentationGenerator
{
public:
    DocumentationGenerator(const Solution*);
    QString getDocumentation();
    QString generateHtmlForProject(const Project*);

private:
    const Solution* m_solution;
};

#endif // DOCUMENTATIONGENERATOR_H
