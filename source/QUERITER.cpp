#include <sql.hpp>

/**********************************************************
odbcSYSINFOITERATOR

Constructor for iterator class to iterate result set.
**********************************************************/

odbcSYSINFOITERATOR::odbcSYSINFOITERATOR(odbcCONNECT*   pC)
    : odbcCURSOR(pC)
{
    bCanScroll = false;
}

/**********************************************************
    ~odbcSYSINFOITERATOR

    Destructor.
**********************************************************/

odbcSYSINFOITERATOR:: ~odbcSYSINFOITERATOR() = default;

/**********************************************************
DoQuery

Process query.
**********************************************************/

bool  odbcSYSINFOITERATOR::DoQuery()
{
    if (pConn->CursorsCanScroll())
    {
        UWORD bSaveReport = AutoReport(odbcNOREPORT);
        UWORD bSaveRetrieve = AutoRetrieve(odbcNOREPORT);

        UDWORD ScrollOptions = 0;
        pConn->GetInfo(SQL_SCROLL_OPTIONS,
            &ScrollOptions,
            sizeof(UDWORD),
            nullptr);

        SetRC(SQL_SUCCESS);

        // we favor keyset-driven cursors over static...
        if (ScrollOptions & SQL_SO_KEYSET_DRIVEN)
        {
            // set up for extended fetch operations
            ExtFetchSetup(SQL_CONCUR_VALUES,
                SQL_CURSOR_KEYSET_DRIVEN,
                1
            );
        }
        // if we didn't succeed with the keyset-driven cursor,
        // or we didn't try because keyset-driven cursors weren't supported,
        // try a static cursor using the ODBC 2.0 constants
        if (
            (!sqlsuccess() ||
            (!(ScrollOptions & SQL_SO_KEYSET_DRIVEN))
                )
            && (ScrollOptions
                & SQL_SO_STATIC))
        {
            ExtFetchSetup(SQL_CONCUR_VALUES,
                SQL_CURSOR_STATIC,
                1
            );
        }

        // if we didn't succeed with the static or keyset-driven cursor,
        // but keyset-driven cursors are known to be possible,
        // try a keyset-driven cursor using the deprecated (ODBC v1.0)
        // value
        if (!sqlsuccess() && (ScrollOptions & SQL_SO_KEYSET_DRIVEN))
        {
            ExtFetchSetup(SQL_CONCUR_VALUES,
                SQL_SCROLL_KEYSET_DRIVEN,
                1
            );
        }
        else if (!sqlsuccess())
            SetRC(SQL_ERROR);

        // turn back on the error handling
        AutoReport(bSaveReport);
        AutoRetrieve(bSaveRetrieve);
        bCanScroll = sqlsuccess();
        SetRC(SQL_SUCCESS);
    }
    else
        bCanScroll = false;

    bFirst = true;

    return sqlsuccess();
}

/***************************************************

    GetFirst

    Call Fetch() to get first row in the result set
    (or ExtFetchFirst() to get first rowset in result set).
    Returns non-zero value if it succeeds, zero if
    it fails.
***************************************************/

bool odbcSYSINFOITERATOR::GetFirst()
{
    if (bError)
        return false;

    if (bCanScroll)
        ExtFetchFirst();
    else
    {
        if (!bFirst)
        {
            Close();
            DoQuery();
        }
        Fetch();
        bFirst = false;
    }
    return sqlsuccess();
}

/***************************************************

    GetNext

    Call Fetch() to get next row in the result set
    (or ExtFetchNext() to get next rowset in result set).
    Returns non-zero value if it succeeds, zero if
    it fails.
***************************************************/

bool odbcSYSINFOITERATOR::GetNext()
{
    if (bError)
        return false;

    if (bCanScroll)
        ExtFetchNext();
    else
        Fetch();
    bFirst = false;
    return sqlsuccess();
}

/***************************************************

    GetLast

    Call Fetch() to get last rowset in the result set.
    Only valid if extended fetch operations are in use.
    Returns non-zero value if it succeeds, zero if
    it fails.
***************************************************/

bool odbcSYSINFOITERATOR::GetLast()
{
    if (bError)
        return false;

    if (bCanScroll)
        ExtFetchLast();
    else
        return false;
    return sqlsuccess();
}

/***************************************************

    GetPrev

    Call ExtFetchPrev() to get prior rowset in the result set.
    Only valid if extended fetch operations are in use.
    Returns non-zero value if it succeeds, zero if
    it fails.
***************************************************/

bool odbcSYSINFOITERATOR::GetPrev()
{
    if (bError)
        return false;

    if (bCanScroll)
        ExtFetchPrevious();
    else
        return false;
    return sqlsuccess();
}

/**********************************************************
    ~odbcTableIterator

    Destructor.
**********************************************************/

odbcTableIterator::~odbcTableIterator() = default;

/**********************************************************
    odbcTableIterator

    Constructor for iterator class to process result set
    for SQLTables.
**********************************************************/

odbcTableIterator::odbcTableIterator(
    odbcCONNECT*    pC,
    LPUSTR                  szTableQualifier,
    LPUSTR                  szTableOwner,
    LPUSTR                  szTableName,
    LPUSTR                  szTableType
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = szTableQualifier;
    szOwner = szTableOwner;
    szName = szTableName;
    szType = szTableType;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLTablesRSB(),
            pC->SQLTablesRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

odbcTableIterator::odbcTableIterator(
    odbcCONNECT*    pC,
    LPSTR                   szTableQualifier,
    LPSTR                   szTableOwner,
    LPSTR                   szTableName,
    LPSTR                   szTableType
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = (LPUSTR)szTableQualifier;
    szOwner = (LPUSTR)szTableOwner;
    szName = (LPUSTR)szTableName;
    szType = (LPUSTR)szTableType;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLTablesRSB(),
            pC->SQLTablesRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

/**********************************************************
    DoQuery

    Process query.
**********************************************************/

bool odbcTableIterator::DoQuery()
{
    odbcSYSINFOITERATOR::DoQuery();

    Tables
    (
        szQualifier,
        szOwner,
        szName,
        szType
    );

    return sqlsuccess();
}

/**********************************************************
    ~odbcStatisticsIterator

    Destructor.
**********************************************************/

odbcStatisticsIterator::~odbcStatisticsIterator() = default;

/**********************************************************
    odbcStatisticsIterator

    Constructor for iterator class to process result set
    for SQLStatistics.
**********************************************************/

odbcStatisticsIterator::odbcStatisticsIterator(
    odbcCONNECT*    pC,
    LPUSTR                  szTableQualifier,
    LPUSTR                  szTableOwner,
    LPUSTR                  szTableName,
    UWORD                   fTblUnique,
    UWORD                   fTblAccuracy
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = szTableQualifier;
    szOwner = szTableOwner;
    szName = szTableName;
    fUnique = fTblUnique;
    fAccuracy = fTblAccuracy;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLStatisticsRSB(),
            pC->SQLStatisticsRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

odbcStatisticsIterator::odbcStatisticsIterator(
    odbcCONNECT*    pC,
    LPSTR                   szTableQualifier,
    LPSTR                   szTableOwner,
    LPSTR                   szTableName,
    UWORD                   fTblUnique,
    UWORD                   fTblAccuracy
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = (LPUSTR)szTableQualifier;
    szOwner = (LPUSTR)szTableOwner;
    szName = (LPUSTR)szTableName;
    fUnique = fTblUnique;
    fAccuracy = fTblAccuracy;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLStatisticsRSB(),
            pC->SQLStatisticsRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

/**********************************************************
    DoQuery

    Process query.
**********************************************************/

bool odbcStatisticsIterator::DoQuery()
{
    odbcSYSINFOITERATOR::DoQuery();

    Statistics
    (
        szQualifier,
        szOwner,
        szName,
        fUnique,
        fAccuracy
    );

    return sqlsuccess();
}

/**********************************************************
    ~odbcColumnIterator

    Destructor.
**********************************************************/

odbcColumnIterator::~odbcColumnIterator() = default;

/**********************************************************
    odbcColumnIterator

    Constructor for iterator class to process result set
    for SQLColumns.
**********************************************************/

odbcColumnIterator::odbcColumnIterator(
    odbcCONNECT* pC,
    LPUSTR szTableQualifier,
    LPUSTR szTableOwner,
    LPUSTR szTableName,
    LPUSTR szColumnName
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = szTableQualifier;
    szOwner = szTableOwner;
    szName = szTableName;
    szColName = szColumnName;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLColumnsRSB(),
            pC->SQLColumnsRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

odbcColumnIterator::odbcColumnIterator(
    odbcCONNECT* pC,
    LPSTR szTableQualifier,
    LPSTR szTableOwner,
    LPSTR szTableName,
    LPSTR szColumnName
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = (LPUSTR)szTableQualifier;
    szOwner = (LPUSTR)szTableOwner;
    szName = (LPUSTR)szTableName;
    szColName = (LPUSTR)szColumnName;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLColumnsRSB(),
            pC->SQLColumnsRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

/**********************************************************
    DoQuery

    Process query.
**********************************************************/

bool odbcColumnIterator::DoQuery()
{
    odbcSYSINFOITERATOR::DoQuery();

    Columns
    (
        szQualifier,
        szOwner,
        szName,
        szColName
    );

    return sqlsuccess();
}

/**********************************************************
    ~odbcSpecialColumnIterator

    Destructor.
**********************************************************/

odbcSpecialColumnIterator::~odbcSpecialColumnIterator() = default;

/**********************************************************
    odbcSpecialColumnIterator

    Constructor for iterator class to process result set
    for SQLSpecialColumns.
**********************************************************/

odbcSpecialColumnIterator::odbcSpecialColumnIterator(
    odbcCONNECT*    pC,
    UWORD                   fColType,
    LPUSTR                  szTableQualifier,
    LPUSTR                  szTableOwner,
    LPUSTR                  szTableName,
    UWORD                   fScope,
    UWORD                   fNullable
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = szTableQualifier;
    szOwner = szTableOwner;
    szName = szTableName;
    Nullable = fNullable;
    Type = fColType;
    Scope = fScope;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLSpecialColumnsRSB(),
            pC->SQLSpecialColumnsRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

odbcSpecialColumnIterator::odbcSpecialColumnIterator(
    odbcCONNECT*    pC,
    UWORD                   fColType,
    LPSTR                   szTableQualifier,
    LPSTR                   szTableOwner,
    LPSTR                   szTableName,
    UWORD                   fScope,
    UWORD                   fNullable
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = (LPUSTR)szTableQualifier;
    szOwner = (LPUSTR)szTableOwner;
    szName = (LPUSTR)szTableName;
    Nullable = fNullable;
    Type = fColType;
    Scope = fScope;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLSpecialColumnsRSB(),
            pC->SQLSpecialColumnsRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

/**********************************************************
    DoQuery

    Process query.
**********************************************************/

bool odbcSpecialColumnIterator::DoQuery()
{
    odbcSYSINFOITERATOR::DoQuery();

    SpecialColumns
    (
        Type,
        szQualifier,
        szOwner,
        szName,
        Scope,
        Nullable
    );

    return sqlsuccess();
}

/**********************************************************
    ~odbcTypeInfoIterator

    Destructor.
**********************************************************/

odbcTypeInfoIterator::~odbcTypeInfoIterator() = default;

/**********************************************************
    odbcTypeInfoIterator

    Constructor.
**********************************************************/

odbcTypeInfoIterator::odbcTypeInfoIterator(
    odbcCONNECT*    pC
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLTypeInfoRSB(),
            pC->SQLTypeInfoRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

/**********************************************************
    DoQuery

    Process query.
**********************************************************/

bool odbcTypeInfoIterator::DoQuery()
{
    odbcSYSINFOITERATOR::DoQuery();

    GetTypeInfo
    (
        SQL_ALL_TYPES
    );

    return sqlsuccess();
}

// new in v2.0

/**********************************************************
    ~odbcColumnPrivilegesIterator

    Destructor.
**********************************************************/

odbcColumnPrivilegesIterator::~odbcColumnPrivilegesIterator() = default;

/**********************************************************
    odbcColumnPrivilegesIterator

    Constructor for iterator class to process result set
    for SQLColumnPrivileges.
**********************************************************/

odbcColumnPrivilegesIterator::odbcColumnPrivilegesIterator(
    odbcCONNECT* pC,
    LPUSTR szTableQualifier,
    LPUSTR szTableOwner,
    LPUSTR szTableName,
    LPUSTR szColumnName
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = szTableQualifier;
    szOwner = szTableOwner;
    szName = szTableName;
    szColName = szColumnName;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLColumnPrivilegesRSB(),
            pC->SQLColumnPrivilegesRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

odbcColumnPrivilegesIterator::odbcColumnPrivilegesIterator(
    odbcCONNECT* pC,
    LPSTR szTableQualifier,
    LPSTR szTableOwner,
    LPSTR szTableName,
    LPSTR szColumnName
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = (LPUSTR)szTableQualifier;
    szOwner = (LPUSTR)szTableOwner;
    szName = (LPUSTR)szTableName;
    szColName = (LPUSTR)szColumnName;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLColumnPrivilegesRSB(),
            pC->SQLColumnPrivilegesRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

/**********************************************************
    DoQuery

    Process query.
**********************************************************/

bool odbcColumnPrivilegesIterator::DoQuery()
{
    odbcSYSINFOITERATOR::DoQuery();

    ColumnPrivileges
    (
        szQualifier,
        szOwner,
        szName,
        szColName
    );

    return sqlsuccess();
}

/**********************************************************
    ~odbcTablePrivilegesIterator

    Destructor.
**********************************************************/

odbcTablePrivilegesIterator::~odbcTablePrivilegesIterator() = default;

/**********************************************************
    odbcTablePrivilegesIterator

    Constructor for iterator class to process result set
    for SQLTablePrivileges.
**********************************************************/

odbcTablePrivilegesIterator::odbcTablePrivilegesIterator(
    odbcCONNECT* pC,
    LPUSTR szTableQualifier,
    LPUSTR szTableOwner,
    LPUSTR szTableName
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = szTableQualifier;
    szOwner = szTableOwner;
    szName = szTableName;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLTablePrivilegesRSB(),
            pC->SQLTablePrivilegesRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

odbcTablePrivilegesIterator::odbcTablePrivilegesIterator(
    odbcCONNECT* pC,
    LPSTR szTableQualifier,
    LPSTR szTableOwner,
    LPSTR szTableName
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = (LPUSTR)szTableQualifier;
    szOwner = (LPUSTR)szTableOwner;
    szName = (LPUSTR)szTableName;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLTablePrivilegesRSB(),
            pC->SQLTablePrivilegesRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

/**********************************************************
    DoQuery

    Process query.
**********************************************************/

bool odbcTablePrivilegesIterator::DoQuery()
{
    odbcSYSINFOITERATOR::DoQuery();

    TablePrivileges
    (
        szQualifier,
        szOwner,
        szName
    );

    return sqlsuccess();
}

/**********************************************************
    ~odbcProceduresIterator

    Destructor.
**********************************************************/

odbcProceduresIterator::~odbcProceduresIterator() = default;

/**********************************************************
    odbcProceduresIterator

    Constructor for iterator class to process result set
    for SQLProcedures.
**********************************************************/

odbcProceduresIterator::odbcProceduresIterator(
    odbcCONNECT* pC,
    LPUSTR szProcQualifier,
    LPUSTR szProcOwner,
    LPUSTR szProcName
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = szProcQualifier;
    szOwner = szProcOwner;
    szName = szProcName;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLProceduresRSB(),
            pC->SQLProceduresRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

odbcProceduresIterator::odbcProceduresIterator(
    odbcCONNECT* pC,
    LPSTR szProcQualifier,
    LPSTR szProcOwner,
    LPSTR szProcName
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = (LPUSTR)szProcQualifier;
    szOwner = (LPUSTR)szProcOwner;
    szName = (LPUSTR)szProcName;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLProceduresRSB(),
            pC->SQLProceduresRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

/**********************************************************
    DoQuery

    Process query.
**********************************************************/

bool odbcProceduresIterator::DoQuery()
{
    odbcSYSINFOITERATOR::DoQuery();

    Procedures
    (
        szQualifier,
        szOwner,
        szName
    );

    return sqlsuccess();
}

/**********************************************************
    ~odbcProcedureColumnsIterator

    Destructor.
**********************************************************/

odbcProcedureColumnsIterator::~odbcProcedureColumnsIterator() = default;

/**********************************************************
    odbcProcedureColumnsIterator

    Constructor for iterator class to process result set
    for SQLProcedureColumns.
**********************************************************/

odbcProcedureColumnsIterator::odbcProcedureColumnsIterator(
    odbcCONNECT* pC,
    LPUSTR szProcQualifier,
    LPUSTR szProcOwner,
    LPUSTR szProcName,
    LPUSTR szProcColumn
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = szProcQualifier;
    szOwner = szProcOwner;
    szName = szProcName;
    szColName = szProcColumn;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLProcedureColumnsRSB(),
            pC->SQLProcedureColumnsRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

odbcProcedureColumnsIterator::odbcProcedureColumnsIterator(
    odbcCONNECT* pC,
    LPSTR szProcQualifier,
    LPSTR szProcOwner,
    LPSTR szProcName,
    LPSTR szProcColumn
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = (LPUSTR)szProcQualifier;
    szOwner = (LPUSTR)szProcOwner;
    szName = (LPUSTR)szProcName;
    szColName = (LPUSTR)szProcColumn;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLProcedureColumnsRSB(),
            pC->SQLProcedureColumnsRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

/**********************************************************
    DoQuery

    Process query.
**********************************************************/

bool odbcProcedureColumnsIterator::DoQuery()
{
    odbcSYSINFOITERATOR::DoQuery();

    ProcedureColumns
    (
        szQualifier,
        szOwner,
        szName,
        szColName
    );

    return sqlsuccess();
}

/**********************************************************
    ~odbcPrimaryKeysIterator

    Destructor.
**********************************************************/

odbcPrimaryKeysIterator::~odbcPrimaryKeysIterator() = default;

/**********************************************************
    odbcPrimaryKeysIterator

    Constructor for iterator class to process result set
    for SQLPrimaryKeys.
**********************************************************/

odbcPrimaryKeysIterator::odbcPrimaryKeysIterator(
    odbcCONNECT* pC,
    LPUSTR szTableQualifier,
    LPUSTR szTableOwner,
    LPUSTR szTableName
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = szTableQualifier;
    szOwner = szTableOwner;
    szName = szTableName;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLPrimaryKeysRSB(),
            pC->SQLPrimaryKeysRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

odbcPrimaryKeysIterator::odbcPrimaryKeysIterator(
    odbcCONNECT* pC,
    LPSTR szTableQualifier,
    LPSTR szTableOwner,
    LPSTR szTableName
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szQualifier = (LPUSTR)szTableQualifier;
    szOwner = (LPUSTR)szTableOwner;
    szName = (LPUSTR)szTableName;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLPrimaryKeysRSB(),
            pC->SQLPrimaryKeysRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

/**********************************************************
    DoQuery

    Process query.
**********************************************************/

bool odbcPrimaryKeysIterator::DoQuery()
{
    odbcSYSINFOITERATOR::DoQuery();

    PrimaryKeys
    (
        szQualifier,
        szOwner,
        szName
    );

    return sqlsuccess();
}

/**********************************************************
    ~odbcForeignKeysIterator

    Destructor.
**********************************************************/

odbcForeignKeysIterator::~odbcForeignKeysIterator() = default;

/**********************************************************
    odbcForeignKeysIterator

    Constructor for iterator class to process result set
    for SQLForeignKeys.
**********************************************************/

odbcForeignKeysIterator::odbcForeignKeysIterator(
    odbcCONNECT* pC,
    LPUSTR szPkTableQualifier,
    LPUSTR szPkTableOwner,
    LPUSTR szPkTableName,
    LPUSTR szFkTableQualifier,
    LPUSTR szFkTableOwner,
    LPUSTR szFkTableName
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szPkQualifier = szPkTableQualifier;
    szPkOwner = szPkTableOwner;
    szPkName = szPkTableName;
    szFkQualifier = szFkTableQualifier;
    szFkOwner = szFkTableOwner;
    szFkName = szFkTableName;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLForeignKeysRSB(),
            pC->SQLForeignKeysRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

odbcForeignKeysIterator::odbcForeignKeysIterator(
    odbcCONNECT* pC,
    LPSTR szPkTableQualifier,
    LPSTR szPkTableOwner,
    LPSTR szPkTableName,
    LPSTR szFkTableQualifier,
    LPSTR szFkTableOwner,
    LPSTR szFkTableName
) :
    odbcSYSINFOITERATOR
    (
        pC
    )
{
    szPkQualifier = (LPUSTR)szPkTableQualifier;
    szPkOwner = (LPUSTR)szPkTableOwner;
    szPkName = (LPUSTR)szPkTableName;
    szFkQualifier = (LPUSTR)szFkTableQualifier;
    szFkOwner = (LPUSTR)szFkTableOwner;
    szFkName = (LPUSTR)szFkTableName;
    if (sqlsuccess())
    {
        SetColBindings
        (
            pC->SQLForeignKeysRSB(),
            pC->SQLForeignKeysRSBCount(),
            &Set
        );
        if (!bError)
        {
            QueryAndBind();
        }
    }
}

/**********************************************************
    DoQuery

    Process query.
**********************************************************/

bool odbcForeignKeysIterator::DoQuery()
{
    odbcSYSINFOITERATOR::DoQuery();

    ForeignKeys
    (
        szPkQualifier,
        szPkOwner,
        szPkName,
        szFkQualifier,
        szFkOwner,
        szFkName
    );

    return sqlsuccess();
}


// end new in v2.0
