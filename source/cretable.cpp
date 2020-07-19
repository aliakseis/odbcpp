#include <sql.hpp>
#include <memory.h> // for memset
#include <cstring> // for strcpy, strcat, ...
#include <cctype>  // for isdigit, isspace, ...
#include <cstdio>  // sprintf...
#include <cstdlib> // itoa

// function to look back in an array of sCOLBINDs to see if the name
// at a given position is duplicated in a previous entry.
static bool DupName(
    LPSTR lpszName,
    const sCOLBIND* cpsColCreate,
    UWORD uPosInArray
)
{
    // look back and see if this name duplicates
    // any name that came before.
    for (UWORD j = 0; j < uPosInArray; j++)
    {
        if (!_stricmp(lpszName,
            (LPSTR)cpsColCreate[j].szColName))
        {
            return true;
        }
    }
    return false;
}

/***************************************************

    odbcTABLECREATOR

    Constructor.
***************************************************/

odbcTABLECREATOR::odbcTABLECREATOR(odbcCONNECT* pConnect)
    : odbcCURSOR(pConnect)
{
    SWORD bSave = AutoReport(odbcNOREPORT);

    lpszCreateStmt_ = new char[SQLSTMT_MAX];
    token_ = new char[STRING2_MAX];
    if (!token_ || !lpszCreateStmt_)
    {
        SetRC(SQL_ALLOC_FAILED);
        bError = true;
    }

    szTab = "\t";
    szEoln = "\n";

    AutoReport(bSave);
}

/***************************************************

    ~odbcTABLECREATOR

    Destructor.
***************************************************/

odbcTABLECREATOR::~odbcTABLECREATOR()
{
    delete[] token_;
    delete[] lpszCreateStmt_;
}

RETCODE odbcTABLECREATOR::CreateTable(
    LPCSTR       lpszTableName,
    const sCOLBIND*   cpsColCreate,
    UWORD        uColCount,
    LPCSTR       lpszTableConstraints   /* = NULL */,
    LPSTR        lpszSqlStmtCreated     /* = NULL */,
    UWORD *      puStmtBufferSize       /* = NULL */,
    bool         bJustGenerateSql       /* = false */,
    bool         bNoExtraByteOnStrings  /* = false */,
    bool         bResolveDupNames       /* = true */
)
{
    RETCODE ret = SQL_SUCCESS;
    size_t len = 0;
    UWORD i;
    sCOLBIND*   psColCreate = nullptr;
    bool bSaveExtraByteValue;
    UDWORD cbValueMax;
    LPCSTR lpszQuote = pConn->GetIdentifierQuoteMark();
    // allocate a result set struct for SQLGetTypeInfo
    auto* pTypeInfoSet = new sSQLTYPEINFORESULTSET;

    if (!pTypeInfoSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    if (!lpszCreateStmt_)
    {
        SetRC(SQL_ALLOC_FAILED);
        delete pTypeInfoSet;

        return lastRC();
    }

    // clean up in case this is the second time through
    Close();
    Unbind();
    FreeColBindings();

    // set up for extended fetch operation for type info
    UDWORD ScrollOptions = 0;

    pConn->GetInfo(SQL_SCROLL_OPTIONS,
        &ScrollOptions,
        sizeof(UDWORD),
        nullptr);
    if (ScrollOptions & SQL_SO_KEYSET_DRIVEN)
    {
        ExtFetchSetup(SQL_CONCUR_VALUES,
            SQL_CURSOR_KEYSET_DRIVEN,
            1
        );
    }
    else
    {
        ExtFetchSetup(SQL_CONCUR_VALUES,
            SQL_CURSOR_STATIC,
            1
        );
    }

    if (!sqlsuccess())
    {
        delete pTypeInfoSet;

        return lastRC();
    }

    // get type information
    GetTypeInfo(SQL_ALL_TYPES);

    if (!sqlsuccess())
    {
        delete pTypeInfoSet;

        return lastRC();
    }

    bSaveExtraByteValue = SetNoExtraByteOnStrings(true);

    // use automatic data dictionary binding
    SetColBindings(
        pConn->SQLTypeInfoRSB(),
        pConn->SQLTypeInfoRSBCount(),
        pTypeInfoSet
    );
    BindCol();

    SetNoExtraByteOnStrings(bSaveExtraByteValue);

    if (!sqlsuccess())
    {
        delete pTypeInfoSet;

        return lastRC();
    }

    // resolve duplicate names, if so desired
    if (bResolveDupNames)
    {
        bool bDupWasFound = false;
        psColCreate = new sCOLBIND[uColCount];

        // if we couldn't allocate, leave
        if (!psColCreate)
        {
            SetRC(SQL_ALLOC_FAILED);
            delete pTypeInfoSet;

            return lastRC();
        }

        // make copy of array; we'll modify the copy
        for (i = 0; i < uColCount; i++)
            psColCreate[i] = cpsColCreate[i];

        // cycle through the list starting with the second element
        for (i = 1; i < uColCount; i++)
        {
            // ignore entries that aren't duplicates
            if (!DupName((LPSTR)psColCreate[i].szColName,
                psColCreate,
                i)
                )
                continue;

            // duplication has occurred; change the i'th name
            bDupWasFound = true;

            char * pDupName
                = new char[strlen((LPSTR)psColCreate[i].szColName)
                + 6];

            // if we couldn't allocate, leave
            if (!pDupName)
            {
                delete[] psColCreate;
                delete pTypeInfoSet;

                SetRC(SQL_ALLOC_FAILED);
                return lastRC();
            }
            // Now we will attempt to use a tack-on value
            // to attempt to make the name unique. If for some
            // reason this still results in a duplicate, add
            // one to the position and try again until it works.
            // (use a duplicate to work with so we don't lose
            // our position.)
            UWORD uAddOn = 2;
            do {
                // make a copy of the original
                strcpy(pDupName, (LPSTR)psColCreate[i].szColName);

                // make sure we've got room to work
                if (strlen(pDupName)
        > sizeof(psColCreate[i].szColName) - 6)
                    pDupName[sizeof(psColCreate[i].szColName) - 6]
                    = 0;
                // convert number to string at end of name
                _itoa((int)uAddOn, &pDupName[strlen(pDupName)], 10);

                // add one in case this name produces a duplicate.
                uAddOn += (UWORD)1;
            } while (DupName(pDupName, psColCreate, i));

            // copy non-duplicate name to the array
            strcpy((LPSTR)psColCreate[i].szColName, pDupName);

            // free local memory
            delete[] pDupName;

        } // end for loop

        if (!bDupWasFound)
        {
            delete[] psColCreate;
            psColCreate = nullptr;
        }
        else
            cpsColCreate = psColCreate;
    }

    // begin statement
    sprintf(lpszCreateStmt_, "CREATE TABLE %s%s%s (%s",
        lpszQuote,
        lpszTableName,
        lpszQuote,
        szEoln);

    // cycle through the columns

    for (i = 0; i < uColCount; i++)
    {
        if (i > 0)
        {
            strcat(lpszCreateStmt_, ",");
            strcat(lpszCreateStmt_, szEoln);
        }

        strcat(lpszCreateStmt_, szTab);

        // add name and a space
        strcat(lpszCreateStmt_, lpszQuote);
        strcat(lpszCreateStmt_, (LPCSTR)cpsColCreate[i].szColName);
        strcat(lpszCreateStmt_, lpszQuote);
        strcat(lpszCreateStmt_, " ");

        // add type information

        // we don't use this but we do it to keep our call pro forma
        len = strlen(lpszCreateStmt_);

        // if bNoExtraByteOnStrings is false, subtract 1 from the
        // length of any character SQL type columns, since
        // the data dictionary defines the receiving structure,
        // which has an extra byte on string fields.

        cbValueMax = cpsColCreate[i].cbValueMax;

        if (TypeIsCharacter(cpsColCreate[i].fSqlType))
            if (!bNoExtraByteOnStrings)
                cbValueMax--;

        ret = ConvertCreateColumnType(
            lpszCreateStmt_,
            &len,
            GetCreateTypeName(cpsColCreate[i].fSqlType),
            cpsColCreate[i].fSqlType,
            cbValueMax,
            cpsColCreate[i].ibScale
        );

        if (ret != SQL_SUCCESS)
        {
            break;
        }

        // add column constraints if present
        if (cpsColCreate[i].pConstraints)
        {
            strcat(lpszCreateStmt_, szEoln);
            strcat(lpszCreateStmt_, szTab);
            strcat(lpszCreateStmt_, szTab);
            strcat(lpszCreateStmt_,
                (LPCSTR)
                cpsColCreate[i].pConstraints);
        }
    } // end for loop

    if (NoDataFound())
        SetRC(SQL_SUCCESS);

    if (ret == SQL_SUCCESS)
    {
        // add table constraints if they exist
        if (lpszTableConstraints)
        {
            if (i > 0)
            {
                strcat(lpszCreateStmt_, ",");
                strcat(lpszCreateStmt_, szEoln);
            }

            strcat(lpszCreateStmt_, szTab);

            strcat(lpszCreateStmt_, lpszTableConstraints);
        }

        strcat(lpszCreateStmt_, szEoln);
        strcat(lpszCreateStmt_, ")");
        strcat(lpszCreateStmt_, szEoln);

        // execute if so desired
        if (!bJustGenerateSql)
        {
            Close();
            Unbind();
            FreeColBindings();
            ExecDirect(lpszCreateStmt_);
        }

        // copy to buffer if so desired
        if (lpszSqlStmtCreated)
        {
            // if the buffer can hold the created string,
            // copy it to the buffer whose address the caller
            // passed in.
            if (*puStmtBufferSize > strlen(lpszCreateStmt_))
                strcpy(lpszSqlStmtCreated, lpszCreateStmt_);

            // always copy the size of the string output.
            *puStmtBufferSize = (UWORD)(strlen(lpszCreateStmt_) + 1);
        }
    } // end if OK so far
    else
    {
        SetRC(ret);
        delete pTypeInfoSet;

        return lastRC();
    }

    // free column bindings array if we allocated it.
    if (psColCreate)
    {
        delete[] psColCreate;
        psColCreate = nullptr;
    }

    // clean up the cursor state
    if (!bJustGenerateSql)
    {
        Close();
        Unbind();
        FreeColBindings();

    }

    delete pTypeInfoSet;

    // return status
    return lastRC();
}

RETCODE odbcTABLECREATOR::CreateTable(
    LPCSTR       lpszSqlIn,
    LPSTR        lpszSqlStmtCreated /* = NULL */,
    UWORD *      puStmtBufferSize /* = NULL */,
    bool         bJustGenerateSql /* = false */
)
{
    LPCSTR curpos = lpszSqlIn;
    LPSTR innerpos;
    UDWORD cbColDef;
    SWORD  ibScale;
    RETCODE ret = SQL_SUCCESS;
    RETCODE subret = SQL_SUCCESS;
    unsigned i;
    unsigned j = 0;
    bool bInToken;
    // allocate a result set struct for SQLGetTypeInfo
    auto* pTypeInfoSet = new sSQLTYPEINFORESULTSET;

    if (!pTypeInfoSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    if (!lpszCreateStmt_)
    {
        SetRC(SQL_ALLOC_FAILED);
        delete pTypeInfoSet;

        return lastRC();
    }

    memset(lpszCreateStmt_, 0, SQLSTMT_MAX);

    if (!token_)
    {
        SetRC(SQL_ALLOC_FAILED);
        delete pTypeInfoSet;
        return lastRC();
    }

    memset(token_, 0, STRING2_MAX);

    // set up for extended fetch operation for type info
    UDWORD ScrollOptions = 0;

    pConn->GetInfo(SQL_SCROLL_OPTIONS,
        &ScrollOptions,
        sizeof(UDWORD),
        nullptr);
    if (ScrollOptions & SQL_SO_KEYSET_DRIVEN)
    {
        ExtFetchSetup(SQL_CONCUR_VALUES,
            SQL_CURSOR_KEYSET_DRIVEN,
            1
        );
    }
    else
    {
        ExtFetchSetup(SQL_CONCUR_VALUES,
            SQL_CURSOR_STATIC,
            1
        );
    }

    if (!sqlsuccess())
    {
        delete pTypeInfoSet;
        return lastRC();
    }

    // get type information
    GetTypeInfo(SQL_ALL_TYPES);

    if (!sqlsuccess())
    {
        delete pTypeInfoSet;
        return lastRC();
    }

    SetNoExtraByteOnStrings(true);

    // use automatic data dictionary binding
    SetColBindings(
        pConn->SQLTypeInfoRSB(),
        pConn->SQLTypeInfoRSBCount(),
        pTypeInfoSet
    );
    BindCol();

    if (!sqlsuccess())
    {
        delete pTypeInfoSet;
        return lastRC();
    }

    // copy until the opening parenthesis is reached
    for (i = 0; *curpos != '(' && *curpos != 0; curpos++)
        lpszCreateStmt_[i++] = *curpos;

    // if ill-formed, exit early
    if (*curpos != '(')
    {
        ret = SQL_ILLFORMED_STMT;
    }
    else
    {
        lpszCreateStmt_[i++] = *curpos;
    }

    // now until we encounter end of parentheses or
    // end of string, we process column definitions

    for (curpos++, bInToken = false;
        ret == SQL_SUCCESS && *curpos != 0;
        curpos++)
    {
        if (*curpos == '<') // opening of token...
        {
            if (bInToken)
            {
                ret = SQL_ILLFORMED_STMT;
                continue;
            }


            bInToken = true;
            j = 0;
            memset(token_, 0, STRING2_MAX);
            continue;

        }
        if (*curpos == '>') // close of token...
        {
            if (!bInToken)
            {
                ret = SQL_ILLFORMED_STMT;
                continue;
            }


            bInToken = false;
            // end-of-token reached.
            // this is the only place in this nested
            // if where we don't continue

        }
        else if (!bInToken)
        {
            lpszCreateStmt_[i++] = *curpos;
            continue;
        }
        else
        {
            token_[j++] = *curpos;
            continue;
        }

        // if we get this far, we have reached an end-of-token
        // and should process the token.

        if (j == 0)
        {
            ret = SQL_ILLFORMED_STMT;
            continue;
        }

        // we have collected a token. Now we must parse it and find
        // out the desired type, and get the precision and scale
        // if any.

        cbColDef = 0;
        ibScale = 0;

        // find out if there is a precision and/or scale
        // form of token is "<name(precision,scale)>"
        // and we have removed the outer angle braces.

        innerpos = strstr(token_, "(");

        // eliminate any trailing blanks on the token
        TrimTrailingBlanks(token_);

        if (innerpos)
        {
            // null-terminate name portion of token
            *innerpos = 0;

            // look beyond where opening parenthesis was
            innerpos++;

            // find beginning of precision
            while (*innerpos
                && isspace(*innerpos))
                innerpos++;

            if (!(*innerpos) || !isdigit(*innerpos))
            {
                ret = SQL_ILLFORMED_STMT;
                continue;
            }

            do
            {
                cbColDef *= 10;
                cbColDef += (*innerpos - '0');
                innerpos++;
            } while (*innerpos && isdigit(*innerpos));

            // now do scale
            while (*innerpos && isspace(*innerpos))
                innerpos++;

            // if we're out of string, or if the next character
            // is not the end of the parentheses or the comma before
            // the scale, it's an error.
            if (!(*innerpos)
                || (!(
                ((*innerpos == ')')
                    ||
                    (*innerpos == ',')
                    )
                    )
                    )
                )
            {
                ret = SQL_ILLFORMED_STMT;
                continue;
            }

            if (*innerpos == ',')
            {
                // look beyond where comma was
                innerpos++;

                // there is a scale. get past white space
                while (*innerpos && isspace(*innerpos))
                    innerpos++;

                // if there is no more, it's an error

                if (!(*innerpos) || !isdigit(*innerpos))
                {
                    ret = SQL_ILLFORMED_STMT;
                    continue;
                }

                // collect the scale
                do
                {
                    ibScale *= (SWORD)10;
                    ibScale += (SWORD)(*innerpos - '0');
                    innerpos++;
                } while (*innerpos && isdigit(*innerpos));

                // if we didn't end on white space or an end parenthesis,
                // it's an error
                while (*innerpos && isspace(*innerpos))
                    innerpos++;

                if (!(*innerpos == ')'))
                {
                    ret = SQL_ILLFORMED_STMT;
                    continue;
                }
            } // end if there was a scale
        } // end if there was a precision

        // we now have a token, a precision, and a scale.
        // look up the token and process the entry
        ret = ConvertCreateColumnType(lpszCreateStmt_, &i, token_, cbColDef, ibScale);

        // keep processing columns if there was an error, so
        // all errors are found even if one occurs early.
        if (ret != SQL_SUCCESS)
        {
            subret = ret;
            ret = SQL_SUCCESS;
        }
    } // end for loop

    if (NoDataFound())
        SetRC(SQL_SUCCESS);

    if (subret != SQL_SUCCESS)
    {
        ret = subret;
        SetRC(ret);
    }

    // execute if so desired
    if (sqlsuccess() && !bJustGenerateSql)
    {
        Close();
        ExecDirect(lpszCreateStmt_);
    }
    else if (subret != SQL_SUCCESS)
    {
        SetRC(subret); // register internal error
        ret = lastRC();   // get new error/warning code
    }

    // copy to buffer if so desired
    if (lpszSqlStmtCreated)
    {
        // if the buffer can hold the created string,
        // copy it to the buffer whose address the caller
        // passed in.
        if (*puStmtBufferSize > strlen(lpszCreateStmt_))
            strcpy(lpszSqlStmtCreated, lpszCreateStmt_);

        // always copy the size of the string output.
        *puStmtBufferSize = (UWORD)(strlen(lpszCreateStmt_) + 1);
    }

    if (sqlsuccess())
    {
        // clean up the cursor state
        Close();
        Unbind();
    }

    // return status
    delete pTypeInfoSet;

    return lastRC();
}

/***************************************************

    ColumnTypeFromToken

    Convert user-passed standard column type name to
    an ODBC SQL_xxxx data type; also set precision to
    minimum value if equal to zero.
***************************************************/

RETCODE odbcTABLECREATOR::ColumnTypeFromToken(LPCSTR       token,
    SWORD     *  pfType,
    UDWORD     * pfPrecision)
{
    SWORD fType = 0;
    RETCODE ret = SQL_SUCCESS;
    UDWORD fPrecision = *pfPrecision;

    if (_stricmp(token, "CHAR") == 0)
        fType = SQL_CHAR;

    if (fType == 0 && _stricmp(token, "NUMERIC") == 0)
    {
        fType = SQL_NUMERIC;
    }

    if (fType == 0 && _stricmp(token, "DECIMAL") == 0)
        fType = SQL_DECIMAL;

    if (fType == 0 && _stricmp(token, "INTEGER") == 0)
    {
        fType = SQL_INTEGER;
        if (fPrecision < 10) fPrecision = 10;
    }

    if (fType == 0 && _stricmp(token, "SMALLINT") == 0)
    {
        fType = SQL_SMALLINT;
        if (fPrecision < 5) fPrecision = 5;
    }


    if (fType == 0 && _stricmp(token, "FLOAT") == 0)
    {
        fType = SQL_FLOAT;
        if (fPrecision < 7) fPrecision = 7;
    }

    if (fType == 0 && _stricmp(token, "REAL") == 0)
    {
        fType = SQL_REAL;
        if (fPrecision < 7) fPrecision = 7;
    }

    if (fType == 0 && _stricmp(token, "DOUBLE") == 0)
    {
        fType = SQL_DOUBLE;
        if (fPrecision < 15) fPrecision = 15;
    }

    if (fType == 0 && _stricmp(token, "VARCHAR") == 0)
        fType = SQL_VARCHAR;

    if (fType == 0 && _stricmp(token, "DATE") == 0)
        fType = SQL_DATE;

    if (fType == 0 && _stricmp(token, "TIME") == 0)
        fType = SQL_TIME;

    if (fType == 0 && _stricmp(token, "TIMESTAMP") == 0)
        fType = SQL_TIMESTAMP;

    if (fType == 0 && _stricmp(token, "LONGVARCHAR") == 0)
        fType = SQL_LONGVARCHAR;

    if (fType == 0 && _stricmp(token, "BINARY") == 0)
        fType = SQL_BINARY;

    if (fType == 0 && _stricmp(token, "VARBINARY") == 0)
        fType = SQL_VARBINARY;

    if (fType == 0 && _stricmp(token, "LONGVARBINARY") == 0)
        fType = SQL_LONGVARBINARY;

    if (fType == 0 && _stricmp(token, "BIGINT") == 0)
    {
        fType = SQL_BIGINT;
        if (fPrecision < 15) fPrecision = 15;
    }

    if (fType == 0 && _stricmp(token, "TINYINT") == 0)
    {
        fType = SQL_TINYINT;
        if (fPrecision < 3) fPrecision = 3;
    }

    if (fType == 0 && _stricmp(token, "BIT") == 0)
    {
        fType = SQL_BIT;
        if (fPrecision < 1) fPrecision = 1;
    }

    if (fType == 0)
        ret = SQL_ERROR;
    else
    {
        *pfPrecision = fPrecision;
        *pfType = fType;
    }

    return ret;
}

/***************************************************

    GetCreateTypeName

    GetCreateTypeName takes an ODBC SQL type and returns the name as
    a string for use in CreateTable.
***************************************************/

LPCSTR odbcTABLECREATOR::GetCreateTypeName(SWORD fSqlType)
{
    switch (fSqlType)
    {
    case  SQL_CHAR: //  1
        return "CHAR";

    case  SQL_NUMERIC: //  2
        return "NUMERIC";

    case  SQL_DECIMAL: //  3
        return "DECIMAL";

    case  SQL_INTEGER: //  4
        return "INTEGER";

    case  SQL_SMALLINT: //  5
        return "SMALLINT";

    case  SQL_FLOAT: //  6
        return "FLOAT";

    case  SQL_REAL: //  7
        return "REAL";

    case  SQL_DOUBLE: //  8
        return "DOUBLE";

    case  SQL_DATE: //  9
        return "DATE";

    case  SQL_TIME: // 10
        return "TIME";

    case  SQL_TIMESTAMP: // 11
        return "TIMESTAMP";

    case  SQL_VARCHAR: // 12
        return "VARCHAR";

    case  SQL_LONGVARCHAR: // (-1)
        return "LONGVARCHAR";

    case  SQL_BINARY: // (-2)
        return "BINARY";

    case  SQL_VARBINARY: // (-3)
        return "VARBINARY";

    case  SQL_LONGVARBINARY: // (-4)
        return "LONGVARBINARY";

    case  SQL_BIGINT: // (-5)
        return "BIGINT";

    case  SQL_TINYINT: // (-6)
        return "TINYINT";

    case  SQL_BIT: // (-7)
        return "BIT";

    default:
        break;
    } // end switch
    return "?";
}


RETCODE odbcTABLECREATOR::ConvertCreateColumnType(
    LPSTR       lpszCreateStmt,
    size_t *    pi,
    LPCSTR      token,
    UDWORD      cbColDef,
    SWORD       ibScale
)
{
    RETCODE ret = SQL_SUCCESS;
    SWORD  fType = 0;
    UDWORD fPrecision = cbColDef;
    size_t uOldLen = strlen(lpszCreateStmt);

    ret = ColumnTypeFromToken(token, &fType, &fPrecision);

    if (ret != SQL_SUCCESS || fType == 0)
    {
        sprintf(&lpszCreateStmt[strlen(lpszCreateStmt)],
            "<<Unrecognized type %s>>", token);
        // return length...
        *pi += (strlen(lpszCreateStmt) - uOldLen);
        // ... and error that occurred
        return SQL_ILLFORMED_STMT;
    }

    return ConvertCreateColumnType(
        lpszCreateStmt,
        pi,
        token,
        fType,
        fPrecision,
        ibScale
    );
}

RETCODE odbcTABLECREATOR::ConvertCreateColumnType(
    LPSTR       lpszCreateStmt,
    size_t *    pi,
    LPCSTR      token,
    SWORD       fType,
    UDWORD      cbColDef,
    SWORD       ibScale
)
{
    RETCODE ret = SQL_SUCCESS;
    UDWORD uNumOfRows = 0;
    auto fPrecision = (SDWORD)cbColDef;
    UDWORD fBestPrecision = 0;
    sSQLTYPEINFORESULTSET Set{};
    const sSQLTYPEINFORESULTSET* pSet;
    size_t uOldLen = strlen(lpszCreateStmt);
    bool bFound = false;

    pSet = (const sSQLTYPEINFORESULTSET*)pResultSet();

    // try to find an exact type match on SQL type.

    for (fBestPrecision = ~0,
        uNumOfRows = ExtFetchFirst();
        sqlsuccess() && uNumOfRows;
        uNumOfRows = ExtFetchNext())
    {
        if (fType == pSet->fDataType
            && pSet->fPrecision >= fPrecision)
        {
            bFound = true;
            if ((UDWORD)(pSet->fPrecision) < fBestPrecision)
            {
                Set = *pSet;
                fBestPrecision = pSet->fPrecision;
            }
        }
    } // end for loop

    if (bFound &&
        TypeIsInteger(fType) &&
        Set.fAutoIncrement)
    {
        // we found a match on an integer type, but
        // it was an autoincrement type; try for a better
        // match
        for (fBestPrecision = ~0,
            uNumOfRows = ExtFetchFirst();
            sqlsuccess() && uNumOfRows;
            uNumOfRows = ExtFetchNext())
        {
            if (fType == pSet->fDataType
                && pSet->fPrecision >= fPrecision
                && !pSet->fAutoIncrement)
            {
                if ((UDWORD)(pSet->fPrecision) < fBestPrecision)
                {
                    Set = *pSet;
                    fBestPrecision = pSet->fPrecision;
                }
            }
        } // end for loop
    } // end if

// try to find an exact type match on name.

    if (!bFound)
    {
        for (bFound = false, fBestPrecision = ~0,
            uNumOfRows = ExtFetchFirst();
            sqlsuccess() && uNumOfRows;
            uNumOfRows = ExtFetchNext())
        {
            if (_stricmp(token, (LPSTR)pSet->szTypeName) == 0
                && pSet->fPrecision >= fPrecision)
            {
                bFound = true;
                if ((UDWORD)(pSet->fPrecision) < fBestPrecision)
                {
                    Set = *pSet;
                    fBestPrecision = pSet->fPrecision;
                }
            }
        } // end for loop
    }

    if (!bFound && TypeIsCurrency(fType))
    {
        // find the best one we can convert to the desired type
        // (one that's also a currency type in this case)
        for (fBestPrecision = ~0,
            uNumOfRows = ExtFetchFirst();
            sqlsuccess() && uNumOfRows;
            uNumOfRows = ExtFetchNext())
        {
            if (sqlsuccess())
                if (pSet->fPrecision >= fPrecision)
                    if (TypeIsCurrency(pSet->fDataType))
                    {
                        bFound = true;
                        if ((UDWORD)(pSet->fPrecision) < fBestPrecision)
                        {
                            Set = *pSet;
                            fBestPrecision = pSet->fPrecision;
                        }
                    }
        } // end for loop

    } // sqlsuccess()

    if (!bFound && TypeIsInteger(fType))
    {
        // find the best one we can convert to the desired type
        // (one that's also an integral type in this case and not
        //   an autoincrement type integer)
        for (fBestPrecision = ~0,
            uNumOfRows = ExtFetchFirst();
            sqlsuccess() && uNumOfRows;
            uNumOfRows = ExtFetchNext())
        {
            if (sqlsuccess())
                if (pSet->fPrecision >= fPrecision)
                    if (TypeIsInteger(pSet->fDataType)
                        && !pSet->fAutoIncrement)
                    {
                        bFound = true;
                        if ((UDWORD)(pSet->fPrecision) < fBestPrecision)
                        {
                            Set = *pSet;
                            fBestPrecision = pSet->fPrecision;
                        }
                    }
        } // end for loop

    } // sqlsuccess()

// now live with an autoincrement type if it's an integer
// and that's all that's available that would work

    if (!bFound && TypeIsInteger(fType))
    {
        // find the best one we can convert to the desired type
        // (one that's also an integral type in this case)
        for (fBestPrecision = ~0,
            uNumOfRows = ExtFetchFirst();
            sqlsuccess() && uNumOfRows;
            uNumOfRows = ExtFetchNext())
        {
            if (sqlsuccess())
                if (pSet->fPrecision >= fPrecision)
                    if (TypeIsInteger(pSet->fDataType))
                    {
                        bFound = true;
                        if ((UDWORD)(pSet->fPrecision) < fBestPrecision)
                        {
                            Set = *pSet;
                            fBestPrecision = pSet->fPrecision;
                        }
                    }
        } // end for loop

    } // sqlsuccess()

    if (!bFound && (TypeIsNumeric(fType) || TypeIsInteger(fType)))
    {
        // find the best one we can convert to the desired type
        // (one that's also a numeric type in this case)
        for (fBestPrecision = ~0,
            uNumOfRows = ExtFetchFirst();
            sqlsuccess() && uNumOfRows;
            uNumOfRows = ExtFetchNext())
        {
            if (sqlsuccess())
                if (pSet->fPrecision >= fPrecision)
                    if (TypeIsNumeric(pSet->fDataType))
                    {
                        bFound = true;
                        if ((UDWORD)(pSet->fPrecision) < fBestPrecision)
                        {
                            Set = *pSet;
                            fBestPrecision = pSet->fPrecision;
                        }
                    }
        } // end for loop

    } // sqlsuccess()

    if (!bFound && TypeIsDate(fType))
    {
        // find the best one we can convert to the desired type
        // (one that's also a date type in this case)
        for (fBestPrecision = ~0,
            uNumOfRows = ExtFetchFirst();
            sqlsuccess() && uNumOfRows;
            uNumOfRows = ExtFetchNext())
        {
            if (sqlsuccess())
                if (pSet->fPrecision >= fPrecision)
                    if (TypeIsDate(pSet->fDataType)
                        || TypeIsTimestamp(pSet->fDataType))
                    {
                        bFound = true;
                        if ((UDWORD)(pSet->fPrecision) < fBestPrecision)
                        {
                            Set = *pSet;
                            fBestPrecision = pSet->fPrecision;
                        }
                    }
        } // end for loop

    } // sqlsuccess()

    if (!bFound && TypeIsTime(fType))
    {
        // find the best one we can convert to the desired type
        // (one that's also a time type in this case)
        for (fBestPrecision = ~0,
            uNumOfRows = ExtFetchFirst();
            sqlsuccess() && uNumOfRows;
            uNumOfRows = ExtFetchNext())
        {
            if (sqlsuccess())
                if (pSet->fPrecision >= fPrecision)
                    if (TypeIsTime(pSet->fDataType)
                        || TypeIsTimestamp(pSet->fDataType))
                    {
                        bFound = true;
                        if ((UDWORD)(pSet->fPrecision) < fBestPrecision)
                        {
                            Set = *pSet;
                            fBestPrecision = pSet->fPrecision;
                        }
                    }
        } // end for loop

    } // sqlsuccess()

    if (!bFound && TypeIsTimestamp(fType))
    {
        // find the best one we can convert to the desired type
        // (one that's also a timestamp type in this case)
        for (fBestPrecision = ~0,
            uNumOfRows = ExtFetchFirst();
            sqlsuccess() && uNumOfRows;
            uNumOfRows = ExtFetchNext())
        {
            if (sqlsuccess())
                if (pSet->fPrecision >= fPrecision)
                    if (TypeIsTimestamp(pSet->fDataType))
                    {
                        bFound = true;
                        if ((UDWORD)(pSet->fPrecision) < fBestPrecision)
                        {
                            Set = *pSet;
                            fBestPrecision = pSet->fPrecision;
                        }
                    }
        } // end for loop

    } // sqlsuccess()

    if (!bFound && TypeIsCharacter(fType))
    {
        // find the best one we can convert to the desired type
        // (one that's also a character type in this case)
        for (fBestPrecision = ~0,
            uNumOfRows = ExtFetchFirst();
            sqlsuccess() && uNumOfRows;
            uNumOfRows = ExtFetchNext())
        {
            if (sqlsuccess())
                if (pSet->fPrecision >= fPrecision)
                    if (TypeIsCharacter(pSet->fDataType))
                    {
                        bFound = true;
                        if ((UDWORD)(pSet->fPrecision) < fBestPrecision)
                        {
                            Set = *pSet;
                            fBestPrecision = pSet->fPrecision;
                        }
                    }
        } // end for loop

    } // sqlsuccess()

    if (!bFound && TypeIsBinary(fType))
    {
        // find the best one we can convert to the desired type
        // (one that's also a binary type in this case)
        for (fBestPrecision = ~0,
            uNumOfRows = ExtFetchFirst();
            sqlsuccess() && uNumOfRows;
            uNumOfRows = ExtFetchNext())
        {
            if (sqlsuccess())
                if (pSet->fPrecision >= fPrecision)
                    if (TypeIsBinary(pSet->fDataType))
                    {
                        bFound = true;
                        if ((UDWORD)(pSet->fPrecision) < fBestPrecision)
                        {
                            Set = *pSet;
                            fBestPrecision = pSet->fPrecision;
                        }
                    }
        } // end for loop

    } // sqlsuccess()

    if (!bFound)
    {
        sprintf(&lpszCreateStmt[strlen(lpszCreateStmt)],
            "<<Can't convert %s>>", token);
        // return length
        *pi += (strlen(lpszCreateStmt) - uOldLen);
        ret = SQL_CANT_CONVERT_DATATYPE;
    }
    else
    {
        // point to the best one we've got.
        pSet = &Set;

        // one way or another, we've got a viable data type info
        // structure if we get here
        strcat(lpszCreateStmt, (LPSTR)pSet->szTypeName);

        TrimTrailingBlanks((LPSTR)pSet->szCreateParams);

        // if there are creation parameters, at least
        // append the precision.
        if (strlen((LPSTR)pSet->szCreateParams))
        {
            sprintf(&lpszCreateStmt[strlen(lpszCreateStmt)],
                "( %lu",
                cbColDef != 0 ?
                (unsigned long)cbColDef :
                (unsigned long)pSet->fPrecision
            );

            // if there's more than one create param,
            // also append the scale.
            if (strstr((LPSTR)pSet->szCreateParams, ","))
            {
                sprintf(&lpszCreateStmt[strlen(lpszCreateStmt)],
                    ", %d", ibScale);
            }
            // ...and append a closing parenthesis.
            strcat(lpszCreateStmt, " )");
        }

        // return length
        *pi += (strlen(lpszCreateStmt) - uOldLen);

    } // sqlsuccess()

    return ret;
}


