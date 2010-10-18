#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QVector>

class Solution;
class Project;
class Version;
class DatabaseEngine;
class UserDataType;

/**
 * The role of the workspace class is to be a collection point for the solutions of the
 * current session. The workspace is a singleton, since in a session there can be only one
 * workspace.
 */
class Workspace
{
public:

    static Workspace* getInstance();

public:

    Solution* currentSolution() const
    {
        return m_currentSolution;
    }

    bool hasCurrentSolution() const
    {
        return m_currentSolution != 0;
    }

    bool createCurrentSolution(const QString&);

    bool createSolution(const QString&);

    bool addProjectToSolution(Solution*, Project*);

    bool currentProjectIsOop() const;

    Project* currentProject() const;

    Version* workingVersion() const;

    bool saveCurrentSolution(const QString&);

    bool currentSolutionWasSavedAlready() const;

    bool onCloseSolution();

    bool loadSolution(const QString&);

    DatabaseEngine* currentProjectsEngine() const;

    QVector<UserDataType*> loadDefaultDatatypesIntoCurrentSolution();

private:

    Workspace();

private:

    static Workspace* m_instance;

private:

    // the loaded solutions
    QVector<Solution*> m_solutions;

    // we are working on this solution
    Solution* m_currentSolution;

};

#endif // WORKSPACE_H
