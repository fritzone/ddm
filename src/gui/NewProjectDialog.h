#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QtGui/QDialog>
#include <QString>
#include <QMap>
#include <QAbstractButton>

class DatabaseEngine;

namespace Ui {
    class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT
public:

    static const int PRJ_DATAMODEL = 1;
    static const int PRJ_BINDTODATABASE = 2;
    static const int PRJ_REVERSEENGINEER = 3;

    NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();


    QString getProjectName() const
    {
        return projectName;
    }

    QString getSolutionName() const
    {
        return solutionName;
    }

    QString getTargetDatabase() const;

    bool enableOOPFeatures() const;

    void focusOnEditField() const;

    bool inheritDefaultDatatypes() const;

    DatabaseEngine* getDatabaseEngine() const;

    int getProjectType() const;

protected:
    void changeEvent(QEvent *e);

public slots:

    void onHelp();
    void onAccept();
    void onReject();
    void onChangeProjectType(int);

private:

    void setupGuiForDatamodelProject();

private:
    Ui::NewProjectDialog *m_ui;

    // the name of the project
    QString projectName;
    QString solutionName;
};

#endif // NEWPROJECTDIALOG_H
