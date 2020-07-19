#pragma once

#include "sql.hpp"

class odbcEXPORTED odbcRECORD : public odbcCURSOR
{
    //#include <odbclslb\recordwn.hpp>
protected:
    char *      lpszTblName;
    char *      lpszStmt;
    odbcCURSOR* pStmt;
    sCOLBIND*   pActualCols;
    UWORD       uNoOfActualCols;

    bool Allocate(LPCSTR lpszSentTblName);

    virtual RETCODE GetActualColumnDefinitions(bool bSaveColBindings);

public:
    odbcRECORD(odbcCONNECT* pConn, LPCSTR lpszSentTblName);
    virtual ~odbcRECORD();

    virtual const odbcCURSOR* GetStmtObject()
    {
        return pStmt;
    }

    virtual LPCSTR GetSqlStmt()
    {
        return lpszStmt;
    }

    /**********************************************************
            SetWnd

            set window handle for message boxes.
            Echo setting in internal statement handle.
    **********************************************************/

    void SetWnd(HWND Wnd) override
    {
        odbcBASE::SetWnd(Wnd);
        if (pStmt)
            pStmt->SetWnd(Wnd);
    }

    /**********************************************************
            SetMBoxFlags

            set flags for message boxes.
            Echo setting in internal statement handle.
    **********************************************************/

    void SetMBoxFlags(UWORD f) override
    {
        odbcBASE::SetMBoxFlags(f);
        if (pStmt)
            pStmt->SetMBoxFlags(f);

    }

    /**********************************************************

            SetErrHandler

            Install a new error handler.
            Returns the old error handler address.
            Echo setting in internal statement handle.
    **********************************************************/
    odbcERRHANDLER SetErrHandler(odbcERRHANDLER New) override
    {
        odbcERRHANDLER Temp = odbcBASE::SetErrHandler(New);
        if (pStmt)
            pStmt->SetErrHandler(New);

        return Temp;
    }
    /**********************************************************
            AutoRetrieve

            Turns on and off the automatic error handling.
            Echo setting in internal statement handle.
    **********************************************************/

    SWORD AutoRetrieve(SWORD bOn) override
    {
        SWORD bSave = odbcBASE::AutoRetrieve(bOn);
        if (pStmt)
            pStmt->AutoRetrieve(bOn);

        return bSave;
    }

    /**********************************************************
            AutoReport

            Turns on and off the automatic error reporting.
            Echo setting in internal statement handle.
    **********************************************************/

    SWORD AutoReport(SWORD bOn) override
    {
        SWORD bSave = odbcBASE::AutoReport(bOn);

        if (pStmt)
            pStmt->AutoReport(bOn);

        return bSave;
    }

    /****************************************************

        ColRecAddr

        Given a record address and column identification,
        determine the offset of the column's field within
        the record and return its address.  Returns NULL
        if the record address is invalid or the column
        identifier is invalid.

    ****************************************************/
    virtual void* odbcRECORD::ColRecAddr(
        void*     pRecord,
        LPUCSTR ColName);

    virtual void* odbcRECORD::ColRecAddr(
        void*     pRecord,
        LPCSTR  ColName)
    {
        return ColRecAddr(
            pRecord,
            (LPUCSTR)ColName
        );
    }

    virtual void* odbcRECORD::ColRecAddr(
        void*     pRecord,
        UWORD   icol);

    odbcCURSOR* GetUpdateStmt();

}; // end odbcRECORD class

class odbcEXPORTED odbcRECINSERTER : public odbcRECORD {
protected:
    /***************************************************

        SetupForInsert

        Set up column bindings, record buffer, etc. for
        insert.
    ***************************************************/

    RETCODE     SetupForInsert(
        sCOLBIND*       pColBinds = NULL,
        UWORD           uNumColBindings = 0,
        void*             pRecord = NULL
    );

    bool IsInserter() override { return true; }

public:
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
    odbcRECINSERTER(
        odbcCONNECT*    pConn,
        LPCSTR          lpszSentTblName,
        sCOLBIND*       pColBinds = NULL,
        UWORD           uNumColBindings = 0,
        void*             pRecord = NULL
    );

    odbcRECINSERTER(odbcCONNECT*    pConn,
        LPUCSTR         lpszSentTblName,
        sCOLBIND*       pColBinds = NULL,
        UWORD           uNumColBindings = 0,
        void*             pRecord = NULL
    );

    virtual ~odbcRECINSERTER();

    /***************************************************

        InsertRecord

        Bind storage of one row's worth of data in the
        passed user-allocated record structure, for use
        in the automatically generated insert statement.

        The structure of the data in the record is de-
        fined by the contents of the internal column
        bindings array, either set by a data dictionary
        operation such as SetColBindings() or direct
        execution of the statement 'SELECT * FROM <table>
        WHERE 0 = 1' followed by a call to AutoBind().

        The data dictionary method must provide sCOLBIND binding
        definitions for all non-NULL columns in the table.

        Parameters
        ==========

            Name               Default  Description
            ------------------------------------------------------------------
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

    ***************************************************/

    virtual RETCODE InsertRecord
    (
        void*         pRecord,
        size_t      Size,
        SDWORD *    pcbValueArray = NULL
    );

}; // end odbcRECINSERTER class

class odbcEXPORTED odbcRECUPDATER : public odbcRECORD {
protected:

    /***************************************************

        SetupForPosUpdDels

        Set up for positioned updates and deletes.
    ***************************************************/

    RETCODE     SetupForPosUpdDels
    (
        LPCSTR          szSelectStmt,
        bool            bExecDirect,
        sCOLBIND*       pColBinds,
        UWORD           uNumColBindings,
        void*             pRecord,
        UWORD           fConcur,
        SDWORD          fKeyset,
        UWORD           fRowSet
    );

public:
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
    odbcRECUPDATER(odbcCONNECT*    pConn,
        LPCSTR          lpszSentTblName,
        LPCSTR          lpszSelectStmt,
        sCOLBIND*       pColBinds = NULL,
        UWORD           uNumColBindings = 0,
        void*             pRecord = NULL,
        bool            bExecDirect = true,
        UWORD           fConcur = SQL_CONCUR_VALUES,
        SDWORD          fKeyset = SQL_CURSOR_STATIC,
        UWORD           fRowSet = 1
    );

    odbcRECUPDATER(odbcCONNECT*    pConn,
        LPUCSTR         lpszSentTblName,
        LPUCSTR         lpszSelectStmt,
        sCOLBIND*       pColBinds = NULL,
        UWORD           uNumColBindings = 0,
        void*             pRecord = NULL,
        bool            bExecDirect = true,
        UWORD           fConcur = SQL_CONCUR_VALUES,
        SDWORD          fKeyset = SQL_CURSOR_STATIC,
        UWORD           fRowSet = 1
    );


    virtual ~odbcRECUPDATER();

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
           table ... FOR UPDATE OF columns

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

            pRowCount          NULL     Address of storage for row count returned
                                        by the update operation; hopefully 1 if
                                        the update succeeded. If the value is
                                        greater than one, the query did not
                                        contain columns representing the table's
                                        primary key, so a unique row could not be
                                        identified and all rows matching the row's
                                        data values were updated.

        Return Values:

        Return value will be the number of rows updated; may be the constant
        SQL_NEED_DATA if any of your columns are to be bound with SQL_DATA_AT_EXEC.

    ***************************************************/
    virtual SDWORD UpdateRecord
    (
        UWORD       irow,
        void*         pRecord,
        size_t      Size,
        SDWORD *    pcbValueArray = NULL
    );

    /***************************************************

        DeleteRecord

        Delete a row from the rowset using
        the automatically generated delete
        that is an output of this operation.

        This will only work if the following conditions
        are met:

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

        Parameters
        ==========

            Name               Default  Description
            ------------------------------------------------------------------
            irow                        Row in rowset to delete


    ***************************************************/

    virtual SDWORD DeleteRecord
    (
        UWORD       irow
    );

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
protected:
    virtual bool AppendWhereClause();

}; // end odbcRECUPDATER class
