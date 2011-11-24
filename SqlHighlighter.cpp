#include "SqlHighlighter.h"
#include "Workspace.h"
#include "Version.h"
#include "Table.h"
#include "Column.h"

SqlHighlighter::SqlHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;  // this is sort of reused

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns
            <<"\\bADD\\b"	                        <<"\\badd\\b"
            <<"\\bALL\\b"	                        <<"\\ball\\b"
            <<"\\bALTER\\b"	                        <<"\\balter\\b"
            <<"\\bANALYZE\\b"	                        <<"\\banalyze\\b"
            <<"\\bAND\\b"	                        <<"\\band\\b"
            <<"\\bAS\\b"	                        <<"\\bas\\b"
            <<"\\bASC\\b"	                        <<"\\basc\\b"
            <<"\\bASENSITIVE\\b"	                <<"\\basensitive\\b"
            <<"\\bBEFORE\\b"	                        <<"\\bbefore\\b"
            <<"\\bBETWEEN\\b"	                        <<"\\bbetween\\b"
            <<"\\bBOTH\\b"	                        <<"\\bboth\\b"
            <<"\\bBY\\b"	                        <<"\\bby\\b"
            <<"\\bCALL\\b"	                        <<"\\bcall\\b"
            <<"\\bCASCADE\\b"	                        <<"\\bcascade\\b"
            <<"\\bCASE\\b"	                        <<"\\bcase\\b"
            <<"\\bCHANGE\\b"	                        <<"\\bchange\\b"
            <<"\\bCHECK\\b"	                        <<"\\bcheck\\b"
            <<"\\bCOLLATE\\b"	                        <<"\\bcollate\\b"
            <<"\\bCOLUMN\\b"	                        <<"\\bcolumn\\b"
            <<"\\bCONDITION\\b"	                        <<"\\bcondition\\b"
            <<"\\bCONSTRAINT\\b"	                <<"\\bconstraint\\b"
            <<"\\bCONTINUE\\b"	                        <<"\\bcontinue\\b"
            <<"\\bCONVERT\\b"	                        <<"\\bconvert\\b"
            <<"\\bCREATE\\b"	                        <<"\\bcreate\\b"
            <<"\\bCROSS\\b"	                        <<"\\bcross\\b"
            <<"\\bCURRENT_DATE\\b"	                <<"\\bcurrent_date\\b"
            <<"\\bCURRENT_TIME\\b"	                <<"\\bcurrent_time\\b"
            <<"\\bCURRENT_TIMESTAMP\\b"	                <<"\\bcurrent_timestamp\\b"
            <<"\\bCURRENT_USER\\b"	                <<"\\bcurrent_user\\b"
            <<"\\bCURSOR\\b"	                        <<"\\bcursor\\b"
            <<"\\bDATABASE\\b"	                        <<"\\bdatabase\\b"
            <<"\\bDATABASES\\b"	                        <<"\\bdatabases\\b"
            <<"\\bDAY_HOUR\\b"	                        <<"\\bday_hour\\b"
            <<"\\bDAY_MICROSECOND\\b"	                <<"\\bday_microsecond\\b"
            <<"\\bDAY_MINUTE\\b"	                <<"\\bday_minute\\b"
            <<"\\bDAY_SECOND\\b"	                <<"\\bday_second\\b"
            <<"\\bDECLARE\\b"	                        <<"\\bdeclare\\b"
            <<"\\bDEFAULT\\b"	                        <<"\\bdefault\\b"
            <<"\\bDELAYED\\b"	                        <<"\\bdelayed\\b"
            <<"\\bDELETE\\b"	                        <<"\\bdelete\\b"
            <<"\\bDESC\\b"	                        <<"\\bdesc\\b"
            <<"\\bDESCRIBE\\b"	                        <<"\\bdescribe\\b"
            <<"\\bDETERMINISTIC\\b"	                <<"\\bdeterministic\\b"
            <<"\\bDISTINCT\\b"	                        <<"\\bdistinct\\b"
            <<"\\bDISTINCTROW\\b"	                <<"\\bdistinctrow\\b"
            <<"\\bDIV\\b"	                        <<"\\bdiv\\b"
            <<"\\bDROP\\b"	                        <<"\\bdrop\\b"
            <<"\\bDUAL\\b"	                        <<"\\bdual\\b"
            <<"\\bEACH\\b"	                        <<"\\beach\\b"
            <<"\\bELSE\\b"	                        <<"\\belse\\b"
            <<"\\bELSEIF\\b"	                        <<"\\belseif\\b"
            <<"\\bENCLOSED\\b"	                        <<"\\benclosed\\b"
            <<"\\bESCAPED\\b"	                        <<"\\bescaped\\b"
            <<"\\bEXISTS\\b"	                        <<"\\bexists\\b"
            <<"\\bEXIT\\b"	                        <<"\\bexit\\b"
            <<"\\bEXPLAIN\\b"	                        <<"\\bexplain\\b"
            <<"\\bFALSE\\b"	                        <<"\\bfalse\\b"
            <<"\\bFETCH\\b"	                        <<"\\bfetch\\b"
            <<"\\bFOR\\b"	                        <<"\\bfor\\b"
            <<"\\bFORCE\\b"	                        <<"\\bforce\\b"
            <<"\\bFOREIGN\\b"	                        <<"\\bforeign\\b"
            <<"\\bFROM\\b"	                        <<"\\bfrom\\b"
            <<"\\bFULLTEXT\\b"	                        <<"\\bfulltext\\b"
            <<"\\bGRANT\\b"	                        <<"\\bgrant\\b"
            <<"\\bGROUP\\b"	                        <<"\\bgroup\\b"
            <<"\\bHAVING\\b"	                        <<"\\bhaving\\b"
            <<"\\bHIGH_PRIORITY\\b"	                <<"\\bhigh_priority\\b"
            <<"\\bHOUR_MICROSECOND\\b"	                <<"\\bhour_microsecond\\b"
            <<"\\bHOUR_MINUTE\\b"	                <<"\\bhour_minute\\b"
            <<"\\bHOUR_SECOND\\b"	                <<"\\bhour_second\\b"
            <<"\\bIF\\b"	                        <<"\\bif\\b"
            <<"\\bIGNORE\\b"	                        <<"\\bignore\\b"
            <<"\\bIN\\b"	                        <<"\\bin\\b"
            <<"\\bINDEX\\b"	                        <<"\\bindex\\b"
            <<"\\bINFILE\\b"	                        <<"\\binfile\\b"
            <<"\\bINNER\\b"	                        <<"\\binner\\b"
            <<"\\bINOUT\\b"	                        <<"\\binout\\b"
            <<"\\bINSENSITIVE\\b"	                <<"\\binsensitive\\b"
            <<"\\bINSERT\\b"	                        <<"\\binsert\\b"
            <<"\\bINTERVAL\\b"	                        <<"\\binterval\\b"
            <<"\\bINTO\\b"	                        <<"\\binto\\b"
            <<"\\bIS\\b"	                        <<"\\bis\\b"
            <<"\\bITERATE\\b"	                        <<"\\biterate\\b"
            <<"\\bJOIN\\b"	                        <<"\\bjoin\\b"
            <<"\\bKEY\\b"	                        <<"\\bkey\\b"
            <<"\\bKEYS\\b"	                        <<"\\bkeys\\b"
            <<"\\bKILL\\b"	                        <<"\\bkill\\b"
            <<"\\bLEADING\\b"	                        <<"\\bleading\\b"
            <<"\\bLEAVE\\b"	                        <<"\\bleave\\b"
            <<"\\bLEFT\\b"	                        <<"\\bleft\\b"
            <<"\\bLIKE\\b"	                        <<"\\blike\\b"
            <<"\\bLIMIT\\b"	                        <<"\\blimit\\b"
            <<"\\bLINES\\b"	                        <<"\\blines\\b"
            <<"\\bLOAD\\b"	                        <<"\\bload\\b"
            <<"\\bLOCALTIME\\b"	                        <<"\\blocaltime\\b"
            <<"\\bLOCALTIMESTAMP\\b"	                <<"\\blocaltimestamp\\b"
            <<"\\bLOCK\\b"	                        <<"\\block\\b"
            <<"\\bLONG\\b"	                        <<"\\blong\\b"
            <<"\\bLONGBLOB\\b"	                        <<"\\blongblob\\b"
            <<"\\bLONGTEXT\\b"	                        <<"\\blongtext\\b"
            <<"\\bLOOP\\b"	                        <<"\\bloop\\b"
            <<"\\bLOW_PRIORITY\\b"	                <<"\\blow_priority\\b"
            <<"\\bMATCH\\b"	                        <<"\\bmatch\\b"
            <<"\\bMEDIUMBLOB\\b"	                <<"\\bmediumblob\\b"
            <<"\\bMEDIUMINT\\b"	                        <<"\\bmediumint\\b"
            <<"\\bMEDIUMTEXT\\b"	                <<"\\bmediumtext\\b"
            <<"\\bMIDDLEINT\\b"	                        <<"\\bmiddleint\\b"
            <<"\\bMINUTE_MICROSECOND\\b"	        <<"\\bminute_microsecond\\b"
            <<"\\bMINUTE_SECOND\\b"	                <<"\\bminute_second\\b"
            <<"\\bMOD\\b"	                        <<"\\bmod\\b"
            <<"\\bMODIFIES\\b"	                        <<"\\bmodifies\\b"
            <<"\\bNATURAL\\b"	                        <<"\\bnatural\\b"
            <<"\\bNOT\\b"	                        <<"\\bnot\\b"
            <<"\\bNO_WRITE_TO_BINLOG\\b"	        <<"\\bno_write_to_binlog\\b"
            <<"\\bNULL\\b"	                        <<"\\bnull\\b"
            <<"\\bON\\b"	                        <<"\\bon\\b"
            <<"\\bOPTIMIZE\\b"	                        <<"\\boptimize\\b"
            <<"\\bOPTION\\b"	                        <<"\\boption\\b"
            <<"\\bOPTIONALLY\\b"	                <<"\\boptionally\\b"
            <<"\\bOR\\b"	                        <<"\\bor\\b"
            <<"\\bORDER\\b"	                        <<"\\border\\b"
            <<"\\bOUT\\b"	                        <<"\\bout\\b"
            <<"\\bOUTER\\b"	                        <<"\\bouter\\b"
            <<"\\bOUTFILE\\b"	                        <<"\\boutfile\\b"
            <<"\\bPRECISION\\b"	                        <<"\\bprecision\\b"
            <<"\\bPRIMARY\\b"	                        <<"\\bprimary\\b"
            <<"\\bPROCEDURE\\b"	                        <<"\\bprocedure\\b"
            <<"\\bPURGE\\b"	                        <<"\\bpurge\\b"
            <<"\\bREAD\\b"	                        <<"\\bread\\b"
            <<"\\bREADS\\b"	                        <<"\\breads\\b"
            <<"\\bREAL\\b"	                        <<"\\breal\\b"
            <<"\\bREFERENCES\\b"	                <<"\\breferences\\b"
            <<"\\bREGEXP\\b"	                        <<"\\bregexp\\b"
            <<"\\bRELEASE\\b"	                        <<"\\brelease\\b"
            <<"\\bRENAME\\b"	                        <<"\\brename\\b"
            <<"\\bREPEAT\\b"	                        <<"\\brepeat\\b"
            <<"\\bREPLACE\\b"	                        <<"\\breplace\\b"
            <<"\\bREQUIRE\\b"	                        <<"\\brequire\\b"
            <<"\\bRESTRICT\\b"	                        <<"\\brestrict\\b"
            <<"\\bRETURN\\b"	                        <<"\\breturn\\b"
            <<"\\bREVOKE\\b"	                        <<"\\brevoke\\b"
            <<"\\bRIGHT\\b"	                        <<"\\bright\\b"
            <<"\\bRLIKE\\b"	                        <<"\\brlike\\b"
            <<"\\bSCHEMA\\b"	                        <<"\\bschema\\b"
            <<"\\bSCHEMAS\\b"	                        <<"\\bschemas\\b"
            <<"\\bSECOND_MICROSECOND\\b"	        <<"\\bsecond_microsecond\\b"
            <<"\\bSELECT\\b"	                        <<"\\bselect\\b"
            <<"\\bSENSITIVE\\b"	                        <<"\\bsensitive\\b"
            <<"\\bSEPARATOR\\b"	                        <<"\\bseparator\\b"
            <<"\\bSET\\b"	                        <<"\\bset\\b"
            <<"\\bSHOW\\b"	                        <<"\\bshow\\b"
            <<"\\bSONAME\\b"	                        <<"\\bsoname\\b"
            <<"\\bSPATIAL\\b"	                        <<"\\bspatial\\b"
            <<"\\bSPECIFIC\\b"	                        <<"\\bspecific\\b"
            <<"\\bSQL\\b"	                        <<"\\bsql\\b"
            <<"\\bSQLEXCEPTION\\b"	                <<"\\bsqlexception\\b"
            <<"\\bSQLSTATE\\b"	                        <<"\\bsqlstate\\b"
            <<"\\bSQLWARNING\\b"	                <<"\\bsqlwarning\\b"
            <<"\\bSQL_BIG_RESULT\\b"	                <<"\\bsql_big_result\\b"
            <<"\\bSQL_CALC_FOUND_ROWS\\b"	        <<"\\bsql_calc_found_rows\\b"
            <<"\\bSQL_SMALL_RESULT\\b"	                <<"\\bsql_small_result\\b"
            <<"\\bSSL\\b"	                        <<"\\bssl\\b"
            <<"\\bSTARTING\\b"	                        <<"\\bstarting\\b"
            <<"\\bSTRAIGHT_JOIN\\b"	                <<"\\bstraight_join\\b"
            <<"\\bTABLE\\b"	                        <<"\\btable\\b"
            <<"\\bTERMINATED\\b"	                <<"\\bterminated\\b"
            <<"\\bTHEN\\b"	                        <<"\\bthen\\b"
            <<"\\bTO\\b"	                        <<"\\bto\\b"
            <<"\\bTRAILING\\b"	                        <<"\\btrailing\\b"
            <<"\\bTRIGGER\\b"	                        <<"\\btrigger\\b"
            <<"\\bTRUE\\b"	                        <<"\\btrue\\b"
            <<"\\bUNDO\\b"	                        <<"\\bundo\\b"
            <<"\\bUNION\\b"	                        <<"\\bunion\\b"
            <<"\\bUNIQUE\\b"	                        <<"\\bunique\\b"
            <<"\\bUNLOCK\\b"	                        <<"\\bunlock\\b"
            <<"\\bUNSIGNED\\b"	                        <<"\\bunsigned\\b"
            <<"\\bUPDATE\\b"	                        <<"\\bupdate\\b"
            <<"\\bUSAGE\\b"	                        <<"\\busage\\b"
            <<"\\bUSE\\b"	                        <<"\\buse\\b"
            <<"\\bUSING\\b"	                        <<"\\busing\\b"
            <<"\\bUTC_DATE\\b"	                        <<"\\butc_date\\b"
            <<"\\bUTC_TIME\\b"	                        <<"\\butc_time\\b"
            <<"\\bUTC_TIMESTAMP\\b"	                <<"\\butc_timestamp\\b"
            <<"\\bVALUES\\b"	                        <<"\\bvalues\\b"
            <<"\\bVARYING\\b"	                        <<"\\bvarying\\b"
            <<"\\bWHEN\\b"	                        <<"\\bwhen\\b"
            <<"\\bWHERE\\b"	                        <<"\\bwhere\\b"
            <<"\\bWHILE\\b"	                        <<"\\bwhile\\b"
            <<"\\bWITH\\b"	                        <<"\\bwith\\b"
            <<"\\bWRITE\\b"	                        <<"\\bwrite\\b"
            <<"\\bXOR\\b"	                        <<"\\bxor\\b"
            <<"\\bYEAR_MONTH\\b"	                <<"\\byear_month\\b"
            <<"\\bZEROFILL\\b"	                        <<"\\bzerofill\\b"
            <<"\\bASENSITIVE\\b"                        <<"\\basensitive\\b"
            <<"\\bCONNECTION\\b"                        <<"\\bconnection\\b"
            <<"\\bDECLARE\\b"                           <<"\\bdeclare\\b"
            <<"\\bELSEIF\\b"                            <<"\\belseif\\b"
            <<"\\bGOTO\\b"                              <<"\\bgoto\\b"
            <<"\\bITERATE\\b"                           <<"\\biterate\\b"
            <<"\\bLOOP\\b"                              <<"\\bloop\\b"
            <<"\\bREADS\\b"                             <<"\\breads\\b"
            <<"\\bRETURN\\b"                            <<"\\breturn\\b"
            <<"\\bSENSITIVE\\b"                         <<"\\bsensitive\\b"
            <<"\\bSQLEXCEPTION\\b"                      <<"\\bsqlexception\\b"
            <<"\\bTRIGGER\\b"                           <<"\\btrigger\\b"
            <<"\\bWHILE\\b"                             <<"\\bwhile\\b"
            <<"\\bCALL\\b"                              <<"\\bcall\\b"
            <<"\\bCONTINUE\\b"                          <<"\\bcontinue\\b"
            <<"\\bDETERMINISTIC\\b"                     <<"\\bdeterministic\\b"
            <<"\\bEXIT\\b"                              <<"\\bexit\\b"
            <<"\\bINOUT\\b"                             <<"\\binout\\b"
            <<"\\bLABEL\\b"                             <<"\\blabel\\b"
            <<"\\bMODIFIES\\b"                          <<"\\bmodifies\\b"
            <<"\\bRELEASE\\b"                           <<"\\brelease\\b"
            <<"\\bSCHEMA\\b"                            <<"\\bschema\\b"
            <<"\\bSPECIFIC\\b"                          <<"\\bspecific\\b"
            <<"\\bSQLSTATE\\b"                          <<"\\bsqlstate\\b"
            <<"\\bUNDO\\b"                              <<"\\bundo\\b"
            <<"\\bCONDITION\\b"                         <<"\\bcondition\\b"
            <<"\\bCURSOR\\b"                            <<"\\bcursor\\b"
            <<"\\bEACH\\b"                              <<"\\beach\\b"
            <<"\\bFETCH\\b"                             <<"\\bfetch\\b"
            <<"\\bINSENSITIVE\\b"                       <<"\\binsensitive\\b"
            <<"\\bLEAVE\\b"                             <<"\\bleave\\b"
            <<"\\bOUT\\b"                               <<"\\bout\\b"
            <<"\\bREPEAT\\b"                            <<"\\brepeat\\b"
            <<"\\bSCHEMAS\\b"                           <<"\\bschemas\\b"
            <<"\\bVIEW\\b"                              <<"\\bview\\b"
            <<"\\bSQL\\b"                               <<"\\bsql\\b"
            <<"\\bSQLWARNING\\b"                	<<"\\bsqlwarning\\b"
            <<"\\bUPGRADE\\b"                           <<"\\bupgrade\\b";

    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    commentStartExpression = QRegExp("--");

    typesFormat.setForeground(Qt::darkBlue);
    typesFormat.setFontWeight(QFont::Bold);

    QStringList typesPatterns;
    typesPatterns
            <<"\\bBIGINT\\b"	    <<"\\bbigint\\b"
            <<"\\bBINARY\\b"	    <<"\\bbinary\\b"
            <<"\\bBLOB\\b"	    <<"\\bblob\\b"
            <<"\\bBOOL\\b"	    <<"\\bbool\\b"
            <<"\\bCHAR\\b"	    <<"\\bchar\\b"
            <<"\\bCHARACTER\\b"	    <<"\\bcharacter\\b"
            <<"\\bDEC\\b"	    <<"\\bdec\\b"
            <<"\\bDECIMAL\\b"	    <<"\\bdecimal\\b"
            <<"\\bDOUBLE\\b"	    <<"\\bdouble\\b"
            <<"\\bFLOAT\\b"	    <<"\\bfloat\\b"
            <<"\\bFLOAT4\\b"	    <<"\\bfloat4\\b"
            <<"\\bFLOAT8\\b"	    <<"\\bfloat8\\b"
            <<"\\bINT\\b"	    <<"\\bint\\b"
            <<"\\bINT1\\b"	    <<"\\bint1\\b"
            <<"\\bINT2\\b"	    <<"\\bint2\\b"
            <<"\\bINT3\\b"	    <<"\\bint3\\b"
            <<"\\bINT4\\b"	    <<"\\bint4\\b"
            <<"\\bINT8\\b"	    <<"\\bint8\\b"
            <<"\\bINTEGER\\b"	    <<"\\binteger\\b"
            <<"\\bSMALLINT\\b"	    <<"\\bsmallint\\b"
            <<"\\bTINYBLOB\\b"	    <<"\\btinyblob\\b"
            <<"\\bTINYINT\\b"	    <<"\\btinyint\\b"
            <<"\\bTINYTEXT\\b"	    <<"\\btinytext\\b"
            <<"\\bVARBINARY\\b"    <<"\\bvarbinary\\b"
            <<"\\bVARCHAR\\b"	    <<"\\bvarchar\\b"
            <<"\\bVARCHARACTER\\b"  <<"\\bvarcharacter\\b"
            <<"\\bNUMERIC\\b"	    <<"\\bnumeric\\b";

    // the keywords
    foreach (const QString &pattern, typesPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = typesFormat;
        highlightingRules.append(rule);
    }

    // the one line comment
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    singleLineCommentFormat.setFontItalic(true);
    rule.pattern = QRegExp("--[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // the funny (??) question mark
    questionMarksFormat.setForeground(Qt::red);
    questionMarksFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("(?:\\s|^)\\(\\?\\?\\)(?:\\s|$)"); /*QRegExp("\\B\\(\\?\\?\\)\\B");*/
    rule.format = questionMarksFormat;
    highlightingRules.append(rule);

    // the table names
    tableNamesFormat.setForeground(Qt::darkGray);
    tableNamesFormat.setFontWeight(QFont::Bold);
    QVector<Table*> tables = Workspace::getInstance()->workingVersion()->getTables();
    for(int i=0; i< tables.size(); i++)
    {
        QString p = "\\b" + tables.at(i)->getName() + "\\b";
        rule.pattern = QRegExp(p);
        rule.format = tableNamesFormat;
        highlightingRules.append(rule);
    }

    // the column names
    columnNamesFormat.setForeground(QColor(87,17,6));
    columnNamesFormat.setFontWeight(QFont::Bold);
    for(int i=0; i< tables.size(); i++)
    {
        for(int j=0; j<tables.at(i)->fullColumns().size(); j++)
        {
            QString p = "\\b" + tables.at(i)->fullColumns().at(j) + "\\b";
            rule.pattern = QRegExp(p);
            rule.format = columnNamesFormat;
            highlightingRules.append(rule);
        }
    }

    // TODO: Add the columns from the views too, with the same format as above
}

void SqlHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}
