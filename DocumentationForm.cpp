#include "DocumentationForm.h"
#include "ui_DocumentationForm.h"

DocumentationForm::DocumentationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DocumentationForm)
{
    ui->setupUi(this);
}

DocumentationForm::~DocumentationForm()
{
    delete ui;
}
