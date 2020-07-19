#include <sql.hpp>
#include <memory.h> // for memset
#include <cstring>     // for strcpy, strcat, ...
#include <cctype>      // for isdigit, isspace, ...
#include <cstdio>      // sprintf...
#include <cstdlib>     // generalized max...
#include <climits>     // <?>_MIN, <?>_MAX
#include <algorithm>


enum { COLUMN_NAME_COMPARE_MAX = 30 };

/**********************************************************
    odbcCURSOR

    Constructor. See constructor for odbcSTMT for description
    of arguments passed to odbcSTMT().  Cursor-specific
    arguments:

        cpsCOLBIND      pColBindings

            If non-NULL, address of array of structures
            defining the binding of columns in the
            SQL statement to members of the structure
            to contain data values after successful
            call to fetch(). Default value is NULL.

        UWORD                   ColCount

            Count of array elements in psColBindings.
            Default value is 0.

        void*                             pvColStruct

            Address of structure containing slots for
            values. Default value is NULL.


**********************************************************/

odbcCURSOR::odbcCURSOR
(
    odbcCONNECT*            pC,
    LPUSTR                  szStmt,
    bool                    bAutoBind,
    sPARMBIND*              psParmBindings,
    UWORD                   uParmCount,
    void*                     pvParmStruct,
    sCOLBIND*               psColBindings,
    UWORD                   uColCount,
    void*                     pvColStruct
) : odbcSTMT(
    pC,
    szStmt,
    psParmBindings,
    uParmCount,
    pvParmStruct
)
{
    // turn autoretrieve on while in constructor
    AutoRetrieve(odbcREPSUCCESSWITHINFO);
    AutoReport(odbcNOREPORT);

    bTrimAllTrailingBlanks = pC->bTrimAllTrailingBlanks;
    pSSize = 0;
    pColBindings = psColBindings;
    pcbValues = nullptr;
    ColCount = uColCount;
    pSet_ = pvColStruct;
    bFirst = true;
    bColsBound = bAutoBound = false;
    AutoBindCols = bAutoBind;
    bNoExtraByteOnStrings_ = false;
    iCol_ = 0;
    bBlobsBound = false;
    ExtFetchStatus = nullptr;
    ExtFetchRowCount = 0;
    ExtFetchRow = 0;
    ExtFetchRelAbsRow = 0;
    ExtFetchType = 0;

    if (
        sqlsuccess()            // odbcSTMT constructor didn't fail
        && szStmt != nullptr   // there is a statement
        &&
        (
            bPrepared               // statement was prepared already 
            || bExecuted    // or executed already
            )
        )
    {
        // if column bindings exist, bind them now
        if (
            pColBindings    // column bindings are here
            && ColCount     // there are more than zero of them
            && pSet_                 // there is a result set structure
            )
        {
            SetColBindings(
                pColBindings,
                ColCount,
                pSet_
            );
            BindCol();
            bError = !sqlsuccess();
        }
        else if (bAutoBind)
        {
            AutoBind();
            bError = !sqlsuccess();
        }
    }

    // If any error occurred during construction,
    // the info has been retrieved.
    // Now inherit error handling from connection.
    ErrHandler = pC->ErrHandler;
    bGetErrorInfo = pC->bGetErrorInfo;
    bReportErrorInfo = pC->bReportErrorInfo;
    hwnd_ = pC->hwnd_;
    flags = pC->flags;

}

odbcCURSOR::odbcCURSOR
(
    odbcCONNECT*            pC,
    LPSTR                   szStmt,
    bool                    bAutoBind,
    sPARMBIND*              psParmBindings,
    UWORD                   uParmCount,
    void*                     pvParmStruct,
    sCOLBIND*               psColBindings,
    UWORD                   uColCount,
    void*                     pvColStruct
) : odbcSTMT(
    pC,
    (LPUSTR)szStmt,
    psParmBindings,
    uParmCount,
    pvParmStruct
)
{
    // turn autoretrieve on while in constructor
    AutoRetrieve(odbcREPSUCCESSWITHINFO);
    AutoReport(odbcNOREPORT);

    bTrimAllTrailingBlanks = pC->bTrimAllTrailingBlanks;
    pSSize = 0;
    pColBindings = psColBindings;
    ColCount = uColCount;
    pSet_ = pvColStruct;
    pcbValues = nullptr;
    bFirst = true;
    bColsBound = bAutoBound = false;
    AutoBindCols = bAutoBind;
    bNoExtraByteOnStrings_ = false;
    iCol_ = 0;
    bBlobsBound = false;
    ExtFetchStatus = nullptr;
    ExtFetchRowCount = 0;
    ExtFetchRow = 0;
    ExtFetchRelAbsRow = 0;
    ExtFetchType = 0;

    if (
        sqlsuccess()            // odbcSTMT constructor didn't fail
        && szStmt != nullptr   // there is a statement
        &&
        (
            bPrepared               // statement was prepared already 
            || bExecuted    // or executed already
            )
        )
    {
        // if column bindings exist, bind them now
        if (
            pColBindings    // column bindings are here
            && ColCount     // there are more than zero of them
            && pSet_        // there is a result set structure
            )
        {
            SetColBindings(
                pColBindings,
                ColCount,
                pSet_
            );
            BindCol();
            bError = !sqlsuccess();
        }
        else if (bAutoBind)
        {
            AutoBind();
            bError = !sqlsuccess();
        }
    }

    // If any error occurred during construction,
    // the info has been retrieved.
    // Now inherit error handling from connection.
    ErrHandler = pC->ErrHandler;
    bGetErrorInfo = pC->bGetErrorInfo;
    bReportErrorInfo = pC->bReportErrorInfo;
    hwnd_ = pC->hwnd_;
    flags = pC->flags;

}

/***************************************************

    ~odbcCURSOR

    Destructor.
***************************************************/

odbcCURSOR::~odbcCURSOR()
{
    FreeColBindings();
    if (ExtFetchStatus)
    {
        delete[] ExtFetchStatus;
        ExtFetchStatus = nullptr;
    }
}

/***************************************************

    BindCol

    Bind a single column in the result set by invoking
    SQLBindCol.
***************************************************/

RETCODE odbcCURSOR::BindCol(
    UWORD       icol,
    SWORD       fCType,
    void*         rgbValue,
    SDWORD      cbValueMax,
    SDWORD     *pcbValue)
{
    SetRC(SQLBindCol(
        hstmt,
        icol,
        fCType,
        rgbValue,
        cbValueMax,
        pcbValue
    ));

    if (sqlsuccess())
        bColsBound = true;

    return lastRC();
}

/***************************************************

    BindCol

    Bind multiple columns as represented by the
    elements of the array of sCOLBIND structures.
***************************************************/

RETCODE odbcCURSOR::BindCol(
    sCOLBIND*       psResultSetBindings,
    UWORD           uCount,
    void*             pvBuf
)
{
    auto rgbBuf = (LPSTR)pvBuf;
    LPSTR pBuf;

    // allocate array of values
    if (ExtFetchRowCount)
    {
        pcbValues = new SDWORD[ExtFetchRowCount * uCount];
        memset(
            pcbValues,
            0,
            sizeof(SDWORD) * ExtFetchRowCount * uCount
        );
    }

    if (ExtFetchRowCount && !pcbValues)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    for (iCol_ = 0;
        iCol_ < uCount;
        iCol_++
        )
    {
        // if this is a BLOB type, allocate an odbcBLOB object to handle it.
        // the column is not bound; rather, the odbcBLOB object will be
        // used in a GetData call during Fetch and ExtendedFetch operations.
        if (psResultSetBindings[iCol_].fSqlType == SQL_LONGVARCHAR
            || psResultSetBindings[iCol_].fSqlType == SQL_LONGVARBINARY
            )
        {
            auto* *ppBlob = (odbcBLOB* *)(rgbBuf + psResultSetBindings[iCol_].iOffset);

            odbcBLOB* pBlob = nullptr;

            // allocate an array of BLOBs if extended fetch
            // operations are in use; otherwise, just allocate
            // a single BLOB.

            if (ExtFetchRowCount)
            {
                UWORD iRowCount = ExtFetchRowCount;

                do {
                    pBlob = new odbcBLOB(
                        this,
                        psResultSetBindings[iCol_].iCol,
                        psResultSetBindings[iCol_].cbValueMax,
                        psResultSetBindings[iCol_].fSqlType
                    );

                    *ppBlob = pBlob;
                    ++ppBlob;
                } while (--iRowCount > 0 && pBlob != nullptr);
            }
            else
            {
                pBlob = new odbcBLOB(
                    this,
                    psResultSetBindings[iCol_].iCol,
                    psResultSetBindings[iCol_].cbValueMax,
                    psResultSetBindings[iCol_].fSqlType
                );

                *ppBlob = pBlob;
            }

            if (pBlob == nullptr)
            {
                SetRC(SQL_ALLOC_FAILED);
            }
        }
        else
        {
            // if the fPtr flag is set, the buffer contains a pointer
            // to the storage for this column rather than the storage
            // itself.  If the fPtr flag is NOT set, the buffer contains
            // the actual storage location.

            if (psResultSetBindings[iCol_].fPtr)
                pBuf = *((LPSTR *)((void*)(rgbBuf + psResultSetBindings[iCol_].iOffset)));
            else
                pBuf = (LPSTR)(void*)(rgbBuf + psResultSetBindings[iCol_].iOffset);

            // bind the column as directed. 
            BindCol(
                (UWORD)psResultSetBindings[iCol_].iCol,
                (SWORD)psResultSetBindings[iCol_].fCType,
                (void*)pBuf,
                (SDWORD)psResultSetBindings[iCol_].cbValueMax,
                ExtFetchRowCount
                ?
                &pcbValues[iCol_ * ExtFetchRowCount]
                :
                (SDWORD *)&psResultSetBindings[iCol_].cbValue
            );

            if (lastRC() != SQL_SUCCESS)
                break;

        } // end if
    } // end for loop

    return lastRC();
}

/***************************************************

    BindCol

    Bind multiple columns as represented by the
    elements of the array of sCOLBIND structures
    supplied during construction.
***************************************************/

RETCODE odbcCURSOR::BindCol()
{
    // bind columns if column binding definitions
    // are available and we haven't done it yet.
    if (
        pColBindings    // column bindings are here
        && ColCount     // there are more than zero of them
        && pSet_        // there is a result set structure
        && !bColsBound  // we haven't done it yet
        )
    {
        BindCol(
            pColBindings,
            ColCount,
            pSet_
        );

    }

    return lastRC();
}

/***************************************************

    DescribeCol

    Invoke SQLDescribeCol to retrieve attributes
    for a given column in the result set.
***************************************************/

RETCODE odbcCURSOR::DescribeCol(
    UWORD       icol,
    LPUSTR          szColName,
    SWORD       cbColNameMax,
    SWORD      *pcbColName,
    SWORD      *pfSqlType,
    UDWORD     *pcbColDef,
    SWORD      *pibScale,
    SWORD      *pfNullable)
{

    SetRC(SQLDescribeCol(
        hstmt,
        icol,
        szColName,
        cbColNameMax,
        pcbColName,
        pfSqlType,
        pcbColDef,
        pibScale,
        pfNullable));

    return lastRC();
}

/***************************************************

    ColAttributes

    Invoke SQLColAttributes to retrieve attributes
    for a given column in the result set.
***************************************************/

RETCODE odbcCURSOR::ColAttributes(
    UWORD        icol,
    UWORD        fDescType,
    void*          rgbDesc,
    SWORD        cbDescMax,
    SWORD       *pcbDesc,
    SDWORD      *pfDesc)
{
    SetRC(SQLColAttributes(
        hstmt,
        icol,
        fDescType,
        rgbDesc,
        cbDescMax,
        pcbDesc,
        pfDesc));

    return lastRC();
}

/***************************************************

    Fetch

    Invoke SQLFetch.
***************************************************/

RETCODE odbcCURSOR::Fetch()
{
    SetRC(SQLFetch(hstmt));

    if (sqlsuccess() && pColBindings)
    {
        // see if we need to get data for any of the
        // columns (BLOBs).

        for (unsigned i = 0; i < ColCount; i++)
        {
            if (pColBindings[i].fSqlType == SQL_LONGVARCHAR
                || pColBindings[i].fSqlType == SQL_LONGVARBINARY
                )
            {
                odbcBLOB* pBlob = ColResultAsBLOB(pColBindings[i].iCol);

                if (pBlob)
                {
                    pBlob->GetData();
                    if (NoDataFound())
                        SetRC(SQL_SUCCESS);
                }

            }
            else
            {
                if (pColBindings[i].cbValue == SQL_NULL_DATA)
                {
                    // if the return value is NULL,
                    // clear the column data
                    memset(ColResultAddr(pColBindings[i].iCol),
                        0,
                        (size_t)pColBindings[i].cbValueMax
                    );
                }
                else if ((pColBindings[i].fSqlType == SQL_CHAR ||
                    pColBindings[i].fSqlType == SQL_VARCHAR)
                    && pColBindings[i].fCType == SQL_C_CHAR
                    && bTrimAllTrailingBlanks)
                {
                    TrimTrailingBlanks(ColResultAsLPSTR(pColBindings[i].iCol));
                }
            }
        } // end for loop
    }

    return lastRC();
}

/***************************************************

    GetCursorName

    Get the cursor name.
***************************************************/

RETCODE odbcCURSOR::GetCursorName(
    LPUSTR       szCursor,
    SWORD        cbCursorMax,
    SWORD      * pcbCursor
)
{
    SWORD sWord = 0;

    SetRC(SQLGetCursorName(
        hstmt,
        szCursor,
        cbCursorMax,
        pcbCursor ? pcbCursor : &sWord));

    return lastRC();
}

/***************************************************

    SetCursorName

    Set the cursor name.
***************************************************/

RETCODE odbcCURSOR::SetCursorName(
    LPUSTR szCursor
)
{
    SetRC(SQLSetCursorName(
        hstmt,
        szCursor,
        SQL_NTS));

    return lastRC();
}

/***************************************************

    ExecDirect

    Invoke SQLExecDirect.
***************************************************/

RETCODE odbcCURSOR::ExecDirect(
    LPUCSTR szSqlStr
)
{
    odbcSTMT::ExecDirect(szSqlStr);

    // if we need data and are using automatic binding,
    // apply the BLOBs if any exist.        
    if (needData() && bBlobsBound)
    {
        odbcBLOB* pBlob = nullptr;
        ParamData((void* *)&pBlob);

        // handle all BLOBs with a loop.
        if (needData())
        {
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
    else if (sqlsuccess())
    {
        // auto-bind if so requested
        if (AutoBindCols)
        {
            AutoBind();
        }

        // or, bind columns if column binding definitions
        // are available and we haven't done it yet.
        else if (
            pColBindings    // column bindings are here
            && ColCount     // there are more than zero of them
            && pSet_        // there is a result set structure
            && !bColsBound  // we haven't done it yet
            )
        {
            BindCol(
                pColBindings,
                ColCount,
                pSet_
            );
        }
    }

    if (sqlsuccess())
        bFirst = true;

    return lastRC();
}

/***************************************************

    Unbind

    Invoke SQLFreeStmt with SQL_UNBIND flag. Removes
    column bindings for result set.
***************************************************/

RETCODE odbcCURSOR::Unbind()
{
    SetRC(SQLFreeStmt(
        hstmt,
        SQL_UNBIND
    ));

    if (sqlsuccess())
        FreeColBindings();

    return lastRC();
}

/***************************************************

    Execute

    Invoke SQLExecute.
***************************************************/

RETCODE odbcCURSOR::Execute()
{
    odbcSTMT::Execute();

    // if we need data and are using automatic binding,
    // apply the BLOBs if any exist.        
    if (needData() && bBlobsBound)
    {
        odbcBLOB* pBlob = nullptr;
        ParamData((void* *)&pBlob);

        // handle all BLOBs with a loop.
        if (needData())
        {
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
    else if (sqlsuccess())
        bFirst = true;

    return lastRC();
}

/***************************************************

    Prepare

    Invoke SQLPrepare; bind parameters if parameter
    bindings need to be done. Overloaded version without
    argument prepares statement stored internally.
***************************************************/

RETCODE odbcCURSOR::Prepare(LPUCSTR szSqlStr)
{
    odbcSTMT::Prepare(szSqlStr);

    if (sqlsuccess())
    {
        // auto-bind if so requested
        if (AutoBindCols)
        {
            AutoBind();
        }

        // or, bind columns if column binding definitions
        // are available and we haven't done it yet.
        else if (
            pColBindings    // column bindings are here
            && ColCount     // there are more than zero of them
            && pSet_        // there is a result set structure
            && !bColsBound  // we haven't done it yet
            )
        {
            BindCol(
                pColBindings,
                ColCount,
                pSet_
            );
        }
    }

    return lastRC();
}

RETCODE odbcCURSOR::Prepare()
{
    odbcSTMT::Prepare();

    if (sqlsuccess())
    {
        // auto-bind if so requested
        if (AutoBindCols)
        {
            AutoBind();
        }

        // or, bind columns if column binding definitions
        // are available and we haven't done it yet.
        else if (
            pColBindings    // column bindings are here
            && ColCount     // there are more than zero of them
            && pSet_        // there is a result set structure
            && !bColsBound  // we haven't done it yet
            )
        {
            BindCol(
                pColBindings,
                ColCount,
                pSet_
            );
        }
    }

    return lastRC();
}

/***************************************************

    Tables

    Retrieve result set containing information about
    the tables in the data source.
***************************************************/

RETCODE odbcCURSOR::Tables(
    LPUSTR szTableQualifier,
    LPUSTR szTableOwner,
    LPUSTR szTableName,
    LPUSTR szTableType)
{
    SetRC(
        SQLTables(
            hstmt,
            szTableQualifier,
            SQL_NTS,
            szTableOwner,
            SQL_NTS,
            szTableName,
            SQL_NTS,
            szTableType,
            SQL_NTS
        ));

    return lastRC();

}

/***************************************************

    ForeignKeys

    Retrieve result set containing information about
    foreign keys relating the tables in the data source.
***************************************************/

RETCODE odbcCURSOR::ForeignKeys(
    LPUSTR szPkTableQualifier,
    LPUSTR szPkTableOwner,
    LPUSTR szPkTableName,
    LPUSTR szFkTableQualifier,
    LPUSTR szFkTableOwner,
    LPUSTR szFkTableName)
{
    SetRC(
        SQLForeignKeys(
            hstmt,
            szPkTableQualifier,
            SQL_NTS,
            szPkTableOwner,
            SQL_NTS,
            szPkTableName,
            SQL_NTS,
            szFkTableQualifier,
            SQL_NTS,
            szFkTableOwner,
            SQL_NTS,
            szFkTableName,
            SQL_NTS
        ));

    return lastRC();

}

/***************************************************

    TablePrivileges

    Retrieve result set containing information about
    the tables in the data source.
***************************************************/

RETCODE odbcCURSOR::TablePrivileges(
    LPUSTR szTableQualifier,
    LPUSTR szTableOwner,
    LPUSTR szTableName)
{
    SetRC(
        SQLTablePrivileges(
            hstmt,
            szTableQualifier,
            SQL_NTS,
            szTableOwner,
            SQL_NTS,
            szTableName,
            SQL_NTS
        ));

    return lastRC();

}

/***************************************************

    PrimaryKeys

    Retrieve result set containing information about
    the primary keys of the tables in the data source.
***************************************************/

RETCODE odbcCURSOR::PrimaryKeys(
    LPUSTR szTableQualifier,
    LPUSTR szTableOwner,
    LPUSTR szTableName)
{
    SetRC(
        SQLPrimaryKeys(
            hstmt,
            szTableQualifier,
            SQL_NTS,
            szTableOwner,
            SQL_NTS,
            szTableName,
            SQL_NTS
        ));

    return lastRC();

}


/***************************************************

    GetTypeInfo

    Get information concerning supported types.
***************************************************/

RETCODE odbcCURSOR::GetTypeInfo(SWORD fSqlType)
{
    SetRC(
        SQLGetTypeInfo(
            hstmt,
            fSqlType
        ));

    return lastRC();
}

/***************************************************

    SpecialColumns

    Retrieve result set containing information about
    special columns, such as the set of column(s) that
    (taken together and in the order presented) form(s) an
    unique identifier for a row in the table.
***************************************************/

RETCODE odbcCURSOR::SpecialColumns(
    UWORD      fColType,
    LPUSTR szTableQualifier,
    LPUSTR szTableOwner,
    LPUSTR szTableName,
    UWORD      fScope,
    UWORD      fNullable)
{
    SetRC(
        SQLSpecialColumns(
            hstmt,
            fColType,
            szTableQualifier,
            SQL_NTS,
            szTableOwner,
            SQL_NTS,
            szTableName,
            SQL_NTS,
            fScope,
            fNullable
        ));

    return lastRC();
}

/***************************************************

    NumResultCols

    Returns number of columns in the result set. This
    overloaded version returns the result code, and
    the number of columns in the signed word pointed
    to by pccol.
***************************************************/

RETCODE odbcCURSOR::NumResultCols(
    SWORD      *pccol
)
{
    SetRC(SQLNumResultCols(
        hstmt,
        pccol));

    return lastRC();
}

/***************************************************

    NumResultCols

    Returns number of columns in the result set. This
    overloaded version returns the number directly
***************************************************/

SWORD odbcCURSOR::NumResultCols()
{
    SWORD sword;

    SetRC(SQLNumResultCols(
        hstmt,
        &sword));

    return sword;
}

/***************************************************

    Columns

    Invoke SQLColumns to obtain information on the
    columns in the tables in this data source. Results
    are returned as a result set.
***************************************************/

RETCODE odbcCURSOR::Columns(
    LPUSTR szTableQualifier,
    LPUSTR szTableOwner,
    LPUSTR szTableName,
    LPUSTR szColumnName)
{
    SetRC(
        SQLColumns(
            hstmt,
            szTableQualifier,
            SQL_NTS,
            szTableOwner,
            SQL_NTS,
            szTableName,
            SQL_NTS,
            szColumnName,
            SQL_NTS
        ));

    return lastRC();

}

/***************************************************

    Procedures

    Invoke SQLProcedures to obtain information on the
    available procedures in this data source. Results
    are returned as a result set.
***************************************************/

RETCODE odbcCURSOR::Procedures(
    LPUSTR szProcQualifier,
    LPUSTR szProcOwner,
    LPUSTR szProcName)
{
    SetRC(
        SQLProcedures(
            hstmt,
            szProcQualifier,
            SQL_NTS,
            szProcOwner,
            SQL_NTS,
            szProcName,
            SQL_NTS
        ));

    return lastRC();

}

/***************************************************

    ProcedureColumns

    Invoke SQLProcedureColumns to obtain information
    on the parameters and the result set columns for the
    available procedures in this data source. Results
    are returned as a result set.

***************************************************/

RETCODE odbcCURSOR::ProcedureColumns(
    LPUSTR szProcQualifier,
    LPUSTR szProcOwner,
    LPUSTR szProcName,
    LPUSTR szColumnName)
{
    SetRC(
        SQLProcedureColumns(
            hstmt,
            szProcQualifier,
            SQL_NTS,
            szProcOwner,
            SQL_NTS,
            szProcName,
            SQL_NTS,
            szColumnName,
            SQL_NTS
        ));

    return lastRC();

}

/***************************************************

    GetData

    Invoke SQLGetData.
***************************************************/

RETCODE odbcCURSOR::GetData(
    UWORD      icol,
    SWORD      fCType,
    void*        rgbValue,
    SDWORD     cbValueMax,
    SDWORD     *pcbValue)
{
    SetRC(
        SQLGetData(
            hstmt,
            icol,
            fCType,
            rgbValue,
            cbValueMax,
            pcbValue
        ));

    return lastRC();

}

/***************************************************

    Statistics

    Return statistics on the tables in this data source
    as a result set. Invokes SQLStatistics.
***************************************************/

RETCODE odbcCURSOR::Statistics(
    LPUSTR szTableQualifier,
    LPUSTR szTableOwner,
    LPUSTR szTableName,
    UWORD      fUnique,
    UWORD      fAccuracy)
{
    SetRC(
        SQLStatistics(
            hstmt,
            szTableQualifier,
            SQL_NTS,
            szTableOwner,
            SQL_NTS,
            szTableName,
            SQL_NTS,
            fUnique,
            fAccuracy
        ));

    return lastRC();
}

/***************************************************

    ColumnPrivileges

    Invoke SQLColumnPrivileges to obtain information
    on the columns in the tables in this data source.
    Results are returned as a result set.
***************************************************/

RETCODE odbcCURSOR::ColumnPrivileges(
    LPUSTR szTableQualifier,
    LPUSTR szTableOwner,
    LPUSTR szTableName,
    LPUSTR szColumnName)
{
    SetRC(
        SQLColumnPrivileges(
            hstmt,
            szTableQualifier,
            SQL_NTS,
            szTableOwner,
            SQL_NTS,
            szTableName,
            SQL_NTS,
            szColumnName,
            SQL_NTS
        ));

    return lastRC();

}

/***************************************************

    SetPos

    Set cursor position in a returned result set block (rowset).
***************************************************/

RETCODE odbcCURSOR::SetPos(
    UWORD      irow,
    UWORD      fRefresh,
    UWORD      fLock
)
{
    SetRC(SQLSetPos(
        hstmt,
        irow,
        fRefresh,
        fLock)
    );

    return lastRC();
}

/***************************************************

    SetScrollOptions

    Set concurrency and other factors for use in ExtendedFetch.
***************************************************/

RETCODE odbcCURSOR::SetScrollOptions(
    UWORD      fConcurrency,
    SDWORD     crowKeyset,
    UWORD      crowRowset
)
{
    SetRC(SQLSetScrollOptions(
        hstmt,
        fConcurrency,
        crowKeyset,
        crowRowset)
    );

    return lastRC();
}
/***************************************************

    QueryAndBind

    Execute the query and bind result columns.
***************************************************/

bool odbcCURSOR::QueryAndBind()
{
    if (DoQuery())
    {
        // bind columns for result set
        BindCol(
            pColBindings,
            ColCount,
            pSet_);
        if (!sqlsuccess())
            return false;

        bFirst = true;
    } // !sqlsuccess()

    return !bError;
}

/***************************************************

    GetFirst

    Call Fetch() to get first row in the result set
    (or ExtFetchFirst() to get first rowset in result set).
    Returns non-zero value if it succeeds, zero if
    it fails.
***************************************************/

bool odbcCURSOR::GetFirst()
{
    if (bFirst || ExtFetchRowCount)
    {
        bFirst = false;
        if (ExtFetchRowCount)
            ExtFetchFirst();
        else
            Fetch();
        return sqlsuccess();
    }

    return false;
}

/***************************************************

    GetNext

    Call Fetch() to get next row in the result set
    (or ExtFetchNext() to get next rowset in result set).
    Returns non-zero value if it succeeds, zero if
    it fails.
***************************************************/

bool odbcCURSOR::GetNext()
{
    if (!bFirst || ExtFetchRowCount)
    {
        if (ExtFetchRowCount)
            ExtFetchNext();
        else
            Fetch();
        return sqlsuccess();
    }

    return false;
}

/***************************************************

    GetLast

    Call Fetch() to get last rowset in the result set.
    Only valid if extended fetch operations are in use.
    Returns non-zero value if it succeeds, zero if
    it fails.
***************************************************/

bool odbcCURSOR::GetLast()
{
    if (ExtFetchRowCount)
    {
        ExtFetchLast();
        return sqlsuccess();
    }

    return false;
}

/***************************************************

    GetPrev

    Call ExtFetchPrev() to get prior rowset in the result set.
    Only valid if extended fetch operations are in use.
    Returns non-zero value if it succeeds, zero if
    it fails.
***************************************************/

bool odbcCURSOR::GetPrev()
{
    if (ExtFetchRowCount)
    {
        ExtFetchPrevious();
        return sqlsuccess();
    }

    return false;
}

/***************************************************

    EnumResults

    Using Fetch(), retrieve rows in the result set;
    for each row retrieved, invoke the callback
    function supplied by the caller, passing the 'this'
    pointer and the user-supplied pointer 'pUser'.

    The callback function must conform to the typedef
    pfENUMRESULTS. A sample would be

    RETCODE     MyEnumFunc( podbcCURSOR thisObj,
                        void*                     pMyData
                      )
    {
    // you could assign and cast thisObj to a descendant
    // of odbcCURSOR here.
    // you could also assign and cast pMyData to a pointer
    // to a data structure of your own design.

    return SQL_SUCCESS;     // == zero
    }

    return a non-zero value to terminate enumeration
    of the result set; zero return continues enumeration.

***************************************************/

RETCODE odbcCURSOR::EnumResults(pfENUMRESULTS pfEnum, void* pUser)
{
    RETCODE ret;
    // cycle through the result set. 
    for (
        Fetch();
        sqlsuccess();
        Fetch()
        )
    {
        ret = (*pfEnum)(this, pUser);
        if (ret != SQL_SUCCESS)
        {
            return ret;
        }
    }

    return lastRC();
}


/***************************************************

    MoreResults

    Invoke SQLMoreResults().
***************************************************/

RETCODE odbcCURSOR::MoreResults()
{
    SetRC(
        SQLMoreResults(hstmt));

    return lastRC();
}

/***************************************************

    ExtendedFetch

    Invoke SQLExtendedFetch.
***************************************************/

RETCODE odbcCURSOR::ExtendedFetch
(
    UWORD      fFetchType,
    SDWORD     irow,
    UDWORD     *pcrow,
    UWORD      *rgfRowStatus
)
{
    SetRC(SQLExtendedFetch(
        hstmt,
        fFetchType,
        irow,
        pcrow,
        rgfRowStatus));

    if (sqlsuccess() && pColBindings && ExtFetchRowCount)
    {
        // see if we need to get data for any of the
        // columns (BLOBs).
        UWORD row = 1;
        for (; row <= *pcrow; row++)
        {
            // position to the next row if past the first row
            if (row > 1)
                if (static_cast<bool>(pConn->GetFunctions(SQL_API_SQLSETPOS)))
                    SetPos(row, SQL_POSITION, SQL_LOCK_NO_CHANGE);

            if (!sqlsuccess())
                break;

            // search for BLOBs
            for (UWORD i = 0; i < ColCount; i++)
            {
                if (pColBindings[i].fSqlType == SQL_LONGVARCHAR
                    || pColBindings[i].fSqlType == SQL_LONGVARBINARY
                    )
                {
                    // we found us a BLOB.
                    odbcBLOB* pBlob = ColResultAsBLOB(
                        pColBindings[i].iCol, row);

                    // the BLOB object knows how to fetch its data.
                    if (pBlob)
                    {
                        pBlob->GetData();
                        if (NoDataFound())
                            SetRC(SQL_SUCCESS);
                    }
                }
                // also clean up trailing blanks if needed on SQL_CHARs
                // and set NULL columns to empty (the Cursor Library
                // leaves garbage in there.)
                else
                {
                    SDWORD *pcbValue
                        = &pcbValues[i * ExtFetchRowCount];

                    if (pcbValue[row - 1] == SQL_NULL_DATA)
                    {
                        // if the return value is NULL,
                        // clear the column data
                        memset(ColResultAddr(pColBindings[i].iCol, row),
                            0,
                            (size_t)pColBindings[i].cbValueMax
                        );
                    }
                    else if ((pColBindings[i].fSqlType == SQL_CHAR ||
                        pColBindings[i].fSqlType == SQL_VARCHAR)
                        && pColBindings[i].fCType == SQL_C_CHAR
                        && bTrimAllTrailingBlanks)
                    {
                        TrimTrailingBlanks(ColResultAsLPSTR(
                            pColBindings[i].iCol, row));
                    }
                }
            } // end inner for loop

        } // end outer for loop

    // reposition to first row in rowset if we got beyond it
        if (sqlsuccess())
            if (row > 2)
                if (static_cast<bool>(pConn->GetFunctions(SQL_API_SQLSETPOS)))
                    SetPos(1, SQL_POSITION, SQL_LOCK_NO_CHANGE);
    } // end if

    return lastRC();
}

/***************************************************

    ExtFetchSetup

    Set up for automatic features surrounding
    SQLExtendedFetch.  Calls SetScrollOptions and
    allocates space for the row statuses.
***************************************************/

RETCODE odbcCURSOR::ExtFetchSetup(
    UWORD      fConcurrency,
    SDWORD     crowKeyset,
    UWORD      crowRowset
)
{
    char szDriverVersion[11];
    SWORD cbRetBytes;
    SWORD bSave = pConn->AutoReport(odbcNOREPORT);
    SDWORD crowOldKeyset;

    pConn->GetInfo(
        SQL_DRIVER_ODBC_VER,
        szDriverVersion,
        sizeof(szDriverVersion),
        &cbRetBytes
    );

    if (!pConn->sqlsuccess())
    {
        szDriverVersion[0] = '0';
        szDriverVersion[1] = '1';
        szDriverVersion[2] = 0;
    }

    pConn->AutoReport(bSave);

    if (ExtFetchStatus)
    {
        delete[] ExtFetchStatus;
        ExtFetchStatus = nullptr;
        ExtFetchRowCount = 0;
    }

    // if compiled out, use old form.
#if (ODBCVER < 0x0200)
    SetScrollOptions(fConcurrency, crowKeyset, crowRowset);
#else
    // trust, but verify
    bSave = AutoReport(odbcNOREPORT);
    unsigned tries = 0;
    SetRC(SQL_SUCCESS);

    // if this is a 1.0 driver, see if SQLSetScrollOptions will
    // work.

    if (szDriverVersion[0] == '0' && szDriverVersion[1] == '1')
    {
        SetScrollOptions(fConcurrency, crowKeyset, crowRowset);
        // if above failed, we may be using v2 constant for
        // cursor type. Try the corresponding v1.0 type.
        if (!sqlsuccess() && !tries)
        {
            crowOldKeyset = crowKeyset;

            switch (crowOldKeyset)
            {
            case SQL_CURSOR_STATIC:
                crowOldKeyset = SQL_SCROLL_STATIC;
                break;

            case SQL_CURSOR_DYNAMIC:
                crowOldKeyset = SQL_SCROLL_DYNAMIC;
                break;

            case SQL_CURSOR_KEYSET_DRIVEN:
                crowOldKeyset = SQL_SCROLL_KEYSET_DRIVEN;
                break;

            case SQL_CURSOR_FORWARD_ONLY:
                crowOldKeyset = SQL_SCROLL_FORWARD_ONLY;
                break;

            default:
                break;
            }
            // if we changed values, we now have a v1.0 constant;
            // try it out.
            if (crowKeyset != crowOldKeyset)
                SetScrollOptions(fConcurrency, crowOldKeyset, crowRowset);
        }
    }

    // if we didn't go into the above 'if', we are still set to
    // sqlsuccess() == true.  We do the following block only if
    // we didn't go into the above block, or we failed in the
    // above block (sqlsuccess() == false now).

    // in short, we end up in the following block if we have a v2.0
    // driver, or we have a 1.0 driver that didn't respond to
    // SQLSetScrollOptions (as it won't for example, if the Cursor
    // Library is in use or if the driver really doesn't support
    // SQLSetScrollOptions).

    if (!(szDriverVersion[0] == '0' && szDriverVersion[1] == '1')
        || !sqlsuccess())
    {
        do {
            SetStmtOption(SQL_CONCURRENCY, fConcurrency);
            if (sqlsuccess())
            {
                SetStmtOption(SQL_CURSOR_TYPE, crowKeyset);
                if (sqlsuccess())
                    SetStmtOption(SQL_ROWSET_SIZE, crowRowset);
            }

            if (!sqlsuccess() && tries)
                // we have reported already; break out
                break;

            // if the above failed, the user may still be passing
            // old values. Convert them.
            if (!sqlsuccess())
            {
                crowOldKeyset = crowKeyset;

                // The caller may be using old values but the driver
                // expects new values. Try to translate.

                switch (crowKeyset)
                {
                case SQL_SCROLL_STATIC:
                    crowOldKeyset = SQL_CURSOR_STATIC;
                    break;

                case SQL_SCROLL_DYNAMIC:
                    crowOldKeyset = SQL_CURSOR_DYNAMIC;
                    break;

                case SQL_SCROLL_KEYSET_DRIVEN:
                    crowOldKeyset = SQL_CURSOR_KEYSET_DRIVEN;
                    break;

                case SQL_SCROLL_FORWARD_ONLY:
                    crowOldKeyset = SQL_CURSOR_FORWARD_ONLY;
                    break;

                default:
                    break;
                }

                // now we'll do a trick or two here. If we swapped
                // constants from v1.0 to v2.0, OR if our v2.0
                // constant was a static cursor, try to do our thing.

                if (crowOldKeyset != crowKeyset
                    || crowOldKeyset == SQL_CURSOR_STATIC)
                {
                    SetStmtOption(SQL_CONCURRENCY, fConcurrency);
                    if (sqlsuccess())
                    {
                        SetStmtOption(SQL_CURSOR_TYPE, crowOldKeyset);

                        // if the static type failed, try keyset-driven
                        // instead.
                        if (!sqlsuccess() &&
                            !tries &&
                            crowOldKeyset == SQL_CURSOR_STATIC)
                            SetStmtOption(SQL_CURSOR_TYPE,
                                SQL_CURSOR_KEYSET_DRIVEN);
                        if (sqlsuccess())
                            SetStmtOption(SQL_ROWSET_SIZE, crowRowset);
                    }
                }

                // if the user expected to report errors, repeat the
                // operations with reporting back on. We do this by allowing
                // the do-loop to execute once more after turning on
                // reporting.

                if (bSave != odbcNOREPORT && !sqlsuccess())
                {
                    AutoReport(bSave);
                    continue;
                }

                break;
            }

            break;
        } // end do loop block
        while (tries++ == 0);
    } // end if not an odbc v1.0 driver

// restore value of auto-report flag
    AutoReport(bSave);
#endif

    if (sqlsuccess())
    {
        ExtFetchStatus = new UWORD[ExtFetchRowCount = crowRowset];
        if (!ExtFetchStatus)
        {
            SetRC(SQL_ALLOC_FAILED);
            return lastRC();
        }
        memset(ExtFetchStatus, 0, crowRowset * sizeof(UWORD));
    }

    return lastRC();
}

/***************************************************

    ExtFetchFirst

    Invoke SQLExtendedFetch to fetch first batch.

    Returns the number of rows actually fetched.  If this
    number is not equal to ExtFetchRowCount, an error may
    have occurred; use the sqlsuccess() member function
    (and other odbcBASE members) to analyze the situation.
***************************************************/

UDWORD odbcCURSOR::ExtFetchFirst()
{
    ExtendedFetch(
        ExtFetchType = SQL_FETCH_FIRST,
        ExtFetchRelAbsRow = 0,
        &ExtFetchRow,
        ExtFetchStatus
    );

    return ExtFetchRow;
}

/***************************************************

    ExtFetchNext

    Invoke SQLExtendedFetch to fetch next batch.

    Returns the number of rows actually fetched.  If this
    number is not equal to ExtFetchRowCount, an error may
    have occurred; use the sqlsuccess() member function
    (and other odbcBASE members) to analyze the situation.
***************************************************/

UDWORD odbcCURSOR::ExtFetchNext()
{
    ExtendedFetch(
        ExtFetchType = SQL_FETCH_NEXT,
        ExtFetchRelAbsRow = 0,
        &ExtFetchRow,
        ExtFetchStatus
    );

    return ExtFetchRow;
}

/***************************************************

    ExtFetchLast

    Invoke SQLExtendedFetch to fetch last batch.

    Returns the number of rows actually fetched.  If this
    number is not equal to ExtFetchRowCount, an error may
    have occurred; use the sqlsuccess() member function
    (and other odbcBASE members) to analyze the situation.
***************************************************/

UDWORD odbcCURSOR::ExtFetchLast()
{
    ExtendedFetch(
        ExtFetchType = SQL_FETCH_LAST,
        ExtFetchRelAbsRow = 0,
        &ExtFetchRow,
        ExtFetchStatus
    );

    return ExtFetchRow;
}

/***************************************************

    ExtFetchPrevious

    Invoke SQLExtendedFetch to fetch previous batch.

    Returns the number of rows actually fetched.  If this
    number is not equal to ExtFetchRowCount, an error may
    have occurred; use the sqlsuccess() member function
    (and other odbcBASE members) to analyze the situation.
***************************************************/

UDWORD odbcCURSOR::ExtFetchPrevious()
{
    ExtendedFetch(
        ExtFetchType = SQL_FETCH_PREV,
        ExtFetchRelAbsRow = 0,
        &ExtFetchRow,
        ExtFetchStatus
    );

    return ExtFetchRow;
}

/***************************************************

    ExtFetchRelative

    Invoke SQLExtendedFetch to fetch batch at relative
    position (n rows forward/backward).

    Returns the number of rows actually fetched.  If this
    number is not equal to ExtFetchRowCount, an error may
    have occurred; use the sqlsuccess() member function
    (and other odbcBASE members) to analyze the situation.
***************************************************/

UDWORD odbcCURSOR::ExtFetchRelative(
    SDWORD     irow
)
{
    ExtendedFetch(
        ExtFetchType = SQL_FETCH_RELATIVE,
        ExtFetchRelAbsRow = irow,
        &ExtFetchRow,
        ExtFetchStatus
    );

    return ExtFetchRow;
}

/***************************************************

    ExtFetchAbsolute

    Invoke SQLExtendedFetch to fetch batch at absolute
    position (beginning at nth row).

    Returns the number of rows actually fetched.  If this
    number is not equal to ExtFetchRowCount, an error may
    have occurred; use the sqlsuccess() member function
    (and other odbcBASE members) to analyze the situation.
***************************************************/

UDWORD odbcCURSOR::ExtFetchAbsolute(
    SDWORD     irow
)
{
    ExtendedFetch(
        ExtFetchType = SQL_FETCH_ABSOLUTE,
        ExtFetchRelAbsRow = irow,
        &ExtFetchRow,
        ExtFetchStatus
    );

    return ExtFetchRow;
}

/***************************************************

    ExtFetchResume

    Invoke SQLExtendedFetch to resume fetching batch after
    an error occurred during one of the other extended
    fetch operations.

    Returns the number of rows actually fetched.  If this
    number is not equal to ExtFetchRowCount, an error may
    have occurred; use the sqlsuccess() member function
    (and other odbcBASE members) to analyze the situation.
***************************************************/

//UDWORD odbcCURSOR::ExtFetchResume()
//    {
//    ExtendedFetch(
//        ExtFetchType = SQL_FETCH_RESUME,
//        ExtFetchRelAbsRow,
//        &ExtFetchRow,
//        ExtFetchStatus
//        );
//        
//    return ExtFetchRow;
//    }

/***************************************************

    AutoBind

    Retrieve array of column bindings.  Accepts address
    of unsigned double-word for output structure size,
    and address of a void* to store pointer to allocated
    buffer.
***************************************************/

RETCODE odbcCURSOR::AutoBind
(
    UWORD     *pcbStructSize,       //      = NULL
    void*     *ppStruct               //      = NULL
)
{
    UWORD i; // loop index
    UDWORD cbValueMax;

    if (pcbStructSize)
        *pcbStructSize = 0;

    if (ppStruct)
        *ppStruct = nullptr;

    // free the old buffer and column bindings
    FreeColBindings();

    // 
    // get the number of columns we're working with here.
    NumResultCols((SWORD *)&ColCount);

    // sum the sizes
    if (sqlsuccess())
    {
        if (ColCount)
        {
            // allocate a buffer for col bindings
            pColBindings = new sCOLBIND[ColCount];

            if (pColBindings)
            {
                // cycle through once to get the binding info
                int iCol = 1;
                for (i = 0; i < ColCount; i++, iCol++)
                {

                    if (0 == i && IsInserter())
                    {
                        SDWORD sdw = 0;
                        ColAttributes(iCol, SQL_COLUMN_AUTO_INCREMENT, nullptr, 0, nullptr, &sdw);
                        if (sdw)
                        {
                            i--; ColCount--;
                            continue;
                        }
                    }

                    // we need this storage location for call to
                    // DescribeCol, but it is not used for column
                    // binding.
                    SWORD       cbColName;

                    // columns are numbered beginning
                    // with 1, array index is zero-based
                    pColBindings[i].iCol = i + 1;

                    // call DescribeCol()
                    DescribeCol(
                        /*pColBindings[i].*/iCol,
                        pColBindings[i].szColName,
                        sizeof(pColBindings[i].szColName),
                        &cbColName,
                        &pColBindings[i].fSqlType,
                        (UDWORD *)&pColBindings[i].cbValueMax,
                        &pColBindings[i].ibScale,
                        &pColBindings[i].fNullable
                    );

                    pColBindings[i].fPtr = 0;
                    pColBindings[i].pConstraints = nullptr;

                    cbValueMax = pColBindings[i].cbValueMax;

                    // determine C type for SQL type
                    switch (pColBindings[i].fSqlType)
                    {
                    case SQL_LONGVARCHAR:
                        cbValueMax = sizeof(odbcBLOB*);
                        pColBindings[i].fCType = SQL_C_CHAR;

                        break;

                    case SQL_LONGVARBINARY:
                        cbValueMax = sizeof(odbcBLOB*);
                        pColBindings[i].fCType = SQL_C_BINARY;
                        break;

                    case SQL_CHAR:
                    case SQL_VARCHAR:
                    case SQL_BIGINT:
                        pColBindings[i].fCType = SQL_C_CHAR;

                        // unless the user didn't want to do so,
                        // make the buffer one byte bigger to
                        // accommodate null-terminator byte.

                        if (cbValueMax > MAX_REASONABLE_C_CHAR_SIZE)
                            cbValueMax = pColBindings[i].cbValueMax =
                            MAX_REASONABLE_C_CHAR_SIZE;
                        if (!bNoExtraByteOnStrings_)
                        {
                            cbValueMax++;
                            pColBindings[i].cbValueMax++;
                        }

                        break;


                    case SQL_BIT:
                        pColBindings[i].fCType = SQL_C_BIT;
                        pColBindings[i].cbValueMax = cbValueMax = sizeof(UCHAR);
                        break;

                    case SQL_TINYINT:
                        pColBindings[i].fCType = SQL_C_TINYINT;
                        pColBindings[i].cbValueMax = cbValueMax = sizeof(SCHAR);
                        break;


                    case SQL_SMALLINT:
                        pColBindings[i].fCType = SQL_C_SHORT;
                        pColBindings[i].cbValueMax = cbValueMax = sizeof(SWORD);
                        break;


                    case SQL_INTEGER:
                        pColBindings[i].fCType = SQL_C_LONG;
                        pColBindings[i].cbValueMax = cbValueMax = sizeof(SDWORD);
                        break;


                    case SQL_REAL:
                        pColBindings[i].fCType = SQL_C_FLOAT;
                        pColBindings[i].cbValueMax = cbValueMax = sizeof(SFLOAT);
                        break;


                    case SQL_DECIMAL:
                    case SQL_NUMERIC:
                    case SQL_FLOAT:
                    case SQL_DOUBLE:
                        pColBindings[i].fCType = SQL_C_DOUBLE;
                        pColBindings[i].cbValueMax = cbValueMax = sizeof(SDOUBLE);
                        break;


                    case SQL_DATE:
                        pColBindings[i].fCType = SQL_C_DATE;
                        pColBindings[i].cbValueMax = cbValueMax = sizeof(DATE_STRUCT);
                        break;


                    case SQL_TIME:
                        pColBindings[i].fCType = SQL_C_TIME;
                        pColBindings[i].cbValueMax = cbValueMax = sizeof(TIME_STRUCT);
                        break;


                    case SQL_TIMESTAMP:
                        pColBindings[i].fCType = SQL_C_TIMESTAMP;
                        pColBindings[i].cbValueMax = cbValueMax = sizeof(TIMESTAMP_STRUCT);
                        break;

                    case SQL_BINARY:
                    case SQL_VARBINARY:
                    default:
                        if (cbValueMax > MAX_REASONABLE_C_CHAR_SIZE)
                            cbValueMax = pColBindings[i].cbValueMax =
                            MAX_REASONABLE_C_BINARY_SIZE;
                        pColBindings[i].fCType = SQL_C_BINARY;
                        break;
                    } // end switch

                // size alloted for preceding struct members is the
                // offset to the start of the current column's
                // binding location.

                    pColBindings[i].iOffset = (UWORD)pSSize;

                    // Add size of current column's binding location to
                    // the size of the to-be-allocated data structure.
                    // Take into account that if you are doing a block
                    // cursor, you are allocating an array of storage
                    // locations for each column, not just a single 
                    // location. The number of elements in the array
                    // is in ExtFetchRowCount, if it is greater than zero.

                    pSSize +=
                        (UWORD)(UDWORD)
                        (cbValueMax *
                        (ExtFetchRowCount ? ExtFetchRowCount : 1)
                            );
                } // end for loop

            // allocate data structure
                pSet_ = (void *)(new char[pSSize]);

                if (pSet_ != nullptr)
                {
                    // zero it
                    memset(pSet_, 0, pSSize);

                    // use overloaded BindCol() to do column binding now.
                    BindCol();

                    // set flag
                    if (sqlsuccess())
                    {
                        bAutoBound = true;
                        AutoBindCols = false;
                    }
                    else
                    {
                        delete[](char *)pSet_;
                        pSet_ = nullptr;
                        pSSize = 0;
                    }

                    // if user wants size, give her/him size
                    if (bAutoBound && pcbStructSize)
                        *pcbStructSize = pSSize;

                    // if user wants struct ptr, give her/him struct ptr
                    if (bAutoBound && ppStruct)
                        *ppStruct = pSet_;
                } // if (pSet)
                else
                {
                    SetRC(SQL_ALLOC_FAILED);
                    return lastRC();
                }


            } // if (pColBindings)
            else
            {
                SetRC(SQL_ALLOC_FAILED);
                return lastRC();
            }

        } // if (ColCount)
    } // if (sqlsuccess())

    return lastRC();
}

/***************************************************

    ColResultAddr

    Return address of bound column output. Six
    overloaded forms: The first four accept
    the name of the column and the second two
    accept the column number (they start with 1).

    The irow argument can be used with block cursors,
    to get the address of the column data for the
    irow'th row.  Row numbers start with 1.

    In all cases a NULL is returned if either the
    icol does not match a bound column OR the irow
    argument is out of bounds.
***************************************************/

void* odbcCURSOR::ColResultAddr(LPUCSTR ColName, UWORD irow)
{
    UWORD i = 0;
    size_t uThisSize;

    if (!pColBindings || !pSet_)
        return nullptr;

    if (irow > 0)
        if (ExtFetchRowCount == 0
            || irow > ExtFetchRowCount)
            return nullptr;
        else
            irow--; // zero-based arrays
    else
        return nullptr;

    if (!ColName)
        return nullptr;
    int nStrLen = strlen((LPCSTR)ColName) + 1;
    if (nStrLen > COLUMN_NAME_COMPARE_MAX)
        nStrLen = COLUMN_NAME_COMPARE_MAX;
    for (
        ;
        i < ColCount
        && _memicmp(ColName, pColBindings[i].szColName, nStrLen) != 0;
        i++
        )
        ;

    if (i == ColCount)
        return nullptr;

    uThisSize = (size_t)pColBindings[i].cbValueMax;

    if (pColBindings[i].fSqlType == SQL_LONGVARCHAR ||
        pColBindings[i].fSqlType == SQL_LONGVARBINARY
        )
        uThisSize = sizeof(odbcBLOB*);

    return (void*)(
        (LPSTR)pSet_
        + pColBindings[i].iOffset
        + (irow
            * uThisSize)
        );
}

// same call as preceding function, but with signed char string
void* odbcCURSOR::ColResultAddr(LPCSTR ColName, UWORD irow)
{
    return ColResultAddr((LPUCSTR)ColName, irow);
}

// for use with simple fetch into automatically bound struct.
void* odbcCURSOR::ColResultAddr(LPUCSTR ColName)
{
    UWORD i = 0;

    if (!pColBindings || !pSet_)
        return nullptr;

    if (!ColName)
        return nullptr;
    int nStrLen = strlen((LPCSTR)ColName) + 1;
    if (nStrLen > COLUMN_NAME_COMPARE_MAX)
        nStrLen = COLUMN_NAME_COMPARE_MAX;
    for (
        ;
        i < ColCount
        && _memicmp(ColName, pColBindings[i].szColName, nStrLen) != 0;
        i++
        )
        ;

    if (i == ColCount)
        return nullptr;

    return (void*)(
        (LPSTR)pSet_
        + pColBindings[i].iOffset
        );
}

// same call as preceding function, but with signed char string
void* odbcCURSOR::ColResultAddr(LPCSTR ColName)
{
    return ColResultAddr((LPUCSTR)ColName);
}

void* odbcCURSOR::ColResultAddr(UWORD icol, UWORD irow)
{
    UWORD i = 0;
    size_t uThisSize;

    if (!pColBindings || !pSet_)
        return nullptr;

    // validate the irow argument, if non-zero
    if (irow > 0)
        if (ExtFetchRowCount == 0
            || irow > ExtFetchRowCount)
            return nullptr;
        else
            irow--; // zero-based arrays
    else
        return nullptr;

    for (
        ;
        i < ColCount && pColBindings[i].iCol != icol;
        i++
        )
        ;

    if (i == ColCount)
        return nullptr;

    uThisSize = (size_t)pColBindings[i].cbValueMax;

    if (pColBindings[i].fSqlType == SQL_LONGVARCHAR ||
        pColBindings[i].fSqlType == SQL_LONGVARBINARY
        )
        uThisSize = sizeof(odbcBLOB*);

    return (void*)(
        (LPSTR)pSet_
        + pColBindings[i].iOffset
        + (irow
            * uThisSize)
        );
}


void* odbcCURSOR::ColResultAddr(UWORD icol)
{
    UWORD i = 0;

    if (!pColBindings || !pSet_)
        return nullptr;

    for (
        ;
        i < ColCount && pColBindings[i].iCol != icol;
        i++
        )
        ;

    if (i == ColCount)
        return nullptr;

    return (void*)
        (
        (LPSTR)pSet_
            + pColBindings[i].iOffset
            );
}

/***************************************************

    ColResultAs<type>

    Return value of bound column output. forty-two
    overloaded forms, accepting signed and unsigned
        char strings for column names, column numbers in
        the result set (numbered beginning with 1), and
        row within the rowset if using a block cursor.

        <type> can be Short, Unsigned Short, Long, Unsigned
        Long, Float, Double, or LPSTR.

        ColName is the name of a bound column; can be signed
        or unsigned character string.

        icol is the number of a bound column.

    The irow argument can be used with block cursors,
    to get the address of the column data for the
    irow'th row.  Row numbers start with 1.

    In all cases a NULL is returned if either the
    icol does not match a bound column OR the irow
    argument is out of bounds.
***************************************************/

// some macros to help out.  Too bad all compiler vendors don't
// support templates yet, think how easy this would be...

#define retColNameValue1(type,defvalue)\
       type *pValue =\
              (type *)ColResultAddr(ColName);\
       return pValue ? *pValue : (type)defvalue

#define retIcolValue1(type,defvalue)\
       type *pValue =\
              (type *)ColResultAddr(icol);\
       return pValue ? *pValue : (type)defvalue

#define retColNameValueN(type,defvalue)\
       type *pValue =\
              (type *)ColResultAddr(ColName, irow);\
       return pValue ? *pValue : (type)defvalue

#define retIcolValueN(type,defvalue)\
       type *pValue =\
              (type *)ColResultAddr(icol, irow);\
       return pValue ? *pValue : (type)defvalue

// overloads for single row fetch operations...
SWORD  odbcCURSOR::ColResultAsShort(LPCSTR ColName)
{
    retColNameValue1(SWORD, 0);
}
SWORD  odbcCURSOR::ColResultAsShort(LPUCSTR ColName)
{
    retColNameValue1(SWORD, 0);
}
SWORD  odbcCURSOR::ColResultAsShort(UWORD icol)
{
    retIcolValue1(SWORD, 0);
}
UWORD  odbcCURSOR::ColResultAsUnsignedShort(LPCSTR ColName)
{
    retColNameValue1(UWORD, 0);
}
UWORD  odbcCURSOR::ColResultAsUnsignedShort(LPUCSTR ColName)
{
    retColNameValue1(UWORD, 0);
}
UWORD  odbcCURSOR::ColResultAsUnsignedShort(UWORD icol)
{
    retIcolValue1(UWORD, 0);
}
SDWORD odbcCURSOR::ColResultAsLong(LPCSTR ColName)
{
    retColNameValue1(SDWORD, 0);
}
SDWORD odbcCURSOR::ColResultAsLong(LPUCSTR ColName)
{
    retColNameValue1(SDWORD, 0);
}
SDWORD odbcCURSOR::ColResultAsLong(UWORD icol)
{
    retIcolValue1(SDWORD, 0);
}
UDWORD odbcCURSOR::ColResultAsUnsignedLong(LPCSTR ColName)
{
    retColNameValue1(UDWORD, 0);
}
UDWORD odbcCURSOR::ColResultAsUnsignedLong(LPUCSTR ColName)
{
    retColNameValue1(UDWORD, 0);
}
UDWORD odbcCURSOR::ColResultAsUnsignedLong(UWORD icol)
{
    retIcolValue1(UDWORD, 0);
}
float  odbcCURSOR::ColResultAsFloat(LPCSTR ColName)
{
    retColNameValue1(float, 0.0);
}
float  odbcCURSOR::ColResultAsFloat(LPUCSTR ColName)
{
    retColNameValue1(float, 0.0);
}
float  odbcCURSOR::ColResultAsFloat(UWORD icol)
{
    retIcolValue1(float, 0.0);
}
double odbcCURSOR::ColResultAsDouble(LPCSTR ColName)
{
    retColNameValue1(double, 0.0);
}
double odbcCURSOR::ColResultAsDouble(LPUCSTR ColName)
{
    retColNameValue1(double, 0.0);
}
double odbcCURSOR::ColResultAsDouble(UWORD icol)
{
    retIcolValue1(double, 0.0);
}
LPSTR  odbcCURSOR::ColResultAsLPSTR(LPCSTR ColName)
{
    return (LPSTR)ColResultAddr(ColName);
}
LPSTR  odbcCURSOR::ColResultAsLPSTR(LPUCSTR ColName)
{
    return (LPSTR)ColResultAddr(ColName);
}
LPSTR  odbcCURSOR::ColResultAsLPSTR(UWORD icol)
{
    return (LPSTR)ColResultAddr(icol);
}


// overloads for extended fetch block cursor operations...
SWORD  odbcCURSOR::ColResultAsShort(LPCSTR ColName, UWORD irow)
{
    retColNameValueN(SWORD, 0);
}
SWORD  odbcCURSOR::ColResultAsShort(LPUCSTR ColName, UWORD irow)
{
    retColNameValueN(SWORD, 0);
}
SWORD  odbcCURSOR::ColResultAsShort(UWORD icol, UWORD irow)
{
    retIcolValueN(SWORD, 0);
}
UWORD  odbcCURSOR::ColResultAsUnsignedShort(LPCSTR ColName, UWORD irow)
{
    retColNameValueN(UWORD, 0);
}
UWORD  odbcCURSOR::ColResultAsUnsignedShort(LPUCSTR ColName, UWORD irow)
{
    retColNameValueN(UWORD, 0);
}
UWORD  odbcCURSOR::ColResultAsUnsignedShort(UWORD icol, UWORD irow)
{
    retIcolValueN(UWORD, 0);
}
SDWORD odbcCURSOR::ColResultAsLong(LPCSTR ColName, UWORD irow)
{
    retColNameValueN(SDWORD, 0);
}
SDWORD odbcCURSOR::ColResultAsLong(LPUCSTR ColName, UWORD irow)
{
    retColNameValueN(SDWORD, 0);
}
SDWORD odbcCURSOR::ColResultAsLong(UWORD icol, UWORD irow)
{
    retIcolValueN(SDWORD, 0);
}
UDWORD odbcCURSOR::ColResultAsUnsignedLong(LPCSTR ColName, UWORD irow)
{
    retColNameValueN(UDWORD, 0);
}
UDWORD odbcCURSOR::ColResultAsUnsignedLong(LPUCSTR ColName, UWORD irow)
{
    retColNameValueN(UDWORD, 0);
}
UDWORD odbcCURSOR::ColResultAsUnsignedLong(UWORD icol, UWORD irow)
{
    retIcolValueN(UDWORD, 0);
}
float  odbcCURSOR::ColResultAsFloat(LPCSTR ColName, UWORD irow)
{
    retColNameValueN(float, 0.0);
}
float  odbcCURSOR::ColResultAsFloat(LPUCSTR ColName, UWORD irow)
{
    retColNameValueN(float, 0.0);
}
float  odbcCURSOR::ColResultAsFloat(UWORD icol, UWORD irow)
{
    retIcolValueN(float, 0.0);
}
double odbcCURSOR::ColResultAsDouble(LPCSTR ColName, UWORD irow)
{
    retColNameValueN(double, 0.0);
}
double odbcCURSOR::ColResultAsDouble(LPUCSTR ColName, UWORD irow)
{
    retColNameValueN(double, 0.0);
}
double odbcCURSOR::ColResultAsDouble(UWORD icol, UWORD irow)
{
    retIcolValueN(double, 0.0);
}
LPSTR  odbcCURSOR::ColResultAsLPSTR(LPCSTR ColName, UWORD irow)
{
    return (LPSTR)ColResultAddr(ColName, irow);
}
LPSTR  odbcCURSOR::ColResultAsLPSTR(LPUCSTR ColName, UWORD irow)
{
    return (LPSTR)ColResultAddr(ColName, irow);
}
LPSTR  odbcCURSOR::ColResultAsLPSTR(UWORD icol, UWORD irow)
{
    return (LPSTR)ColResultAddr(icol, irow);
}

/***************************************************

    ColResultSizeArray

    Return address of column result size array for a
    bound column when extended fetch block cursor in use
    and either form of automatic binding is in use.
    Two overloaded forms: The first accepts the name of the
    column and the second accepts the column number (they
    start with 1).
***************************************************/

const SDWORD     * odbcCURSOR::ColResultSizeArray(LPUCSTR ColName)
{
    UWORD i = 0;

    if (!pColBindings)
        return nullptr;

    if (!ColName)
        return nullptr;
    int nStrLen = strlen((LPCSTR)ColName) + 1;
    if (nStrLen > COLUMN_NAME_COMPARE_MAX)
        nStrLen = COLUMN_NAME_COMPARE_MAX;
    for (
        ;
        i < ColCount
        && _memicmp(ColName, pColBindings[i].szColName, nStrLen) != 0;
        i++
        )
        ;

    if (i == ColCount)
        return nullptr;

    if (!ExtFetchRowCount)
        return &pColBindings[i].cbValue;

    return &pcbValues[i * ExtFetchRowCount];
}

const SDWORD     * odbcCURSOR::ColResultSizeArray(UWORD icol)
{
    UWORD i = 0;

    if (!pColBindings)
        return nullptr;

    for (
        ;
        i < ColCount
        && icol != pColBindings[i].iCol;
        i++
        )
        ;

    if (i == ColCount)
        return nullptr;

    if (!ExtFetchRowCount)
        return &pColBindings[i].cbValue;

    return &pcbValues[i * ExtFetchRowCount];
}

/***************************************************

    ColResultInfo

    Return address of data structure describing bound column.
    Two overloaded forms: The first accepts the name of the
    column and the second accepts the column number (they
    start with 1).
***************************************************/

sCOLBIND* odbcCURSOR::ColResultInfo(LPUCSTR ColName)
{
    UWORD i = 0;

    if (!pColBindings)
        return nullptr;

    if (!ColName)
        return nullptr;
    int nStrLen = strlen((LPCSTR)ColName) + 1;
    if (nStrLen > COLUMN_NAME_COMPARE_MAX)
        nStrLen = COLUMN_NAME_COMPARE_MAX;
    for (
        ;
        i < ColCount
        && _memicmp(ColName, pColBindings[i].szColName, nStrLen) != 0;
        i++
        )
        ;

    if (i == ColCount)
        return nullptr;

    return &pColBindings[i];
}

sCOLBIND* odbcCURSOR::ColResultInfo(LPCSTR ColName)
{
    return
        ColResultInfo((LPUCSTR)ColName);
}

sCOLBIND* odbcCURSOR::ColResultInfo(UWORD icol)
{
    UWORD i = 0;

    if (!pColBindings)
        return nullptr;

    for (
        ;
        i < ColCount && pColBindings[i].iCol != icol;
        i++
        )
        ;

    if (i == ColCount)
        return nullptr;

    return &pColBindings[i];
}

/***************************************************

    FreeColBindings

    Free dynamically allocated column bindings
    and result set.  This function was moved here
    from cursor.hpp in version 2, since its
    increased complexity makes it unsuitable for
    an inline function.
***************************************************/

void odbcCURSOR::FreeColBindings()
{
    // free old column bindings
    if (pColBindings)
    {
        // we free BLOBs that were allocated for either AutoBind()
        // or data dictionary bindings. These are allocated in the
        // call to BindCol() for either kind of automatic binding.
        for (unsigned i = 0; i < ColCount; i++)
        {
            if (pColBindings[i].fSqlType == SQL_LONGVARCHAR
                || pColBindings[i].fSqlType
                == SQL_LONGVARBINARY
                )
            {
                // get address of odbcBLOB object
                auto* *ppBlob = (odbcBLOB* *)((char *)pSet_ + pColBindings[i].iOffset);

                // free it/them if it/they is/are not NULL
                if (ExtFetchRowCount)
                {
                    UWORD uRowCount = ExtFetchRowCount;

                    do
                    {
                        if ((*ppBlob))
                        {
                            delete (*ppBlob);
                            *ppBlob = nullptr;
                        }
                        ppBlob++;
                    } while (--uRowCount);
                }
                else
                {
                    if ((*ppBlob))
                    {
                        delete (*ppBlob);
                        *ppBlob = nullptr;
                    }
                }
            } // end if BLOB data type
        } // end for loop
    }  // end if pColBindings not NULL

// only delete the column bindings if they were generated in
// AutoBind().
    if (pColBindings && bAutoBound)
    {
        delete[] pColBindings;
    }

    // free the old buffer if one was allocated during AutoBind()
    if (pSet_ && bAutoBound)
    {
        delete[](char *)pSet_;
    }

    // free old column size dword array


    delete[] pcbValues;


    // reset variables to known starting point for a closed cursor
    pcbValues = nullptr;
    pColBindings = nullptr;
    ColCount = 0;
    bColsBound = false;
    pSet_ = nullptr;
    pSSize = 0;
    bAutoBound = false;
}

// new in v2.0

/***************************************************

     RowsetRowRecordSize

    Return size of one row's worth of data in the
    automatically allocated structure for storing
    bound column output during extended fetches,
    when the internal data structure consists of
    arrays of column data.

    Used to determine space allocation for a single
    rowset row structure (record structure) for use
    in call to RowsetRowAsRecord().
***************************************************/

size_t odbcCURSOR::RowsetRowRecordSize()
{
    // return no bytes copied if we exit early
    size_t uBytesToBeCopied = 0;

    // if we have no bindings, or no record, return an error.
    if (!pColBindings
        || !pSet_)
        return 0;

    for (UWORD icol = 0; icol < ColCount; icol++)
    {
        // get address of column info                
        sCOLBIND* pColBind = &pColBindings[icol];
        size_t uThisSize;

        uThisSize = (size_t)pColBind->cbValueMax;

        if (pColBind->fSqlType == SQL_LONGVARCHAR ||
            pColBind->fSqlType == SQL_LONGVARBINARY
            )
            uThisSize = sizeof(odbcBLOB*);

        // increment size
        uBytesToBeCopied += uThisSize;
    } // end for loop    
    return uBytesToBeCopied;
}

/***************************************************

    MoveRowsetRowToRecord

    Return one row's worth of data in the
    automatically allocated structure for storing
    bound column output during extended fetches,
    when the internal data structure consists of
    arrays of column data.

    Use RowsetRowRecordSize() to determine space allocation
    for a single rowset row structure (record).
***************************************************/

RETCODE odbcCURSOR::MoveRowsetRowToRecord
(
    UWORD irow,
    void* pRecord,
    size_t uSize,
    size_t     *puBytesCopied
)
{
    // return no bytes copied if we exit early
    *puBytesCopied = 0;

    // clear error
    SetRC(SQL_SUCCESS);

    // if we have no bindings, or no record, or not doing an extended
    //    fetch, or the row number is out of range, return an error.
    if (!pColBindings
        || !pSet_
        || (!ExtFetchRowCount && irow != 1))
    {
        SetRC(SQL_NOT_EXTSETUP);
        return lastRC();
    }

    if (irow < 1
        || (ExtFetchRowCount && irow > ExtFetchRow))
    {
        SetRC(SQL_BAD_PARAMETER);
        return lastRC();
    }

    for (UWORD icol = 0; icol < ColCount; icol++)
    {
        // get address of column info
        sCOLBIND* pColBind = &pColBindings[icol];

        size_t uThisSize;

        // BLOBs are represented by a odbcBLOB pointer.
        // other types use their actual size.

        if (pColBind->fSqlType == SQL_LONGVARCHAR ||
            pColBind->fSqlType == SQL_LONGVARBINARY)
            uThisSize = sizeof(odbcBLOB*);
        else
            uThisSize = (size_t)pColBind->cbValueMax;

        if ((*puBytesCopied + uThisSize) > uSize)
        {
            SetRC(SQL_BUFFER_TOO_SMALL);
            return lastRC();
        }

        // copy the data
        memcpy((void *)((LPSTR)pRecord + *puBytesCopied),
            ExtFetchRowCount ?
            (void *)ColResultAddr(pColBind->iCol, irow) :
            (void *)ColResultAddr(pColBind->iCol),
            uThisSize
        );

        // increment size
        *puBytesCopied += uThisSize;
    } // end for loop
    return SQL_SUCCESS;
}

/***************************************************

    MoveRecordToRowsetRow

    Return one row's worth of data from a single-row
    record structure to a rowset row in the internal
    rowset data structure dynamically allocated
    during AutoBind(). The rowset's single-row
    record structure is the same as that returned by
    MoveRowsetRowToRecord.

***************************************************/

RETCODE odbcCURSOR::MoveRecordToRowsetRow
(
    UWORD irow,
    void* pRecord,
    size_t uSize,
    size_t     *puBytesCopied
)
{
    // return no bytes copied if we exit early
    *puBytesCopied = 0;

    // clear error
    SetRC(SQL_SUCCESS);

    // if we have no bindings, or no record, or not doing an extended
    //    fetch, or the row number is out of range, return an error.
    if (!pColBindings
        || !pSet_
        || (!ExtFetchRowCount && irow != 1))
    {
        SetRC(SQL_NOT_EXTSETUP);
        return lastRC();
    }

    if (irow < 1
        || (ExtFetchRowCount && irow > ExtFetchRow))
    {
        SetRC(SQL_BAD_PARAMETER);
        return lastRC();
    }

    for (UWORD icol = 0; icol < ColCount; icol++)
    {
        // get address of column info
        sCOLBIND* pColBind = &pColBindings[icol];
        size_t uThisSize;

        // BLOBs are represented by a odbcBLOB pointer.
        // other types use their actual size.

        if (pColBind->fSqlType == SQL_LONGVARCHAR ||
            pColBind->fSqlType == SQL_LONGVARBINARY)
            uThisSize = sizeof(odbcBLOB*);
        else
            uThisSize = (size_t)pColBind->cbValueMax;

        if ((*puBytesCopied + uThisSize) > uSize)
        {
            SetRC(SQL_BUFFER_TOO_SMALL);
            return lastRC();
        }

        // copy the data
        memcpy(ExtFetchRowCount ?
            (void *)ColResultAddr(pColBind->iCol, irow) :
            (void *)ColResultAddr(pColBind->iCol),
            (void *)((LPSTR)pRecord + *puBytesCopied),
            uThisSize
        );

        // increment size
        *puBytesCopied += uThisSize;
    } // end for loop
    return SQL_SUCCESS;
}

//////////////////////////////////////////////////////////////

/***************************************************

    SetParam

    Bind multiple parameters as represented by the
    array of sPARMBIND structures pointed to by
    psParmBindings. The count of structures is
    passed as the uCount argument.If desired, these
    are bound to the structure pointed to by pvBuf.
    (You can also use the constants SQL_DATA_AT_EXEC
    and SQL_NULL_DATA to indicate run-time parameter
    binding or null data values; see sqlstruc.hpp
    for instructions).

    Unlike its equivalent in odbcSTMT, this one will
    handle BLOBs (the odbcBLOB class constructor requires
    an odbcCURSOR instance, since it can handle reading
    as well as writing BLOB data).

    One disadvantage is that this function binds
    only a single value to each parameter.
***************************************************/

RETCODE odbcCURSOR::SetParam(
    const sPARMBIND* psParmBindings,
    UWORD           uCount,
    void*             pvBuf
)
{
    auto rgbBuf = (LPSTR)pvBuf;
    LPSTR pBuf;

    for (iParm = 0;
        iParm < uCount;
        iParm++
        )
    {
        // if there is a BLOB in use, bind it now.
        if (psParmBindings[iParm].fSqlType == SQL_LONGVARCHAR
            || psParmBindings[iParm].fSqlType == SQL_LONGVARBINARY
            )
        {
            auto* *ppBlob
                = (odbcBLOB* *)
                (rgbBuf + psParmBindings[iParm].iOffset);

            odbcBLOB* pBlob = *ppBlob;

            if (pBlob)
                pBlob->BindParameter();
        }
        else  // not a BLOB type
        {
            // if the iOffset member contains odbcUSE_RGBVALUE,
            // send the rgbValue member value rather than the computed
            // offset into the structure pointed to by pvBuf
            if (psParmBindings[iParm].iOffset != odbcUSE_RGBVALUE)
                pBuf = (LPSTR)(void*)(rgbBuf + psParmBindings[iParm].iOffset);
            else
                pBuf = (LPSTR)psParmBindings[iParm].rgbValue;

#if (ODBCVER >= 0x0200)
            // see if we can use new ODBC API function SQLBindParameter via odbcSTMT
            // member function BindParameter().
            if (static_cast<bool>(pConn->GetFunctions(SQL_API_SQLBINDPARAMETER)))
            {
                // bind the column as directed.
                BindParameter(
                    (UWORD)psParmBindings[iParm].iParm,
                    (SWORD)(psParmBindings[iParm].fParamType == 0 ?
                        SQL_PARAM_INPUT :
                        psParmBindings[iParm].fParamType),
                        (SWORD)psParmBindings[iParm].fCType,
                    (SWORD)psParmBindings[iParm].fSqlType,
                    (UDWORD)psParmBindings[iParm].cbColDef,
                    (SWORD)psParmBindings[iParm].ibScale,
                    (void*)pBuf,
                    (SDWORD)psParmBindings[iParm].cbColDef,
                    (SDWORD *)(&psParmBindings[iParm].cbValue)
                );
            }
            else
            {
                // bind the column as directed.
                SetParam(
                    psParmBindings[iParm].iParm,
                    psParmBindings[iParm].fCType,
                    psParmBindings[iParm].fSqlType,
                    psParmBindings[iParm].cbColDef,
                    psParmBindings[iParm].ibScale,
                    pBuf,
                    (SDWORD *)(&psParmBindings[iParm].cbValue)
                );
            }
#else
            // bind the column as directed.
            SetParam(
                psParmBindings[iParm].iParm,
                psParmBindings[iParm].fCType,
                psParmBindings[iParm].fSqlType,
                psParmBindings[iParm].cbColDef,
                psParmBindings[iParm].ibScale,
                pBuf,
                (SDWORD *)(&psParmBindings[iParm].cbValue)
            );
#endif
        }
        if (lastRC() != SQL_SUCCESS)
            break;

    }

    return lastRC();
}

/***************************************************

    ColResultAsBLOB

    Return value of bound column output where the column
    is of SQL type SQL_LONGVARBINARY or SQL_LONGVARCHAR,
    and was automatically bound as a BLOB (using the odbcBLOB
    class).

        ColName is the name of a bound column; can be signed
        or unsigned character string.

        icol is the number of a bound column.

    The irow argument can be used with block cursors,
    to get the address of the column data for the
    irow'th row.  Row numbers start with 1.

    In all cases a NULL is returned if either the
    icol does not match a bound column OR the irow
    argument is out of bounds.
***************************************************/

odbcBLOB*  odbcCURSOR::ColResultAsBLOB(LPCSTR ColName)
{
    // handle out-of-range cases with extra code (compared
    // with ColResultAsLPSTR, for example) because we must
    // do an extra layer of dereference and don't want the
    // caller's program to go << BANG! >>
    if (!ColResultAddr(ColName))
        return nullptr;
    return *(odbcBLOB* *)ColResultAddr(ColName);
}

odbcBLOB*  odbcCURSOR::ColResultAsBLOB(LPUCSTR ColName)
{
    // handle out-of-range cases with extra code (compared
    // with ColResultAsLPSTR, for example) because we must
    // do an extra layer of dereference and don't want the
    // caller's program to go << BANG! >>
    if (!ColResultAddr(ColName))
        return nullptr;
    return *(odbcBLOB* *)ColResultAddr(ColName);
}

odbcBLOB*  odbcCURSOR::ColResultAsBLOB(UWORD icol)
{
    // handle out-of-range cases with extra code (compared
    // with ColResultAsLPSTR, for example) because we must
    // do an extra layer of dereference and don't want the
    // caller's program to go << BANG! >>
    if (!ColResultAddr(icol))
        return nullptr;
    return *(odbcBLOB* *)ColResultAddr(icol);
}

odbcBLOB*  odbcCURSOR::ColResultAsBLOB(LPCSTR ColName, UWORD irow)
{
    // handle out-of-range cases with extra code (compared
    // with ColResultAsLPSTR, for example) because we must
    // do an extra layer of dereference and don't want the
    // caller's program to go << BANG! >>
    if (!ColResultAddr(ColName, irow))
        return nullptr;
    return *(odbcBLOB* *)ColResultAddr(ColName, irow);
}

odbcBLOB*  odbcCURSOR::ColResultAsBLOB(LPUCSTR ColName, UWORD irow)
{
    // handle out-of-range cases with extra code (compared
    // with ColResultAsLPSTR, for example) because we must
    // do an extra layer of dereference and don't want the
    // caller's program to go << BANG! >>
    if (!ColResultAddr(ColName, irow))
        return nullptr;
    return *(odbcBLOB* *)ColResultAddr(ColName, irow);
}

odbcBLOB*  odbcCURSOR::ColResultAsBLOB(UWORD icol, UWORD irow)
{
    // handle out-of-range cases with extra code (compared
    // with ColResultAsLPSTR, for example) because we must
    // do an extra layer of dereference and don't want the
    // caller's program to go << BANG! >>
    if (!ColResultAddr(icol, irow))
        return nullptr;
    return *(odbcBLOB* *)ColResultAddr(icol, irow);
}

// end new in v2.0

// new in v2.1

/*************************************************************

    ClearRowsetBuffer

    Clears contents of rowset buffer.

*************************************************************/
RETCODE odbcCURSOR::ClearRowsetBuffer()
{
    // clear error
    SetRC(SQL_SUCCESS);

    // if we have no bindings, or no record, or not doing an extended
    //    fetch, or the row number is out of range, simply return.
    if (!pColBindings
        || !pSet_)
    {
        return lastRC();
    }

    for (UWORD icol = 0; icol < ColCount; icol++)
    {
        // get address of column info
        sCOLBIND* pColBind = &pColBindings[icol];

        size_t uThisSize;

        // BLOBs are represented by a odbcBLOB pointer.
        // call SetNull() on the BLOB to null its value.
        // other types can be memset to zeros.

        if (pColBind->fSqlType == SQL_LONGVARCHAR ||
            pColBind->fSqlType == SQL_LONGVARBINARY)
        {
            // if the rowset contains multiple rows, null each BLOB
            // otherwise there's only one, so just null the one BLOB.
            if (ExtFetchRowCount)
                for (UWORD irow = 1; irow <= ExtFetchRowCount; irow++)
                {
                    odbcBLOB* pBlob = ColResultAsBLOB(
                        pColBind->iCol, irow);
                    if (pBlob)
                        pBlob->SetNull();
                }
            else
            {
                odbcBLOB* pBlob = ColResultAsBLOB(
                    pColBind->iCol);
                if (pBlob)
                    pBlob->SetNull();
            }
        }
        else
        {
            uThisSize = (size_t)pColBind->cbValueMax;

            // copy the data
            memset((void *)ColResultAddr(pColBind->iCol),
                0,
                ExtFetchRowCount ?
                uThisSize * ExtFetchRowCount :
                uThisSize
            );
        }
    } // end for loop
    return SQL_SUCCESS;
}

/*************************************************************

    IsColValueNull

    Returns true if column has value of NULL, false otherwise.

*************************************************************/

bool odbcCURSOR::IsColValueNull(LPUCSTR ColName)
{
    UWORD i = 0;

    if (!pColBindings)
        return NULL;

    if (!ColName)
        return NULL;
    int nStrLen = strlen((LPCSTR)ColName) + 1;
    if (nStrLen > COLUMN_NAME_COMPARE_MAX)
        nStrLen = COLUMN_NAME_COMPARE_MAX;
    for (
        ;
        i < ColCount
        && _memicmp(ColName, pColBindings[i].szColName, nStrLen) != 0;
        i++
        )
        ;

    if (i == ColCount)
        return NULL;

    if (!ExtFetchRowCount)
        return pColBindings[i].cbValue == SQL_NULL_DATA;

    return pcbValues[i * ExtFetchRowCount] == SQL_NULL_DATA;
}

bool odbcCURSOR::IsColValueNull(UWORD icol)
{
    UWORD i = 0;

    if (!pColBindings)
        return NULL;

    //    if ( ExtFetchRowCount == 0 )
    //        return NULL;

    for (
        ;
        i < ColCount
        && icol != pColBindings[i].iCol;
        i++
        )
        ;

    if (i == ColCount)
        return NULL;

    if (!ExtFetchRowCount)
        return pColBindings[i].cbValue == SQL_NULL_DATA;

    return pcbValues[i * ExtFetchRowCount] == SQL_NULL_DATA;
}

bool odbcCURSOR::IsColValueNull(LPUCSTR ColName, UWORD irow)
{
    UWORD i = 0;

    if (!pColBindings)
        return NULL;

    if (ExtFetchRowCount == 0)
        return NULL;

    if (irow == 0 || irow > ExtFetchRowCount)
        return NULL;

    if (!ColName)
        return NULL;
    int nStrLen = strlen((LPCSTR)ColName) + 1;
    if (nStrLen > COLUMN_NAME_COMPARE_MAX)
        nStrLen = COLUMN_NAME_COMPARE_MAX;
    for (
        ;
        i < ColCount
        && _memicmp(ColName, pColBindings[i].szColName, nStrLen) != 0;
        i++
        )
        ;

    if (i == ColCount)
        return NULL;

    return (&pcbValues[i * ExtFetchRowCount])[irow - 1] == SQL_NULL_DATA;
}

bool odbcCURSOR::IsColValueNull(UWORD icol, UWORD irow)
{
    UWORD i = 0;

    if (!pColBindings)
        return NULL;

    if (ExtFetchRowCount == 0)
        return NULL;

    if (irow == 0 || irow > ExtFetchRowCount)
        return NULL;

    for (
        ;
        i < ColCount
        && icol != pColBindings[i].iCol;
        i++
        )
        ;

    if (i == ColCount)
        return NULL;

    return (&pcbValues[i * ExtFetchRowCount])[irow - 1] == SQL_NULL_DATA;
}

/*************************************************************

    SetColValueNull

    Returns true if column can be set to NULL, false otherwise.

*************************************************************/

bool odbcCURSOR::SetColValueNull(LPUCSTR ColName)
{
    UWORD i = 0;

    if (!pColBindings)
        return NULL;

    if (!ColName)
        return NULL;
    int nStrLen = strlen((LPCSTR)ColName) + 1;
    if (nStrLen > COLUMN_NAME_COMPARE_MAX)
        nStrLen = COLUMN_NAME_COMPARE_MAX;
    for (
        ;
        i < ColCount
        && _memicmp(ColName, pColBindings[i].szColName, nStrLen) != 0;
        i++
        )
        ;

    if (i == ColCount)
        return NULL;

    if (!ExtFetchRowCount)
        pColBindings[i].cbValue = SQL_NULL_DATA;
    else
        pcbValues[i * ExtFetchRowCount] = SQL_NULL_DATA;

    return true;
}

bool odbcCURSOR::SetColValueNull(UWORD icol)
{
    UWORD i = 0;

    if (!pColBindings)
        return NULL;

    if (ExtFetchRowCount == 0)
        return NULL;

    for (
        ;
        i < ColCount
        && icol != pColBindings[i].iCol;
        i++
        )
        ;

    if (i == ColCount)
        return NULL;

    if (!ExtFetchRowCount)
        pColBindings[i].cbValue = SQL_NULL_DATA;
    else
        pcbValues[i * ExtFetchRowCount] = SQL_NULL_DATA;
    return true;

}

bool odbcCURSOR::SetColValueNull(LPUCSTR ColName, UWORD irow)
{
    UWORD i = 0;

    if (!pColBindings)
        return NULL;

    if (ExtFetchRowCount == 0)
        return NULL;

    if (irow == 0 || irow > ExtFetchRowCount)
        return NULL;

    if (!ColName)
        return NULL;
    int nStrLen = strlen((LPCSTR)ColName) + 1;
    if (nStrLen > COLUMN_NAME_COMPARE_MAX)
        nStrLen = COLUMN_NAME_COMPARE_MAX;
    for (
        ;
        i < ColCount
        && _memicmp(ColName, pColBindings[i].szColName, nStrLen) != 0;
        i++
        )
        ;

    if (i == ColCount)
        return NULL;

    (&pcbValues[i * ExtFetchRowCount])[irow - 1] = SQL_NULL_DATA;
    return true;
}

bool odbcCURSOR::SetColValueNull(UWORD icol, UWORD irow)
{
    UWORD i = 0;

    if (!pColBindings)
        return NULL;

    if (ExtFetchRowCount == 0)
        return NULL;

    if (irow == 0 || irow > ExtFetchRowCount)
        return NULL;

    for (
        ;
        i < ColCount
        && icol != pColBindings[i].iCol;
        i++
        )
        ;

    if (i == ColCount)
        return NULL;

    (&pcbValues[i * ExtFetchRowCount])[irow - 1] = SQL_NULL_DATA;
    return true;
}


// end new in v2.1

