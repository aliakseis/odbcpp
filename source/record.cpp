#include <sql.hpp>
#include <cstring>

/****************************************************************
    odbcRECORD constructor

    This class acts as a base class for the odbcRECINSERTER
    and odbcRECUPDATER classes.  The constructor takes the
    table name and allocates a SQL statement buffer and
    an odbcCURSOR object for the doing the inserts, updates,
    and deletes. (The parent odbcCURSOR object is used for
    running a SELECT to get the table definition in the
    case of the odbcRECINSERTER class, and in the case of
    the odbcRECUPDATER to run the SELECT ... FOR UPDATE OF
    statement.

    Arguments are the address of a connection object and
    the name of a table on the connection. ODBC currently
    does not allow updateable joins/views.
****************************************************************/
odbcRECORD::odbcRECORD(
    odbcCONNECT* pConn,
    LPCSTR lpszSentTblName
)
    :
    odbcCURSOR(pConn)
{
    lpszTblName = nullptr;
    lpszStmt = nullptr;
    pStmt = nullptr;
    pActualCols = nullptr;
    uNoOfActualCols = 0;

    AutoRetrieve(odbcREPSUCCESSWITHINFO);
    AutoReport(odbcNOREPORT);

    if (!Allocate(lpszSentTblName))
        bError = true;

    // If any error occurred during construction,
    // the info has been retrieved.
    // Now inherit error handling from connection.
    ErrHandler = pConn->ErrHandler;
    AutoRetrieve(pConn->bGetErrorInfo);
    AutoReport(pConn->bReportErrorInfo);
    hwnd_ = pConn->hwnd_;
    flags = pConn->flags;
}

/**********************************************************
        ~odbcRECORD

        destructor.
**********************************************************/
odbcRECORD::~odbcRECORD()
{
    if (pActualCols)
    {
        delete[] pActualCols;
        pActualCols = nullptr;
        uNoOfActualCols = 0;
    }

    if (lpszTblName)
    {
        delete[] lpszTblName;
        lpszTblName = nullptr;
    }

    if (lpszStmt)
    {
        delete[] lpszStmt;
        lpszStmt = nullptr;
    }

    if (pStmt)
    {
        delete pStmt;
        pStmt = nullptr;
    }
}




/****************************************************************
    Allocate

    Protected function to allocate memory and other common
    goodies for odbcRECORD descendants.
****************************************************************/
bool odbcRECORD::Allocate(LPCSTR lpszSentTblName)
{
    // make a copy of the table name
    if (lpszSentTblName)
    {
        lpszTblName = new char[strlen(lpszSentTblName) + 1];
        if (!lpszTblName)
        {
            bError = true;
            SetRC(SQL_ALLOC_FAILED);
            return false;
        }


        strcpy(lpszTblName, lpszSentTblName);

    }
    else
    {
        SetRC(SQL_NULL_TBL_NAME);
        return false;
    }

    // allocate space for a SQL statement
    lpszStmt = new char[SQLSTMT_MAX];

    if (!lpszStmt)
    {
        SetRC(SQL_ALLOC_FAILED);
        return false;
    }
    /*
            // allocate a secondary odbcCURSOR object.
            pStmt = new odbcCURSOR( pConn );

            if ( !pStmt )
                {
    //            delete[] lpszStmt;
    //            lpszStmt = NULL;
                SetRC( SQL_ALLOC_FAILED );
                return lastRC();
                }

            if ( !pStmt->sqlsuccess() )
                {
                delete pStmt;
                pStmt = NULL;
    //            delete[] lpszStmt;
    //            lpszStmt = NULL;
                SetRC( SQL_STMT_CTOR_FAILED );
                return false;
                }
    */
    return true;
}

odbcCURSOR* odbcRECORD::GetUpdateStmt()
{
    if (nullptr == pStmt)
    {
        // allocate a secondary odbcCURSOR object.
        pStmt = new odbcCURSOR(pConn);
        pStmt->SetWnd(GetWnd());
        pStmt->SetMBoxFlags(GetMBoxFlags());
        pStmt->SetErrHandler(GetErrHandler());
        pStmt->AutoRetrieve(GetAutoRetrieve());
        pStmt->AutoReport(GetAutoReport());
    }
    return pStmt;
}


/***************************************************

    GetActualColumnDefinitions

    Get actual column defs for table for use in
    updates and inserts.  Called by constructor.
***************************************************/
RETCODE odbcRECORD::GetActualColumnDefinitions(bool bSaveColBindings)
{
    LPCSTR lpszQuote = pConn->GetIdentifierQuoteMark();
    // template for SQL SELECT to get table definition
    const char szTemplate1[] = "SELECT * FROM ";
    const char szTemplate2[] = " WHERE 0 = 1";
    // temporary SQL statement storage
    char *szStmt = nullptr;

    // build SQL SELECT statement
    szStmt = new char[strlen(szTemplate1)
        + (2 * strlen(lpszQuote))
        + strlen(szTemplate2)
        + strlen(lpszTblName)
        + 1];

    if (!szStmt)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }


    strcpy(szStmt, szTemplate1);
    strcat(szStmt, lpszQuote);
    strcat(szStmt, lpszTblName);
    strcat(szStmt, lpszQuote);
    strcat(szStmt, szTemplate2);


    // get description of result set.
    Prepare(szStmt);

    if (sqlsuccess())
    {
        AutoBind();
        // discard non-existent results in driver, but leave column bindings
        // intact.
        Close();
    }

    if (sqlsuccess() && bSaveColBindings)
    {
        // save actual column bindings
        pActualCols = new sCOLBIND[ColCount];
        if (!pActualCols)
        {
            delete[] szStmt;
            SetRC(SQL_ALLOC_FAILED);
            return lastRC();
        }

        memcpy(pActualCols,
            pColBindings,
            sizeof(sCOLBIND) * ColCount);

        uNoOfActualCols = ColCount;
        Unbind();
    }
    delete[] szStmt;

    return lastRC();
}

/***************************************************

    odbcRECINSERTER constructor

    Sets up column bindings, record buffer, etc. for
    inserts.  Builds and prepares the INSERT statement.

    Arguments:
    ----------
    pConn               Connection object address.

    lpszSentTableName   Name of the table into which you
                        are inserting records.  This must be a
                        table on the current connection.

    pColBinds
    uNumColBindings
    pRecord             If supplied, these are used for data
                        dictionary-style column binding instead
                        of the default AutoBind() mechanism.
                        The user is responsible for ensuring'
                        that the dictionary entry's column
                        definitions will match the order
                        of the query result set's columns
                        (matching SELECT * FROM <tablename>),
                        and that the conversion types make
                        sense (e.g., if you bind a SQL_NUMERIC
                        column to a storage location of type
                        SQL_DATE, you are responsible for the
                        resulting garbaggio in your record).

***************************************************/
odbcRECINSERTER::odbcRECINSERTER(
    odbcCONNECT*    pConn,
    LPCSTR          lpszSentTblName,
    sCOLBIND*       pColBinds /* = NULL */,
    UWORD           uNumColBindings /* = 0 */,
    void*             pRec /* = NULL */
)
    : odbcRECORD(pConn, lpszSentTblName)
{
    AutoRetrieve(odbcREPSUCCESSWITHINFO);
    AutoReport(odbcNOREPORT);

    if (SetupForInsert(pColBinds,
        uNumColBindings,
        pRec) != SQL_SUCCESS)
        bError = true;

    // If any error occurred during construction,
    // the info has been retrieved.
    // Now inherit error handling from connection.
    ErrHandler = pConn->ErrHandler;
    AutoRetrieve(pConn->bGetErrorInfo);
    AutoReport(pConn->bReportErrorInfo);
    hwnd_ = pConn->hwnd_;
    flags = pConn->flags;

}

odbcRECINSERTER::odbcRECINSERTER(
    odbcCONNECT*    pConn,
    LPUCSTR         lpszSentTblName,
    sCOLBIND*       pColBinds /* = NULL */,
    UWORD           uNumColBindings /* = 0 */,
    void*             pRec /* = NULL */
)
    : odbcRECORD(pConn, (LPCSTR)lpszSentTblName)
{
    AutoRetrieve(odbcREPSUCCESSWITHINFO);
    AutoReport(odbcNOREPORT);

    if (SetupForInsert(pColBinds,
        uNumColBindings,
        pRec
    ) != SQL_SUCCESS)
        bError = true;

    // If any error occurred during construction,
    // the info has been retrieved.
    // Now inherit error handling from connection.
    ErrHandler = pConn->ErrHandler;
    AutoRetrieve(pConn->bGetErrorInfo);
    AutoReport(pConn->bReportErrorInfo);
    hwnd_ = pConn->hwnd_;
    flags = pConn->flags;

}

/**********************************************************
        ~odbcRECINSERTER

        destructor.
**********************************************************/

odbcRECINSERTER::~odbcRECINSERTER() = default;
/***************************************************

    SetupForInsert

    Set up column bindings, record buffer, etc. for
    insert.  Called by constructor.
***************************************************/

RETCODE     odbcRECINSERTER::SetupForInsert(
    sCOLBIND*       pColBinds,
    UWORD           uNumColBindings,
    void*             pRecord
)
{
    LPCSTR lpszQuote = pConn->GetIdentifierQuoteMark();
    // used in building  INSERT statement
    UWORD       icol;

    GetActualColumnDefinitions(pColBinds != nullptr);

    if (!sqlsuccess())
        return lastRC();

    if (pColBinds)
    {

        // use passed data dictionary
        SetColBindings(pColBinds, uNumColBindings, pRecord);
        BindCol();
    }

    if (!sqlsuccess())
        return lastRC();

    // build INSERT statement
    strcpy(lpszStmt, "INSERT INTO ");
    strcat(lpszStmt, lpszQuote);
    strcat(lpszStmt, lpszTblName);
    strcat(lpszStmt, lpszQuote);
    strcat(lpszStmt, "\nVALUES\n(\t");

    for (icol = 0;
        sqlsuccess() && icol < ColCount;
        icol++
        )
    {
        // get address of column info
        sCOLBIND* pColBind = &pColBindings[icol];

        if (pColBind->fSqlType == SQL_LONGVARCHAR
            || pColBind->fSqlType == SQL_LONGVARBINARY)
        {
            // we got us a BLOB here... let it bind itself.
            odbcBLOB* pBlob = ColResultAsBLOB(pColBind->iCol);
            if (pBlob)
            {
                pBlob->SetPutCursor(GetUpdateStmt());
            }
        }

        // add this column to insert statement
        if (icol > 0)
            strcat(lpszStmt, ",\n\t");

        strcat(lpszStmt, "?");

    } // end for loop

// finish statement
    strcat(lpszStmt, "\n)\n");

    // prepare it
    GetUpdateStmt()->Prepare(lpszStmt);

    if (!GetUpdateStmt()->sqlsuccess())
        SetRC(SQL_INTERNAL_PREPARE_FAILED);

    return lastRC();
}

/***************************************************

    InsertRecord

    Bind storage of one row's worth of data in the
    passed user-allocated record structure, for use
    in the automatically generated insert statement
    that was an output of the constructor's call to
    SetupForInsert().

    The structure of the data in the record is de-
    fined by the contents of the internal column
    bindings array, as determined by a SELECT *
    FROM <table-name> WHERE 0 = 1.


    Parameters
    ==========

        Name               Default  Description
        ------------------------------------------------------------------
        pRecord                     Address of rowset record struct
                                    (if NULL, use internal record struct).

        Size                        Size of rowset record struct (zero
                                    if pRecord is NULL).

        pcbValueArray      NULL     If non-NULL, pass the address of an
                                    an array of SDWORDs, one for each
                                    parameter involved in the update
                                    (corresponding to a column in the original
                                    query). These are passed as the last
                                    argument in the call to BindParameter or
                                    SetParam. For each parameter, set the
                                    corresponding array element (e.g., array
                                    element zero for the first parameter) to
                                    one of the following:

                                    1. For non-NULL binary parameters, store
                                       the  length of the data.

                                    2. For character parameters, store SQL_NTS
                                       if the parameter value is a null-
                                       terminated string, or the length of the
                                       parameter otherwise.

                                    3. SQL_NULL_DATA if the parameter is NULL.

                                    4. SQL_DATA_AT_EXEC (if calling an ODBC 1.0
                                       driver) or the result of the macro
                                       SQL_LEN_DATA_AT_EXEC(max-length) macro
                                       (if calling an ODBC 2.0 driver). This
                                       variation is used if the data is to be
                                       passed at execution time; in this case,
                                       the caller must be ready to interpret
                                       the address of the parameter in the
                                       record structure as the token returned
                                       from ParamData() when performing the
                                       ParamData()/PutData() loop to pass
                                       parameter data at execution time.

                                    5. Zero, in which case NULL will be passed
                                       for the pcbValue argument for this
                                       parameter when calling BindParameter or
                                       SetParam. In this case, the parameter
                                       is assumed to be non-NULL and if a
                                       character or binary parameter to be a
                                       null-terminated string of bytes.

                                    If the pcbValueArray argument is NULL, then
                                    NULL will be passed for the pcbValue
                                    argument for this parameter when calling
                                    BindParameter or SetParam. In this case,
                                    the parameter is assumed to be non-NULL and
                                    if a character or binary parameter to be a
                                    null-terminated string of bytes.


***************************************************/

RETCODE odbcRECINSERTER::InsertRecord
(
    void*         pRecord,
    size_t      Size,
    SDWORD *    pcbValueArray   /* = NULL  */
)
{
    RETCODE     ret = SQL_SUCCESS;
    SDWORD      lLength;
    UWORD       icol;
    size_t      uBytesCopied;
    bool        bWeAllocated = (pcbValueArray == nullptr);

    if (bWeAllocated)
    {
        pcbValueArray = new SDWORD[ColCount];
        if (!pcbValueArray)
        {
            SetRC(SQL_ALLOC_FAILED);
            return lastRC();
        }

        memset(pcbValueArray, 0, sizeof(SDWORD) * ColCount);

    } // end if bWeAllocated

// check incoming parameters
    if (pRecord)
        if (IsBadReadPtr((void *)pRecord, Size)
            || Size < RowsetRowRecordSize())
        {
            SetRC(SQL_BAD_PARAMETER);
            if (bWeAllocated)
                delete[] pcbValueArray;
            return lastRC();
        }
        else
            ;
    else
    {
        pRecord = pSet_;
        Size = pSSize;
    }

    // if we have no bindings, or no record, or the
    // row number is out of range, return an error.
    if (!pColBindings
        || !pSet_
        )
    {
        SetRC(SQL_NOT_EXTSETUP);
        ret = lastRC();
    }
    else if (pSet_ != pRecord)
        MoveRecordToRowsetRow(
            1, pRecord, Size, &uBytesCopied); // move to internal rec

    // our inherited cursor's column bindings describe
    // the table. Use this definition to bind parameters
    // on the internal record.
    for (icol = 0;
        sqlsuccess() && icol < ColCount;
        icol++
        )
    {
        // get address of column info
        sCOLBIND* pColBind = &pColBindings[icol];

        // the actual SQL type in the table may be different from what
        // we bound if we used the data dictionary method; BindParameter
        // requires that we submit the actual SQL type in this case.

        UWORD       iActual = 0;
        SWORD fActualSqlType = pColBind->fSqlType;

        if (pActualCols)
        {
            for (iActual = 0; iActual < uNoOfActualCols; iActual++)
                if (!_stricmp((LPSTR)pColBind->szColName,
                    (LPSTR)pActualCols[iActual].szColName))
                {
                    fActualSqlType = pActualCols[iActual].fSqlType;
                    break;
                }
        }

        if (pColBind->fSqlType == SQL_LONGVARCHAR
            || pColBind->fSqlType == SQL_LONGVARBINARY)
        {
            // we got us a BLOB here... let it bind itself.
            odbcBLOB* pBlob = ColResultAsBLOB(pColBind->iCol);
            if (pBlob)
            {
                if (sqlsuccess())
                    if (pcbValueArray)
                        if (pcbValueArray[icol] ==
                            SQL_NULL_DATA)
                            pBlob->SetNull();
                pBlob->BindParameter();
            }
        }
        else
        {
            // get precision for this SQL type.

            UDWORD fPrecision = PrecisionForSqlType(
                pColBind->fSqlType,
                pColBind->cbValueMax);

            // reduce the precision if a string type and we added
            // an extra byte for the null terminator to cbValueMax
            // (the normal case)                        
            if (pColBind->fCType == SQL_C_CHAR &&
                !TypeIsBinary(pColBind->fSqlType)
                && !bNoExtraByteOnStrings_)
                fPrecision--;

            // if pcbValueArray was passed in, one or more of the
            // columns needs special treatment, e.g. because it is
            // NULL or needs to pass data at execution time.

            lLength = pcbValueArray[icol];

            if (lLength > 0)
                if (pColBind->fCType == SQL_C_CHAR &&
                    !TypeIsBinary(pColBind->fSqlType))
                    lLength = 0;
                else
                    lLength = LengthForCType(pColBind->fCType, pColBind->cbValueMax);

            pcbValueArray[icol] = lLength;

            // if the data is NULL, clear the buffer.

            if (lLength == SQL_NULL_DATA)
            {
                memset(ColResultAddr(pColBind->iCol),
                    0, (size_t)(pColBind->cbValueMax));
                SetColValueNull(pColBind->iCol);
            }

            // otherwise...
            //   if a character array, if a string, save strlen() + 1
            //     in the column's SQLColBind pcbValue area;
            //   or if binary, the actual length must be passed in.

            else if (pColBind->fCType == SQL_C_CHAR)
                if (!TypeIsBinary(pColBind->fSqlType))
                    if (!ExtFetchRowCount)
                        pColBind->cbValue = strlen(
                        (LPSTR)ColResultAddr(pColBind->iCol)) + 1;
                    else
                        (&pcbValues[icol * ExtFetchRowCount])[0] = strlen(
                        (LPSTR)ColResultAddr(pColBind->iCol)) + 1;
                else
                    if (!ExtFetchRowCount)
                        pColBind->cbValue = pcbValueArray[icol];
                    else
                        (&pcbValues[icol * ExtFetchRowCount])[0] = pcbValueArray[icol];

            long lTemporaryNullPointer = SQL_NTS;

#if (ODBCVER >= 0x0200)
            // see if we can use new ODBC API function SQLBindParameter via odbcSTMT
            // member function BindParameter().
            if (static_cast<bool>(pConn->GetFunctions(SQL_API_SQLBINDPARAMETER)))
            {
                // copy the data
                GetUpdateStmt()->BindParameter(
                    pColBind->iCol,
                    SQL_PARAM_INPUT,
                    pColBind->fCType,
                    fActualSqlType,
                    fPrecision,
                    pColBind->ibScale,
                    ColResultAddr(pColBind->iCol),
                    pColBind->cbValueMax,
                    lLength ?
                    &pcbValueArray[icol] :
                    &lTemporaryNullPointer
                );
            }
            else
            {
                // do a SetParam call instead
                // copy the data
                GetUpdateStmt()->SetParam(
                    pColBind->iCol,
                    pColBind->fCType,
                    fActualSqlType,
                    fPrecision,
                    pColBind->ibScale,
                    ColResultAddr(pColBind->iCol),
                    lLength ?
                    &pcbValueArray[icol] :
                    nullptr
                );
            }
#else // ODBC v1.0 follow
            // do a SetParam call instead
            // copy the data
            pStmt->SetParam(
                pColBind->iCol,
                pColBind->fCType,
                fActualSqlType,
                fPrecision,
                pColBind->ibScale,
                ColResultAddr(pColBind->iCol),
                lLength ?
                &pcbValueArray[icol] :
                NULL
            );
#endif // ODBCVER >= 0x0200
        } // end if BLOB
    } // end for loop

// execute the statement if the caller wanted to and we have not yet
// had an execution error.
    if (sqlsuccess() && GetUpdateStmt()->sqlsuccess())
    {
        GetUpdateStmt()->Execute();

        // if data-at-exec was involved, let the user handle it.

        if (GetUpdateStmt()->needData())
            return GetUpdateStmt()->lastRC();

        if (!GetUpdateStmt()->sqlsuccess())
        {
            SetRC(SQL_INTERNAL_INSERT_FAILED);
            ret = lastRC();
        }
        else
            GetUpdateStmt()->Close();
    }

    if (bWeAllocated)
        delete[] pcbValueArray;

    return ret;
}

/***************************************************

    SetupForPosUpdDels

    Set up column bindings, record buffer, etc. for
    insert.  Called by constructor.
***************************************************/

RETCODE     odbcRECUPDATER::SetupForPosUpdDels
(
    LPCSTR          szSelectStmt,
    bool            bExecDirect,
    sCOLBIND*       pColBinds,
    UWORD           uNumColBindings,
    void*             pRecord,
    UWORD           fConcur,
    SDWORD          fKeyset,
    UWORD           fRowset
)
{
    const char szTemplate[] = "SELECT * FROM ";
    // setup extended operations
    ExtFetchSetup(
        fConcur,
        fKeyset,
        fRowset
    );

    if (!sqlsuccess())
        return lastRC();

    bool bAccelerate
        = nullptr == pColBinds && nullptr != szSelectStmt
        && !_strnicmp(szSelectStmt, szTemplate, sizeof(szTemplate) / sizeof(szTemplate[0]) - 1);

    if (!bAccelerate)
    {
        GetActualColumnDefinitions(true);
        if (!sqlsuccess())
            return lastRC();
    }

    // copy user's select statement
    strcpy(lpszStmt, szSelectStmt);

    // execute directly or prepare
    if (bExecDirect)
        ExecDirect(lpszStmt);
    else
    {
        Prepare(lpszStmt);
    }

    // Make sure we succeeded and that it was a select statement
//    if ( sqlsuccess() && !NumResultCols() )
//        {
//        Close();
//        SetRC( SQL_NO_SELECT );
//        return lastRC();
//        }

    // bind columns
    if (sqlsuccess())
    {
        if (pColBinds)
        {
            // Make sure we succeeded and that it was a select statement
            if (!NumResultCols())
            {
                Close();
                SetRC(SQL_NO_SELECT);
                return lastRC();
            }
            SetColBindings(
                pColBinds,
                uNumColBindings,
                pRecord
            );
            BindCol();
        }
        else
        {
            AutoBind();
            // Make sure we succeeded and that it was a select statement
            if (!ColCount)
            {
                Close();
                SetRC(SQL_NO_SELECT);
                return lastRC();
            }
            if (bAccelerate)
            {
                // save actual column bindings
                pActualCols = new sCOLBIND[ColCount];
                if (!pActualCols)
                {
                    SetRC(SQL_ALLOC_FAILED);
                    return lastRC();
                }

                memcpy(pActualCols,
                    pColBindings,
                    sizeof(sCOLBIND) * ColCount);

                uNoOfActualCols = ColCount;
            }
        }
    }

    return lastRC();
}

/***************************************************

    odbcRECUPDATER constructor

    Sets up column bindings, record buffer, etc. for
    positioned updates and deletes.

    Arguments:

    pConn               Address of governing connection.

    lpszSelectStmt      SELECT statement to pull rows for
                        updating.  This must conform to
                        the following rules:

                1. A single table is used in the query that
                   retrieved the data, and it is the same table
                   name that is passed in.

                2. The query is of the form SELECT columns FROM
                   table ... FOR UPDATE OF columns

                3. Each row in the result set contains values
                   that uniquely identify the row (i.e., you must
                   make sure that the primary key of the table -
                   or another unique key - is present among the
                   columns selected).

    lpszSentTblName     name of the table on which you
                        want to perform updates. This must be
                        a table on the current connection, of
                        course, and be the same table used
                        in the select statement.

    bExecDirect         If true, SELECT statement will be
                        executed immediately.  If false,
                        Prepare() will be called. Use false
                        if you need to bind parameters on the
                        SELECT statement, for example.

                        If you send false, you will have to
                        call Execute() before the result
                        set will be available.

    pColBinds
    uNumColBindings
    pRecord             If supplied, these are used for data
                        dictionary-style column binding instead
                        of the default AutoBind() mechanism.
                        The user is responsible for ensuring'
                        that the dictionary entry's column
                        definitions will match the order
                        of the query result set's columns,
                        and that the conversion types make
                        sense (e.g., if you bind a SQL_NUMERIC
                        column to a storage location of type
                        SQL_DATE, you are responsible for the
                        resulting garbaggio in your record).

                        if the rowset size is greater than 1,
                        be sure that your record structure is
                        properly defined as a struct containing
                        a collection of arrays of column storage
                        locations, not as an array of record
                        data structures as we old-timers of the
                        ISAM world might be tempted to do.

    fConcur
    fKeyset
    fRowset             These arguments are passed to
                        ExtFetchSetup and have the same meanings
                        here as in the description of that
                        function. The fConcur flag must
                        (obviously) not be SQL_CONCUR_READONLY
                        if updates and deletes are to be
                        performed. The defaults work adequately
                        with small to medium-sized result sets.

***************************************************/
odbcRECUPDATER::odbcRECUPDATER
(odbcCONNECT*  pConn,
    LPCSTR          lpszSentTblName,
    LPCSTR          lpszSelectStmt,
    sCOLBIND*       pColBinds       /* = NULL */,
    UWORD           uNumColBindings /* = 0 */,
    void*             pRecord         /* = NULL */,
    bool            bExecDirect     /* = true */,
    UWORD           fConcur         /* = SQL_CONCUR_VALUES */,
    SDWORD          fKeyset         /* = SQL_SCROLL_STATIC */,
    UWORD           fRowset         /* = 1 */
)
    : odbcRECORD(pConn, lpszSentTblName)
{
    AutoRetrieve(odbcREPSUCCESSWITHINFO);
    AutoReport(odbcNOREPORT);

    if (SetupForPosUpdDels(
        lpszSelectStmt,
        bExecDirect,
        pColBinds,
        uNumColBindings,
        pRecord,
        fConcur,
        fKeyset,
        fRowset)
        != SQL_SUCCESS)
        bError = true;

    // If any error occurred during construction,
    // the info has been retrieved.
    // Now inherit error handling from connection.
    ErrHandler = pConn->ErrHandler;
    AutoRetrieve(pConn->bGetErrorInfo);
    AutoReport(pConn->bReportErrorInfo);
    hwnd_ = pConn->hwnd_;
    flags = pConn->flags;
}

odbcRECUPDATER::odbcRECUPDATER
(odbcCONNECT*  pConn,
    LPUCSTR         lpszSentTblName,
    LPUCSTR         lpszSelectStmt,
    sCOLBIND*       pColBinds       /* = NULL */,
    UWORD           uNumColBindings /* = 0 */,
    void*             pRecord         /* = NULL */,
    bool            bExecDirect     /* = true */,
    UWORD           fConcur         /* = SQL_CONCUR_VALUES */,
    SDWORD          fKeyset         /* = SQL_SCROLL_STATIC */,
    UWORD           fRowset         /* = 1 */
)
    : odbcRECORD(pConn, (LPCSTR)lpszSentTblName)
{
    AutoRetrieve(odbcREPSUCCESSWITHINFO);
    AutoReport(odbcNOREPORT);

    if (SetupForPosUpdDels(
        (LPCSTR)lpszSelectStmt,
        bExecDirect,
        pColBinds,
        uNumColBindings,
        pRecord,
        fConcur,
        fKeyset,
        fRowset)
        != SQL_SUCCESS)
        bError = true;
    // If any error occurred during construction,
    // the info has been retrieved.
    // Now inherit error handling from connection.
    ErrHandler = pConn->ErrHandler;
    AutoRetrieve(pConn->bGetErrorInfo);
    AutoReport(pConn->bReportErrorInfo);
    hwnd_ = pConn->hwnd_;
    flags = pConn->flags;
}

/**********************************************************
        ~odbcRECUPDATER

        destructor.
**********************************************************/
odbcRECUPDATER::~odbcRECUPDATER() = default;


/***************************************************

    AppendWhereClause

    Called from within UpdateRecord and DeleteRecord
    to append a WHERE clause. Defaults to a positioned
    WHERE CURRENT OF clause; can be overridden for
    drivers that do not support positioned statements.

    AppendWhereClause works with the protected member
    variable lpszStmt, a char array of size SQLSTMT_MAX.
    The string will already contain the relevant UPDATE or
    DELETE statement.  Append white space (a space or tab)
    to the string before appending the WHERE clause.
***************************************************/
bool odbcRECUPDATER::AppendWhereClause()
{
    // finish statement
    strcat(lpszStmt, "\n\tWHERE CURRENT OF ");

    GetCursorName(&lpszStmt[strlen(lpszStmt)],
        (SWORD)(SQLSTMT_MAX - strlen(lpszStmt)));

    return sqlsuccess();
}

/***************************************************

    UpdateRecord

    Bind storage of one row's worth of data in the
    passed user-allocated record structure, from
    which it will also be transferred to the
    automatically allocated structure for use in
    the automatically generated update
    that is an output of this operation.

    This will only work if the following conditions
    are met:

    1. A single table is used in the query that
       retrieved the data, and it is the same table
       name that is passed in.

    2. The query is of the form SELECT columns FROM
       table ... FOR UPDATE OF [columns]

    3. Each row in the result set contains values
       that uniquely identify the row (i.e., you must
       make sure that the primary key of the table -
       or another unique key - is present among the
       columns selected).

    Parameters
    ==========

        Name               Default  Description
        ------------------------------------------------------------------
        irow                        Row in rowset to update

        pRecord                     Address of rowset record struct

        Size                        Size of rowset record struct

        pcbValueArray      NULL     If non-NULL, pass the address of an
                                    an array of SDWORDs, one for each
                                    parameter involved in the update
                                    (corresponding to a column in the original
                                    query). These are passed as the last
                                    argument in the call to BindParameter or
                                    SetParam. For each parameter, set the
                                    corresponding array element (e.g., array
                                    element zero for the first parameter) to
                                    one of the following:

                                    1. For non-NULL binary parameters, store
                                       the  length of the data.

                                    2. For character parameters, store SQL_NTS
                                       if the parameter value is a null-
                                       terminated string, or the length of the
                                       parameter otherwise.

                                    3. SQL_NULL_DATA if the parameter is NULL.

                                    4. SQL_DATA_AT_EXEC (if calling an ODBC 1.0
                                       driver) or the result of the macro
                                       SQL_LEN_DATA_AT_EXEC(max-length) macro
                                       (if calling an ODBC 2.0 driver). This
                                       variation is used if the data is to be
                                       passed at execution time; in this case,
                                       the caller must be ready to interpret
                                       the address of the parameter in the
                                       record structure as the token returned
                                       from ParamData() when performing the
                                       ParamData()/PutData() loop to pass
                                       parameter data at execution time.

                                    5. Zero, in which case NULL will be passed
                                       for the pcbValue argument for this
                                       parameter when calling BindParameter or
                                       SetParam. In this case, the parameter
                                       is assumed to be non-NULL and if a
                                       character or binary parameter to be a
                                       null-terminated string of bytes.

                                    If the pcbValueArray argument is NULL, then
                                    NULL will be passed for the pcbValue
                                    argument for this parameter when calling
                                    BindParameter or SetParam. In this case,
                                    the parameter is assumed to be non-NULL and
                                    if a character or binary parameter to be a
                                    null-terminated string of bytes.


        Returns the count of rows affected, or zero
        if the update cannot be performed.
***************************************************/

SDWORD odbcRECUPDATER::UpdateRecord
(
    UWORD       irow,
    void*         pRecord,
    size_t      Size,
    SDWORD *    pcbValueArray
)
{
    UWORD       icol = 0;
    SDWORD      lLength = 0;
    LPCSTR      lpszQuote = pConn->GetIdentifierQuoteMark();
    SDWORD      nRowCount = 0;
    bool        bUseSetPos = (bool)((pConn->SetPosSupport()) & SQL_POS_UPDATE);
    bool        bWeAllocated = (pcbValueArray == nullptr);
    odbcBLOB*   pBlob;

    if (bWeAllocated)
    {
        pcbValueArray = new SDWORD[ColCount];
        if (!pcbValueArray)
        {
            SetRC(SQL_ALLOC_FAILED);
            return 0;
        }

        memset(pcbValueArray, 0, sizeof(SDWORD) * ColCount);

    } // end if bWeAllocated

    if (pRecord)
        // check for bad parameters
        if (IsBadReadPtr((void *)pRecord, Size)
            || Size != RowsetRowRecordSize()
            || irow == 0 || irow > ExtFetchRow
            )
        {
            SetRC(SQL_BAD_PARAMETER);
            if (bWeAllocated)
                delete[] pcbValueArray;
            return 0;
        }

    // if we have no bindings, or no record, or the row number is out of range,
    // return an error.
    if (!pColBindings
        || !pSet_
        || !ExtFetchRowCount
        )
    {
        SetRC(SQL_NOT_EXTSETUP);
        if (bWeAllocated)
            delete[] pcbValueArray;
        return 0;
    }

    // move record to rowset row if we have not yet
    // had an execution error, and if the record pointer
    // is not NULL (already there). This is necessary when
    // using the Cursor Library since it updates its
    // internal cache based on the values in the result
    // set buffers of the application.

    if (pRecord)
    {
        size_t cbBytesCopied = 0;
        MoveRecordToRowsetRow(
            irow,
            pRecord,
            Size,
            &cbBytesCopied
        );
    }

    if (!bUseSetPos)
    {
        if (ExtFetchRow > 1)  // if multiple-row rowset...
            SetPos(             // ...make sure we be positioned OK.
                irow,
#if ODBCVER >= 0x0200
                SQL_POSITION,
                SQL_LOCK_NO_CHANGE
#else
                false,
                true
#endif
            );

        if (!sqlsuccess())
        {
            SetRC(SQL_INTERNAL_SETPOS_FAILED);
            return lastRC();
        }

        // construct update statement
        strcpy(lpszStmt, "UPDATE ");
        strcat(lpszStmt, lpszQuote);
        strcat(lpszStmt, lpszTblName);
        strcat(lpszStmt, lpszQuote);
        strcat(lpszStmt, "\n\tSET\t");
        strcat(lpszStmt, lpszQuote);

        for (icol = 0;
            sqlsuccess() && icol < ColCount;
            icol++
            )
        {
            // get address of column info
            sCOLBIND* pColBind = &pColBindings[icol];

            // add this column to update statement
            if (icol > 0)
            {
                strcat(lpszStmt, ",\n\t\t");
                strcat(lpszStmt, lpszQuote);
            }

            strcat(lpszStmt, (LPSTR)pColBind->szColName);
            strcat(lpszStmt, lpszQuote);
            strcat(lpszStmt, " = ? ");

            // the actual SQL type in the table may be different from what
            // we bound if we used the data dictionary method; BindParameter
            // requires that we submit the actual SQL type in this case.

            UWORD iActual = 0;
            SWORD fActualSqlType = pColBind->fSqlType;

            if (pActualCols)
            {
                for (iActual = 0; iActual < uNoOfActualCols; iActual++)
                    if (!_stricmp((LPSTR)pColBind->szColName,
                        (LPSTR)pActualCols[iActual].szColName))
                    {
                        fActualSqlType = pActualCols[iActual].fSqlType;
                        break;
                    }
            }

            if (pColBind->fSqlType == SQL_LONGVARCHAR
                || pColBind->fSqlType == SQL_LONGVARBINARY)
            {
                // we got us a BLOB here... let it bind itself.
                pBlob = ColResultAsBLOB(pColBind->iCol, irow);
                if (pBlob)
                {
                    pBlob->SetPutCursor(GetUpdateStmt());
                    if (sqlsuccess())
                        if (pcbValueArray)
                            if (pcbValueArray[icol] ==
                                SQL_NULL_DATA)
                                pBlob->SetNull();
                    pBlob->BindParameter();
                }
            }
            else
            {
                // get precision for this SQL type.

                UDWORD fPrecision = PrecisionForSqlType(
                    pColBind->fSqlType,
                    pColBind->cbValueMax);

                // reduce the precision if a string type and we added
                // an extra byte for the null terminator to cbValueMax
                // (the normal case)                        
                if (pColBind->fCType == SQL_C_CHAR &&
                    !TypeIsBinary(pColBind->fSqlType)
                    && !bNoExtraByteOnStrings_)
                    fPrecision--;

                lLength = pcbValueArray[icol];

                if (lLength > 0)
                    if (pColBind->fCType == SQL_C_CHAR &&
                        !TypeIsBinary(pColBind->fSqlType))
                        lLength = 0; // assume null-terminated.
                    else
                        lLength = LengthForCType(pColBind->fCType, pColBind->cbValueMax);

                pcbValueArray[icol] = lLength;

                // if the data is NULL, clear the buffer.

                if (lLength == SQL_NULL_DATA)
                {
                    memset(ColResultAddr(pColBind->iCol, irow),
                        0, (size_t)(pColBind->cbValueMax));
                    SetColValueNull(pColBind->iCol, irow);
                }

                // otherwise...
                //   if a character array, if a string, save strlen() + 1
                //     in the column's SQLColBind pcbValue area;
                //   or if a binary buffer, the actual length must be passed in.

                else if (pColBind->fCType == SQL_C_CHAR)
                    if (!TypeIsBinary(pColBind->fSqlType))
                        (&pcbValues[icol * ExtFetchRowCount])[irow - 1] = strlen(
                        (LPSTR)ColResultAddr(pColBind->iCol, irow)) + 1;
                    else
                        (&pcbValues[icol * ExtFetchRowCount])[irow - 1] = pcbValueArray[icol];

#if (ODBCVER >= 0x0200)
                // see if we can use new ODBC API function SQLBindParameter via odbcSTMT
                // member function BindParameter().
                if (static_cast<bool>(pConn->GetFunctions(SQL_API_SQLBINDPARAMETER)))
                {
                    // copy the data
                    GetUpdateStmt()->BindParameter(
                        pColBind->iCol,
                        SQL_PARAM_INPUT,
                        pColBind->fCType,
                        fActualSqlType,
                        fPrecision,
                        pColBind->ibScale,
                        ColResultAddr(pColBind->iCol, irow),
                        pColBind->cbValueMax,
                        lLength ?
                        &pcbValueArray[icol] :
                        nullptr
                    );
                }
                else
                {
                    // do a SetParam call instead
                    // copy the data
                    GetUpdateStmt()->SetParam(
                        pColBind->iCol,
                        pColBind->fCType,
                        fActualSqlType,
                        fPrecision,
                        pColBind->ibScale,
                        ColResultAddr(pColBind->iCol, irow),
                        lLength ?
                        &pcbValueArray[icol] :
                        nullptr
                    );
                }
#else // ODBC v1.0 follow
                // do a SetParam call instead
                // copy the data
                pStmt->SetParam(
                    pColBind->iCol,
                    pColBind->fCType,
                    fActualSqlType,
                    fPrecision,
                    pColBind->ibScale,
                    ColResultAddr(pColBind->iCol, irow),
                    lLength ?
                    &pcbValueArray[icol] :
                    NULL
                );
#endif // ODBCVER >= 0x0200
            } // end if a BLOB type

        } // end for loop

        if (sqlsuccess() && GetUpdateStmt()->sqlsuccess())
        {
            if (!AppendWhereClause())
            {
                SetRC(SQL_APPEND_WHERE_CLAUSE_FAILED);
            }
        }

        // execute the statement if we have not yet
        // had an execution error.

        if (sqlsuccess() && GetUpdateStmt()->sqlsuccess())
        {
            GetUpdateStmt()->ExecDirect(lpszStmt);

            if (!GetUpdateStmt()->sqlsuccess())
            {
                SetRC(SQL_INTERNAL_UPDATE_FAILED);
            }
            else
            {
                nRowCount = (SDWORD)GetUpdateStmt()->RowCount();
                GetUpdateStmt()->Close();
                GetUpdateStmt()->ResetParams();
            }
        }
    }
    else // use SQLSetPos
    {
        // our inherited cursor's column bindings describe
        // the table. Use this definition to bind parameters
        // on the internal record.
        char szValue[4];
        SWORD iValueReturned;
        UDWORD cbColDef;
        char  *lpstr;
        SDWORD i; SDWORD cbAllocatedSize;
        enum {
            SqlDataAtExec,
            NeedLongDataLen,
            DontNeedLongDataLen
        } eWhatValueToPass;
        // don't have error reporting temporarily...
        UWORD bSaveAutoRetrieve
            = pConn->AutoRetrieve(odbcNOREPORT);
        UWORD bSaveAutoReport
            = pConn->AutoReport(odbcNOREPORT);
        // if the driver is a version 1.0 driver that doesn't
        // recognize SQL_NEED_LONG_DATA_LEN, use SQL_DATA_AT_EXEC.
        // if the driver doesn't have to have the data length up
        // front, use SQL_LEN_DATA_AT_EXEC(0). Otherwise we must use
        // the SQL_LEN_DATA_AT_EXEC macro to pass the entire length.

        pConn->GetInfoString(
            SQL_NEED_LONG_DATA_LEN,
            szValue,
            sizeof(szValue),
            &iValueReturned);

        if (!pConn->sqlsuccess())
        {
            eWhatValueToPass = SqlDataAtExec;
        }
        else if (_stricmp(szValue, "Y") != 0)
        {
            eWhatValueToPass = DontNeedLongDataLen;
        }
        else
        {
            eWhatValueToPass = NeedLongDataLen;
        }

        pConn->AutoReport(bSaveAutoReport);
        pConn->AutoRetrieve(bSaveAutoRetrieve);

        for (icol = 0;
            sqlsuccess() && icol < ColCount;
            icol++
            )
        {
            // get address of column info
            sCOLBIND* pColBind = &pColBindings[icol];

            if (pColBind->fSqlType == SQL_LONGVARCHAR
                || pColBind->fSqlType == SQL_LONGVARBINARY)
            {
                // deal with BLOBs here 
                // we got us a BLOB here... let it handle itself.
                pBlob = ColResultAsBLOB(pColBind->iCol, irow);
                if (pBlob)
                {
                    pBlob->SetPutCursor(this);
                    if (pcbValueArray[icol] ==
                        SQL_NULL_DATA)
                        pBlob->SetNull();
                    else
                    {
                        if (pColBind->fSqlType == SQL_LONGVARCHAR)
                        {
                            for (
                                lpstr = (char  *)pBlob->GetMem(),
                                i = 0,
                                cbAllocatedSize = pBlob->GetcbValue();
                                i < cbAllocatedSize - 1;
                                i++
                                )
                                if (!lpstr[i])
                                    break;
                            cbColDef = i + 1;
                        }
                        else
                            cbColDef = pBlob->GetcbValue();

                        switch (eWhatValueToPass)
                        {
                        case SqlDataAtExec:
                            pcbValueArray[icol]
                                = SQL_DATA_AT_EXEC;
                            break;

                        case DontNeedLongDataLen:
                            pcbValueArray[icol]
                                = SQL_LEN_DATA_AT_EXEC(0);
                            break;

                        case NeedLongDataLen:
                            pcbValueArray[icol]
                                = SQL_LEN_DATA_AT_EXEC((SDWORD)cbColDef);
                            break;
                        } // end switch on eWhatValueTopass                            
                    }

                    BindCol(
                        pColBind->iCol,
                        (SWORD)(pColBind->fSqlType == SQL_LONGVARCHAR ?
                            SQL_C_CHAR : SQL_C_BINARY),
                        ColResultAddr(pColBind->iCol),
                        (SDWORD)sizeof(odbcBLOB*),
                        &pcbValueArray[icol]
                    );
                    if (sqlsuccess())
                        bBlobsBound = true;
                } // end if pBlob != NULL

            }
            else
            {
                lLength = pcbValueArray[icol];

                if (lLength > 0)
                    if (pColBind->fCType == SQL_C_CHAR
                        && !TypeIsBinary(pColBind->fSqlType))
                        lLength = 0;
                    else
                        lLength = LengthForCType(pColBind->fCType,
                            pColBind->cbValueMax);

                pcbValueArray[icol] = lLength;

                // if the data is NULL, clear the buffer and put SQL_NULL_DATA
                //     in the column's SQLColBind pcbValue area.

                if (lLength == SQL_NULL_DATA)
                {
                    memset(ColResultAddr(pColBind->iCol, irow), 0,
                        (size_t)(pColBind->cbValueMax));
                    SetColValueNull(pColBind->iCol, irow);
                }

                // otherwise...
                //   if a character array, if a string, save strlen()
                //     in the column's SQLColBind pcbValue area;
                //   or if a binary buffer, the actual length must be passed in.

                else if (pColBind->fCType == SQL_C_CHAR /*)*/ &&
                    /*if (*/ !TypeIsBinary(pColBind->fSqlType))
                    (&pcbValues[icol * ExtFetchRowCount])[irow - 1] =
                    strlen((LPSTR)ColResultAddr(pColBind->iCol, irow));
                else
                    (&pcbValues[icol * ExtFetchRowCount])[irow - 1]
                    = pcbValueArray[icol];
            } // end if or if not BLOB
        } // end for loop

        if (sqlsuccess()) // there could be an error from BLOB column binding...
        {
            // handle SQL_NEED_DATA returns as ExecDirect and
            // Execute do.
            UpdateRow(irow);
            if (needData() && bBlobsBound)
            {
                odbcBLOB* *ppBlob = nullptr;

                // we pass the address of a pointer to a pointer
                //   to an odbcBLOB
                ParamData((void* *)&ppBlob);

                // handle all BLOBs with a loop.
                if (needData())
                {
                    // dereference the pointer-pointer into a pointer
                    pBlob = *ppBlob;

                    // let the BLOB do the put-data
                    do
                    {
                        pBlob->PutData();

                        if (sqlsuccess())
                        {
                            // tell 'em you're done and get next BLOB, if any.
                            ParamData((void* *)&pBlob);
                        }
                    } while (needData());
                }
            }

            // remove any BLOB column bindings
            for (icol = 0;
                bBlobsBound && icol < ColCount;
                icol++
                )
            {
                // get address of column info
                sCOLBIND* pColBind = &pColBindings[icol];

                if (pColBind->fSqlType == SQL_LONGVARCHAR
                    || pColBind->fSqlType == SQL_LONGVARBINARY)
                {
                    // deal with BLOBs here 
                    // we got us a BLOB here... let it handle itself.
                    pBlob = ColResultAsBLOB(pColBind->iCol, irow);
                    if (pBlob)
                    {
                        // remove column bindings by passing NULL rgbValue
                        BindCol(
                            pColBind->iCol,
                            (SWORD)(pColBind->fSqlType == SQL_LONGVARCHAR ?
                                SQL_C_CHAR : SQL_C_BINARY),
                            nullptr,
                            (SDWORD)sizeof(odbcBLOB*),
                            &pcbValueArray[icol]
                        );
                    } // end if pBlob != NULL

                }
            } // end for loop
        } // end if sqlsuccess() from BLOB col bindings            
        bBlobsBound = false;
        if (bWeAllocated)
            delete[] pcbValueArray;
        if (sqlsuccess())
            return RowCount();

        return 0;
    }

    // return number of rows affected
    if (bWeAllocated)
        delete[] pcbValueArray;
    if (sqlsuccess() && GetUpdateStmt()->sqlsuccess())
        return nRowCount;

    return 0;
}

/***************************************************

    DeleteRecord

    Delete a row from the rowset using
    the automatically generated delete statement
    that is an output of this operation.

    Parameters
    ==========

        Name               Default  Description
        -----------------------------------------------------
        irow               1        Row in rowset to delete

    Returns number of affected rows.

***************************************************/

SDWORD odbcRECUPDATER::DeleteRecord
(
    UWORD       irow
)
{
    LPCSTR lpszQuote = pConn->GetIdentifierQuoteMark();
    SDWORD nRowCount = 0;
    bool        bUseSetPos = (bool)((pConn->SetPosSupport()) & SQL_POS_DELETE);

    // check for bad parameters and also make sure
    // we can do either positioned deletes or SQLSetPos deletes
    // if not, return an error.
    if (irow == 0 || irow > ExtFetchRow)
    {
        SetRC(SQL_BAD_PARAMETER);
        return 0;
    }

    if (!bUseSetPos)
    {
        if (ExtFetchRow > 1)  // if multiple-row rowset...
            SetPos(             // ...make sure we be positioned OK.
                irow,
#if ODBCVER >= 0x0200
                SQL_POSITION,
                SQL_LOCK_NO_CHANGE
#else
                false,
                true
#endif
            );

        if (!sqlsuccess())
        {
            SetRC(SQL_INTERNAL_SETPOS_FAILED);
        }

        strcpy(lpszStmt, "DELETE FROM ");
        strcat(lpszStmt, lpszQuote);
        strcat(lpszStmt, lpszTblName);
        strcat(lpszStmt, lpszQuote);

        if (!AppendWhereClause())
        {
            SetRC(SQL_APPEND_WHERE_CLAUSE_FAILED);
        }

        // execute the statement if the caller wanted to and we have not yet
        // had an execution error.
        if (sqlsuccess())
        {
            GetUpdateStmt()->ExecDirect(lpszStmt);
            if (!GetUpdateStmt()->sqlsuccess())
            {
                SetRC(SQL_INTERNAL_DELETE_FAILED);
            }
            else
            {
                nRowCount = (SDWORD)(GetUpdateStmt()->RowCount());
                GetUpdateStmt()->Close();
            }
        }
    }
    else
    {
        DeleteRow(irow);

        if (sqlsuccess())
            return RowCount();

        return 0;
    }

    // return number of rows affected
    if (sqlsuccess() && GetUpdateStmt()->sqlsuccess())
        return nRowCount;

    return 0;
}

/****************************************************

    ColRecAddr

    Given a record address and column identification,
    determine the offset of the column's field within
    the record and return its address.  Returns NULL
    if the record address is invalid or the column
    identifier is invalid.

****************************************************/
void* odbcRECORD::ColRecAddr(
    void*     pRecord,
    LPUCSTR ColName)
{
    sCOLBIND* pColBind = ColResultInfo(ColName);

    if (pColBind)
    {
        unsigned uOffset = 0;
        for (unsigned i = 0; i < ColCount; i++)
        {
            if (pColBindings[i].iCol == pColBind->iCol)
                break;

            uOffset += (unsigned)pColBindings[i].cbValueMax;
        }

        return (void*)((LPSTR)pRecord + uOffset);
    }

    return nullptr;
}

void* odbcRECORD::ColRecAddr(
    void*     pRecord,
    UWORD icol)
{
    sCOLBIND* pColBind = ColResultInfo(icol);

    if (pColBind)
        return ColRecAddr(pRecord, pColBind->szColName);

    return nullptr;
}
