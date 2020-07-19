#include <sql.hpp>
#include <memory.h> // for memset

/***************************************************

    odbcSTMT

    Constructor.

    Arguments:

                odbcCONNECT*    pC

                        Connection object pointer.

                LPUCSTR                 lpszStmt

                        SQL statement string. Default value
                        is NULL.

                bool                    bPrepare

                        If true (non-zero), SQLPrepare should
                        be invoked to prepare the SQL statement.
                        Default value is false.

                bool                    bExecute

                        If true (non-zero), SQLExecute should
                        be invoked (if bPrepare was non-zero)
                        or SQLExecDirect should be invoked
                        (if bprepare was zero) to execute the SQL
                        statement.  Default value is false.

                const sPARMBIND*     psParmBindings

                        If non-NULL, address of array of structures
                        defining the binding of parameters in the
                        SQL statement to members of the structure
                        containing parameter values at execution
                        time. Default value is NULL.

                UWORD                   uParmCount

                        Count of array elements in psParmBindings.
                        Default value is 0.

                void*                             pvParmStruct

                        Address of structure containing parameter
                        values. Default value is NULL.

***************************************************/

odbcSTMT::odbcSTMT(
    odbcCONNECT*            pC,
    LPUCSTR                 lpszSentStmt,
    const sPARMBIND*             psParmBindings,
    UWORD                   uParmCount,
    void*                     pvParmStruct
)
{
    pConn = pC;
    // make sure we retrieve status info during construction
    AutoRetrieve(odbcREPSUCCESSWITHINFO);
    AutoReport(odbcNOREPORT);
    // initialize state variables
    pParmBindings = psParmBindings;
    ParmCount = uParmCount;
    pParms = pvParmStruct;
    lpszStmt = lpszSentStmt;
    iParm = 0;
    iParmRow = 0;
    iParamOptRowCount = 0;
    bParmsBound
        = bError
        = bPrepared
        = bExecuted
        = false;

    // call driver to allocate a statement
    SetRC(SQLAllocStmt(
        pConn->GetHdbc(),
        &hstmt));

    // if we succeeded...
    if (sqlsuccess())
    {
        if (lpszStmt)           // ...and there is a statement...
        {

            SetParams();    // bind parameters explicitly

        } // lpszStmt
        else
            SetParams(); // bind parameters explicitly
    } // sqlsuccess

// If any error occurred during construction,
// the info has been retrieved.
// Now inherit error handling from connection.
    ErrHandler = pC->ErrHandler;
    bGetErrorInfo = pC->bGetErrorInfo;
    bReportErrorInfo = pC->bReportErrorInfo;
    hwnd_ = pC->hwnd_;
    flags = pC->flags;


    pConn->RegisterStmt(this);
}

odbcSTMT::odbcSTMT(
    odbcCONNECT*    pC,
    LPCSTR                  lpszSentStmt,
    const sPARMBIND*     psParmBindings,
    UWORD                   uParmCount,
    void*                             pvParmStruct
)
{
    pConn = pC;
    // make sure we retrieve status info during construction
    AutoRetrieve(odbcREPSUCCESSWITHINFO);
    AutoReport(odbcNOREPORT);
    // initialize state variables
    pParmBindings = psParmBindings;
    ParmCount = uParmCount;
    pParms = pvParmStruct;
    lpszStmt = (LPUCSTR)lpszSentStmt;
    iParm = 0;
    iParmRow = 0;
    iParamOptRowCount = 0;
    bParmsBound
        = bError
        = bPrepared
        = bExecuted
        = false;
    // call driver to allocate a statement
    SetRC(SQLAllocStmt(
        pConn->GetHdbc(),
        &hstmt));

    // if we succeeded...
    if (sqlsuccess())
    {
        if (lpszStmt)           // ...and there is a statement...
        {

            SetParams();    // bind parameters explicitly

        } // lpszStmt
        else
            SetParams(); // bind parameters explicitly
    } // sqlsuccess

// If any error occurred during construction,
// the info has been retrieved.
// Now inherit error handling from connection.
    ErrHandler = pC->ErrHandler;
    bGetErrorInfo = pC->bGetErrorInfo;
    bReportErrorInfo = pC->bReportErrorInfo;
    hwnd_ = pC->hwnd_;
    flags = pC->flags;

    pConn->RegisterStmt(this);
}

/***************************************************

    ~odbcSTMT

    Destructor.
***************************************************/

odbcSTMT::~odbcSTMT()
{
    if (hstmt)
    {
        SQLFreeStmt(
            hstmt,
            SQL_DROP
        );
        hstmt = nullptr;
    }
    if (pConn && !IsBadReadPtr(pConn, sizeof(*pConn)))
        pConn->UnregisterStmt(this);
}

/***************************************************

    Cancel

    Invoke SQLCancel.
***************************************************/

RETCODE odbcSTMT::Cancel()
{
    SetRC(SQLCancel(hstmt));
    return lastRC();
}

/***************************************************

    Close

    Call SQLFreeStmt with SQL_CLOSE argument.
***************************************************/

RETCODE odbcSTMT::Close()
{
    SetRC(SQLFreeStmt(
        hstmt,
        SQL_CLOSE
    ));

    return lastRC();
}

/***************************************************

    ResetParams

    Remove old parameter bindings by calling SQLFreeStmt
    with SQL_RESET_PARAMS flag.
***************************************************/

RETCODE odbcSTMT::ResetParams()
{
    SetRC(SQLFreeStmt(
        hstmt,
        SQL_RESET_PARAMS
    ));

    return lastRC();
}

/***************************************************

    ExecDirect

    Invoke SQLExecDirect.
***************************************************/

RETCODE odbcSTMT::ExecDirect(
    LPUCSTR szSqlStr
)
{
    SetRC(SQLExecDirect(
        hstmt,
        (LPUSTR)szSqlStr,
        SQL_NTS));

    if (sqlsuccess())
        bExecuted = true;

    return lastRC();
}

/***************************************************

    Execute

    Invoke SQLExecute.
***************************************************/

RETCODE odbcSTMT::Execute()
{
    SetRC(SQLExecute(hstmt));

    if (sqlsuccess())
        bExecuted = true;

    return lastRC();
}

/***************************************************

    RowCount

    Get count of rows affected by the statement. Overloaded
    to two versions: one returns a result code, and the count
    is returned in the signed double-word pointed to by pcrow;
    the other version returns the count directly.
***************************************************/

RETCODE odbcSTMT::RowCount(
    SDWORD      *pcrow
)
{
    SetRC(SQLRowCount(
        hstmt,
        pcrow
    ));

    return lastRC();
}


SDWORD odbcSTMT::RowCount()
{
    SDWORD sdword;
    SetRC(SQLRowCount(
        hstmt,
        &sdword
    ));

    return sdword;
}

/***************************************************

    Prepare

    Invoke SQLPrepare; bind parameters if parameter
    bindings need to be done.
***************************************************/

RETCODE odbcSTMT::Prepare(
    LPUCSTR szSqlStr
)
{
    SetRC(SQLPrepare(
        hstmt,
        (LPUSTR)szSqlStr,
        SQL_NTS));

    return lastRC();
}

RETCODE odbcSTMT::Prepare()
{
    Prepare(lpszStmt ? lpszStmt : (LPUCSTR)"");
    if (!sqlsuccess())
        bError = true;
    else
    {
        // we have prepared. Bind parameters now,
        // if there are any to bind directly.
        bPrepared = true;
        if (bParmsBound)
            ResetParams();
        SetParams();
    } // sqlsuccess()

    return lastRC();
}

/***************************************************

    SetParams

    Invoke SetParam member function passing array of
    parameter bindings.
***************************************************/

bool    odbcSTMT::SetParams()
{
    if (
        pParmBindings   // there are more than
        && ParmCount    // zero parameter bindings
        )
    {
        // bind multiple parameters automatically
        SetParam(
            pParmBindings,
            ParmCount,
            pParms
        );
        bError = !sqlsuccess();
    } // pParmBindings && ParmCount
    else
        return false;

    return sqlsuccess();
}

/***************************************************

    SetParam

    Invoke SQLSetParam to bind a parameter.
***************************************************/

RETCODE odbcSTMT::SetParam(
    UWORD       ipar,
    SWORD       fCType,
    SWORD       fSqlType,
    UDWORD      cbColDef,
    SWORD       ibScale,
    void*         rgbValue,
    SDWORD      *pcbValue)
{
#if (ODBCVER >= 0x0300)
    SetRC(SQLBindParameter(
        hstmt,
        ipar,
        SQL_PARAM_INPUT,
        fCType,
        fSqlType,
        cbColDef,
        ibScale,
        rgbValue,
        SQL_SETPARAM_VALUE_MAX,
        pcbValue));
#else
    SetRC(SQLSetParam(
        hstmt,
        ipar,
        fCType,
        fSqlType,
        cbColDef,
        ibScale,
        rgbValue,
        pcbValue));
#endif
    if (sqlsuccess())
        bParmsBound = true;

    return lastRC();
}

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

    One disadvantage is that this function binds
    only a single value to each parameter.
***************************************************/

RETCODE odbcSTMT::SetParam(
    const sPARMBIND* psParmBindings,
    UWORD 		uCount,
    void* 		pvBuf
)
{
    auto rgbBuf = (LPSTR)pvBuf;
    LPSTR pBuf;

    for (iParm = 0;
        iParm < uCount;
        iParm++
        )
    {
        // if the iOffset member contains odbcUSE_RGBVALUE, 
        // send the rgbValue member value rather than the computed
        // offset into the structure pointed to by pvBuf 
        if (psParmBindings[iParm].iOffset != odbcUSE_RGBVALUE)
            pBuf = (LPSTR)(void*)(rgbBuf + psParmBindings[iParm].iOffset);
        else
            pBuf = (LPSTR)psParmBindings[iParm].rgbValue;

        // bind the column as directed.	
#if (ODBCVER >= 0x0200)
            // see if we can use new ODBC API function SQLBindParameter via odbcSTMT
            // member function BindParameter().
        if (static_cast<bool>(pConn->GetFunctions(SQL_API_SQLBINDPARAMETER)))
        {
            // bind the column as directed.
            BindParameter(
                psParmBindings[iParm].iParm,
                psParmBindings[iParm].fParamType == 0 ?
                SQL_PARAM_INPUT :
                psParmBindings[iParm].fParamType,
                psParmBindings[iParm].fCType,
                psParmBindings[iParm].fSqlType,
                PrecisionForSqlType(
                    psParmBindings[iParm].fSqlType,
                    psParmBindings[iParm].cbColDef
                ),
                psParmBindings[iParm].ibScale,
                pBuf,
                psParmBindings[iParm].cbColDef,
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
                PrecisionForSqlType(
                    psParmBindings[iParm].fSqlType,
                    psParmBindings[iParm].cbColDef
                ),
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
            PrecisionForSqlType(
                psParmBindings[iParm].fSqlType,
                psParmBindings[iParm].cbColDef
            ),
            psParmBindings[iParm].ibScale,
            pBuf,
            (SDWORD *)(&psParmBindings[iParm].cbValue)
        );
#endif

        if (lastRC() != SQL_SUCCESS)
            break;

    }

    return lastRC();
}

/***************************************************

    RegisterError

        Get more information on the most recent error code
        from an ODBC operation. Results can be retrieved using
        member functions in the parent odbcBASE class.

        This function calls the base class member function Error()
        with arguments appropriate for this object type.
***************************************************/

RETCODE odbcSTMT::RegisterError()
{
    return Error(
        pConn->GetHenv(),
        (pConn->GetHdbc() != nullptr) ?
        pConn->GetHdbc() :
        SQL_NULL_HDBC,
        (hstmt != nullptr &&
            pConn->GetHdbc() != nullptr) ?
        hstmt :
        SQL_NULL_HSTMT
    );
}

/***************************************************

    GetStmtOption

    GetStmtOption returns the value of a given
    option for this statement.

***************************************************/

UDWORD odbcSTMT::GetStmtOption(UWORD fOption)
{
    UDWORD Ret = 0;

#if (ODBCVER >= 0x0300)
    SetRC(SQLGetStmtAttr(
        hstmt,
        fOption,
        &Ret,
        SQL_IS_UINTEGER,
        nullptr));
#else
    SetRC(SQLGetStmtOption(
        hstmt,
        fOption,
        &Ret));
#endif                

    return Ret;
}


/***************************************************

    SetStmtOption

    SetStmtOption sets the value of a given
    option for this statement.
***************************************************/

RETCODE odbcSTMT::SetStmtOption(UWORD fOption, UDWORD ulValue)
{
#if (ODBCVER >= 0x0300)
    SetRC(SQLSetStmtAttr(hstmt, fOption, &ulValue, SQL_IS_UINTEGER));
#else
    SetRC(SQLSetStmtOption(hstmt, fOption, ulValue));
#endif                

    return lastRC();
}


/***************************************************

    ParamData

    Invoke SQLParamData.
***************************************************/

RETCODE odbcSTMT::ParamData(void*        *prgbValue)
{
    SetRC(
        SQLParamData(
            hstmt,
            prgbValue
        ));

    return lastRC();
}

/***************************************************

    PutData

    Invoke SQLPutData.
***************************************************/

RETCODE odbcSTMT::PutData(
    void* rgbValue,
    SDWORD     cbValue)
{
    SetRC(
        SQLPutData(
            hstmt,
            rgbValue,
            cbValue
        ));

    return lastRC();
}

/***************************************************

    DescribeParam

    Invoke SQLDescribeParam. Overloaded to two forms.  The first
    simply passes through the call to SQLDescribeParam.  The second
    overloaded form returns result in an sPARMBIND struct.

    All members of the struct are first set to zeros.  The
    fSqlType, cbColDef, and ibScale members are set by the
    SQLDescribeParam call.  The value for nullability is put
    in the cbValue member: a non-zero value will indicate that
    the parameter is nullable.  The default C data type is
    put in the fCType member.

***************************************************/

RETCODE odbcSTMT::DescribeParam(
    UWORD      ipar,
    SWORD      *pfSqlType,
    UDWORD     *pcbColDef,
    SWORD      *pibScale,
    SWORD      *pfNullable
)
{
    SetRC(
        SQLDescribeParam(
            hstmt,
            ipar,
            pfSqlType,
            pcbColDef,
            pibScale,
            pfNullable)
    );

    return lastRC();
}

RETCODE odbcSTMT::DescribeParam(
    UWORD      ipar,
    sPARMBIND* pParmBind
)
{
    SWORD fNullable;

    memset(pParmBind, 0, sizeof(sPARMBIND));

    SetRC(
        SQLDescribeParam(
            hstmt,
            ipar,
            &pParmBind->fSqlType,
            &pParmBind->cbColDef,
            &pParmBind->ibScale,
            &fNullable)
    );

    if (sqlsuccess())
    {
        pParmBind->fCType = CTypeFromSqlType(pParmBind->fSqlType);
        pParmBind->cbValue = (SDWORD)fNullable;
    }

    return lastRC();
}

/***************************************************

    CTypeFromSqlType

    Given SQL data type, return corresponding default
    C data type.

***************************************************/
SWORD   odbcSTMT::CTypeFromSqlType(
    SWORD fSqlType
)
{
    // determine C type for SQL type
    switch (fSqlType)
    {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    case SQL_DECIMAL:
    case SQL_NUMERIC:
    case SQL_BIGINT:
        return SQL_C_CHAR;


    case SQL_BIT:
        return SQL_C_BIT;

    case SQL_TINYINT:
        return SQL_C_TINYINT;


    case SQL_SMALLINT:
        return SQL_C_SHORT;


    case SQL_INTEGER:
        return SQL_C_LONG;


    case SQL_REAL:
        return SQL_C_FLOAT;

    case SQL_FLOAT:
    case SQL_DOUBLE:
        return SQL_C_DOUBLE;


    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
        return SQL_C_BINARY;


    case SQL_DATE:
        return SQL_C_DATE;

    case SQL_TIME:
        return SQL_C_TIME;


    case SQL_TIMESTAMP:
        return SQL_C_TIMESTAMP;

    } // end switch

    return 0;
}

/***************************************************

    NumParams

    Invoke SQLNumParams.

    Overloaded to two forms: the first is a pass-through
    version, the second takes no parameters and returns
    the value.  A zero return may indicate an error; use
    the sqlsuccess() member function to check.
***************************************************/

RETCODE odbcSTMT::NumParams(
    SWORD      *pcpar
)
{
    SetRC(
        SQLNumParams(
            hstmt,
            pcpar)
    );

    return lastRC();
}


SWORD odbcSTMT::NumParams()
{
    SWORD par = 0;

    SetRC(
        SQLNumParams(
            hstmt,
            (SWORD     *)&par)
    );

    return par;
}

/***************************************************

    ParamOptions

    Invoke SQLParamOptions. Overloaded to two versions.
    The first is a pass-through call, the second uses the
    address of the internal data member iParmRow as the
    pirow argument.  If an error occurs processing an
    insert or update, call GetParmRow() to retrieve iParmRow,
    the parameter data set index where the error occurred.
***************************************************/

RETCODE odbcSTMT::ParamOptions(
    UDWORD     crow,
    UDWORD     *pirow)
{
    SetRC(
        SQLParamOptions(
            hstmt,
            iParamOptRowCount = crow,
            pirow));

    return lastRC();
}

RETCODE odbcSTMT::ParamOptions(UDWORD crow)
{
    SetRC(
        SQLParamOptions(
            hstmt,
            iParamOptRowCount = crow,
            &iParmRow));

    return lastRC();
}


/***************************************************

    DoStmt

    Execute a statement using SQLExecute or SQLExecDirect
    depending on the value of the bPrepared flag
    (set when SQLPrepare is successfully executed).
***************************************************/

bool odbcSTMT::DoStmt()
{
    if (bPrepared)
        Execute();
    else
        ExecDirect(lpszStmt);

    return sqlsuccess();
}

// new in v2.0

RETCODE odbcSTMT::BindParameter
(
    UWORD           ipar,
    SWORD           fParamType,
    SWORD       fCType,
    SWORD           fSqlType,
    UDWORD      cbColDef,
    SWORD       ibScale,
    void*         rgbValue,
    SDWORD          cbValueMax,
    SDWORD     *pcbValue
)
{
    SetRC(SQLBindParameter
    (
        hstmt,
        ipar,
        fParamType,
        fCType,
        fSqlType,
        cbColDef,
        ibScale,
        rgbValue,
        cbValueMax,
        pcbValue
    ));
    return lastRC();
}

// end new in v2.0

