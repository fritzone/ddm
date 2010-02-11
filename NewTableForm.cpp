#include "NewTableForm.h"
#include "ui_NewTableForm.h"

NewTableForm::NewTableForm(DatabaseEngine* db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewTableForm),
    m_dbEngine(db)
{
    ui->setupUi(this);
}

NewTableForm::~NewTableForm()
{
    delete ui;
}

void NewTableForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void NewTableForm::setMainWindow(MainWindow *mw)
{
    this->m_mw = mw;
}
