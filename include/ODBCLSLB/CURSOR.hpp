#pragma once

///////////////////////////////////////////////////////////
//////////////////////// cursors
///////////////////////////////////////////////////////////

// iterator callback function type used in EnumResults member
// function.

typedef RETCODE(*pfENUMRESULTS)(odbcCURSOR* pCursor, void* pUser);

class odbcEXPORTED odbcCURSOR : public odbcSTMT {
    //#include <odbclslb\cursorwn.hpp>
private:

    // no private members

protected:
    /**********************************************************
        pColBindings

        Array of sCOLBIND structures representing the column
        bindings.

    **********************************************************/

    sCOLBIND*       pColBindings;

    /**********************************************************
        pcbValues

        Array of SDWORDs representing the returned size of column
        data in extende fetch operations.

    **********************************************************/

    SDWORD      *pcbValues;

    /**********************************************************
        ColCount

        Count of array elements in pColBindings.

    **********************************************************/

    UWORD           ColCount;

    /**********************************************************
        pSet

     Address of structure to whose members are bound to columns using the
    information in the pColBindings array.

    **********************************************************/

    void*             pSet_;

    /**********************************************************
        bFirst

        Flag indicating we are ready to perform a GetFirst
        fetch after performing a QueryAndBind call.

    **********************************************************/

    bool            bFirst;

    /**********************************************************
        bColsBound

        Non-zero (true) if columns have been bound.

    **********************************************************/

    bool            bColsBound;

    /**********************************************************
        bAutoBound

        Non-zero (true) if columns have been bound.

    **********************************************************/

    bool            bAutoBound;

    /**********************************************************
        AutoBindCols

        Non-zero (true) if columns should be auto-bound.

    **********************************************************/

    bool            AutoBindCols;

    /***************************************************

        pSSize

        Size of automatically allocated structure
        for storing bound column output.
    ***************************************************/

    UWORD pSSize;

    /**********************************************************
        iCol

        Last column processed in column bindings; useful in
        pinpointing errors when errors occur while binding
        multiple parameters.

    **********************************************************/

    UWORD           iCol_;

    /**********************************************************
        bNoExtraByteOnStrings

        If true, no extra byte should be allocated when dynamically
        binding columns in AutoBind().

    **********************************************************/

    bool            bNoExtraByteOnStrings_;

    /***************************************************

        ExtFetchRowCount

        Row count for use in supporting automated extended
        fetch functions.
    ***************************************************/

    UWORD   ExtFetchRowCount;

    /***************************************************

        ExtFetchRow

        Row variable for supporting automated extended
        fetch functions.
    ***************************************************/

    UDWORD  ExtFetchRow;

    /***************************************************

        ExtFetchRelAbsRow

        Relative/absolute row sent in automated extended
        fetch functions.
    ***************************************************/

    SDWORD  ExtFetchRelAbsRow;

    /***************************************************

        ExtFetchType

        Extended fetch op type for supporting automated extended
        fetch functions.
    ***************************************************/

    UWORD   ExtFetchType;

    /***************************************************

        ExtFetchStatus

        Row status array pointer for supporting automated
        extended fetch functions.
    ***************************************************/

    UWORD       *ExtFetchStatus;

public:

    /**********************************************************
        bTrimAllTrailingBlanks

        Boolean value; 0 if trailing blanks should not be auto-
        matically trimmed from SQL_CHAR columns in result set,
        1 if the trailing blanks should be trimmed. Inherited
        from the connection.
    **********************************************************/

    bool           bTrimAllTrailingBlanks;

    /**********************************************************
        odbcCURSOR

        Constructor. See constructor for odbcSTMT for description
        of arguments passed to odbcSTMT().  Cursor-specific
        arguments:

            bool bAutoBind

                After execution of statement (during constructor
                or otherwise), call AutoBind to bind columns to
                members of automatically allocated structure.

            cpsCOLBIND  pColBindings

                If non-NULL, address of array of structures
                defining the binding of columns in the
                SQL statement to members of the structure
                to contain data values after successful
                call to fetch(). Default value is NULL.

            UWORD           ColCount

                Count of array elements in psColBindings.
                Default value is 0.

            void*             pvColStruct

                Address of structure containing slots for
                values. Default value is NULL.


    **********************************************************/


    odbcCURSOR
    (
        odbcCONNECT*    pC,
        LPUSTR          szStmt = NULL,
        bool            bAutoBind = false,
        sPARMBIND*      psParmBindings = NULL,
        UWORD           uParmCount = 0,
        void*             pvParmStruct = NULL,
        sCOLBIND*       psColBindings = NULL,
        UWORD           ColCount = 0,
        void*             pvColStruct = NULL
    );

    odbcCURSOR
    (
        odbcCONNECT*    pC,
        LPSTR           szStmt,
        bool            bAutoBind = false,
        sPARMBIND*      psParmBindings = NULL,
        UWORD           uParmCount = 0,
        void*             pvParmStruct = NULL,
        sCOLBIND*       psColBindings = NULL,
        UWORD           ColCount = 0,
        void*             pvColStruct = NULL
    );

    /***************************************************

        ~odbcCURSOR

        Destructor.
    ***************************************************/

    virtual ~odbcCURSOR();

    /***************************************************

        FreeColBindings

        Free dynamically allocated column bindings and result set..
    ***************************************************/

    virtual void FreeColBindings();

    /***************************************************

        SetColBindings

        Set value of protected data members that define
        bindings between members of data structure
        and SQL statement result set columns, as defined by the
        array of sCOLBIND structs.
    ***************************************************/

    virtual void        SetColBindings
    (
        sCOLBIND*   psBindings = NULL,
        UWORD       uCount = 0,
        void*         pvStruct = NULL
    )
    {
        FreeColBindings();
        pColBindings = psBindings;
        ColCount = uCount;
        pSet_ = pvStruct;
    }

    /***************************************************

        GetColBindings

        Retrieves values of protected data members that define
        bindings between members of data structure and SQL
        statement result set columns, as defined by the array
        of sCOLBIND structs..
    ***************************************************/

    virtual void        GetColBindings
    (
        sCOLBIND**  ppsBindings,
        UWORD           *puCount,
        void*         *ppvStruct
    )
    {
        *ppsBindings = pColBindings;
        *puCount = ColCount;
        *ppvStruct = pSet_;
    }

    /***************************************************

        SetNoExtraByteOnStrings

        Set value of bNoExtraByteOnStrings.
    ***************************************************/

    virtual bool        SetNoExtraByteOnStrings
    (
        bool        bNewValue
    )
    {
        bool bOldValue = bNoExtraByteOnStrings_;

        bNoExtraByteOnStrings_ = bNewValue;

        return bOldValue;
    }

    /***************************************************

        LastColProcessed

        Zero-based index of the last sCOLBIND processed
        by BindCol() (useful if an error occurs).
    ***************************************************/

    virtual UWORD       LastColProcessed()
    {
        return iCol_;
    }

    /***************************************************

        SetStmt

        Sets the SQL command string associated with
        the statement. You may also need to call
        ResetParams() if parameter bindings have
        taken place.
    ***************************************************/

    LPUCSTR         SetStmt(LPUCSTR Stmt) override
    {
        // inherit base class behavior
        LPUCSTR Temp = odbcSTMT::SetStmt(Stmt);
        // now set our own flag(s)
        bFirst = false;
        return Temp;
    }

    LPCSTR      SetStmt(LPSTR Stmt) override
    {
        // inherit base class behavior
        LPCSTR Temp = (LPCSTR)odbcSTMT::SetStmt((LPUSTR)Stmt);
        // now set our own flag(s)
        bFirst = false;
        return Temp;
    }

    ////////////////////////////////////////////////////////
    ////////////////////// core functions //////////////////
    ////////////////////////////////////////////////////////

/***************************************************

    BindCol

    Bind a single column in the result set by invoking
    SQLBindCol.
***************************************************/

    virtual RETCODE BindCol(
        UWORD       icol,
        SWORD       fCType,
        void*         rgbValue,
        SDWORD      cbValueMax,
        SDWORD     *pcbValue);

    /***************************************************

        BindCol

        Bind multiple columns as represented by the
        elements of the array of sCOLBIND structures.
    ***************************************************/

    virtual RETCODE BindCol(
        sCOLBIND*   psResultSetBindings,
        UWORD       uCount,
        void*         pvBuf
    );

    /***************************************************

        BindCol

        Bind multiple columns as represented by the
        elements of the array of sCOLBIND structures
        supplied during construction.
    ***************************************************/

    virtual RETCODE BindCol();

    /***************************************************

        DescribeCol

        Invoke SQLDescribeCol to retrieve attributes
        for a given column in the result set.
    ***************************************************/

    virtual RETCODE DescribeCol(
        UWORD       icol,
        LPUSTR      szColName,
        SWORD       cbColNameMax,
        SWORD      *pcbColName,
        SWORD      *pfSqlType,
        UDWORD     *pcbColDef,
        SWORD      *pibScale,
        SWORD      *pfNullable);

    virtual RETCODE DescribeCol(
        UWORD       icol,
        LPSTR       szColName,
        SWORD       cbColNameMax,
        SWORD      *pcbColName,
        SWORD      *pfSqlType,
        UDWORD     *pcbColDef,
        SWORD      *pibScale,
        SWORD      *pfNullable)
    {
        return
            DescribeCol(
                icol,
                (LPUSTR)szColName,
                cbColNameMax,
                pcbColName,
                pfSqlType,
                pcbColDef,
                pibScale,
                pfNullable);
    }

    /***************************************************

        ColAttributes

        Invoke SQLColAttributes to retrieve attributes
        for a given column in the result set.
    ***************************************************/

    virtual RETCODE ColAttributes(
        UWORD        icol,
        UWORD        fDescType,
        void*          rgbDesc,
        SWORD        cbDescMax,
        SWORD       *pcbDesc,
        SDWORD      *pfDesc);

    /***************************************************

        Fetch

        Invoke SQLFetch.
    ***************************************************/

    virtual RETCODE Fetch();

    /***************************************************

        GetCursorName

        Get the cursor name.
    ***************************************************/

    virtual RETCODE GetCursorName(
        LPUSTR      szCursor,
        SWORD       cbCursorMax,
        SWORD      *pcbCursor = NULL
    );

    virtual RETCODE GetCursorName(
        LPSTR       szCursor,
        SWORD       cbCursorMax,
        SWORD      *pcbCursor = NULL
    )
    {
        return
            GetCursorName
            (
            (LPUSTR)szCursor,
                cbCursorMax,
                pcbCursor
            );
    }

    /***************************************************

        SetCursorName

        Set the cursor name.
    ***************************************************/

    virtual RETCODE SetCursorName(
        LPUSTR szCursor
    );

    virtual RETCODE SetCursorName(
        LPSTR szCursor
    )
    {
        return
            SetCursorName(
            (LPUSTR)szCursor
            );
    }

    /***************************************************

        NumResultCols

        Returns number of columns in the result set. This
        overloaded version returns the result code, and
        the number of columns in the signed word pointed
        to by pccol.
    ***************************************************/

    virtual RETCODE NumResultCols(
        SWORD      *pccol
    );

    /***************************************************

        NumResultCols

        Returns number of columns in the result set. This
        overloaded version returns the number directly
    ***************************************************/

    virtual SWORD NumResultCols();

    /***************************************************

        ExecDirect

        Invoke SQLExecDirect.
    ***************************************************/

    RETCODE ExecDirect(LPUCSTR szSqlStr) override;

    RETCODE ExecDirect(LPSTR szSqlStr) override
    {
        return ExecDirect((LPUCSTR)szSqlStr);
    }

    /***************************************************

        Unbind

        Invoke SQLFreeStmt with SQL_UNBIND flag. Removes
        column bindings for result set.
    ***************************************************/

    virtual RETCODE Unbind();

    /***************************************************

        Execute

        Invoke SQLExecute.
    ***************************************************/

    RETCODE Execute() override;

    /***************************************************

        Prepare

        Invoke SQLPrepare; bind parameters if parameter
        bindings need to be done. Overloaded version without
        argument prepares statement stored internally.
    ***************************************************/

    RETCODE Prepare(LPUCSTR szSqlStr) override;

    RETCODE Prepare(LPSTR szSqlStr) override
    {
        return Prepare((LPUCSTR)szSqlStr);
    }

    RETCODE Prepare() override;

    ////////////////////////////////////////////////////////
    ////////////////////// level 1 functions ///////////////
    ////////////////////////////////////////////////////////

/***************************************************

    GetData

    Invoke SQLGetData.
***************************************************/

    virtual RETCODE GetData(
        UWORD      icol,
        SWORD      fCType,
        void*        rgbValue,
        SDWORD     cbValueMax,
        SDWORD     *pcbValue);

    /***************************************************

        Columns

        Invoke SQLColumns to obtain information on the
        columns in the tables in this data source. Results
        are returned as a result set.
    ***************************************************/

    virtual RETCODE Columns(
        LPUSTR szTableQualifier,
        LPUSTR szTableOwner,
        LPUSTR szTableName,
        LPUSTR szColumnName);

    virtual RETCODE Columns(
        LPSTR szTableQualifier,
        LPSTR szTableOwner,
        LPSTR szTableName,
        LPSTR szColumnName)
    {
        return
            Columns(
            (LPUSTR)szTableQualifier,
                (LPUSTR)szTableOwner,
                (LPUSTR)szTableName,
                (LPUSTR)szColumnName
            );
    }

    /***************************************************

        ColumnPrivileges

        Invoke SQLColumnPrivileges to obtain information
        on the columns in the tables in this data source.
        Results are returned as a result set.
    ***************************************************/

    virtual RETCODE ColumnPrivileges(
        LPUSTR szTableQualifier,
        LPUSTR szTableOwner,
        LPUSTR szTableName,
        LPUSTR szColumnName);

    virtual RETCODE ColumnPrivileges(
        LPSTR szTableQualifier,
        LPSTR szTableOwner,
        LPSTR szTableName,
        LPSTR szColumnName)
    {
        return
            ColumnPrivileges(
            (LPUSTR)szTableQualifier,
                (LPUSTR)szTableOwner,
                (LPUSTR)szTableName,
                (LPUSTR)szColumnName
            );
    }

    /***************************************************

        Procedures

        Invoke SQLProcedures to obtain information
        on the available procedures in this data source.
        Results are returned as a result set.

    ***************************************************/

    virtual RETCODE Procedures(
        LPUSTR szProcQualifier,
        LPUSTR szProcOwner,
        LPUSTR szProcName);

    virtual RETCODE Procedures(
        LPSTR szProcQualifier,
        LPSTR szProcOwner,
        LPSTR szProcName)
    {
        return
            Procedures(
            (LPUSTR)szProcQualifier,
                (LPUSTR)szProcOwner,
                (LPUSTR)szProcName
            );
    }

    /***************************************************

        ProcedureColumns

        Invoke SQLProcedureColumns to obtain information
        on the parameters and the result set columns for the
        available procedures in this data source. Results
        are returned as a result set.

    ***************************************************/

    virtual RETCODE ProcedureColumns(
        LPUSTR szProcQualifier,
        LPUSTR szProcOwner,
        LPUSTR szProcName,
        LPUSTR szColumnName);

    virtual RETCODE ProcedureColumns(
        LPSTR szProcQualifier,
        LPSTR szProcOwner,
        LPSTR szProcName,
        LPSTR szColumnName)
    {
        return
            ProcedureColumns(
            (LPUSTR)szProcQualifier,
                (LPUSTR)szProcOwner,
                (LPUSTR)szProcName,
                (LPUSTR)szColumnName
            );
    }

    /***************************************************

        Statistics

        Return statistics on the tables in this data source
        as a result set. Invokes SQLStatistics.
    ***************************************************/

    virtual RETCODE Statistics(
        LPUSTR szTableQualifier,
        LPUSTR szTableOwner,
        LPUSTR szTableName,
        UWORD      fUnique,
        UWORD      fAccuracy);

    virtual RETCODE Statistics(
        LPSTR szTableQualifier,
        LPSTR szTableOwner,
        LPSTR szTableName,
        UWORD      fUnique,
        UWORD      fAccuracy)
    {
        return
            Statistics(
            (LPUSTR)szTableQualifier,
                (LPUSTR)szTableOwner,
                (LPUSTR)szTableName,
                fUnique,
                fAccuracy
            );
    }

    /***************************************************

        GetTypeInfo

        Get information concerning supported types.
    ***************************************************/

    virtual RETCODE GetTypeInfo(SWORD fSqlType);

    /***************************************************

        Tables

        Retrieve result set containing information about
        the tables in the data source.
    ***************************************************/

    virtual RETCODE Tables(
        LPUSTR szTableQualifier,
        LPUSTR szTableOwner,
        LPUSTR szTableName,
        LPUSTR szTableType);

    virtual RETCODE Tables(
        LPSTR szTableQualifier,
        LPSTR szTableOwner,
        LPSTR szTableName,
        LPSTR szTableType)
    {
        return
            Tables(
            (LPUSTR)szTableQualifier,
                (LPUSTR)szTableOwner,
                (LPUSTR)szTableName,
                (LPUSTR)szTableType
            );
    }

    /***************************************************

        ForeignKeys

        Retrieve result set containing information about
        foreign keys relating the tables in the data source.
    ***************************************************/

    virtual RETCODE ForeignKeys(
        LPUSTR szPkTableQualifier,
        LPUSTR szPkTableOwner,
        LPUSTR szPkTableName,
        LPUSTR szFkTableQualifier,
        LPUSTR szFkTableOwner,
        LPUSTR szFkTableName);

    virtual RETCODE ForeignKeys(
        LPSTR szPkTableQualifier,
        LPSTR szPkTableOwner,
        LPSTR szPkTableName,
        LPSTR szFkTableQualifier,
        LPSTR szFkTableOwner,
        LPSTR szFkTableName)
    {
        return ForeignKeys(
            (LPUSTR)szPkTableQualifier,
            (LPUSTR)szPkTableOwner,
            (LPUSTR)szPkTableName,
            (LPUSTR)szFkTableQualifier,
            (LPUSTR)szFkTableOwner,
            (LPUSTR)szFkTableName);
    }

    /***************************************************

        TablePrivileges

        Retrieve result set containing information about
        the tables in the data source.
    ***************************************************/

    virtual RETCODE TablePrivileges(
        LPUSTR szTableQualifier,
        LPUSTR szTableOwner,
        LPUSTR szTableName);

    virtual RETCODE TablePrivileges(
        LPSTR szTableQualifier,
        LPSTR szTableOwner,
        LPSTR szTableName)
    {
        return
            TablePrivileges(
            (LPUSTR)szTableQualifier,
                (LPUSTR)szTableOwner,
                (LPUSTR)szTableName
            );
    }

    /***************************************************

        PrimaryKeys

        Retrieve result set containing information about
        the primary keys of the tables in the data source.
    ***************************************************/

    virtual RETCODE PrimaryKeys(
        LPUSTR szTableQualifier,
        LPUSTR szTableOwner,
        LPUSTR szTableName);

    virtual RETCODE PrimaryKeys(
        LPSTR szTableQualifier,
        LPSTR szTableOwner,
        LPSTR szTableName)
    {
        return
            PrimaryKeys(
            (LPUSTR)szTableQualifier,
                (LPUSTR)szTableOwner,
                (LPUSTR)szTableName
            );
    }

    /***************************************************

        SpecialColumns

        Retrieve result set containing information about
        special columns, such as the set of column(s) that
        (taken together and in the order presented) form(s) an
        unique identifier for a row in the table.
    ***************************************************/

    virtual RETCODE SpecialColumns
    (
        UWORD   fColType,
        LPUSTR  szTableQualifier,
        LPUSTR  szTableOwner,
        LPUSTR  szTableName,
        UWORD   fScope,
        UWORD   fNullable
    );

    virtual RETCODE SpecialColumns
    (
        UWORD   fColType,
        LPSTR   szTableQualifier,
        LPSTR   szTableOwner,
        LPSTR   szTableName,
        UWORD   fScope,
        UWORD   fNullable
    )
    {
        return
            SpecialColumns
            (
                fColType,
                (LPUSTR)szTableQualifier,
                (LPUSTR)szTableOwner,
                (LPUSTR)szTableName,
                fScope,
                fNullable
            );
    }

    ////////////////////////////////////////////////////////
    ////////////////////// level 2 functions ///////////////
    ////////////////////////////////////////////////////////

/***************************************************

    SetPos

    Set cursor position in a returned result set block.
***************************************************/

    virtual RETCODE SetPos(
        UWORD      irow,
        UWORD      fRefresh,
        UWORD      fLock
    );

    /***************************************************

        SetScrollOptions

        Set concurrency and other factors for use in ExtendedFetch.
    ***************************************************/

    virtual RETCODE SetScrollOptions(
        UWORD      fConcurrency,
        SDWORD     crowKeyset,
        UWORD      crowRowset
    );

    ////////////////////////////////////////////////
    ///////////// statement executors //////////////
    ////////////////////////////////////////////////

/***************************************************

    DoQuery

    Call DoStmt() to execute the SQL statement.
***************************************************/

    virtual bool DoQuery()
    {
        return DoStmt();
    }

    /***************************************************

        pResultSet

        Return address of result set structure.
    ***************************************************/

    virtual void* pResultSet()
    {
        return pSet_;
    }

    /***************************************************

        QueryAndBind

        Execute the query and bind result columns.
    ***************************************************/

    virtual bool QueryAndBind();

    /***************************************************

        GetFirst

        Call Fetch() to get first row in the result set
        (or ExtFetchFirst() to get first rowset in result set).
        Returns non-zero value if it succeeds, zero if
        it fails.
    ***************************************************/

    virtual bool GetFirst();

    /***************************************************

        GetNext

        Call Fetch() to get next row in the result set
        (or ExtFetchNext() to get next rowset in result set).
        Returns non-zero value if it succeeds, zero if
        it fails.
    ***************************************************/

    virtual bool GetNext();

    /***************************************************

        GetLast

        Call Fetch() to get last rowset in the result set.
        Only valid if extended fetch operations are in use.
        Returns non-zero value if it succeeds, zero if
        it fails.
    ***************************************************/

    virtual bool GetLast();

    /***************************************************

        GetPrev

        Call ExtFetchPrev() to get prior rowset in the result set.
        Only valid if extended fetch operations are in use.
        Returns non-zero value if it succeeds, zero if
        it fails.
    ***************************************************/

    virtual bool GetPrev();

    /***************************************************

        EnumResults

        Using Fetch(), retrieve rows in the result set;
        for each row retrieved, invoke the callback
        unction supplied by the caller, passing the 'this'
        pointer and the user-supplied pointer 'pUser'.

        The callback function must conform to the typedef
        pfENUMRESULTS. A sample would be

        RETCODE     MyEnumFunc( podbcCURSOR thisObj,
                                void*         pMyData
                              )
            {
            // you could assign and cast thisObj to a descendant
            // of odbcCURSOR here.
            // you could also assign and cast pMyData to a pointer
            // to a data structure of your own design.

            return SQL_SUCCESS; // == zero
            }

            return a non-zero value to terminate enumeration
            of the result set; zero return continues enumeration.

    ***************************************************/

    virtual RETCODE EnumResults(pfENUMRESULTS pfEnum, void* pUser = NULL);

    /***************************************************

        ExtendedFetch

        Invoke SQLExtendedFetch.
    ***************************************************/

    virtual RETCODE ExtendedFetch(
        UWORD      fFetchType,
        SDWORD     irow,
        UDWORD     *pcrow,
        UWORD      *rgfRowStatus);

    /***************************************************

        ExtFetchSetup

        Set up for automatic features surrounding
        SQLExtendedFetch.  Calls SetScrollOptions and
        allocates space for the row statuses.
    ***************************************************/

    virtual RETCODE ExtFetchSetup(
        UWORD      fConcurrency,
        SDWORD     crowKeyset,
        UWORD      crowRowset
    );

    /***************************************************

        ExtFetchFirst

        Invoke SQLExtendedFetch to fetch first batch.

        Returns the number of rows actually fetched.  If this
        number is not equal to ExtFetchRowCount, an error may
        have occurred; use the sqlsuccess() member function
        (and other odbcBASE members) to analyze the situation.
    ***************************************************/

    virtual UDWORD ExtFetchFirst();

    /***************************************************

        ExtFetchNext

        Invoke SQLExtendedFetch to fetch next batch.

        Returns the number of rows actually fetched.  If this
        number is not equal to ExtFetchRowCount, an error may
        have occurred; use the sqlsuccess() member function
        (and other odbcBASE members) to analyze the situation.
    ***************************************************/

    virtual UDWORD ExtFetchNext();

    /***************************************************

        ExtFetchLast

        Invoke SQLExtendedFetch to fetch last batch.

        Returns the number of rows actually fetched.  If this
        number is not equal to ExtFetchRowCount, an error may
        have occurred; use the sqlsuccess() member function
        (and other odbcBASE members) to analyze the situation.
    ***************************************************/

    virtual UDWORD ExtFetchLast();

    /***************************************************

        ExtFetchPrevious

        Invoke SQLExtendedFetch to fetch previous batch.

        Returns the number of rows actually fetched.  If this
        number is not equal to ExtFetchRowCount, an error may
        have occurred; use the sqlsuccess() member function
        (and other odbcBASE members) to analyze the situation.
    ***************************************************/

    virtual UDWORD ExtFetchPrevious();

    /***************************************************

        ExtFetchRelative

        Invoke SQLExtendedFetch to fetch batch at relative
        position (n rows forward/backward).

        Returns the number of rows actually fetched.  If this
        number is not equal to ExtFetchRowCount, an error may
        have occurred; use the sqlsuccess() member function
        (and other odbcBASE members) to analyze the situation.
    ***************************************************/

    virtual UDWORD ExtFetchRelative(
        SDWORD     irow
    );

    /***************************************************

        ExtFetchAbsolute

        Invoke SQLExtendedFetch to fetch batch at absolute
        position (beginning at nth row).

        Returns the number of rows actually fetched.  If this
        number is not equal to ExtFetchRowCount, an error may
        have occurred; use the sqlsuccess() member function
        (and other odbcBASE members) to analyze the situation.
    ***************************************************/

    virtual UDWORD ExtFetchAbsolute(
        SDWORD     irow
    );

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

    //virtual UDWORD ExtFetchResume();

/***************************************************

    GetExtFetchType

    Get op type for supporting automated extended
    fetch functions.
***************************************************/

    virtual UWORD   GetExtFetchType()
    {
        return ExtFetchType;
    }

    /***************************************************

        GetExtFetchRow

        Get row variable for supporting automated extended
        fetch functions.
    ***************************************************/

    virtual UDWORD  GetExtFetchRow()
    {
        return ExtFetchRow;
    }

    /***************************************************

        GetExtFetchRelAbsRow

        Get relative/absolute row sent in automated extended
        fetch functions.
    ***************************************************/

    virtual SDWORD  GetExtFetchRelAbsRow()
    {
        return ExtFetchRelAbsRow;
    }

    /***************************************************

        GetExtFetchRowCount

        Get row count variable for supporting automated extended
        fetch functions.
    ***************************************************/

    virtual UWORD   GetExtFetchRowCount()
    {
        return ExtFetchRowCount;
    }

    /***************************************************

        GetExtFetchStatus

        Get row status from automated extended fetch functions.

        First overloaded version returns the status for a given
        row in the rowset.  iRow is 1-based, not zero-based.

        Second overloaded version returns const pointer to array
        of row statuses.  There are ExtFetchRowCount members
    ***************************************************/

    virtual UWORD   GetExtFetchStatus(UWORD iRow)
    {
        if (ExtFetchStatus != 0 &&
            iRow > 0 &&
            iRow <= ExtFetchRowCount
            )
            return ExtFetchStatus[iRow - 1];
        else
            return 0;
    }

    virtual const UWORD     *GetExtFetchStatus()
    {
        return ExtFetchStatus;
    }


    /***************************************************

        MoreResults

        Invoke SQLMoreResults().
    ***************************************************/

    virtual RETCODE MoreResults();

    /***************************************************

        AutoBind

        Retrieve array of column bindings.  Allocate a
        buffer to contain the output data and bind all
        the columns to it.  Accepts two args:
        1) address of unsigned double-word for output structure
           size, and
        2) address of a void* to store pointer to allocated
           buffer.
    ***************************************************/

    virtual RETCODE AutoBind
    (
        UWORD     *pcbStructSize = NULL,
        void*     *ppStruct = NULL
    );

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

    virtual void* ColResultAddr(LPUCSTR ColName);

    virtual void* ColResultAddr(LPCSTR ColName);

    virtual void* ColResultAddr(UWORD icol);

    virtual void* ColResultAddr(LPUCSTR ColName, UWORD irow);

    virtual void* ColResultAddr(LPCSTR ColName, UWORD irow);

    virtual void* ColResultAddr(UWORD icol, UWORD irow);

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

    virtual SWORD  ColResultAsShort(LPCSTR ColName, UWORD irow);
    virtual SWORD  ColResultAsShort(LPUCSTR ColName, UWORD irow);
    virtual SWORD  ColResultAsShort(UWORD icol, UWORD irow);
    virtual UWORD  ColResultAsUnsignedShort(LPCSTR ColName, UWORD irow);
    virtual UWORD  ColResultAsUnsignedShort(LPUCSTR ColName, UWORD irow);
    virtual UWORD  ColResultAsUnsignedShort(UWORD icol, UWORD irow);
    virtual SDWORD ColResultAsLong(LPCSTR ColName, UWORD irow);
    virtual SDWORD ColResultAsLong(LPUCSTR ColName, UWORD irow);
    virtual SDWORD ColResultAsLong(UWORD icol, UWORD irow);
    virtual UDWORD ColResultAsUnsignedLong(LPCSTR ColName, UWORD irow);
    virtual UDWORD ColResultAsUnsignedLong(LPUCSTR ColName, UWORD irow);
    virtual UDWORD ColResultAsUnsignedLong(UWORD icol, UWORD irow);
    virtual float  ColResultAsFloat(LPCSTR ColName, UWORD irow);
    virtual float  ColResultAsFloat(LPUCSTR ColName, UWORD irow);
    virtual float  ColResultAsFloat(UWORD icol, UWORD irow);
    virtual double ColResultAsDouble(LPCSTR ColName, UWORD irow);
    virtual double ColResultAsDouble(LPUCSTR ColName, UWORD irow);
    virtual double ColResultAsDouble(UWORD icol, UWORD irow);
    virtual LPSTR  ColResultAsLPSTR(LPCSTR ColName, UWORD irow);
    virtual LPSTR  ColResultAsLPSTR(LPUCSTR ColName, UWORD irow);
    virtual LPSTR  ColResultAsLPSTR(UWORD icol, UWORD irow);

    virtual SWORD  ColResultAsShort(LPCSTR ColName);
    virtual SWORD  ColResultAsShort(LPUCSTR ColName);
    virtual SWORD  ColResultAsShort(UWORD icol);
    virtual UWORD  ColResultAsUnsignedShort(LPCSTR ColName);
    virtual UWORD  ColResultAsUnsignedShort(LPUCSTR ColName);
    virtual UWORD  ColResultAsUnsignedShort(UWORD icol);
    virtual SDWORD ColResultAsLong(LPCSTR ColName);
    virtual SDWORD ColResultAsLong(LPUCSTR ColName);
    virtual SDWORD ColResultAsLong(UWORD icol);
    virtual UDWORD ColResultAsUnsignedLong(LPCSTR ColName);
    virtual UDWORD ColResultAsUnsignedLong(LPUCSTR ColName);
    virtual UDWORD ColResultAsUnsignedLong(UWORD icol);
    virtual float  ColResultAsFloat(LPCSTR ColName);
    virtual float  ColResultAsFloat(LPUCSTR ColName);
    virtual float  ColResultAsFloat(UWORD icol);
    virtual double ColResultAsDouble(LPCSTR ColName);
    virtual double ColResultAsDouble(LPUCSTR ColName);
    virtual double ColResultAsDouble(UWORD icol);
    virtual LPSTR  ColResultAsLPSTR(LPCSTR ColName);
    virtual LPSTR  ColResultAsLPSTR(LPUCSTR ColName);
    virtual LPSTR  ColResultAsLPSTR(UWORD icol);
    /***************************************************

        ColResultSizeArray

        Return address of column result size array for a
        bound column when extended fetch block cursor in use
        and either form of automatic binding is in use.
        Two overloaded forms: The first accepts the name of the
        column and the second accepts the column number (they
        start with 1).
    ***************************************************/

    virtual const SDWORD     * ColResultSizeArray(LPUCSTR ColName);
    virtual const SDWORD     * ColResultSizeArray(LPCSTR ColName)
    {
        return ColResultSizeArray((LPUCSTR)ColName);
    }
    virtual const SDWORD     * ColResultSizeArray(UWORD icol);
    /***************************************************

        ColResultInfo

        Return address of data structure describing bound column.
        Two overloaded forms: The first accepts the name of the
        column and the second accepts the column number (they
        start with 1).
    ***************************************************/

    virtual sCOLBIND* ColResultInfo(LPUCSTR ColName);

    virtual sCOLBIND* ColResultInfo(LPCSTR ColName);

    virtual sCOLBIND* ColResultInfo(UWORD icol);

    /***************************************************

        StructSize

        Return size of automatically allocated structure
        for storing bound column output.
    ***************************************************/

    virtual UDWORD StructSize() { return pSSize; }

    // new in v2.0

    /**************************************************

        RowsetRowRecordSize

        Return size of one row's worth of data in the
        automatically allocated structure for storing
        bound column output during extended fetches,
        when the internal data structure consists of
        arrays of column data.

        Used to determine space allocation for a single
        rowset row structure (record structure) for use
        in call to MoveRowsetRowToRecord().
    ***************************************************/

    virtual size_t RowsetRowRecordSize();

    /***************************************************

        MoveRowsetRowToRecord

        Return one row's worth of data in the
        automatically allocated structure for storing
        bound column output during extended fetches,
        when the internal data structure consists of
        arrays of column data.

        Use RowsetRowRecordSize() to determine space allocation
        for a single rowset row structure (record structure).
    ***************************************************/

    virtual RETCODE MoveRowsetRowToRecord(UWORD irow,
        void* pRecord,
        size_t uSize,
        size_t     *puBytesCopied);

    /***************************************************

        MoveRecordToRowsetRow

        Return one row's worth of data from a single-row
        record structure to a rowset row in the internal
        rowset data structure dynamically allocated
        during AutoBind(). The rowset's single-row
        record structure is the same as that returned by
        MoveRowsetRowToRecord.

    ***************************************************/

    virtual RETCODE MoveRecordToRowsetRow(UWORD irow,
        void* pRecord,
        size_t uSize,
        size_t     *puBytesCopied);

    /***************************************************

        GetNoExtraByteOnStrings

        Get value of bNoExtraByteOnStrings.
    ***************************************************/

    virtual bool        GetNoExtraByteOnStrings
    (
        void
    )
    {
        return bNoExtraByteOnStrings_;
    }

    /***************************************************

        SetParam

        Invoke SQLSetParam to bind a parameter.
    ***************************************************/

    RETCODE SetParam(
        UWORD       ipar,
        SWORD       fCType,
        SWORD       fSqlType,
        UDWORD      cbColDef,
        SWORD       ibScale,
        void*         rgbValue,
        SDWORD *    pcbValue) override
    {
        return odbcSTMT::SetParam(
            ipar,
            fCType,
            fSqlType,
            cbColDef,
            ibScale,
            rgbValue,
            pcbValue
        );
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

        Unlike its equivalent in odbcSTMT, this one will
        handle BLOBs (the odbcBLOB class constructor requires
        an odbcCURSOR instance, since it can handle reading
        as well as writing BLOB data).

        One disadvantage is that this function binds
        only a single value to each parameter.
    ***************************************************/

    RETCODE SetParam(
        const sPARMBIND* psParmBindings,
        UWORD       uCount,
        void*         pvBuf
    ) override;

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

    virtual odbcBLOB*  ColResultAsBLOB(LPCSTR ColName, UWORD irow);
    virtual odbcBLOB*  ColResultAsBLOB(LPUCSTR ColName, UWORD irow);
    virtual odbcBLOB*  ColResultAsBLOB(UWORD icol, UWORD irow);
    virtual odbcBLOB*  ColResultAsBLOB(LPCSTR ColName);
    virtual odbcBLOB*  ColResultAsBLOB(LPUCSTR ColName);
    virtual odbcBLOB*  ColResultAsBLOB(UWORD icol);

private:

    friend class odbcBLOB; // to get at SetRC()

protected:
    /***************************************************

        bBlobsBound

        Flag indicating that BLOBs are bound, so we can
        know to trigger auto-input behavior in Execute()
        and ExecDirect().
    ***************************************************/
    bool bBlobsBound;

    virtual bool IsInserter() { return false; }

public:
    /***************************************************

        ResetParams

        Remove old parameter bindings by calling SQLFreeStmt
        with SQL_RESET_PARAMS flag.
    ***************************************************/

    RETCODE ResetParams() override
    {
        odbcSTMT::ResetParams();
        bBlobsBound = false;
        return lastRC();
    }

    /************************************************************
        SQLSetPos variations for inserting, updating, and deleting
        directly from the rowset. Also for refreshing the rowset,
        setting position, setting lock status, etc.
    ************************************************************/

    virtual RETCODE PositionToRow(UWORD irow)
    {
        return SetPos(irow, SQL_POSITION, SQL_LOCK_NO_CHANGE);
    }

    virtual RETCODE LockRow(UWORD irow, UWORD fLock)
    {
        return SetPos(irow, SQL_POSITION, fLock);
    }

    virtual RETCODE RefreshRow(UWORD irow, UWORD fLock)
    {
        return SetPos(irow, SQL_REFRESH, fLock);
    }

    virtual RETCODE AddRow(UWORD irow)
    {
        return SetPos(irow, SQL_ADD, SQL_LOCK_NO_CHANGE);
    }

    virtual RETCODE UpdateRow(UWORD irow)
    {
        return SetPos(irow, SQL_UPDATE, SQL_LOCK_NO_CHANGE);
    }

    virtual RETCODE DeleteRow(UWORD irow)
    {
        return SetPos(irow, SQL_DELETE, SQL_LOCK_NO_CHANGE);
    }

    // end new in v2.0

    // new in v2.1

    /*************************************************************

        ClearRowsetBuffer

        Clears contents of rowset buffer.

    *************************************************************/
    virtual RETCODE ClearRowsetBuffer();

    /*************************************************************

        IsColValueNull

        Returns true if column has value of NULL, false otherwise.

    *************************************************************/

    virtual bool IsColValueNull(LPUCSTR ColName);

    virtual bool IsColValueNull(LPUCSTR ColName, UWORD irow);

    virtual bool IsColValueNull(LPCSTR ColName)
    {
        return IsColValueNull((LPUCSTR)ColName);
    }

    virtual bool IsColValueNull(LPCSTR ColName, UWORD irow)
    {
        return IsColValueNull((LPUCSTR)ColName, irow);
    }

    virtual bool IsColValueNull(UWORD icol);

    virtual bool IsColValueNull(UWORD icol, UWORD irow);

    /*************************************************************

        SetColValueNull

        Returns true if column can be set to NULL, false otherwise.

    *************************************************************/

    virtual bool SetColValueNull(LPUCSTR ColName);

    virtual bool SetColValueNull(LPUCSTR ColName, UWORD irow);

    virtual bool SetColValueNull(LPCSTR ColName)
    {
        return SetColValueNull((LPUCSTR)ColName);
    }

    virtual bool SetColValueNull(LPCSTR ColName, UWORD irow)
    {
        return SetColValueNull((LPUCSTR)ColName, irow);
    }

    virtual bool SetColValueNull(UWORD icol);

    virtual bool SetColValueNull(UWORD icol, UWORD irow);

    // end new in v2.1

};  // end of class declaration: odbcCURSOR
