#include "ProcedureForm.h"
#include "ui_ProcedureForm.h"
#include "TextEditWithCodeCompletion.h"
#include "FrameForLineNumbers.h"
#include "Connection.h"
#include "MainWindow.h"
#include "GuiElements.h"
#include "strings.h"
#include "core_ParameterAndDescription.h"
#include "db_DatabaseEngine.h"
#include "core_Function.h"

ProcedureForm::ProcedureForm(Version* v, ProcedureFormMode m, bool guided, bool forced, Connection *c, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProcedureForm), m_textEdit(0), m_frameForLineNumbers(0), m_proc(0),
    m_forcedChange(forced), m_mode(m), m_version(v), m_guided(guided),
    m_currentParameter(0), m_javaBinding(false),
    m_sqlTypeToDTType(), m_DTTypeToJavaType()
{
    ui->setupUi(this);

    m_init = true;
    if(m_guided)
    {
        QTreeWidgetItem *header = ui->lstParameters->headerItem();
        header->setText(0, tr("Name"));
        header->setText(1, tr("Dir"));
        header->setText(2, tr("Type"));
        header->setText(3, tr("Desc"));

        ui->btnLoad->hide();

        QSet<PROGRAMMING_LANGUAGES> supportedLanguages = v->getProject()->getEngine()->storedMethodLanguages();

        if(!supportedLanguages.contains(LANGUAGE_SQL))
        {
            ui->cmbProcedureType->removeItem(ui->cmbProcedureType->findText("SQL"));

            // TODO: Future: This is plain CUBRID/MySql. Will be different
            // and add an extra Java entry to the parameters list box
            header->setText(3, tr("Java Type"));
            header->setText(4, tr("Desc"));

            m_javaBinding = true;
            initJavaMaps();
        }
        if(!supportedLanguages.contains(LANGUAGE_JAVA))
        {
            ui->cmbProcedureType->removeItem(ui->cmbProcedureType->findText("Java"));

            // hide the java related gui controls
            ui->cmbJavaParameterType->hide();
            ui->grpJava->hide();
        }
    }

    m_frameForLineNumbers = new FrameForLineNumbers(this);
    ui->horizontalLayout->addWidget(m_frameForLineNumbers);

    m_textEdit = new TextEditWithCodeCompletion(this, c);
    m_textEdit->setLineNumberFrame(m_frameForLineNumbers);
    ui->horizontalLayout->addWidget(m_textEdit);
    m_textEdit->setFocus();

    ui->btnInject->hide();
    connect(m_textEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));

    // fill the data types for the parameters
    m_availableDataTypes = v->getDataTypes();
    for(int i=0; i<m_availableDataTypes.size(); i++)
    {
        if(m_javaBinding) // seems there is no cubrid support for blob and bool
        {
            if(! (m_availableDataTypes[i]->getType() == DT_BLOB
                    || m_availableDataTypes[i]->getType() == DT_BOOLEAN) )
            {
                ui->cmbParameterType->addItem(IconFactory::getIconForDataType(m_availableDataTypes[i]->getType()),
                                          m_availableDataTypes[i]->getName());
            }
        }
        else
        {
            ui->cmbParameterType->addItem(IconFactory::getIconForDataType(m_availableDataTypes[i]->getType()),
                                      m_availableDataTypes[i]->getName());
        }
    }

    if(m_javaBinding)
    {
        ui->cmbParameterType->addItem(IconFactory::getIconForDataType(DT_MISC), "SET");
        ui->cmbParameterType->addItem(IconFactory::getIconForDataType(DT_MISC), "MULTISET");
        ui->cmbParameterType->addItem(IconFactory::getIconForDataType(DT_MISC), "SEQUENCE");
        ui->cmbParameterType->addItem(IconFactory::getIconForDataType(DT_GENERIC), "CURSOR");
        ui->cmbParameterType->addItem(IconFactory::getIconForDataType(DT_VARIABLE), "OBJECT");

        ui->cmbReturnType->addItem(IconFactory::getIconForDataType(DT_MISC), "SET");
        ui->cmbReturnType->addItem(IconFactory::getIconForDataType(DT_MISC), "MULTISET");
        ui->cmbReturnType->addItem(IconFactory::getIconForDataType(DT_MISC), "SEQUENCE");
        ui->cmbReturnType->addItem(IconFactory::getIconForDataType(DT_GENERIC), "CURSOR");
        ui->cmbReturnType->addItem(IconFactory::getIconForDataType(DT_VARIABLE), "OBJECT");

        ui->txtSpecialJavaReturnType->hide();
    }


    if(m == MODE_PROCEDURE)
    {
        ui->grpReturn->hide();
        ui->txtReturnExplanation->hide();
        ui->lblReturnDesc->hide();
    }
    else
    {
        m_availableDataTypes = v->getDataTypes();
        for(int i=0; i<m_availableDataTypes.size(); i++)
        {
            if(m_javaBinding) // seems there is no cubrid support for blob and bool
            {
                if(! (m_availableDataTypes[i]->getType() == DT_BLOB
                      || m_availableDataTypes[i]->getType() == DT_BOOLEAN) )
                {
                    ui->cmbReturnType->addItem(IconFactory::getIconForDataType(m_availableDataTypes[i]->getType()),
                                               m_availableDataTypes[i]->getName());
                }
            }
            else
            {
                ui->cmbReturnType->addItem(IconFactory::getIconForDataType(m_availableDataTypes[i]->getType()),
                                            m_availableDataTypes[i]->getName());
            }
        }
    }

    toggleGuidedCreationControls(m_guided);
    ui->txtParamDesc->hide();

    if(m_guided)
    {
        // create the funny splitter
        m_splitter = new QSplitter(Qt::Vertical, ui->mainFrame);
        m_splitter->addWidget(ui->frameForParam);
        m_splitter->addWidget(ui->frameForEdit);
        m_splitter->resize(ui->mainFrame->size());
        m_splitter->setCollapsible(0, true);
        QList<int> sizes;
        sizes << 250 << 500;
        m_splitter->setSizes(sizes);
        // tricky ...
        ui->tabWidget->removeTab(0);
        ui->tabWidget->insertTab(0, m_splitter, tr("General"));
    }

    if(m_mode == MODE_PROCEDURE)
    {
        ui->tabWidget->setTabIcon(0, IconFactory::getProcedureIcon());
    }
    else
    {
        ui->tabWidget->setTabIcon(0, IconFactory::getFunctionTreeIcon());
    }
    ui->tabWidget->setCurrentIndex(0);
    if(m_javaBinding)
    {
        onParameterTypeChange(ui->cmbParameterType->currentText());
        ui->cmbReturnType->setCurrentIndex(0);
    }

}

ProcedureForm::~ProcedureForm()
{
    delete ui;
}

void ProcedureForm::disableEditingControls(bool dis)
{
    m_textEdit->setReadOnly(dis);
    ui->btnLoad->setDisabled(dis);
}

void ProcedureForm::toggleGuidedCreationControls(bool guided)
{
    ui->grpJava->setVisible(guided && m_javaBinding);
    ui->grpParameters->setVisible(guided);
    ui->txtProcName->setVisible(guided);
    ui->cmbProcedureType->setVisible(guided);
    ui->txtBrief->setVisible(guided);
    ui->lblBrief->setVisible(guided);
    ui->frameForName->setVisible(guided);
    ui->grpReturn->setVisible(guided && m_mode == MODE_FUNCTION);

    if(!guided)
    {
        ui->tabWidget->removeTab(1);
    }
}

void ProcedureForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    case QEvent::Resize:
        m_splitter->resize(dynamic_cast<QResizeEvent*>(e)->size());
        break;
    default:
        break;
    }
}

QString ProcedureForm::getProcNameFromSql()
{
    int t = 0;
    QString n = m_proc->getNameFromSql(0, t);
    if(n != "UNNAMED")
    {
        if(!m_proc->isDeleted())
        {
            m_proc->setDisplayText(n);
            m_proc->setName(n);
        }
    }
    return m_proc->getName();
}

void ProcedureForm::textChanged()
{
    if(m_proc->isDeleted()) return;

    m_proc->setSql(m_textEdit->toPlainText());
    if(!m_forcedChange)
    {
        if(!m_guided)
        {
            getProcNameFromSql();
        }
    }
}

void ProcedureForm::updateTextEditForGuidedMethod()
{
    m_textEdit->disableRow(1); // disabling the first row with the name. Everything else is usually hand written

    if(m_javaBinding)
    {
        m_textEdit->disableRow(2); // disabling the other rows too
        m_textEdit->disableRow(3); // three in total for CUBRID

        // filling the Java Parameter Types Combo
    }

    m_textEdit->updateLineNumbers();
}

void ProcedureForm::initSql()
{
    QString sql = strCreate + strSpace + (m_mode == MODE_PROCEDURE ? strProcedure : strFunction) + strSpace + m_proc->getName();
    ui->txtProcName->setText(m_proc->getName());
    sql += strOpenParantheses + strCloseParantheses;

    UserDataType *dt = m_version->getDataType(ui->cmbReturnType->currentText()); // 0 if none found, used down
    QString returns = (dt ? dt->sqlAsString() : strInt);

    if(m_mode == MODE_FUNCTION)
    {
        sql += strSpace;
        sql += m_version->getProject()->getEngine()->getStoredMethodReturnKeyword() + strSpace + (m_guided? returns : "");
    }
    sql += strNewline;
    QString defaultBody = m_version->getProject()->getEngine()->getDefaultStoredMethodBody(getTargetLanguage());
    sql += defaultBody;
    m_forcedChange = true;
    m_textEdit->setPlainText(sql);
    m_forcedChange = false;
    m_textEdit->updateLineNumbers();

    if(!m_guided)
    {
        return;
    }
    updateTextEditForGuidedMethod();

    m_init = false;
}

void ProcedureForm::showSql()
{
    m_textEdit->setPlainText(m_proc->getSql());
    m_textEdit->updateLineNumbers();
    if(!m_guided)
    {
        return;
    }
    updateTextEditForGuidedMethod();

    m_init = false;
}

void ProcedureForm::onLockUnlock(bool checked)
{
    if(checked)
    {
        ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
        //ui->grpContent->setEnabled(true);
        disableEditingControls(false);
        m_proc->unlock();
        m_proc->updateGui();
        ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"Procedure ":"Function ") + QObject::tr("is <b>unlocked</b>. Click this button to lock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(false, m_proc->getObjectUid());
    }
    else
    {
        ui->btnLock->setIcon(IconFactory::getLockedIcon());
        //ui->grpContent->setEnabled(false);
        disableEditingControls(true);
        m_proc->lock(LockableElement::LOCKED);
        m_proc->updateGui();
        ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"Procedure ":"Function ") + QObject::tr("is <b>locked</b>. Click this button to lock it."));

        MainWindow::instance()->finallyDoLockLikeOperation(true, m_proc->getObjectUid());
    }
}

void ProcedureForm::setProcedure(StoredMethod *p)
{
    m_proc = p;
    toggleGuidedCreationControls(p->isGuided());

    // and if it's guided populate the guide controls
    if(p->isGuided())
    {
        if(m_mode == MODE_FUNCTION)
        {
            ui->cmbReturnType->setCurrentIndex(ui->cmbReturnType->findText(p->getReturnType()));
            ui->txtReturnExplanation->setText(p->getReturnDesc());
        }

        for(int i=0; i<p->getParametersWithDescription().size(); i++)
        {
            ParameterAndDescription* pad = p->getParametersWithDescription()[i];
            QStringList lst;
            lst << pad->m_parameter << pad->m_direction << pad->m_type;
            if(m_javaBinding)
            {
                lst << pad->m_progLangType;
            }
            lst << pad->m_description;

            ContextMenuEnabledTreeWidgetItem* itm = new ContextMenuEnabledTreeWidgetItem(0, lst);
            ui->lstParameters->addTopLevelItem(itm);
            pad->setLocation(itm);
        }

        ui->txtDescription->setText(p->getDescription());
        ui->txtBrief->setText(p->getBriefDescription());
    }

    ui->btnUndelete->hide();

    // TODO: duplicate with the other forms ... at least, the logic!
    if(!m_proc->wasLocked())
    {
        if(m_proc->isDeleted())
        {
            ui->frameForUnlockButton->show();
            ui->btnLock->hide();
            ui->btnUndelete->show();
        }
        else
        {
            ui->frameForUnlockButton->hide();
        }
    }
    else
    {
        ui->frameForUnlockButton->show();
        if(m_proc->lockState() == LockableElement::LOCKED)
        {
            ui->btnLock->setIcon(IconFactory::getLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(false);
            ui->btnLock->blockSignals(false);
            //ui->grpContent->setEnabled(false);
            disableEditingControls(true);
            ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"This Procedure ":"This Function ") + QObject::tr("is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(true);
            ui->btnLock->blockSignals(false);
            //ui->grpContent->setEnabled(true);
            disableEditingControls(false);
            ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"This Procedure ":"This Function ") + QObject::tr("is <b>unlocked</b>. Click this button to unlock it."));
        }

        if(m_proc->isDeleted())
        {
            ui->btnLock->hide();
            ui->btnUndelete->show();
        }
    }
}

void ProcedureForm::onUndelete()
{
    if(m_version->undeleteObject(m_proc->getObjectUid(), false))
    {
        MainWindow::instance()->getGuiElements()->removeItemForPatch(m_version->getWorkingPatch(), m_proc->getObjectUid());
        // TODO: Duplicate from above
        if(m_proc->lockState() == LockableElement::LOCKED)
        {
            ui->btnLock->setIcon(IconFactory::getLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(false);
            ui->btnLock->blockSignals(false);
            //ui->grpContent->setEnabled(false);
            disableEditingControls(true);
            ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"This Procedure ":"This Function ") + QObject::tr("is <b>locked</b>. Click this button to unlock it."));
        }
        else
        {
            ui->btnLock->setIcon(IconFactory::getUnLockedIcon());
            ui->btnLock->blockSignals(true);
            ui->btnLock->setChecked(true);
            ui->btnLock->blockSignals(false);
            //ui->grpContent->setEnabled(true);
            disableEditingControls(false);
            ui->btnLock->setToolTip((m_mode == MODE_PROCEDURE?"This Procedure ":"This Function ") + QObject::tr("is <b>unlocked</b>. Click this button to unlock it."));
        }
        ui->btnUndelete->hide();
        ui->btnLock->show();
    }
}


void ProcedureForm::onSave()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save sql"), "", tr("SQL files (*.sql)"));
    QFile file(name);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);
    out << m_textEdit->toPlainText() << "\n";
}

void ProcedureForm::onLoadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,  "Load values", "", "SQL files (*.sql)");
    if(fileName.length() == 0)
    {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QString sql = "";
    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();
        sql += line;
        sql += strNewline;
    }

    m_forcedChange = true;
    m_textEdit->setPlainText(sql);
    m_forcedChange = false;
    m_textEdit->updateLineNumbers();

}

void ProcedureForm::onReturnTypeComboChange(QString a)
{
    UserDataType *dt = m_version->getDataType(a); // 0 if none found, used down
    QString returns = (dt ? dt->sqlAsString() : a);

    if(m_init) return;

    QString plainTextEditContents = m_textEdit->toPlainText();
    QStringList lines = plainTextEditContents.split("\n");

    if(lines.empty())
    {
        return;
    }

    m_proc->setReturn(returns);
    QString line0 = lines.at(0);
    int idxOfReturns = line0.indexOf(m_version->getProject()->getEngine()->getStoredMethodReturnKeyword(), Qt::CaseInsensitive);
    line0 = line0.left(idxOfReturns + m_version->getProject()->getEngine()->getStoredMethodReturnKeyword().length()) + " " + returns;
    lines[0] = line0;
    m_textEdit->setPlainText(lines.join("\n"));

    // and now mangle out the java part of it
    if(m_javaBinding)
    {
        DT_TYPE dtType = getDTtypeOfSql(a);
        if(dtType == DT_INVALID) return;
        if(dtType == DT_MISC)
        {
            ui->cmbJavaReturnType->hide();
            ui->txtSpecialJavaReturnType->show();
            onJavaReturnTypeChange(ui->txtSpecialJavaReturnType->text());
        }
        else
        if(m_DTTypeToJavaType.contains(dtType))
        {
            ui->cmbJavaReturnType->show();
            ui->txtSpecialJavaReturnType->hide();

            ui->cmbJavaReturnType->clear();
            QStringList lst = m_DTTypeToJavaType.value(dtType);
            for(int i=0; i<lst.size(); i++)
            {
                ui->cmbJavaReturnType->addItem(IconFactory::getIconForDataType(dtType), lst[i]);
            }
            onJavaReturnTypeChange(ui->cmbJavaReturnType->currentText());
        }
    }
}

void ProcedureForm::onProcNameChange(QString a)
{
    if(m_init)
    {
        return;
    }

    if(a == "UNNAMED")
    {
        return;
    }

    if(m_proc->isDeleted())
    {
        return;
    }

    QString plainTextEditContents = m_textEdit->toPlainText();
    QStringList lines = plainTextEditContents.split("\n");

    if(lines.empty())
    {
        return;
    }

    QString t = m_mode == MODE_PROCEDURE?"PROCEDURE":"FUNCTION";
    QString line0 = lines.at(0);
    int idxOfType = line0.indexOf(t, Qt::CaseInsensitive);
    int i = idxOfType + t.length();
    while(line0.at(i).isSpace()) i++;   // skip space
    while(line0.at(i).isLetterOrNumber() || line0.at(i) == '_') i++; // skip the name
    line0 = line0.left(idxOfType + t.length() + 1) + a + line0.mid(i);
    lines[0] = line0;
    m_textEdit->setPlainText(lines.join("\n"));
    m_proc->setDisplayText(a);
    m_proc->setName(a);
}

void ProcedureForm::onBriefDescrChange(QString a)
{
    m_proc->setBriefDescr(a);
}

void ProcedureForm::onDescrChange()
{
    m_proc->setDescription(ui->txtDescription->toPlainText());
}

void ProcedureForm::updateSqlDueToParamChange()
{
    QString plainTextEditContents = m_textEdit->toPlainText();
    QStringList lines = plainTextEditContents.split("\n");

    if(!lines.empty())
    {
        QString line0 = lines.at(0);
        int idxOfOpenParen = line0.indexOf("(");
        int idxOfCloseParen = idxOfOpenParen;
        int level = 0;
        while(idxOfCloseParen < line0.length())
        {
            idxOfCloseParen ++;
            if(line0[idxOfCloseParen] == '(') level ++;
            if(line0[idxOfCloseParen] == ')')
            {
                level --;
                if(level == -1)
                {
                    break;
                }
            }
        }

        QString params = "";
        const QVector<ParameterAndDescription*>& pars = m_proc->getParametersWithDescription();
        for(int i=0; i<pars.size(); i++)
        {
            UserDataType *dt = m_version->getDataType(pars[i]->m_type); // 0 if none found, used down
            QString parT = (dt ? dt->sqlAsString() : pars[i]->m_type);

            QMap<PARAMETER_FIELD_ROLES, QString> cpr;
            cpr[PARAM_NAME] = pars[i]->m_parameter;
            cpr[PARAM_DIRECTION] = pars[i]->m_direction.trimmed();
            cpr[PARAM_TYPE] = parT;

            QMap<PARAMETER_FIELD_ROLES, int> parmaps = m_version->getProject()->getEngine()->parameterFieldOrders();

            int cidx = 0;
            while(cidx < cpr.keys().size())
            {
                for(int j=0; j<parmaps.keys().size(); j++)
                {
                    if(parmaps.value(parmaps.keys().at(j)) == cidx)
                    {
                        if(cpr[parmaps.keys().at(j)].length())
                        {
                            params += strSpace;
                            params += cpr[parmaps.keys().at(j)];
                        }
                    }

                }
                cidx ++;
            }

            if(i < pars.size() - 1)
            {
                params += ", ";
            }
        }
        QString newLine0 = line0.left(idxOfOpenParen) + "(" + params + line0.mid(idxOfCloseParen);
        lines[0] = newLine0;
        m_textEdit->setPlainText(lines.join("\n"));

        if(m_javaBinding)
        {
            updateJavaBinding();
        }
    }
}

PROGRAMMING_LANGUAGES ProcedureForm::getTargetLanguage()
{
    if(ui->cmbProcedureType->currentText().toUpper() == strSql.toUpper())
    {
        return LANGUAGE_SQL;
    }

    if(ui->cmbProcedureType->currentText().toUpper() == strJava.toUpper())
    {
        return LANGUAGE_JAVA;
    }

    return LANGUAGE_INVALID;
}

void ProcedureForm::initJavaMaps()
{
    m_sqlTypeToDTType["CHAR"] = DT_STRING;
    m_sqlTypeToDTType["VARCHAR"] = DT_STRING;

    m_sqlTypeToDTType["NUMERIC"] = DT_NUMERIC;
    m_sqlTypeToDTType["SHORT"] = DT_NUMERIC;
    m_sqlTypeToDTType["INT"] = DT_NUMERIC;
    m_sqlTypeToDTType["FLOAT"] = DT_NUMERIC;
    m_sqlTypeToDTType["DOUBLE"] = DT_NUMERIC;
    m_sqlTypeToDTType["CURRENCY"] = DT_NUMERIC;

    m_sqlTypeToDTType["DATE"] = DT_DATETIME;
    m_sqlTypeToDTType["TIME"] = DT_DATETIME;
    m_sqlTypeToDTType["TIMESTAMP"] = DT_DATETIME;

    m_sqlTypeToDTType["SET"] = DT_MISC;
    m_sqlTypeToDTType["MULTISET"] = DT_MISC;
    m_sqlTypeToDTType["SEQUENCE"] = DT_MISC;

    m_sqlTypeToDTType["OBJECT"] = DT_VARIABLE;

    m_sqlTypeToDTType["CURSOR"] = DT_GENERIC;

    QStringList javaTextTypes;
    javaTextTypes << "java.lang.String"
                  << "java.sql.Date"
                  << "java.sql.Time"
                  << "java.sql.Timestamp"
                  << "java.lang.Byte"
                  << "java.lang.Short"
                  << "java.lang.Integer"
                  << "java.lang.Long"
                  << "java.lang.Float"
                  << "java.lang.Double"
                  << "java.math.BigDecimal"
                  << "byte"
                  << "short"
                  << "int"
                  << "long"
                  << "float"
                  << "double";

    m_DTTypeToJavaType[DT_STRING] = javaTextTypes;

    QStringList javaNumericTypes;
    javaNumericTypes << "java.lang.Byte"
                   << "java.lang.Short"
                   << "java.lang.Integer"
                   << "java.lang.Long"
                   << "java.lang.Float"
                   << "java.lang.Double"
                   << "java.math.BigDecimal"
                   << "byte"
                   << "short"
                   << "int"
                   << "long"
                   << "float"
                   << "double";

    m_DTTypeToJavaType[DT_NUMERIC] = javaNumericTypes;

    QStringList javaDateTypes;
    javaDateTypes << "java.sql.Date" << "java.sql.Time"
                  << "java.sql.Timestamp" << "java.lang.String";
    m_DTTypeToJavaType[DT_DATETIME] = javaDateTypes;

    QStringList javaMiscTypes;
    javaMiscTypes << "java.lang.Object[]"
                  << "java.lang.Integer[]"
                  << "java.lang.String[]"
                  << "byte[]"
                  << "short[]"
                  << "int[]"
                  << "long[]"
                  << "float[]"
                  << "double[]";
    m_DTTypeToJavaType[DT_MISC] = javaMiscTypes;

    QStringList javaVariableTypes;
    javaVariableTypes << "cubrid.sql.CUBRIDOID";
    m_DTTypeToJavaType[DT_VARIABLE] = javaVariableTypes;

    QStringList javaGenericTypes;
    javaGenericTypes << "cubrid.jdbc.driver.CUBRIDResultSet";
    m_DTTypeToJavaType[DT_GENERIC] = javaGenericTypes;
}

DT_TYPE ProcedureForm::getDTtypeOfSql(const QString& sql)
{
    if(m_sqlTypeToDTType.contains(sql))
    {
        return m_sqlTypeToDTType[sql];
    }

    return m_version->getDataType(sql)->getType();
}

void ProcedureForm::onAddParameter()
{
    if(m_currentParameter != 0)
    {
        m_currentParameter->m_parameter = ui->txtParamName->text();
        m_currentParameter->m_direction = ui->cmbParameterDirection->currentText();
        m_currentParameter->m_type = ui->cmbParameterType->currentText();
        m_currentParameter->m_description = ui->txtParamDesc->toPlainText();
        if(m_javaBinding)
        {
            m_currentParameter->m_progLangType = ui->cmbJavaParameterType->currentText();
        }

        m_currentParameter->getLocation()->setText(0, m_currentParameter->m_parameter);
        m_currentParameter->getLocation()->setText(1, m_currentParameter->m_direction);
        m_currentParameter->getLocation()->setText(2, m_currentParameter->m_type);
        if(m_javaBinding)
        {
            m_currentParameter->getLocation()->setText(3, m_currentParameter->m_progLangType);
            m_currentParameter->getLocation()->setText(4, m_currentParameter->m_description);
        }
        else
        {
            m_currentParameter->getLocation()->setText(3, m_currentParameter->m_description);
        }
    }
    else
    {
        QStringList paramEntry;

        ParameterAndDescription* pd = new ParameterAndDescription(ui->txtParamName->text(),
                                   ui->cmbParameterType->currentText(),
                                   ui->txtParamDesc->toPlainText(),
                                   ui->cmbParameterDirection->currentText(),
                                   m_javaBinding?ui->cmbJavaParameterType->currentText():"",
                                   GUIDED);

        // TODO: For Java based parameters it would be nice to keep a list of
        // previously typed in classes

        if(m_proc->hasParameter(pd->m_parameter))
        {
            QMessageBox::critical(this, tr("Error"), tr("You cannot have another parameter named:<b> ") + pd->m_parameter, QMessageBox::Ok);
            delete pd;
            return;
        }

        m_proc->addParameter(pd);
        paramEntry << pd->m_parameter << pd->m_direction << pd->m_type ;
        if(m_javaBinding)
        {
            paramEntry << pd->m_progLangType;
        }
        paramEntry << pd->m_description;
        ContextMenuEnabledTreeWidgetItem* itm = new ContextMenuEnabledTreeWidgetItem(0, paramEntry);
        ui->lstParameters->addTopLevelItem(itm);
        pd->setLocation(itm);
    }

    // and now patch the textfield
    updateSqlDueToParamChange();

    // and fix the gui
    m_currentParameter = 0;
    ui->btnAddParameter->setIcon(IconFactory::getAddIcon());
    ui->txtParamName->clear();
    ui->txtParamName->setFocus();
}

void ProcedureForm::onDeleteParameter()
{
    if(ui->lstParameters->currentItem() == 0)
    {
        return;
    }

    QString paramName = ui->lstParameters->currentItem()->text(1);
    m_proc->removeParameter(paramName);
    delete ui->lstParameters->currentItem();

    // and now patch the textfield
    updateSqlDueToParamChange();
}

void ProcedureForm::onMoveUpParameter()
{
    QTreeWidgetItem* item = ui->lstParameters->currentItem();
    int row  = ui->lstParameters->currentIndex().row();

    if (item && row > 0)
    {
        ui->lstParameters->takeTopLevelItem(row);
        ui->lstParameters->insertTopLevelItem(row - 1, item);
        ui->lstParameters->setCurrentItem(item);

        m_proc->moveUpParameter(row);
        updateSqlDueToParamChange();
    }
}

void ProcedureForm::onMoveDownParameter()
{
    QTreeWidgetItem* item = ui->lstParameters->currentItem();
    int row  = ui->lstParameters->currentIndex().row();

    if (item && row < ui->lstParameters->topLevelItemCount() - 1)
    {
        ui->lstParameters->takeTopLevelItem(row);
        ui->lstParameters->insertTopLevelItem(row + 1, item);
        ui->lstParameters->setCurrentItem(item);

        m_proc->moveDownParameter(row);
        updateSqlDueToParamChange();
    }
}

void ProcedureForm::onProcTargetLanguageChange(QString a)
{
    if(m_init)
    {
        return;
    }
    qDebug() << a;
}

void ProcedureForm::onSelectParameter(QTreeWidgetItem* current, int)
{
    ParameterAndDescription* pad = m_proc->getParameter(current->text(0));
    if(!pad)
    {
        return;
    }

    ui->txtParamName->setText(pad->m_parameter);

    DT_TYPE dtType = getDTtypeOfSql(pad->m_type);
    int typeIdx = ui->cmbParameterType->findText(pad->m_type);
    if(dtType == DT_MISC)
    {
        ui->cmbParameterType->setEditable(true);
    }
    else
    {
        ui->cmbParameterType->setEditable(false);
    }

    if(typeIdx > -1)
    {
        ui->cmbParameterType->setCurrentIndex(typeIdx);
    }
    else
    {

        ui->cmbParameterType->lineEdit()->setText(pad->m_type);
    }

    ui->cmbParameterDirection->setCurrentIndex(ui->cmbParameterDirection->findText(pad->m_direction));
    ui->txtDescription->setText(pad->m_description);
    m_currentParameter = pad;
    ui->btnAddParameter->setIcon(IconFactory::getApplyIcon());
}

void ProcedureForm::onParameterTypeChange(QString a)
{
    DT_TYPE dtType = getDTtypeOfSql(a);
    if(dtType == DT_INVALID)
    {
        return;
    }


    ui->cmbJavaParameterType->setEditable(false);
    if(dtType == DT_MISC)
    {
        ui->cmbJavaParameterType->setEditable(true);
    }

    if(m_DTTypeToJavaType.contains(dtType))
    {
        ui->cmbJavaParameterType->clear();
        QStringList lst = m_DTTypeToJavaType.value(dtType);
        for(int i=0; i<lst.size(); i++)
        {
            ui->cmbJavaParameterType->addItem(IconFactory::getIconForDataType(dtType), lst[i]);
        }
    }
}

void ProcedureForm::onReturnDescChange()
{
    if(m_init) return;
    m_proc->setReturnDesc(ui->txtReturnExplanation->toPlainText());
}

void ProcedureForm::updateJavaBinding()
{
    QString s = "NAME '";
    s += m_proc->getJavaClassName(); // name
    if(m_proc->getJavaClassName().isEmpty())
    {
        s += "YourClass";
    }

    s+= strDot;

    s += m_proc->getJavaMethodName();
    if(m_proc->getJavaMethodName().isEmpty())
    {
        s += "YourMethod";
    }

    s += strOpenParantheses;
    for(int i=0; i<m_proc->getParametersWithDescription().size(); i++)
    {
        s += m_proc->getParametersWithDescription()[i]->m_progLangType;
        if(i < m_proc->getParametersWithDescription().size() - 1)
        {
            s += ", ";
        }
    }
    s += strCloseParantheses;

    if(m_mode == MODE_FUNCTION)
    {
        s += strSpace + "return" + strSpace + dynamic_cast<Function*>(m_proc)->getJavaReturnType();
    }
    s += "'";

    QString plainTextEditContents = m_textEdit->toPlainText();
    QStringList lines = plainTextEditContents.split("\n");
    if(lines.size() == 3)
    {
        lines[2] = s;
    }
    else
    {
        lines.append(s);
    }
    m_textEdit->setPlainText(lines.join("\n"));
}

void ProcedureForm::onJavaClassNameChange(QString a)
{
    m_proc->setJavaClassName(a);

    updateJavaBinding();
}

void ProcedureForm::onJavaReturnTypeChange(QString a)
{
    if(m_mode == MODE_FUNCTION && m_javaBinding)
    {
        dynamic_cast<Function*>(m_proc)->setJavaReturnType(a);
        updateJavaBinding();
    }
}

void ProcedureForm::onJavaFunctionNameChange(QString a)
{
    m_proc->setJavaMethodName(a);
    updateJavaBinding();
}
