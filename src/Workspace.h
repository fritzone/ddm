#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QVector>
#include <QStringList>
#include <QMap>

class Solution;
class Project;
class Version;
class DatabaseEngine;
class UserDataType;
class Table;
class Patch;
class Trigger;
class Connection;
class ObjectWithUid;

/**
 * The role of the Workspace class is to be a collection point for the "logical" elements of the
 * current session. The Workspace is a singleton, since in a session there can be only one
 * workspace. The Workspace class should be accessible from everywhere in the application.
 */
class Workspace : public QObject
{
    Q_OBJECT

public:

    static Workspace* getInstance();

public slots:

    void cleanup();

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

    QVector<UserDataType*> loadDefaultDatatypesIntoCurrentSolution(DatabaseEngine *eng);

    /**
     * @brief loadUserDefinedDatatypes Loads the user deifned datatypes from
     * the users' home directory and returns the given vector
     * @return
     */
    void loadUserDefinedDatatypes();

    /**
     * @brief saveUserDefinedDatatypes Saves the datatypes the user defined
     * in the users' home directory
     */
    void saveUserDefinedDatatypes();

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

    bool finalizePatch(Patch* p, QString &tempError);

    Trigger* createTrigger(Version *v);

    void addUserDefinedDataType(const QString& dbName, UserDataType* udt);

    void refreshConnection(Connection* c);

    QString getDataLocation();

    bool doLockLikeOperation(bool reLocking, ObjectWithUid *element, Version *v);

    void createPatchElementForGui(Version* v, ObjectWithUid * element, const QString& guid, bool reLocking);
    void updatePatchElementGuiToReflectState(Version *v, ObjectWithUid*, const QString &guid, int state);
    void createPatchItem(Patch*);
private:

    Workspace();

    virtual ~Workspace();

private:

    static Workspace* m_instance;

private:

    // the loaded solutions
    QVector<Solution*> m_solutions;

    // we are working on this solution
    Solution* m_currentSolution;

    // if the workspace was saved or not ...
    bool m_saved;

    // a vector of the data types the user defined while working for
    // each database
    QMap<QString, QVector<UserDataType*> > m_userDefinedDataTypes;

};

#endif // WORKSPACE_H
