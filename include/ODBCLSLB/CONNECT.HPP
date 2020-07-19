#pragma once


///////////////////////////////////////////////////////////
//////////////////////// connections
///////////////////////////////////////////////////////////
class odbcEXPORTED odbcCONNECT : public odbcBASE {
/**********************************************************
    friend classes
    
    Statement must be friend class so it
    can call RegisterStmt() to register its instances.
**********************************************************/

    friend class odbcEXPORTED odbcSTMT;
    friend class odbcEXPORTED odbcCURSOR;
    friend class odbcEXPORTED odbcRECORD;
    friend class odbcEXPORTED odbcRECINSERTER;
    friend class odbcEXPORTED odbcRECUPDATER;
private:

/**********************************************************
    hdbc
    
    ODBC connection handle.
**********************************************************/

    HDBC            hdbc;
    
/**********************************************************
    henv
    
    ODBC environment handle.
**********************************************************/

    HENV            henv{};
    
/**********************************************************
    pEnv
    
    ODBC environment object.
**********************************************************/

    odbcENV*            pEnv;
    
/**********************************************************
    isConnected

    Boolean value; 0 if no ODBC connection is open, 1 if
    such a connection has been opened.
**********************************************************/

    UCHAR           isConnected;

/**********************************************************
    RegisterStmt

    Register an odbcSTMT object.
**********************************************************/

    virtual void RegisterStmt(odbcSTMT* pStmt);

/**********************************************************
    UnregisterStmt
    
    Unregister an odbcSTMT object.
**********************************************************/

    virtual void UnregisterStmt(odbcSTMT* pStmt);

public:         
/**********************************************************
    bTrimAllTrailingBlanks

    Boolean value; 0 if trailing blanks should not be auto-
    matically trimmed from SQL_CHAR columns in result set,
    1 if the trailing blanks should be trimmed. Inherited
    by all cursors on this connection.
**********************************************************/

    bool           bTrimAllTrailingBlanks;

/**********************************************************
    SQLColumnsRSBCount

    Count of structures in array of column bindings for the
    result set of SQLColumns.
**********************************************************/

    virtual UWORD SQLColumnsRSBCount();
    
/**********************************************************
    SQLColumnsRSB
    
    Address of array of structures describing column bindings 
    for the result set of SQLColumns.
**********************************************************/

    virtual sCOLBIND* SQLColumnsRSB();
    
/**********************************************************
    SQLSpecialColumnsRSBCount

    Count of structures in array of column bindings for the
    result set of SQLSpecialColumns.
**********************************************************/

    virtual UWORD SQLSpecialColumnsRSBCount();
    
/**********************************************************
    SQLSpecialColumnsRSB
    
    Address of array of structures describing column bindings 
    for the result set of SQLSpecialColumns.
**********************************************************/

    virtual sCOLBIND* SQLSpecialColumnsRSB();
    
/**********************************************************
    SQLTablesRSBCount

    Count of structures in array of column bindings for the
    result set of SQLTables.
**********************************************************/

    virtual UWORD SQLTablesRSBCount();
    
/**********************************************************
    SQLTablesRSB
    
    Address of array of structures describing column bindings 
    for the result set of SQLTables.
**********************************************************/

    virtual sCOLBIND* SQLTablesRSB();

/**********************************************************
    SQLStatisticsRSBCount

    Count of structures in array of column bindings for the
    result set of SQLStatistics.
**********************************************************/

    virtual UWORD SQLStatisticsRSBCount();
    
/**********************************************************
    SQLStatisticsRSB
    
    Address of array of structures describing column bindings 
    for the result set of SQLStatistics.
**********************************************************/

    virtual sCOLBIND* SQLStatisticsRSB();
    
/**********************************************************
    SQLTypeInfoRSBCount

    Count of structures in array of column bindings for the
    result set of SQLTypeInfo.
**********************************************************/

    virtual UWORD SQLTypeInfoRSBCount();
    
/**********************************************************
    SQLTypeInfoRSB
    
    Address of array of structures describing column bindings 
    for the result set of SQLTypeInfo.
**********************************************************/

    virtual sCOLBIND* SQLTypeInfoRSB();

// new in v2.0

/**********************************************************
    SQLProcedureColumnsRSBCount

    Count of structures in array of column bindings for the
    result set of SQLProcedureColumns.
**********************************************************/

    virtual UWORD SQLProcedureColumnsRSBCount();

/**********************************************************
    SQLProcedureColumnsRSB

    Address of array of structures describing column bindings
    for the result set of SQLProcedureColumns.
**********************************************************/

    virtual sCOLBIND* SQLProcedureColumnsRSB();

/**********************************************************
    SQLColumnPrivilegesRSBCount

    Count of structures in array of column bindings for the
    result set of SQLColumnPrivileges.
**********************************************************/

    virtual UWORD SQLColumnPrivilegesRSBCount();

/**********************************************************
    SQLColumnPrivilegesRSB

    Address of array of structures describing column bindings
    for the result set of SQLColumnPrivileges.
**********************************************************/

    virtual sCOLBIND* SQLColumnPrivilegesRSB();

/**********************************************************
    SQLProceduresRSBCount

    Count of structures in array of column bindings for the
    result set of SQLProcedures.
**********************************************************/

    virtual UWORD SQLProceduresRSBCount();

/**********************************************************
    SQLProceduresRSB

    Address of array of structures describing column bindings
    for the result set of SQLProcedures.
**********************************************************/

    virtual sCOLBIND* SQLProceduresRSB();

/**********************************************************
    SQLForeignKeysRSBCount

    Count of structures in array of column bindings for the
    result set of SQLForeignKeys.
**********************************************************/

    virtual UWORD SQLForeignKeysRSBCount();

/**********************************************************
    SQLForeignKeysRSB

    Address of array of structures describing column bindings
    for the result set of SQLForeignKeys.
**********************************************************/

    virtual sCOLBIND* SQLForeignKeysRSB();

/**********************************************************
    SQLTablePrivilegesRSBCount

    Count of structures in array of column bindings for the
    result set of SQLTablePrivileges.
**********************************************************/

    virtual UWORD SQLTablePrivilegesRSBCount();

/**********************************************************
    SQLTablePrivilegesRSB

    Address of array of structures describing column bindings
    for the result set of SQLTablePrivileges.
**********************************************************/

    virtual sCOLBIND* SQLTablePrivilegesRSB();

/**********************************************************
    SQLPrimaryKeysRSBCount

    Count of structures in array of column bindings for the
    result set of SQLPrimaryKeys.
**********************************************************/

    virtual UWORD SQLPrimaryKeysRSBCount();

/**********************************************************
    SQLPrimaryKeysRSB

    Address of array of structures describing column bindings
    for the result set of SQLPrimaryKeys.
**********************************************************/

    virtual sCOLBIND* SQLPrimaryKeysRSB();

// end new in v2.0

/**********************************************************
    odbcCONNECT

    constructor.  Environment handle is required; other
    arguments are optional.
**********************************************************/

    odbcCONNECT(
        odbcENV*        pEnv,
        LPUSTR          szDSN       = NULL,
        LPUSTR          szUID       = NULL,
        LPUSTR          szAuthStr   = NULL,
        UDWORD          udTimeout   = 0
        );

    odbcCONNECT(
        odbcENV*        pEnv,
        LPSTR           szDSN,
        LPSTR           szUID       = NULL,
        LPSTR           szAuthStr   = NULL,
        UDWORD          udTimeout   = 0
        );

/**********************************************************
    ~odbcCONNECT
    
    Destructor.
**********************************************************/

    virtual ~odbcCONNECT(); 

/**********************************************************
    Connected
    
    Returns non-zero if connection is active.
**********************************************************/

    virtual UCHAR   Connected() { return isConnected;}

/**********************************************************
    GetHdbc
    
    Get ODBC connection handle.
**********************************************************/

    virtual HDBC GetHdbc() 
                    {
                    return hdbc;
                    }
    
/**********************************************************

    GetHenv()
    
    Returns ODBC environment handle.
**********************************************************/

    virtual HENV    GetHenv() { return henv;}

    ////////////////////////////////////////////////////////
    ////////////////////// core functions //////////////////
    ////////////////////////////////////////////////////////
    
/**********************************************************
    Connect
    
    Call to SQLConnect, passing data set name, user ID,
    and password.
**********************************************************/

    RETCODE Connect(
        LPUSTR szDSN,
        LPUSTR szUID,
        LPUSTR szAuthStr
        );

    RETCODE Connect(
        LPCSTR szDSN,
        LPCSTR szUID,
        LPCSTR szAuthStr
        )
        {
        return

            Connect(
                (LPUSTR) szDSN,
                (LPUSTR) szUID,
                (LPUSTR) szAuthStr
                );
        }

/**********************************************************
    Disconnect
    
    Call to SQLDisconnect.
**********************************************************/

    RETCODE Disconnect();

/**********************************************************
    Commit
    
    Call to SQLTransact to commit a transaction.
**********************************************************/

    virtual RETCODE Commit();

/**********************************************************
    RollBack
    
    Call to SQLTransact to roll back a transaction.
**********************************************************/

    virtual RETCODE RollBack();

/**********************************************************
    AllocStmt
    
    Allocate a statement object.
    
    lpszStmt:           SQL statement to use.
    bPrepare:           if non-zero, call SQLPrepare.
    bExecute:           Call SQLExecDirect (or SQLExecute
                if bPrepare was non-zero).
    psParmBindings:     address of array of parameter bindings.
    uParmCount:         count of array elements.
    pvParmStruct:       Address of structure containing
                parameter values.
**********************************************************/

    virtual odbcSTMT*       AllocStmt
                            (
                            LPUSTR          lpszSentStmt                = NULL,
                            sPARMBIND*      psParmBindings  = NULL,
                            UWORD           uParmCount              = 0,
                            void*                     pvParmStruct    = NULL
                            );

    virtual odbcSTMT*       AllocStmt
                            (
                            LPSTR           lpszStmt                = NULL,
                            sPARMBIND*      psParmBindings  = NULL,
                            UWORD           uParmCount              = 0,
                            void*                     pvParmStruct    = NULL
                            )
                            {
                            return
                                AllocStmt
                                    (
                                    (LPUSTR)lpszStmt,
                                    psParmBindings,
                                    uParmCount,
                                    pvParmStruct
                                    );
                            }

/**********************************************************
    AllocCursor
    
    Allocate a cursor object.

    lpszStmt:           SQL statement to use.
    bPrepare:           if non-zero, call SQLPrepare.
    bExecute:           Call SQLExecDirect (or SQLExecute
                if bPrepare was non-zero).
    bAutoBind:          Automatically bind columns 
                to dynamically allocated struct?
    psParmBindings:     Address of array of parameter bindings.
    uParmCount:         Count of array elements.
    pvParmStruct:       Address of structure containing
                parameter values.
    pColBindings:   Address of array of column bindings.
    uColcount:              Count of array elements.
    pvColStruct:    Address of structure for column bindings.                               
**********************************************************/

    virtual odbcCURSOR*     AllocCursor
                            (
                            LPUSTR          lpszSentStmt                = NULL,
                            bool            bAutoBind               = false,
                            sPARMBIND*      psParmBindings  = NULL,
                            UWORD           uParmCount              = 0,
                            void*                     pvParmStruct    = NULL,
                            sCOLBIND*       psColBindings    = NULL,
                            UWORD           uColCount               = 0,
                            void*                     pvColStruct     = NULL
                            );
    
    virtual odbcCURSOR*     AllocCursor
                            (
                            LPSTR           lpszStmt,
                            bool            bAutoBind               = false,
                            sPARMBIND*      psParmBindings  = NULL,
                            UWORD           uParmCount              = 0,
                            void*                     pvParmStruct    = NULL,
                            sCOLBIND*       pColBindings    = NULL,
                            UWORD           uColCount               = 0,
                            void*                     pvColStruct     = NULL
                            )
                            {
                            return
                                AllocCursor
                                    (
                                    (LPUSTR)lpszStmt,
                                    bAutoBind,
                                    psParmBindings,
                                    uParmCount,
                                    pvParmStruct,
                                    pColBindings,
                                    uColCount,
                                    pvColStruct
                                    );
                            }
     
/**********************************************************
    DeAllocStmt
    
    Deallocate a statement object.
**********************************************************/

    virtual void DeAllocStmt(odbcSTMT* pS);
    
    
/**********************************************************
    AllocRecInserter
    
    Allocate a RecInserter object.
**********************************************************/

    virtual odbcRECINSERTER* AllocRecInserter
            (
            LPCSTR          lpszSentTblName,
            sCOLBIND*       pColBinds       = NULL,
            UWORD           uNumColBindings = 0,
            void*             pRecord         = NULL
            );

/**********************************************************
    DeAllocRecInserter
    
    Deallocate a RecInserter object.
**********************************************************/

    virtual void DeAllocRecInserter(odbcRECINSERTER* pS);

/**********************************************************
    AllocRecUpdater
    
    Allocate a RecUpdater object.
**********************************************************/

    virtual odbcRECUPDATER* AllocRecUpdater
            (
            LPCSTR          lpszSentTblName,
            LPCSTR          lpszSelectStmt,
            sCOLBIND*       pColBinds        = NULL,
            UWORD           uNumColBindings  = 0,
            void*             pRecord          = NULL,
            bool            bExecDirect      = true,
            UWORD           fConcur          = SQL_CONCUR_VALUES,
            SDWORD          fKeyset          = SQL_CURSOR_STATIC,
            UWORD           fRowSet          = 1 
            );

/**********************************************************
    DeAllocRecUpdater
    
    Deallocate a RecUpdater object.
**********************************************************/

    virtual void DeAllocRecUpdater(odbcRECUPDATER* pS);

/**********************************************************
    AllocTableCreator
    
    Allocate a TableCreator object.
**********************************************************/

    virtual odbcTABLECREATOR* AllocTableCreator ();

/**********************************************************
    DeAllocTableCreator
    
    Deallocate a TableCreator object.
**********************************************************/

    virtual void DeAllocTableCreator(odbcTABLECREATOR* pS);    
    
/**********************************************************
    AllocBLOB
    
    Allocate a BLOB object.  
    
    Pass address of owning cursor, number of the
    associated column, and maximum size of the parameter;
    optionally also pass the column's SQL data type and
    put- and get-chunk granularities.  
**********************************************************/
                 
    virtual odbcBLOB* AllocBLOB(
                odbcCURSOR* pCurs,
                UWORD iSentCol,
                UDWORD cbSentMaxSize,
                SWORD  fSentSqlType          = SQL_LONGVARBINARY,
                UWORD  iSentParam            = 0,
                SDWORD cbSentPutChunkSize    = BLOB_CHUNK_PUT_SIZE,
                SDWORD cbSentGetChunkSize    = BLOB_CHUNK_GET_SIZE 
                );
    
    
/**********************************************************
    DeAllocBLOB

    Deallocate a BLOB object. 
    
    pS      BLOB to deallocate.
**********************************************************/

    virtual void DeAllocBLOB(
                odbcBLOB* pS
                );

/**********************************************************
    AllocColumnIterator
    
    Allocate a ColumnIterator object.  
**********************************************************/
                 
    virtual odbcColumnIterator* AllocColumnIterator(
            LPSTR szTableQualifier,
            LPSTR szTableOwner,
            LPSTR szTableName,
            LPSTR szColumnName
            );

/**********************************************************
    DeAllocColumnIterator

    Deallocate a ColumnIterator object. 
**********************************************************/

    virtual void DeAllocColumnIterator(
            odbcColumnIterator* pS
            );

/**********************************************************
    AllocColumnPrivilegesIterator
        
    Allocate a ColumnPrivilegesIterator object.  
**********************************************************/
                 
    virtual odbcColumnPrivilegesIterator* AllocColumnPrivilegesIterator(
            LPSTR szTableQualifier,
            LPSTR szTableOwner,
            LPSTR szTableName,
            LPSTR szColumnName
            );

/**********************************************************
    DeAllocColumnPrivilegesIterator

    Deallocate a ColumnPrivilegesIterator object. 
**********************************************************/

    virtual void DeAllocColumnPrivilegesIterator(
            odbcColumnPrivilegesIterator* pS
            );

/**********************************************************
    AllocForeignKeysIterator
    
    Allocate a ForeignKeysIterator object.  
**********************************************************/
                 
    virtual odbcForeignKeysIterator* AllocForeignKeysIterator(
            LPSTR szPkTableQualifier,
            LPSTR szPkTableOwner,
            LPSTR szPkTableName,
            LPSTR szFkTableQualifier,
            LPSTR szFkTableOwner,
            LPSTR szFkTableName
            );

/**********************************************************
    DeAllocForeignKeysIterator

    Deallocate a ForeignKeysIterator object. 
**********************************************************/

    virtual void DeAllocForeignKeysIterator(          
            odbcForeignKeysIterator* pS
            );

/**********************************************************
    AllocPrimaryKeysIterator
    
    Allocate a PrimaryKeysIterator object.  
**********************************************************/
                 
    virtual odbcPrimaryKeysIterator* AllocPrimaryKeysIterator(
            LPSTR szTableQualifier,
            LPSTR szTableOwner,
            LPSTR szTableName
            );

/**********************************************************
    DeAllocPrimaryKeysIterator

    Deallocate a PrimaryKeysIterator object. 
**********************************************************/

    virtual void DeAllocPrimaryKeysIterator(          
            odbcPrimaryKeysIterator* pS
            );

/**********************************************************
    AllocProcedureColumnsIterator
    
    Allocate a ProcedureColumnsIterator object.  
**********************************************************/
                 
    virtual odbcProcedureColumnsIterator* AllocProcedureColumnsIterator(
            LPSTR szProcQualifier,
            LPSTR szProcOwner,
            LPSTR szProcName,
            LPSTR szProcColumn
            );

/**********************************************************
    DeAllocProcedureColumnsIterator

    Deallocate a ProcedureColumnsIterator object. 
**********************************************************/

    virtual void DeAllocProcedureColumnsIterator(
            odbcProcedureColumnsIterator* pS
            );

/**********************************************************
    AllocProceduresIterator
    
    Allocate a ProceduresIterator object.  
**********************************************************/
                 
    virtual odbcProceduresIterator* AllocProceduresIterator(
            LPSTR szProcQualifier,
            LPSTR szProcOwner,
            LPSTR szProcName
            );

/**********************************************************
    DeAllocProceduresIterator

    Deallocate a ProceduresIterator object. 
**********************************************************/

    virtual void DeAllocProceduresIterator(
            odbcProceduresIterator* pS
            );

/**********************************************************
    AllocSpecialColumnIterator
    
    Allocate a SpecialColumnIterator object.  
**********************************************************/
                 
    virtual odbcSpecialColumnIterator* AllocSpecialColumnIterator(
            UWORD                   fColType,
            LPSTR                   szTableQualifier,
            LPSTR                   szTableOwner,
            LPSTR                   szTableName,
            UWORD                   fScope,
            UWORD                   fNullable
            );

/**********************************************************
    DeAllocSpecialColumnIterator

    Deallocate a SpecialColumnIterator object. 
**********************************************************/

    virtual void DeAllocSpecialColumnIterator(
            odbcSpecialColumnIterator* pS
            );

/**********************************************************
    AllocStatisticsIterator
    
    Allocate a StatisticsIterator object.  
**********************************************************/
                 
    virtual odbcStatisticsIterator* AllocStatisticsIterator(
            LPSTR                   szTableQualifier,
            LPSTR                   szTableOwner,
            LPSTR                   szTableName,
            UWORD                   fTblUnique,
            UWORD                   fTblAccuracy
            );

/**********************************************************
    DeAllocStatisticsIterator

    Deallocate a StatisticsIterator object. 
**********************************************************/

    virtual void DeAllocStatisticsIterator(         
            odbcStatisticsIterator* pS
            );

/**********************************************************
    AllocTableIterator
    
    Allocate a TableIterator object.  
**********************************************************/
                 
    virtual odbcTableIterator* AllocTableIterator(
            LPSTR                  szTableQualifier,
            LPSTR                  szTableOwner,
            LPSTR                  szTableName,
            LPSTR                  szTableType
            );

/**********************************************************
    DeAllocTableIterator

    Deallocate a TableIterator object. 
**********************************************************/

    virtual void DeAllocTableIterator(
            odbcTableIterator* pS
            );

/**********************************************************
    AllocTablePrivilegesIterator
    
    Allocate a TablePrivilegesIterator object.  
**********************************************************/
                 
    virtual odbcTablePrivilegesIterator* AllocTablePrivilegesIterator(
            LPSTR szTableQualifier,
            LPSTR szTableOwner,
            LPSTR szTableName
            );

/**********************************************************
    DeAllocTablePrivilegesIterator

    Deallocate a TablePrivilegesIterator object. 
**********************************************************/

    virtual void DeAllocTablePrivilegesIterator(
            odbcTablePrivilegesIterator* pS
            );

/**********************************************************
    AllocTypeInfoIterator
    
    Allocate a TypeInfoIterator object.  
**********************************************************/
                 
    virtual odbcTypeInfoIterator* AllocTypeInfoIterator();

/**********************************************************
    DeAllocTypeInfoIterator

    Deallocate a TypeInfoIterator object. 
**********************************************************/

    virtual void DeAllocTypeInfoIterator(
            odbcTypeInfoIterator* pS
            );
    
/**********************************************************
    DeAllocCursor
    
    Deallocate a cursor object.
**********************************************************/

    virtual void DeAllocCursor(odbcCURSOR* pC)
                    {
                    DeAllocStmt(pC);
                    }
    
/**********************************************************
    RegisterError
    
    Get more information on the most recent error code
    from an ODBC operation. Results can be retrieved using
    member functions in the parent odbcBASE class.
    
    This function calls the base class member function Error()
    with arguments appropriate for this object type.
**********************************************************/

    RETCODE RegisterError() override;

    ////////////////////////////////////////////////////////
    ////////////////////// level 1 functions ///////////////
    ////////////////////////////////////////////////////////


/**********************************************************
    GetConnectOption
    
    Call SQLGetConnectOption.
    
**********************************************************/

    virtual UDWORD GetConnectOption(UWORD fOption);
/**********************************************************
    GetConnectOptionString
    
    Call SQLGetConnectOption to get pointer to string
    option value.
    
**********************************************************/

    // TODO
    //virtual LPCSTR GetConnectOptionString(UWORD fOption);

/**********************************************************
    SetConnectOption
    
    Call SQLSetConnectOption.
    
    The various Set... member functions call the same
    function with the appropriate flag.
**********************************************************/

    virtual RETCODE SetConnectOption
                (
                UWORD   fOption,
                UDWORD  ulValue
                );
/**********************************************************
    SetConnectOptionString
    
    Call SQLSetConnectOption to set a string option.
    
**********************************************************/

    // TODO
    //virtual RETCODE SetConnectOptionString(UWORD  fOption, LPCSTR lpszValue);

/**********************************************************
    DriverConnect
    
    Call SQLDriverConnect.
    
**********************************************************/

    virtual RETCODE DriverConnect(
                HWND       hwnd,
                LPUCSTR    szConnStrIn,
                LPUSTR     szConnStrOut,
                SWORD      cbConnStrOutMax,
                SWORD      *pcbConnStrOut,
                UWORD      fDriverCompletion
                );

    virtual RETCODE DriverConnect(
                HWND            hwnd,
                LPCSTR          szConnStrIn,
                LPSTR           szConnStrOut,
                SWORD           cbConnStrOutMax,
                SWORD      *pcbConnStrOut,
                UWORD           fDriverCompletion
                )
                {
                return
                        DriverConnect(
                        hwnd,
                        (LPUCSTR) szConnStrIn,
                        (LPUSTR) szConnStrOut,
                        cbConnStrOutMax,
                        pcbConnStrOut,
                        fDriverCompletion
                        );
                }

/**********************************************************
    DriverConnectNoPrompt
    
    Call SQLDriverConnect, using contents of szConnStrIn;
    do not prompt with any dialog, but return an error if
    the connection information in the string is not enough.
    Use strlen() to obtain the length of szConnStrOut,
    but only if the function succeeded.
**********************************************************/

    virtual RETCODE DriverConnectNoPrompt(
                LPUCSTR szConnStrIn,
                LPUSTR szConnStrOut,
                SWORD  cbConnStrOutMax
                );

    virtual RETCODE DriverConnectNoPrompt(
                LPCSTR  szConnStrIn,
                LPSTR   szConnStrOut,
                SWORD   cbConnStrOutMax
                )
                {
                return
                        DriverConnectNoPrompt(
                        (LPUCSTR) szConnStrIn,
                        (LPUSTR) szConnStrOut,
                        cbConnStrOutMax
                        );
                }

/**********************************************************
    DriverConnectComplete
    
    Call SQLDriverConnect; prompt to complete the connection,
    if insufficient information is provided in szConnStrIn.
    Use strlen() to obtain the length of szConnStrOut,
    but only if the function succeeded.
**********************************************************/

    virtual RETCODE DriverConnectComplete(
                HWND   hwnd,
                LPUCSTR szConnStrIn,
                LPUSTR szConnStrOut,
                SWORD  cbConnStrOutMax
                );

    virtual RETCODE DriverConnectComplete(
                HWND    hwnd,
                LPCSTR  szConnStrIn,
                LPSTR   szConnStrOut,
                SWORD   cbConnStrOutMax
                )
                {
                return
                        DriverConnectComplete(
                        hwnd,
                        (LPUCSTR) szConnStrIn,
                        (LPUSTR) szConnStrOut,
                        cbConnStrOutMax
                        );
                }

/**********************************************************
    DriverConnectPrompt
    
    Call SQLDriverConnect; tells driver to prompt for
    connection information.
    Use strlen() to obtain the length of szConnStrOut,
    but only if the function succeeded.
**********************************************************/

    virtual RETCODE DriverConnectPrompt(
                HWND   hwnd,
                LPUSTR szConnStrOut,
                SWORD  cbConnStrOutMax
                );

    virtual RETCODE DriverConnectPrompt(
                HWND    hwnd,
                LPSTR   szConnStrOut,
                SWORD   cbConnStrOutMax
                )
                {
                return
                        DriverConnectPrompt(
                        hwnd,
                        (LPUSTR) szConnStrOut,
                        cbConnStrOutMax
                        );
                }

/**********************************************************
    DriverConnectCompleteRequired
    
    Calls SQLDriverConnect; tells driver to complete any
    required items not in the szConnStrIn argument.
    Use strlen() to obtain the length of szConnStrOut,
    but only if the function succeeded.
**********************************************************/

    virtual RETCODE DriverConnectCompleteRequired(
                HWND   hwnd,
                LPUCSTR szConnStrIn,
                LPUSTR szConnStrOut,
                SWORD  cbConnStrOutMax
                );

    virtual RETCODE DriverConnectCompleteRequired(
                HWND    hwnd,
                LPCSTR  szConnStrIn,
                LPSTR   szConnStrOut,
                SWORD   cbConnStrOutMax
                )
                {
                return
                        DriverConnectCompleteRequired(
                        hwnd,
                        (LPUCSTR) szConnStrIn,
                        (LPUSTR) szConnStrOut,
                        cbConnStrOutMax
                        );
                }


/**********************************************************
    GetFunctions

    Call SQLGetFunctions.
    
**********************************************************/

    virtual UWORD GetFunctions(UWORD fFunction);

/**********************************************************
    GetInfo

    Call SQLGetInfo.

**********************************************************/

virtual RETCODE GetInfo(
    UWORD       fInfoType,
    void*         rgbInfoValue,
    SWORD       cbInfoValueMax,
    SWORD      *pcbInfoValue);

/**********************************************************
    GetInfoString
    
    Call SQLGetInfo to get a string value.
    
**********************************************************/

virtual RETCODE GetInfoString(
    UWORD       fInfoType,
    LPSTR       szResult,
    SWORD       cbBufferSize,
    SWORD      *pcbInfoValue)
    {
    return GetInfo
        (
        fInfoType,
        (void*)szResult,
        cbBufferSize,
        pcbInfoValue
        );
        }
    
/**********************************************************
    GetInfoUDWord

    Call SQLGetInfo to get a string value.

**********************************************************/

virtual UDWORD GetInfoUDWord(
    UWORD       fInfoType
                )
    {
    UDWORD  udRet       = 0 ;
    SWORD   cbInfoValue = 0;
//    RETCODE ret         = SQL_SUCCESS;

    GetInfo
        (
        fInfoType,
        (void*)&udRet,
        sizeof(udRet),
        &cbInfoValue
        );

    return sqlsuccess() ? udRet : 0 ;
    }
    

/**********************************************************
    EnumColumns

    Calls SQLColumns and enumerates the result set, passing
    each column's data attributes as a structure to the
    callback function supplied by the caller.
    
**********************************************************/

    virtual RETCODE EnumColumns(
        LPUSTR                  szTableQualifier,
        LPUSTR                  szTableOwner,
        LPUSTR                  szTableName,
        LPUSTR                  szColumnName,
            pfENUMCOLUMNS   pfEnum, 
            void*                             pUser = NULL);

    virtual RETCODE EnumColumns(
        LPSTR                   szTableQualifier,
        LPSTR                   szTableOwner,
        LPSTR                   szTableName,
        LPSTR                   szColumnName,
            pfENUMCOLUMNS   pfEnum,
            void*                             pUser = NULL)
            {
            return

                EnumColumns(
                (LPUSTR)szTableQualifier,
                (LPUSTR)szTableOwner,
                (LPUSTR)szTableName,
                (LPUSTR)szColumnName,
                            pfEnum, 
                            pUser);
            }

/**********************************************************
    EnumSpecialColumns
    
    Calls SQLSpecialColumns and enumerates the result set, 
    passing each column's data attributes as a structure to 
    the callback function supplied by the caller.
    
**********************************************************/

    virtual RETCODE EnumSpecialColumns(
                        UWORD                           fColType,
                    LPUSTR                          szTableQualifier,
                    LPUSTR                          szTableOwner,
                    LPUSTR                          szTableName,
                        UWORD                           fScope,
                        UWORD                           fNullable,
                        pfENUMSPECIALCOL        pfEnum, 
                        void*                                     pUser = NULL
                        );
                        
    virtual RETCODE EnumSpecialColumns(
                        UWORD                           fColType,
                    LPSTR                           szTableQualifier,
                    LPSTR                           szTableOwner,
                    LPSTR                           szTableName,
                        UWORD                           fScope,
                        UWORD                           fNullable,
                        pfENUMSPECIALCOL        pfEnum, 
                        void*                                     pUser = NULL
                        )
                        {
                        return
                            EnumSpecialColumns(
                                fColType,
                            (LPUSTR)szTableQualifier,
                            (LPUSTR)szTableOwner,
                            (LPUSTR)szTableName,
                                fScope,
                                fNullable,
                                pfEnum, 
                                pUser
                                );
                         }
/**********************************************************
    EnumTables

    Calls SQLTables and enumerates the result set,
    passing each table's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

    virtual RETCODE EnumTables(
            LPUSTR          szTableQualifier,
            LPUSTR          szTableOwner,
            LPUSTR          szTableName,
            LPUSTR          szTableType,
            pfENUMTABLES    pfEnum, 
            void*             pUser = NULL);

    virtual RETCODE EnumTables(
            LPSTR           szTableQualifier,
            LPSTR           szTableOwner,
            LPSTR           szTableName,
            LPSTR           szTableType,
            pfENUMTABLES    pfEnum,
            void*             pUser = NULL)
            {
            return
            EnumTables(
                (LPUSTR)szTableQualifier,
                (LPUSTR)szTableOwner,
                (LPUSTR)szTableName,
                (LPUSTR)szTableType,
                pfEnum,
                pUser);
            }

/**********************************************************
    EnumStatistics
    
    Calls SQLStatistics and enumerates the result set, 
    passing each table's data attributes as a structure to 
    the callback function supplied by the caller.

**********************************************************/

    virtual RETCODE EnumStatistics(
            LPUSTR              szTableQualifier,
            LPUSTR              szTableOwner,
            LPUSTR              szTableName,
            UWORD               fUnique,
            UWORD               fAccuracy,
            pfENUMSTATISTICS    pfEnum,
            void*                 pUser = NULL);

    virtual RETCODE EnumStatistics(
            LPSTR               szTableQualifier,
            LPSTR               szTableOwner,
            LPSTR               szTableName,
            UWORD               fUnique,
            UWORD               fAccuracy,
            pfENUMSTATISTICS    pfEnum,
            void*                 pUser = NULL)
            {
            return
                EnumStatistics(
                    (LPUSTR)szTableQualifier,
                    (LPUSTR)szTableOwner,
                    (LPUSTR)szTableName,
                    fUnique,
                    fAccuracy,
                    pfEnum,
                    pUser);
            }

/**********************************************************
    GetTypeInfo
    
    Calls SQLGetTypeInfo. Pass the flag of a specific type,
    and it returns the type's attributes in the result
    set structure.
    
**********************************************************/

    virtual RETCODE GetTypeInfo(
                            UWORD                   fSQLType,
                            sSQLTYPEINFORESULTSET&  rsResultSet
                            );

/**********************************************************
    EnumTypeInfo
    
    Calls SQLGetTypeInfo with flag to get attributes of
    all types and enumerates the result set,
    passing each table's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

    virtual RETCODE EnumTypeInfo(
                            pfENUMTYPEINFO  pfEnum,
                            void*             pUser = NULL);

// new in v2.0

/**********************************************************
    EnumProcedureColumns

    Calls SQLProcedureColumns and enumerates the result set,
    passing each column's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

    virtual RETCODE EnumProcedureColumns(
                        LPUSTR              szProcQualifier,
                        LPUSTR              szProcOwner,
                        LPUSTR              szProcName,
                        LPUSTR              szColumnName,
                        pfENUMPROCEDURECOL  pfEnum,
                        void*                 pUser = NULL
                        );
                        
    virtual RETCODE EnumProcedureColumns(
                        LPSTR               szProcQualifier,
                        LPSTR               szProcOwner,
                        LPSTR               szProcName,
                        LPSTR               szColumnName,
                        pfENUMPROCEDURECOL  pfEnum,
                        void*                 pUser = NULL
                        )
                        {
                        return EnumProcedureColumns(
                                (LPUSTR)szProcQualifier,
                                (LPUSTR)szProcOwner,
                                (LPUSTR)szProcName,
                                (LPUSTR)szColumnName,
                                pfEnum,
                                pUser
                                );
                         }

/**********************************************************
    EnumColumnPrivileges

    Calls SQLColumnPrivileges and enumerates the result set,
    passing each column's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

    virtual RETCODE EnumColumnPrivileges(
                        LPUSTR              szTableQualifier,
                        LPUSTR              szTableOwner,
                        LPUSTR              szTableName,
                        LPUSTR              szColumnName,
                        pfENUMCOLPRIVS      pfEnum,
                        void*                 pUser = NULL
                        );

    virtual RETCODE EnumColumnPrivileges(
                        LPSTR               szTableQualifier,
                        LPSTR               szTableOwner,
                        LPSTR               szTableName,
                        LPSTR               szColumnName,
                        pfENUMCOLPRIVS      pfEnum,
                        void*                 pUser = NULL
                        )
                        {
                        return EnumColumnPrivileges(
                                (LPUSTR)szTableQualifier,
                                (LPUSTR)szTableOwner,
                                (LPUSTR)szTableName,
                                (LPUSTR)szColumnName,
                                pfEnum,
                                pUser
                                );
                         }

/**********************************************************
    EnumTablePrivileges

    Calls SQLTablePrivileges and enumerates the result set,
    passing each column's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

    virtual RETCODE EnumTablePrivileges(
                        LPUSTR              szTableQualifier,
                        LPUSTR              szTableOwner,
                        LPUSTR              szTableName,
                        pfENUMTABLEPRIVS    pfEnum,
                        void*                 pUser = NULL
                        );

    virtual RETCODE EnumTablePrivileges(
                        LPSTR               szTableQualifier,
                        LPSTR               szTableOwner,
                        LPSTR               szTableName,
                        pfENUMTABLEPRIVS    pfEnum,
                        void*                 pUser = NULL
                        )
                        {
                        return EnumTablePrivileges(
                                (LPUSTR)szTableQualifier,
                                (LPUSTR)szTableOwner,
                                (LPUSTR)szTableName,
                                pfEnum,
                                pUser
                                );
                         }

/**********************************************************
    EnumProcedures

    Calls SQLProcedures and enumerates the result set,
    passing each procedure's attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

    virtual RETCODE EnumProcedures(
                        LPUSTR              szProcQualifier,
                        LPUSTR              szProcOwner,
                        LPUSTR              szProcName,
                        pfENUMPROCEDURES    pfEnum,
                        void*                 pUser = NULL
                        );

    virtual RETCODE EnumProcedures(
                        LPSTR               szProcQualifier,
                        LPSTR               szProcOwner,
                        LPSTR               szProcName,
                        pfENUMPROCEDURES    pfEnum,
                        void*                 pUser = NULL
                        )
                        {
                        return EnumProcedures(
                                (LPUSTR)szProcQualifier,
                                (LPUSTR)szProcOwner,
                                (LPUSTR)szProcName,
                                pfEnum,
                                pUser
                                );
                         }

/***************************************************

    ForeignKeys
    
    Calls SQLForeignKeys and enumerates the result set,
    passing each procedure's attributes as a structure to
    the callback function supplied by the caller.

***************************************************/

virtual RETCODE EnumForeignKeys(
        LPUSTR              szPkTableQualifier,
        LPUSTR              szPkTableOwner,
        LPUSTR              szPkTableName,
        LPUSTR              szFkTableQualifier,
        LPUSTR              szFkTableOwner,
        LPUSTR              szFkTableName,
        pfENUMFOREIGNKEYS   pfEnum,
        void*                 pUser = NULL
        );

virtual RETCODE ForeignKeys(
        LPSTR               szPkTableQualifier,
        LPSTR               szPkTableOwner,
        LPSTR               szPkTableName,
        LPSTR               szFkTableQualifier,
        LPSTR               szFkTableOwner,
        LPSTR               szFkTableName,
        pfENUMFOREIGNKEYS   pfEnum,
        void*                 pUser = NULL
        )
        {
        return EnumForeignKeys(
                   (LPUSTR)szPkTableQualifier,
                   (LPUSTR)szPkTableOwner,
                   (LPUSTR)szPkTableName,
                   (LPUSTR)szFkTableQualifier,
                   (LPUSTR)szFkTableOwner,
                   (LPUSTR)szFkTableName,
                   pfEnum,
                   pUser
                   );
        }

/***************************************************

    PrimaryKeys

    Calls SQLPrimaryKeys and enumerates the result set,
    passing each procedure's attributes as a structure to
    the callback function supplied by the caller.

***************************************************/

virtual RETCODE EnumPrimaryKeys(
        LPUSTR              szTableQualifier,
        LPUSTR              szTableOwner,
        LPUSTR              szTableName,
        pfENUMPRIMARYKEYS   pfEnum,
        void*                 pUser = NULL
        );

virtual RETCODE PrimaryKeys(
        LPSTR               szTableQualifier,
        LPSTR               szTableOwner,
        LPSTR               szTableName,
        pfENUMPRIMARYKEYS   pfEnum,
        void*                 pUser = NULL
        )
        {
        return EnumPrimaryKeys(
                   (LPUSTR)szTableQualifier,
                   (LPUSTR)szTableOwner,
                   (LPUSTR)szTableName,
                   pfEnum,
                   pUser
                   );
        }

/***************************************************

    CursorsCanScroll

    Calls GetInfoUDWord and checks to make sure the
    connection supports last, first, and prior fetches.
    Since all connections must support fetch-next,
    .

***************************************************/

    virtual bool CursorsCanScroll()
        {
        // if we can't do extended fetches, we can't
        // do any scrolling operations.
        if ( !GetFunctions(SQL_API_SQLEXTENDEDFETCH))
            return false;

        UDWORD udWord = GetInfoUDWord( SQL_FETCH_DIRECTION );

        // now we see if directional fetches are supported.

        if ( !sqlsuccess() )
            return false;

        return ( udWord & SQL_FD_FETCH_PRIOR )
                    && ( udWord & SQL_FD_FETCH_LAST )
                    && ( udWord & SQL_FD_FETCH_FIRST );
        }

///////////////////////////////////////////////////////////////
private:
/**********************************************************
    szQuote

    Quote mark for SQL identifiers; initialized by
    GetIdentifierQuoteMark().
**********************************************************/

    UCHAR             szQuote[ 6 ]{};

public:
/**********************************************************
    GetIdentifierQuoteMark

    Get quote mark for SQL identifiers.
**********************************************************/
    virtual LPCSTR GetIdentifierQuoteMark()
        {
        SWORD cbReturned = 0;
        // temporarily turn off auto reporting
        UWORD bSaveAutoReport = AutoReport( odbcNOREPORT );
        UWORD bSaveAutoRetrieve = AutoRetrieve( odbcNOREPORT );
        bool bOk;

        GetInfoString(
                SQL_IDENTIFIER_QUOTE_CHAR,
                (LPSTR)szQuote,
                sizeof( szQuote ),
                &cbReturned
                );

        AutoRetrieve( bSaveAutoRetrieve );
        AutoReport( bSaveAutoReport );
        bOk = sqlsuccess();
        SetRC( SQL_SUCCESS );

        if ( bOk )
            return (LPCSTR)szQuote;
        else
            return (LPCSTR)" ";
        }

/**********************************************************
    GetQuotedIdentifierCase

    Get double word describing data source treatment
    of case for quoted SQL identifiers.
**********************************************************/
    virtual UDWORD GetQuotedIdentifierCase()
        {
        // temporarily turn off auto reporting
        UWORD bSaveAutoReport = AutoReport( odbcNOREPORT );
        UWORD bSaveAutoRetrieve = AutoRetrieve( odbcNOREPORT );
        UDWORD udRet =
            GetInfoUDWord( SQL_QUOTED_IDENTIFIER_CASE );
        SetRC( SQL_SUCCESS );
        AutoRetrieve( bSaveAutoRetrieve );
        AutoReport( bSaveAutoReport );

        return udRet;
        }

/**********************************************************
    GetIdentifierCase

    Get double word describing data source treatment
    of case for SQL identifiers.
**********************************************************/
    virtual UDWORD GetIdentifierCase()
        {
        // temporarily turn off auto reporting
        UWORD bSaveAutoReport = AutoReport( odbcNOREPORT );
        UWORD bSaveAutoRetrieve = AutoRetrieve( odbcNOREPORT );
        
        UDWORD udRet =
            GetInfoUDWord( SQL_IDENTIFIER_CASE );
        SetRC( SQL_SUCCESS );
        AutoRetrieve( bSaveAutoRetrieve );
        AutoReport( bSaveAutoReport );

        return udRet;
        }

/**********************************************************
    PosStmtSupport

    Call SQLGetInfo to get bitmap showing support
    level for positioned update statements.

**********************************************************/

virtual UDWORD PosStmtSupport()
    {
    UWORD bSaveAutoReport = AutoReport( odbcNOREPORT );
    UWORD bSaveAutoRetrieve = AutoRetrieve( odbcNOREPORT );
    UDWORD  udRet = GetInfoUDWord( SQL_POSITIONED_STATEMENTS );
    AutoRetrieve( bSaveAutoRetrieve );
    AutoReport( bSaveAutoReport );
    return sqlsuccess() ? udRet : 0 ;
    }

/**********************************************************
    SetPosSupport

    Call SQLGetInfo to get bitmap showing support
    level for SQLSetPos ODBC 2.0 extensions.

**********************************************************/

virtual UDWORD SetPosSupport()
    {
    UWORD bSaveAutoReport = AutoReport( odbcNOREPORT );
    UWORD bSaveAutoRetrieve = AutoRetrieve( odbcNOREPORT );
    UDWORD  udRet = GetInfoUDWord( SQL_POS_OPERATIONS );
    AutoRetrieve( bSaveAutoRetrieve );
    AutoReport( bSaveAutoReport );
    return sqlsuccess() ? udRet : 0 ;
    }



// end new in v2.0

/**********************************************************
    NativeSql

    Calls SQLNativeSql to have the driver translate the
    given SQL statement into native format.

**********************************************************/

    virtual RETCODE NativeSql(
            LPUSTR          szSqlStrIn,
            LPUSTR          szSqlStr,
            SDWORD          cbSqlStrMax,
            SDWORD     *pcbSqlStr);

    virtual RETCODE NativeSql(
            LPSTR           szSqlStrIn,
            LPSTR           szSqlStr,
            SDWORD          cbSqlStrMax,
            SDWORD     *pcbSqlStr)
            {
            return    
                    NativeSql(
                    (LPUSTR)szSqlStrIn,
                    (LPUSTR)szSqlStr,
                    cbSqlStrMax,
                    pcbSqlStr);
            }
    


}; // end of class odbcCONNECT
