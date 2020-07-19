#pragma once

class odbcEXPORTED odbcTABLECREATOR : public odbcCURSOR {
private:
    // don't allow copy
    odbcTABLECREATOR & operator=(odbcTABLECREATOR &);
    odbcTABLECREATOR(odbcTABLECREATOR &);

protected:
    char *lpszCreateStmt_{};
    char *token_{};
    /***************************************************

        szTab

        String to use as tabs in CREATE TABLE statements.
    ***************************************************/

    LPCSTR   szTab{};
    /***************************************************

        szEoln

        String to use as end-of-line in CREATE TABLE statements.
    ***************************************************/

    LPCSTR   szEoln{};

    /***************************************************

        ConvertCreateColumnType

        Convert user-passed standard column type name to
        a data-source specific name on this connection.
    ***************************************************/

    virtual RETCODE ConvertCreateColumnType(
        LPSTR      lpszCreateStmt,
        size_t *   pi,
        LPCSTR     token,
        UDWORD     cbColDef,
        SWORD      ibScale);

    virtual RETCODE ConvertCreateColumnType(
        LPUSTR     lpszCreateStmt,
        size_t *   pi,
        LPUCSTR    token,
        UDWORD     ibColDef,
        SWORD      ibScale)
    {
        return ConvertCreateColumnType(
            (LPSTR)lpszCreateStmt,
            pi,
            (LPCSTR)token,
            ibColDef,
            ibScale);
    }

    virtual RETCODE ConvertCreateColumnType(
        LPSTR      lpszCreateStmt,
        size_t *   pi,
        LPCSTR     token,
        SWORD      fType,
        UDWORD     cbColDef,
        SWORD      ibScale);

    virtual RETCODE ConvertCreateColumnType(
        LPUSTR     lpszCreateStmt,
        size_t *   pi,
        LPUCSTR    token,
        SWORD      fType,
        UDWORD     ibColDef,
        SWORD      ibScale)
    {
        return ConvertCreateColumnType(
            (LPSTR)lpszCreateStmt,
            pi,
            (LPCSTR)token,
            fType,
            ibColDef,
            ibScale);
    }
    /***************************************************

        ColumnTypeFromToken

        Convert user-passed standard column type name to
        an ODBC SQL_xxxx data type; also set precision to
        minimum value if equal to zero.
    ***************************************************/

    virtual RETCODE ColumnTypeFromToken(
        LPCSTR      lpszToken,
        SWORD     * pfType,
        UDWORD     *pfPrecision);

    virtual RETCODE ColumnTypeFromToken(
        LPUCSTR     lpszToken,
        SWORD     * pfType,
        UDWORD     *pfPrecision)
    {
        return
            ColumnTypeFromToken(
            (LPCSTR)lpszToken,
                pfType,
                pfPrecision);

    }

    /***************************************************

        GetCreateTypeName

        GetCreateTypeName takes an ODBC SQL type and returns the name as
        a string for use in CreateTable.
    ***************************************************/

    virtual LPCSTR GetCreateTypeName(SWORD fSqlType);

public:
    /***************************************************

        odbcTABLECREATOR

        Constructor.
    ***************************************************/

    odbcTABLECREATOR(odbcCONNECT* pConnect);

    /***************************************************

        ~odbcTABLECREATOR

        Destructor.
    ***************************************************/

    virtual ~odbcTABLECREATOR();

    /***************************************************

        SetszTab

        Set string to use as tabs in CREATE TABLE statements.
    ***************************************************/

    LPCSTR   SetszTab(LPCSTR szNew)
    {
        LPCSTR szRet = szTab;
        szTab = szNew;
        return szRet;
    }

    /***************************************************

        SetszEoln

        Set string to use as end-of-line in CREATE TABLE statements.
    ***************************************************/

    LPCSTR   SetszEoln(LPCSTR szNew)
    {
        LPCSTR szRet = szEoln;
        szEoln = szNew;
        return szRet;
    }


    /***************************************************

        CreateTable

        Create a table (or just generate a CREATE TABLE
        SQL statement) from a passed SQL statement template
        using the driver-independent type names.
    ***************************************************/

    virtual RETCODE CreateTable(
        LPCSTR       lpszSqlIn,
        LPSTR        lpszSqlStmtCreated = NULL,
        UWORD *      puStmtBufferSize = NULL,
        bool         bJustGenerateSql = false
    );

    virtual RETCODE CreateTable(
        LPUCSTR      lpszSqlIn,
        LPUSTR       lpszSqlStmtCreated = NULL,
        UWORD *      puStmtBufferSize = NULL,
        bool         bJustGenerateSql = false
    )
    {
        return  CreateTable
        (
            (LPCSTR)lpszSqlIn,
            (LPSTR)lpszSqlStmtCreated,
            puStmtBufferSize,
            bJustGenerateSql
        );
    }

    /***************************************************

        CreateTable

        Create a table (or just generate a CREATE TABLE
        SQL statement) from an array of sCOLBIND structs.
    ***************************************************/

    virtual RETCODE CreateTable(
        LPCSTR       lpszTableName,
        const sCOLBIND*   cpsColCreate,
        UWORD        uColCount,
        LPCSTR       lpszTableConstraints = NULL,
        LPSTR        lpszSqlStmtCreated = NULL,
        UWORD *      puStmtBufferSize = NULL,
        bool         bJustGenerateSql = false,
        bool         bNoExtraByteOnStrings = true,
        bool         bResolveDupNames = true
    );

    virtual RETCODE CreateTable(
        LPUCSTR      lpszTableName,
        const sCOLBIND*   cpsColInfo,
        UWORD        uColCount,
        LPUCSTR      lpszTableConstraints = NULL,
        LPUSTR       lpszSqlStmtCreated = NULL,
        UWORD *      puStmtBufferSize = NULL,
        bool         bJustGenerateSql = false,
        bool         bNoExtraByteOnStrings = true,
        bool         bResolveDupNames = true
    )
    {
        return CreateTable(
            (LPCSTR)lpszTableName,
            cpsColInfo,
            uColCount,
            (LPCSTR)lpszTableConstraints,
            (LPSTR)lpszSqlStmtCreated,
            puStmtBufferSize,
            bJustGenerateSql,
            bNoExtraByteOnStrings,
            bResolveDupNames
        );
    }
    /***************************************************

        CreateTable

        Create a table (or just generate a CREATE TABLE
        SQL statement) from an odbcCURSOR object.
    ***************************************************/

    virtual RETCODE CreateTable(
        LPCSTR       lpszTableName,
        odbcCURSOR*  pCursor,
        LPCSTR       lpszTableConstraints = NULL,
        LPSTR        lpszSqlStmtCreated = NULL,
        UWORD *      puStmtBufferSize = NULL,
        bool         bJustGenerateSql = false,
        bool         bResolveDupNames = true
    )
    {
        const sCOLBIND* cpsColBind = NULL;
        UWORD      uColCount = 0;
        if (pCursor && !IsBadReadPtr(pCursor, sizeof(*pCursor)))
        {
            cpsColBind = pCursor->ColResultInfo(1);
            uColCount = pCursor->NumResultCols();
        }

        if (uColCount && cpsColBind)
        {
            return CreateTable
            (
                lpszTableName,
                cpsColBind,
                uColCount,
                lpszTableConstraints,
                lpszSqlStmtCreated,
                puStmtBufferSize,
                bJustGenerateSql,
                pCursor->GetNoExtraByteOnStrings(),
                bResolveDupNames
            );
        }
        else
            return SQL_BAD_CURSOR;
    }

    virtual RETCODE CreateTable(
        LPUCSTR      lpszTableName,
        odbcCURSOR*  pCursor,
        LPUCSTR      lpszTableConstraints = NULL,
        LPUSTR       lpszSqlStmtCreated = NULL,
        UWORD *      puStmtBufferSize = NULL,
        bool         bJustGenerateSql = false,
        bool         bResolveDupNames = true
    )
    {
        return CreateTable(
            (LPCSTR)lpszTableName,
            pCursor,
            (LPCSTR)lpszTableConstraints,
            (LPSTR)lpszSqlStmtCreated,
            puStmtBufferSize,
            bJustGenerateSql,
            bResolveDupNames
        );
    }
};      // end of class declaration: odbcTABLECREATOR
