#pragma once

/**********************************************************
    odbcSYSINFOITERATOR

    Base iterator class.
**********************************************************/
class odbcEXPORTED odbcSYSINFOITERATOR : public odbcCURSOR {

protected:

    /**********************************************************
    bCanScroll

    true if scrolling cursor behavior is enabled.
    **********************************************************/
    bool bCanScroll;

public:

    /**********************************************************
    ~odbcSYSINFOITERATOR

    Destructor.
    **********************************************************/

    virtual ~odbcSYSINFOITERATOR();

    /**********************************************************
    odbcSYSINFOITERATOR

    Constructor for iterator class to iterate result set.
    **********************************************************/

    odbcSYSINFOITERATOR(odbcCONNECT*   pC);

    /**********************************************************
    DoQuery

    Process query.
    **********************************************************/

    bool  DoQuery() override;
    /***************************************************

    GetFirst

    Call Fetch() to get first row in the result set
    (or ExtFetchFirst() to get first rowset in result set).
    Returns non-zero value if it succeeds, zero if
    it fails.
    ***************************************************/

    bool GetFirst() override;

    /***************************************************

    GetNext

    Call Fetch() to get next row in the result set
    (or ExtFetchNext() to get next rowset in result set).
    Returns non-zero value if it succeeds, zero if
    it fails.
    ***************************************************/

    bool GetNext() override;

    /***************************************************

    GetLast

    Call Fetch() to get last rowset in the result set.
    Only valid if extended fetch operations are in use.
    Returns non-zero value if it succeeds, zero if
    it fails.
    ***************************************************/

    bool GetLast() override;

    /***************************************************

    GetPrev

    Call ExtFetchPrev() to get prior rowset in the result set.
    Only valid if extended fetch operations are in use.
    Returns non-zero value if it succeeds, zero if
    it fails.
    ***************************************************/

    bool GetPrev() override;


}; // end odbcSYSINFOITERATOR class


/**********************************************************
    odbcStatisticsIterator

    Iterator class to process result set for SQLStatistics.
**********************************************************/

class odbcEXPORTED odbcStatisticsIterator : public odbcSYSINFOITERATOR {

protected:

    /**********************************************************
        szQualifier

        Table qualifier to pass to SQLStatistics.
    **********************************************************/

    LPUSTR                                  szQualifier;

    /**********************************************************
        szOwner

        Owner name to pass to SQLStatistics.
    **********************************************************/

    LPUSTR                                  szOwner;

    /**********************************************************
        szName

        Table name to pass to SQLStatistics.
    **********************************************************/

    LPUSTR                                  szName;

    /**********************************************************
        fUnique

        Uniqueness argument to pass to SQLStatistics.
    **********************************************************/

    UWORD                                   fUnique;

    /**********************************************************
        fAccuracy

        Accuracy flag to pass to SQLStatistics.
    **********************************************************/

    UWORD                                   fAccuracy;

    /**********************************************************
        Set

        Structure for result set.
    **********************************************************/

    sSQLSTATISTICSRESULTSET Set{};

public:
    /**********************************************************
        odbcStatisticsIterator

        Constructor for iterator class to process result set
        for SQLStatistics.
    **********************************************************/

    odbcStatisticsIterator
    (
        odbcCONNECT*                    pC,
        LPUSTR                                  szTableQualifier,
        LPUSTR                                  szTableOwner,
        LPUSTR                                  szTableName,
        UWORD                                   fTblUnique,
        UWORD                                   fTblAccuracy
    );

    odbcStatisticsIterator
    (
        odbcCONNECT*                    pC,
        LPSTR                                   szTableQualifier,
        LPSTR                                   szTableOwner,
        LPSTR                                   szTableName,
        UWORD                                   fTblUnique,
        UWORD                                   fTblAccuracy
    );

    /**********************************************************
        ~odbcStatisticsIterator

        Destructor.
    **********************************************************/

    virtual ~odbcStatisticsIterator();

    /**********************************************************
        DoQuery

        Process query.
    **********************************************************/

    bool  DoQuery() override;

    /**********************************************************
        pStatisticsResultSet

        Returns pointer to row data structure for result
        set from SQLStatistics.
    **********************************************************/

    virtual const sSQLSTATISTICSRESULTSET* pStatisticsResultSet()
    {
        return (const sSQLSTATISTICSRESULTSET*)
            pResultSet();
    }
};

/**********************************************************
    odbcColumnIterator

    Iterator class to process result set for SQLColumns.
**********************************************************/

class odbcEXPORTED odbcColumnIterator : public odbcSYSINFOITERATOR {
protected:
    /**********************************************************
        szQualifier

        Table qualifier to pass to SQLColumns.
    **********************************************************/

    LPUSTR                                  szQualifier;

    /**********************************************************
        szOwner

        Owner name to pass to SQLColumns.
    **********************************************************/

    LPUSTR                                  szOwner;

    /**********************************************************
        szName

        Table name to pass to SQLColumns.
    **********************************************************/

    LPUSTR                                  szName;

    /**********************************************************
        szColName

        Column name to pass to SQLColumns.
    **********************************************************/

    LPUSTR szColName;

    /**********************************************************
        Set

        Structure for result set.
    **********************************************************/

    sSQLCOLUMNSRESULTSET Set{};
public:
    /**********************************************************
        odbcColumnIterator

        Constructor for iterator class to process result set
        for SQLColumns.
    **********************************************************/

    odbcColumnIterator(
        odbcCONNECT*    pC,
        LPUSTR                  szTableQualifier,
        LPUSTR                  szTableOwner,
        LPUSTR                  szTableName,
        LPUSTR                  szColumnName
    );

    odbcColumnIterator(
        odbcCONNECT*    pC,
        LPSTR                   szTableQualifier,
        LPSTR                   szTableOwner,
        LPSTR                   szTableName,
        LPSTR                   szColumnName
    );

    /**********************************************************
        ~odbcColumnIterator

        Destructor.
    **********************************************************/

    virtual ~odbcColumnIterator();

    /**********************************************************
        DoQuery

        Process query.
    **********************************************************/

    bool  DoQuery() override;

    /**********************************************************
        pColumnsResultSet

        Returns pointer to row data structure for result
        set from SQLColumns.
    **********************************************************/

    virtual const sSQLCOLUMNSRESULTSET* pColumnsResultSet()
    {
        return (const sSQLCOLUMNSRESULTSET*)
            pResultSet();
    }
}; // end odbcColumnIterator class

/**********************************************************
    odbcTableIterator

    Iterator class to process result set for SQLTables.
**********************************************************/

class odbcEXPORTED odbcTableIterator : public odbcSYSINFOITERATOR {
protected:
    /**********************************************************
        szQualifier

        Table qualifier to pass to SQLTables.
    **********************************************************/

    LPUSTR                                  szQualifier;

    /**********************************************************
        szOwner

        Owner name to pass to SQLTables.
    **********************************************************/

    LPUSTR                                  szOwner;

    /**********************************************************
        szName

        Table name to pass to SQLTables.
    **********************************************************/

    LPUSTR                                  szName;

    /**********************************************************
        szType

        Table type to pass to SQLTables.
    **********************************************************/

    LPUSTR                          szType;

    /**********************************************************
        Set

        Structure for result set.
    **********************************************************/

    sSQLTABLESRESULTSET Set{};
public:
    /**********************************************************
        odbcTableIterator

        Constructor for iterator class to process result set
        for SQLTables.
    **********************************************************/

    odbcTableIterator(
        odbcCONNECT*    pC,
        LPUSTR                  szTableQualifier,
        LPUSTR                  szTableOwner,
        LPUSTR                  szTableName,
        LPUSTR                  szTableType
    );

    odbcTableIterator(
        odbcCONNECT*    pC,
        LPSTR                   szTableQualifier,
        LPSTR                   szTableOwner,
        LPSTR                   szTableName,
        LPSTR                   szTableType
    );

    /**********************************************************
        ~odbcTableIterator

        Destructor.
    **********************************************************/

    virtual ~odbcTableIterator();

    /**********************************************************
        DoQuery

        Process query.
    **********************************************************/

    bool  DoQuery() override;

    /**********************************************************
        pTablesResultSet

        Returns pointer to row data structure for result
        set from SQLTables.
    **********************************************************/

    virtual const sSQLTABLESRESULTSET* pTablesResultSet()
    {
        return (const sSQLTABLESRESULTSET*)
            pResultSet();
    }
};

/**********************************************************
    odbcSpecialColumnIterator

    Iterator class to process result set for
    SQLSpecialColumns.
**********************************************************/

class odbcEXPORTED odbcSpecialColumnIterator : public odbcSYSINFOITERATOR {
protected:
    /**********************************************************
        szQualifier

        Table qualifier to pass to SQLSpecialColumns.
    **********************************************************/

    LPUSTR                                  szQualifier;

    /**********************************************************
        szOwner

        Owner name to pass to SQLSpecialColumns.
    **********************************************************/

    LPUSTR                                  szOwner;

    /**********************************************************
        szName

        Table name to pass to SQLSpecialColumns.
    **********************************************************/

    LPUSTR                                  szName;

    /**********************************************************
        Type

        Type argument to pass to SQLSpecialColumns.
    **********************************************************/

    UWORD                                   Type;

    /**********************************************************
        Scope

        Scope argument to pass to SQLSpecialColumns.
    **********************************************************/

    UWORD                                   Scope;

    /**********************************************************
        Nullable

        Nullable argument to pass to SQLSpecialColumns.
    **********************************************************/

    UWORD                                   Nullable;

    /**********************************************************
        Set

        Structure for result set.
    **********************************************************/

    sSQLSPECIALCOLRESULTSET Set{};
public:
    /**********************************************************
        odbcSpecialColumnIterator

        Constructor for iterator class to process result set
        for SQLSpecialColumns.
    **********************************************************/

    odbcSpecialColumnIterator(
        odbcCONNECT*    pC,
        UWORD                   fColType,
        LPUSTR                  szTableQualifier,
        LPUSTR                  szTableOwner,
        LPUSTR                  szTableName,
        UWORD                   fScope,
        UWORD                   fNullable
    );

    odbcSpecialColumnIterator(
        odbcCONNECT*    pC,
        UWORD                   fColType,
        LPSTR                   szTableQualifier,
        LPSTR                   szTableOwner,
        LPSTR                   szTableName,
        UWORD                   fScope,
        UWORD                   fNullable
    );

    /**********************************************************
        ~odbcSpecialColumnIterator

        Destructor.
    **********************************************************/

    virtual ~odbcSpecialColumnIterator();

    /**********************************************************
        DoQuery

        Process query.
    **********************************************************/

    bool  DoQuery() override;

    /**********************************************************
        pSpecialColsResultSet

        Returns pointer to row data structure for result
        set from SQLSpecialColumns.
    **********************************************************/

    virtual const sSQLSPECIALCOLRESULTSET* pSpecialColsResultSet()
    {
        return (const sSQLSPECIALCOLRESULTSET*)
            pResultSet();
    }
}; // end of class odbcSpecialColumnIterator

/**********************************************************
    odbcTypeInfoIterator

    Iterator class to process result set for
    SQLTypeInfo.
**********************************************************/

class odbcEXPORTED odbcTypeInfoIterator : public odbcSYSINFOITERATOR {
protected:

    /**********************************************************
        Set

        Structure for result set.
    **********************************************************/

    sSQLTYPEINFORESULTSET Set{};
public:
    /**********************************************************
        odbcTypeInfoIterator

        Constructor.
    **********************************************************/

    odbcTypeInfoIterator(
        odbcCONNECT*    pC
    );

    /**********************************************************
        ~odbcTypeInfoIterator

        Destructor.
    **********************************************************/

    virtual ~odbcTypeInfoIterator();

    /**********************************************************
        DoQuery

        Process query.
    **********************************************************/

    bool  DoQuery() override;

    /**********************************************************
        pTypeInfoResultSet

        Returns pointer to row data structure for result
        set from SQLTypeInfo.
    **********************************************************/

    virtual const sSQLTYPEINFORESULTSET* pTypeInfoResultSet()
    {
        return (const sSQLTYPEINFORESULTSET*)
            pResultSet();
    }
}; // end of class odbcTypeInfoIterator

// new in v2.0

/**********************************************************
    odbcColumnPrivilegesIterator

    Iterator class to process result set for SQLColumnPrivileges.
**********************************************************/

class odbcEXPORTED odbcColumnPrivilegesIterator :
    public odbcSYSINFOITERATOR {
protected:
    /**********************************************************
        szQualifier

        Table qualifier to pass to SQLColumnPrivileges.
    **********************************************************/

    LPUSTR                                  szQualifier;

    /**********************************************************
        szOwner

        Owner name to pass to SQLColumnPrivileges.
    **********************************************************/

    LPUSTR                                  szOwner;

    /**********************************************************
        szName

        Table name to pass to SQLColumnPrivileges.
    **********************************************************/

    LPUSTR                                  szName;

    /**********************************************************
        szColName

        Column name to pass to SQLColumnPrivileges.
    **********************************************************/

    LPUSTR szColName;

    /**********************************************************
        Set

        Structure for result set.
    **********************************************************/

    sCOLUMNPRIVILEGESRESULTSET Set{};
public:
    /**********************************************************
        odbcColumnPrivilegesIterator

        Constructor for iterator class to process result set
        for SQLColumnPrivileges.
    **********************************************************/

    odbcColumnPrivilegesIterator(
        odbcCONNECT*    pC,
        LPUSTR                  szTableQualifier,
        LPUSTR                  szTableOwner,
        LPUSTR                  szTableName,
        LPUSTR                  szColumnName
    );

    odbcColumnPrivilegesIterator(
        odbcCONNECT*    pC,
        LPSTR                   szTableQualifier,
        LPSTR                   szTableOwner,
        LPSTR                   szTableName,
        LPSTR                   szColumnName
    );

    /**********************************************************
        ~odbcColumnPrivilegesIterator

        Destructor.
    **********************************************************/

    virtual ~odbcColumnPrivilegesIterator();

    /**********************************************************
        DoQuery

        Process query.
    **********************************************************/

    bool  DoQuery() override;

    /**********************************************************
        pColumnPrivilegesResultSet

        Returns pointer to row data structure for result
        set from SQLColumnPrivileges.
    **********************************************************/

    virtual const sCOLUMNPRIVILEGESRESULTSET* pColumnPrivilegesResultSet()
    {
        return (const sCOLUMNPRIVILEGESRESULTSET*)
            pResultSet();
    }
}; // end odbcColumnPrivilegesIterator class

/**********************************************************
    odbcTablePrivilegesIterator

    Iterator class to process result set for SQLTablePrivileges.
**********************************************************/

class odbcEXPORTED odbcTablePrivilegesIterator :
    public odbcSYSINFOITERATOR {
protected:
    /**********************************************************
        szQualifier

        Table qualifier to pass to SQLTablePrivileges.
    **********************************************************/

    LPUSTR                                  szQualifier;

    /**********************************************************
        szOwner

        Owner name to pass to SQLTablePrivileges.
    **********************************************************/

    LPUSTR                                  szOwner;

    /**********************************************************
        szName

        Table name to pass to SQLTablePrivileges.
    **********************************************************/

    LPUSTR                                  szName;

    /**********************************************************
        Set

        Structure for result set.
    **********************************************************/

    sTABLEPRIVILEGESRESULTSET Set{};
public:
    /**********************************************************
        odbcTablePrivilegesIterator

        Constructor for iterator class to process result set
        for SQLTablePrivileges.
    **********************************************************/

    odbcTablePrivilegesIterator(
        odbcCONNECT*    pC,
        LPUSTR                  szTableQualifier,
        LPUSTR                  szTableOwner,
        LPUSTR                  szTableName
    );

    odbcTablePrivilegesIterator(
        odbcCONNECT*    pC,
        LPSTR                   szTableQualifier,
        LPSTR                   szTableOwner,
        LPSTR                   szTableName
    );

    /**********************************************************
        ~odbcTablePrivilegesIterator

        Destructor.
    **********************************************************/

    virtual ~odbcTablePrivilegesIterator();

    /**********************************************************
        DoQuery

        Process query.
    **********************************************************/

    bool  DoQuery() override;

    /**********************************************************
        pTablePrivilegesResultSet

        Returns pointer to row data structure for result
        set from SQLTablePrivileges.
    **********************************************************/

    virtual const sTABLEPRIVILEGESRESULTSET* pTablePrivilegesResultSet()
    {
        return (const sTABLEPRIVILEGESRESULTSET*)
            pResultSet();
    }
}; // end odbcTablePrivilegesIterator class

/**********************************************************
    odbcProceduresIterator

    Iterator class to process result set for SQLProcedures.
**********************************************************/

class odbcEXPORTED odbcProceduresIterator :
    public odbcSYSINFOITERATOR {
protected:
    /**********************************************************
        szQualifier

        Proc qualifier to pass to SQLProcedures.
    **********************************************************/

    LPUSTR                                  szQualifier;

    /**********************************************************
        szOwner

        Owner name to pass to SQLProcedures.
    **********************************************************/

    LPUSTR                                  szOwner;

    /**********************************************************
        szName

        Proc name to pass to SQLProcedures.
    **********************************************************/

    LPUSTR                                  szName;

    /**********************************************************
        Set

        Structure for result set.
    **********************************************************/

    sPROCEDURESRESULTSET Set{};
public:
    /**********************************************************
        odbcProceduresIterator

        Constructor for iterator class to process result set
        for SQLProcedures.
    **********************************************************/

    odbcProceduresIterator(
        odbcCONNECT*    pC,
        LPUSTR                  szProcQualifier,
        LPUSTR                  szProcOwner,
        LPUSTR                  szProcName
    );

    odbcProceduresIterator(
        odbcCONNECT*    pC,
        LPSTR                   szProcQualifier,
        LPSTR                   szProcOwner,
        LPSTR                   szProcName
    );

    /**********************************************************
        ~odbcProceduresIterator

        Destructor.
    **********************************************************/

    virtual ~odbcProceduresIterator();

    /**********************************************************
        DoQuery

        Process query.
    **********************************************************/

    bool  DoQuery() override;

    /**********************************************************
        pProceduresResultSet

        Returns pointer to row data structure for result
        set from SQLProcedures.
    **********************************************************/

    virtual const sPROCEDURESRESULTSET* pProceduresResultSet()
    {
        return (const sPROCEDURESRESULTSET*)
            pResultSet();
    }
}; // end odbcProceduresIterator class

/**********************************************************
    odbcProcedureColumnsIterator

    Iterator class to process result set for SQLProcedureColumns.
**********************************************************/

class odbcEXPORTED odbcProcedureColumnsIterator :
    public odbcSYSINFOITERATOR {
protected:
    /**********************************************************
        szQualifier

        Proc qualifier to pass to SQLProcedureColumns.
    **********************************************************/

    LPUSTR                                  szQualifier;

    /**********************************************************
        szOwner

        Owner name to pass to SQLProcedureColumns.
    **********************************************************/

    LPUSTR                                  szOwner;

    /**********************************************************
        szName

        Proc name to pass to SQLProcedureColumns.
    **********************************************************/

    LPUSTR                                  szName;

    /**********************************************************
        szColName

        Column name to pass to SQLColumnPrivileges.
    **********************************************************/

    LPUSTR                  szColName;

    /**********************************************************
        Set

        Structure for result set.
    **********************************************************/

    sPROCEDURECOLUMNSRESULTSET Set{};
public:
    /**********************************************************
        odbcProcedureColumnsIterator

        Constructor for iterator class to process result set
        for SQLProcedureColumns.
    **********************************************************/

    odbcProcedureColumnsIterator(
        odbcCONNECT*    pC,
        LPUSTR                  szProcQualifier,
        LPUSTR                  szProcOwner,
        LPUSTR                  szProcName,
        LPUSTR                  szProcColumn
    );

    odbcProcedureColumnsIterator(
        odbcCONNECT*    pC,
        LPSTR           szProcQualifier,
        LPSTR           szProcOwner,
        LPSTR           szProcName,
        LPSTR           szProcColumn
    );

    /**********************************************************
        ~odbcProcedureColumnsIterator

        Destructor.
    **********************************************************/

    virtual ~odbcProcedureColumnsIterator();

    /**********************************************************
        DoQuery

        Process query.
    **********************************************************/

    bool  DoQuery() override;

    /**********************************************************
        pProcedureColumnsResultSet

        Returns pointer to row data structure for result
        set from SQLProcedureColumns.
    **********************************************************/

    virtual const sPROCEDURECOLUMNSRESULTSET* pProcedureColumnsResultSet()
    {
        return (const sPROCEDURECOLUMNSRESULTSET*)
            pResultSet();
    }
}; // end odbcProcedureColumnsIterator class

/**********************************************************
    odbcPrimaryKeysIterator

    Iterator class to process result set for SQLPrimaryKeys.
**********************************************************/

class odbcEXPORTED odbcPrimaryKeysIterator :
    public odbcSYSINFOITERATOR {
protected:
    /**********************************************************
        szQualifier

        Table qualifier to pass to SQLPrimaryKeys.
    **********************************************************/

    LPUSTR                                  szQualifier;

    /**********************************************************
        szOwner

        Owner name to pass to SQLPrimaryKeys.
    **********************************************************/

    LPUSTR                                  szOwner;

    /**********************************************************
        szName

        Table name to pass to SQLPrimaryKeys.
    **********************************************************/

    LPUSTR                                  szName;

    /**********************************************************
        Set

        Structure for result set.
    **********************************************************/

    sPRIMARYKEYSRESULTSET Set{};
public:
    /**********************************************************
        odbcPrimaryKeysIterator

        Constructor for iterator class to process result set
        for SQLPrimaryKeys.
    **********************************************************/

    odbcPrimaryKeysIterator(
        odbcCONNECT*    pC,
        LPUSTR                  szTableQualifier,
        LPUSTR                  szTableOwner,
        LPUSTR                  szTableName
    );

    odbcPrimaryKeysIterator(
        odbcCONNECT*    pC,
        LPSTR           szTableQualifier,
        LPSTR           szTableOwner,
        LPSTR           szTableName
    );

    /**********************************************************
        ~odbcPrimaryKeysIterator

        Destructor.
    **********************************************************/

    virtual ~odbcPrimaryKeysIterator();

    /**********************************************************
        DoQuery

        Process query.
    **********************************************************/

    bool  DoQuery() override;

    /**********************************************************
        pPrimaryKeysResultSet

        Returns pointer to row data structure for result
        set from SQLPrimaryKeys.
    **********************************************************/

    virtual const sPRIMARYKEYSRESULTSET* pPrimaryKeysResultSet()
    {
        return (const sPRIMARYKEYSRESULTSET*)
            pResultSet();
    }
}; // end odbcPrimaryKeysIterator class

/**********************************************************
    odbcForeignKeysIterator

    Iterator class to process result set for SQLForeignKeys.
**********************************************************/

class odbcEXPORTED odbcForeignKeysIterator :
    public odbcSYSINFOITERATOR {
protected:
    /**********************************************************
        szPkQualifier

        Primary key table qualifier to pass to SQLForeignKeys.
    **********************************************************/

    LPUSTR                                  szPkQualifier;

    /**********************************************************
        szPkOwner

        Primary key table owner name to pass to SQLForeignKeys.
    **********************************************************/

    LPUSTR                                  szPkOwner;

    /**********************************************************
        szPkName

        Primary key table name to pass to SQLForeignKeys.
    **********************************************************/

    LPUSTR                                  szPkName;

    /**********************************************************
        szFkQualifier

        Foreign key table qualifier to pass to SQLForeignKeys.
    **********************************************************/

    LPUSTR                                  szFkQualifier;

    /**********************************************************
        szFkOwner

        Foreign key table owner name to pass to SQLForeignKeys.
    **********************************************************/

    LPUSTR                                  szFkOwner;

    /**********************************************************
        szFkName

        Foreign key table name to pass to SQLForeignKeys.
    **********************************************************/

    LPUSTR                                  szFkName;

    /**********************************************************
        Set

        Structure for result set.
    **********************************************************/

    sFOREIGNKEYSRESULTSET Set{};
public:
    /**********************************************************
        odbcForeignKeysIterator

        Constructor for iterator class to process result set
        for SQLForeignKeys.
    **********************************************************/

    odbcForeignKeysIterator(
        odbcCONNECT*    pC,
        LPUSTR                  szPkTableQualifier,
        LPUSTR                  szPkTableOwner,
        LPUSTR                  szPkTableName,
        LPUSTR                  szFkTableQualifier,
        LPUSTR                  szFkTableOwner,
        LPUSTR                  szFkTableName
    );

    odbcForeignKeysIterator(
        odbcCONNECT*    pC,
        LPSTR           szPkTableQualifier,
        LPSTR           szPkTableOwner,
        LPSTR           szPkTableName,
        LPSTR           szFkTableQualifier,
        LPSTR           szFkTableOwner,
        LPSTR           szFkTableName
    );

    /**********************************************************
        ~odbcForeignKeysIterator

        Destructor.
    **********************************************************/

    virtual ~odbcForeignKeysIterator();

    /**********************************************************
        DoQuery

        Process query.
    **********************************************************/

    bool  DoQuery() override;

    /**********************************************************
        pForeignKeysResultSet

        Returns pointer to row data structure for result
        set from SQLForeignKeys.
    **********************************************************/

    virtual const sFOREIGNKEYSRESULTSET* pForeignKeysResultSet()
    {
        return (const sFOREIGNKEYSRESULTSET*)
            pResultSet();
    }
}; // end odbcForeignKeysIterator class


// end new in v2.0
