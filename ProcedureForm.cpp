#include "ProcedureForm.h"
#include "ui_ProcedureForm.h"
#include "QTextEditWithCodeCompletion.h"
#include "FrameForLineNumbers.h"

ProcedureForm::ProcedureForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProcedureForm)
{
    ui->setupUi(this);

    m_frameForLineNumbers = new FrameForLineNumbers(this);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_frameForLineNumbers->sizePolicy().hasHeightForWidth());
    m_frameForLineNumbers->setSizePolicy(sizePolicy);
    m_frameForLineNumbers->setMinimumSize(QSize(48, 0));
    m_frameForLineNumbers->setMaximumSize(QSize(48, 16777215));
    m_frameForLineNumbers->setFrameShape(QFrame::StyledPanel);
    m_frameForLineNumbers->setFrameShadow(QFrame::Raised);

    ui->horizontalLayout->addWidget(m_frameForLineNumbers);
    m_textEdit = new QTextEditWithCodeCompletion(this);
    m_textEdit->setLineNumberFrame(m_frameForLineNumbers);
    ui->horizontalLayout->addWidget(m_textEdit);
    m_textEdit->setFocus();

}

ProcedureForm::~ProcedureForm()
{
    delete ui;
}

void ProcedureForm::changeEvent(QEvent *e)
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
