#pragma once

///////////////////////////////////////////////////////////
//////////////////////// statements
///////////////////////////////////////////////////////////

class odbcEXPORTED odbcSTMT : public odbcBASE {

protected:
    /***************************************************

        pConn

        Pointer to owning connection object.
    ***************************************************/

    odbcCONNECT*        pConn;

    /***************************************************

        hstmt

        ODBC statement handle.
    ***************************************************/

    HSTMT           hstmt{};
    /***************************************************

        lpszStmt

        SQL statement string.
    ***************************************************/

    LPUCSTR                 lpszStmt;
    /***************************************************

        pParmBindings

        Array of parameter bindings represented by
        sPARMBIND structs.
    ***************************************************/

    const sPARMBIND*     pParmBindings;
    /***************************************************

        ParmCount

        count of sPARMBIND structs in array.
    ***************************************************/

    UWORD                   ParmCount;
    /***************************************************

        pParms

        Pointer to parameter data structure.
    ***************************************************/

    void*                     pParms;
    /***************************************************

        bError

        Non-zero if an error occurred (useful if error
        occurs during construction).
    ***************************************************/

    bool                    bError;
    /***************************************************

        bPrepared

        Non-zero if we have prepared the statement.
    ***************************************************/

    bool                    bPrepared;
    /***************************************************

        bParmsBound

        Non-zero if we have bound the parameters represented
        by the array of sPARMBIND structures.
    ***************************************************/

    bool                    bParmsBound;
    /***************************************************

        bExecuted

        Non-zero if we have executed the SQL statement
        via a call to Execute or ExecDirect member functions.
    ***************************************************/

    bool                    bExecuted;

    /***************************************************

        iParm

        Index of last sPARMBIND structure processed.
    ***************************************************/

    UWORD                   iParm;

    /***************************************************

        iParmRow

        Index of last parameter set processed, when multiple
        parameter sets are indicated by calling ParamOptions.
    ***************************************************/

    UDWORD                  iParmRow;

    // new in v2.0
    /***************************************************

        iParamOptRowCount

        Number of parameter rows passed in call to
        ParamOptions.
    ***************************************************/

    UDWORD                  iParamOptRowCount;
    // end new in v2.0

public:
    /***************************************************

        odbcSTMT

        Constructor.

        Arguments:

                    podbcCONNECT    pC

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

                    cpsPARMBIND     psParmBindings

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

    odbcSTMT(
        odbcCONNECT*    pC,
        LPUCSTR                 lpszSentStmt = NULL,
        const sPARMBIND*     psParmBindings = NULL,
        UWORD                   uParmCount = 0,
        void*                             pvParmStruct = NULL
    );

    odbcSTMT(
        odbcCONNECT*    pC,
        LPCSTR                  lpszSentStmt,
        const sPARMBIND*     psParmBindings = NULL,
        UWORD                   uParmCount = 0,
        void*                             pvParmStruct = NULL
    );

    /***************************************************

        ~odbcSTMT

        Destructor.
    ***************************************************/

    virtual ~odbcSTMT();

    /***************************************************

        SetParmBindings

        Set value of protected data members that define
        bindings between members of parameter structure
        and SQL statement parameters, as defined by the
        array of sPARMBIND structs..
    ***************************************************/

    virtual void            SetParmBindings
    (
        const sPARMBIND* psParmBindings = NULL,
        UWORD           uParmCount = 0,
        void*                     pvParmStruct = NULL
    )
    {
        pParmBindings = psParmBindings;
        ParmCount = uParmCount;
        pParms = pvParmStruct;
        bExecuted = bParmsBound = false;
    }

    /***************************************************

        LastParmProcessed

        Zero-based index of the last parameter processed
        by SetParams() (useful if an error occurs).
    ***************************************************/

    virtual UWORD           LastParmProcessed()
    {
        return iParm;
    }

    /***************************************************

        GetHstmt

        Get ODBC HSTMT associated with this statement.
    ***************************************************/

    virtual HSTMT GetHstmt() { return hstmt; }

    /***************************************************

        GetParmBindings

        Get parameter bindings array address, count,
        and struct address.  Note that the first and
        third arguments are addresses of pointers (pointers
        to pointers).
    ***************************************************/

    virtual void            GetParmBindings
    (
        const sPARMBIND**   ppsParmBindings,
        UWORD     *                     puParmCount,
        void*     *                       ppvParmStruct
    )
    {
        *ppsParmBindings = pParmBindings;
        *puParmCount = ParmCount;
        *ppvParmStruct = pParms;
    }

    /***************************************************

        SetStmt

        Sets the SQL command string associated with
        the statement. You may also need to call
        ResetParams() if parameter bindings have
        taken place.
    ***************************************************/

    virtual LPUCSTR                 SetStmt(LPUCSTR Stmt)
    {
        LPUCSTR Temp = lpszStmt;
        lpszStmt = Stmt;
        return Temp;
    }

    virtual LPCSTR          SetStmt(LPSTR Stmt)
    {
        LPCSTR Temp = (LPCSTR)lpszStmt;
        lpszStmt = (LPUCSTR)Stmt;
        return Temp;
    }

    /***************************************************

        GetStmt

        Get current SQL command string associated with
        the statement.
    ***************************************************/

    virtual LPUCSTR         GetStmt()
    {
        return lpszStmt;
    }


    /***************************************************

        GetParmRow

        Get index of last parameter set processed, when multiple
        parameter sets are indicated by calling ParamOptions.
    ***************************************************/

    virtual UDWORD  GetParmRow()
    {
        return iParmRow;
    }

    /***************************************************

        ErrorFlag

        Gets current value of internal error flag; useful
        if determining if an error occurred during construc-
        tion when return codes were not available.
    ***************************************************/

    virtual bool    ErrorFlag()
    {
        return bError;
    }

    /***************************************************

        Prepared

        true if SQLPrepare was called successfully
        already on this statement object.
    ***************************************************/

    virtual bool    Prepared()
    {
        return bPrepared;
    }

    /***************************************************

        ParmsBound

        Returns true if parameter bindings have been
        done on this statement.
    ***************************************************/

    virtual bool    ParmsBound()
    {
        return bParmsBound;
    }

    /***************************************************

        Executed

        Returns true if a successful call to SQLExecute
        or SQLExecDirect has occurred on this object.
    ***************************************************/

    virtual bool    Executed()
    {
        return bExecuted;
    }

    ////////////////////////////////////////////////////////
    ////////////////////// core functions //////////////////
    ////////////////////////////////////////////////////////

/***************************************************

    Cancel

    Invoke SQLCancel.
***************************************************/

    virtual RETCODE Cancel();

    /***************************************************

        Close

        Call SQLFreeStmt with SQL_CLOSE argument.
    ***************************************************/

    virtual RETCODE Close();

    /***************************************************

        ResetParams

        Remove old parameter bindings by calling SQLFreeStmt
        with SQL_RESET_PARAMS flag.
    ***************************************************/

    virtual RETCODE ResetParams();

    /***************************************************

        RowCount

        Get count of rows affected by the statement. Overloaded
        to two versions: one returns a result code, and the count
        is returned in the signed double-word pointed to by pcrow;
        the other version returns the count directly.
    ***************************************************/

    virtual RETCODE RowCount(
        SDWORD  *pcrow
    );

    virtual SDWORD RowCount();

    /***************************************************

        Prepare

        Invoke SQLPrepare; bind parameters if parameter
        bindings need to be done. Overloaded version without
        argument prepares statement stored internally.
    ***************************************************/

    virtual RETCODE Prepare(LPUCSTR szSqlStr);

    virtual RETCODE Prepare(LPSTR szSqlStr)
    {
        return Prepare((LPUCSTR)szSqlStr);
    }

    virtual RETCODE Prepare();

    /***************************************************

        ExecDirect

        Invoke SQLExecDirect.
    ***************************************************/

    virtual RETCODE ExecDirect(LPUCSTR szSqlStr);

    virtual RETCODE ExecDirect(LPSTR szSqlStr)
    {
        return ExecDirect((LPUCSTR)szSqlStr);
    }

    /***************************************************

        Execute

        Invoke SQLExecute.
    ***************************************************/

    virtual RETCODE Execute();

    /***************************************************

        SetParams

        Invoke SetParam member function passing internal array of
        parameter bindings.
    ***************************************************/

    bool        SetParams();

    /***************************************************

        SetParam

        Invoke SQLSetParam to bind a parameter.
    ***************************************************/

    virtual RETCODE SetParam(
        UWORD       ipar,
        SWORD       fCType,
        SWORD       fSqlType,
        UDWORD      cbColDef,
        SWORD       ibScale,
        void*         rgbValue,
        SDWORD      *pcbValue);

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

    virtual RETCODE SetParam(
        const sPARMBIND* psParmBindings,
        UWORD           uCount,
        void*                     pvBuf
    );

    /***************************************************

        RegisterError

            Get more information on the most recent error code
            from an ODBC operation. Results can be retrieved using
            member functions in the parent odbcBASE class.

            This function calls the base class member function Error()
            with arguments appropriate for this object type.
    ***************************************************/

    RETCODE RegisterError() override;

    ////////////////////////////////////////////////////////
    ////////////////////// level 1 functions ///////////////
    ////////////////////////////////////////////////////////

/***************************************************

    GetStmtOption

    GetStmtOption returns the value of a given
    option for this statement.

***************************************************/

    virtual UDWORD GetStmtOption(UWORD fOption);

    /***************************************************

        SetStmtOption

        SetStmtOption sets the value of a given
        option for this statement.
    ***************************************************/

    virtual RETCODE SetStmtOption(UWORD fOption, UDWORD ulValue);

    /***************************************************

        ParamData

        Invoke SQLParamData.
    ***************************************************/

    virtual RETCODE ParamData(void*        *prgbValue);

    /***************************************************

        PutData

        Invoke SQLPutData.
    ***************************************************/

    virtual RETCODE PutData(
        void* prgbValue,
        SDWORD     cbValue);

    /***************************************************

        DescribeParam

        Invoke SQLDescribeParam.
    ***************************************************/

    virtual RETCODE DescribeParam(
        UWORD      ipar,
        SWORD      *pfSqlType,
        UDWORD     *pcbColDef,
        SWORD      *pibScale,
        SWORD      *pfNullable
    );

    virtual RETCODE DescribeParam(
        UWORD      ipar,
        sPARMBIND* pParmBind
    );

    /***************************************************

        CTypeFromSqlType

        Given SQL data type, return corresponding default
        C data type.

    ***************************************************/
    virtual SWORD   CTypeFromSqlType(
        SWORD fSqlType
    );

    /***************************************************

        NumParams

        Invoke SQLNumParams.
    ***************************************************/

    virtual RETCODE NumParams(
        SWORD      *pcpar
    );

    virtual SWORD NumParams();

    /***************************************************

        ParamOptions

        Invoke SQLParamOptions.
    ***************************************************/

    virtual RETCODE ParamOptions(
        UDWORD     crow,
        UDWORD     *pirow);

    virtual RETCODE ParamOptions(UDWORD crow);

    ////////////////////////////////////////////////
    ///////////// statement executors //////////////
    ////////////////////////////////////////////////

/***************************************************

    DoStmt

    Execute a statement using SQLExecute or SQLExecDirect
    depending on the value of the bPrepared flag
    (set when SQLPrepare is successfully executed).
***************************************************/

    virtual bool DoStmt();

    /***************************************************

        pParmStruct

        Returns address of parameter structure.
    ***************************************************/

    virtual void* pParmStruct()
    {
        return pParms;
    }

    // new in v2.0

#if (ODBCVER >= 0x0200)
/***************************************************

    BindParameter

    Bind storage for parameter data.
***************************************************/

    virtual RETCODE BindParameter
    (
        UWORD     ipar,
        SWORD     fParamType,
        SWORD     fCType,
        SWORD     fSqlType,
        UDWORD    cbColDef,
        SWORD     ibScale,
        void*       rgbValue,
        SDWORD    cbValueMax,
        SDWORD *  pcbValue
    );

#endif // #if (ODBCVER >= 0x0200)

    // end new in v2.0

};
