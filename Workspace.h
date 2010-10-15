#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QVector>

class Solution;

/**
 * The role of the workspace class is to be a collection point for the solutions of the
 * current session. The workspace is a singleton, since in a session there can be only one
 * workspace.
 */
class Workspace
{
public:

    static Workspace* getInstance();

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
