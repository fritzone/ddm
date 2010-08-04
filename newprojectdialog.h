#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QtGui/QDialog>
#include <QString>
#include <QMap>

class DatabaseEngine;

namespace Ui {
    class NewProjectDialog;
}

class NewProjectDialog : public QDialog {
    Q_OBJECT
public:
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

    DatabaseEngine* getDatabaseEngine() const;

protected:
    void changeEvent(QEvent *e);
    void accept();

public slots:

    void onHelp();

private:
    Ui::NewProjectDialog *m_ui;

    // the name of the project
    QString projectName;
    QString solutionName;
};

#endif // NEWPROJECTDIALOG_H
