#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QVector>
#include <QStringList>

class Solution;
class Project;
class Version;
class DatabaseEngine;
class UserDataType;
class Table;
class Patch;
class Trigger;

/**
 * The role of the Workspace class is to be a collection point for the "logical" elements of the
 * current session. The Workspace is a singleton, since in a session there can be only one
 * workspace. The Workspace class should be accessible from everywhere in the application.
 */
class Workspace
{
public:

    static Workspace* getInstance();

public:

    /**
     * Return the current solution of the workspace
     */
    Solution* currentSolution() const
    {
        return m_currentSolution;
    }

    /**
     * Tells us if there's a current solution or not
     */
    bool hasCurrentSolution() const
    {
        return m_currentSolution != 0;
    }

    /**
     * Create a current solution. Initialize the solutions vector. Called when creating a new solution from scratch
     */
    bool createCurrentSolution(const QString&);

    /**
     * Create a solution, add it to the solutions vector
     */
    bool createSolution(const QString&);

    /**
     * Add a project to the given solution
     */
    bool addProjectToSolution(Solution*, Project*);

    /**
     * Tells us if the project we are working on is an OOP project or not
     */
    bool currentProjectIsOop() const;

    /**
     * Return the current project
     */
    Project* currentProject() const;

    /**
     * Return the version on which we are working right now
     */
    Version* workingVersion() const;

    /**
     * @brief getVersion
     * @param uid the uid of the version
     * @return
     */
    Version* getVersion(const QString& uid);

    /**
     * Saves the current solution to a file
     */
    bool saveCurrentSolution(const QString&);

    /**
     * Tells us if the current solution was saved or not
     */
    bool currentSolutionWasSavedAlready() const;

    /**
     * Called when a solution is being closed
     */
    bool onCloseSolution();

    /**
     * Loads the given solution in the workspace
     */
    bool loadSolution(const QString&);

    /**
     * Return the engine of the current project
     */
    DatabaseEngine* currentProjectsEngine() const;

    /**
     * Loads the default data types in the current solution
     */
    QVector<UserDataType*> loadDefaultDatatypesIntoCurrentSolution(Solution *s);

    /**
     * Pastes a table from the clipboard
     */
    Table* pasteTable(Version *v);

    /**
     * This method gets called when a new table is saved
     */
    bool onSaveNewTable(Table* tbl);

    /**
     * This method gets called when a table is updated. The only thing this must do
     * for now is to update the tree entry in the project tree and the associated tree widget
     */
    bool onUpdateTable(Table* tbl);


    bool wasSaved() const
    {
        return m_saved;
    }

    void save()
    {
        m_saved = true;
    }

    void change()
    {
        m_saved = false;
    }

    void createNewConnection();

    QStringList getAllVersions();

    bool initiatePatch();

    bool deleteDataType(UserDataType* udt);

    bool deployVersion(Version *v);

    bool finalizePatch(Patch* p);

    Trigger* createTrigger(Version *v);

private:

    Workspace();

private:

    static Workspace* m_instance;

private:

    // the loaded solutions
    QVector<Solution*> m_solutions;

    // we are working on this solution
    Solution* m_currentSolution;

    // if the workspace was saved or not ...
    bool m_saved;

};

#endif // WORKSPACE_H
