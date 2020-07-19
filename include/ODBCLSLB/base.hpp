#pragma once

/*
        * * *  General Discussion On Error Handling  * * *

        The design of the library's error and exception handling
        has two main goals: flexibility and ease of use.

    Two kinds of problem conditions are handled by the base
    class's member functions and data.  Errors are conditions
    that arise when an ODBC function is called and an unsuc-
    cessful result code is returned. Exceptions are critical
    errors that are essentially unrecoverable, such as failed
    memory allocations.

    Errors are normally handled by the application code rather
    than the library.  To assist the application, the library
    provides facilities for retrieving the status information
    the ODBC Driver Manager and driver can return when an
    error occurs.  This is accomplished by calling the member
    function AutoRetrieve() with one of the following symbolic
    constants:

    odbcREPERRS                         Action is taken when SQL_ERROR
                                                is returned (this is the default)

    odbcREPSUCCESSWITHINFO      Action is taken when
                                                SQL_SUCCESS_WITH_INFO is returned

    Automatic retrieval can be turned off by calling AutoRetrieve()
    with the following symbolic constant:

    odbcNOREPORT                        No action is taken when SQL_ERROR
                                                or SQL_SUCCESS_WITH_INFO is returned

    Some errors are actually exceptions, just as some success-
    with-info returns are actually errors, but the library would
    be overstepping its authority if it attempted to determine
    the difference on behalf of the application.  The application
    can handle these when the member function that brings on the
    exception is invoked, but this leads to messy code, since the
    exception must usually be handled in multiple places in the
    application.

    A better approach is for the application and library to
    cooperate in handling these problems.  The way the library
    supports this kind of cooperation is through an installable
    error handler.

    This error handler is supplied by the application by passing
    its address to the library. It will be invoked by the library
    when an error occurs, after the status information has been
    retrieved.

    The error handler is installed on a per-object basis.  A global
    error handler could also be used if the library were bound
    statically, but the multi-user nature of dynamic-link libraries
    under Windows 3.1 makes use of application-specific resources
    of any kind very difficult, so at this stage the library
    places the burden of error handler installation on the
    application every time it instantiates an object.

    To turn on use of an error handler, you must take two
    steps:

    1)  Install the error handler function address with a call
        to the member function SetErrHandler.

    2)  Determine the level of severity at which the error
        handler is invoked by calling the member function
        AutoReport() with one of the constants cited above
        (odbcREPERRS or odbcREPSUCCESSWITHINFO).

    The library also supplies its own built-in error handler
    mechanism when no error handler is installed but one of
    the triggers has been set using AutoReport() with one of
    handler-triggering constants (odbcREPERRS or
    odbcREPSUCCESSWITHINFO).  The default setting is not to
    report errors, and this can be reinstated at any time by
    calling AutoReport() with the constant odbcNOREPORT.

    If one of these trigger levels are set but no error handler
    has been installed, the member function Report() is invoked.
    This member function by default puts up a Windows message
    box, using the window handle installed by calling the member
    function SetWnd() and the flags set by a call to SetMBoxFlags(). The default
    window handle is HWND_DESKTOP and the default flags are
    MB_OK | MB_ICONASTERISK.

    Users can provide a class-specific error handler by overriding
    the Report() function in a derived class. Public member
    functions are provided that could be used by such a derived
    function access to the status information stored in the object.

    Error handling is invoked during the execution of the member
    function SetRC(), which is called to handle the return from
    a call into the ODBC library (calls to the Driver Manager or
    the driver).  The logic is as follows:

    if the return is SQL_SUCCESS_WITH_INFO or SQL_ERROR
        if status-info retrieval severity level is set to
                                report success with info
                or (the status-info retrieval severity level is set to
                                report errors and the return is SQL_ERROR)
                begin
                        call RegisterError
                                        (this invokes class-specific behavior
                                        to set up and execute a call to the
                                        ODBC function SQLError to retrieve
                                        status information)

                        if error-reporting severity level is set to
                                                report success with info
                                or (the error-reporting severity level
                                                is set to report errors and the return
                                                is SQL_ERROR)
                                begin
                                        if there is an error handler installed
                                                invoke the error handler
                                        else
                                                call Report()
                                end block
                        end if
                end block
        end if

*/

/*
        Typedef Name:           odbcERRHANDLER

        Description:

        This function prototype typedef represents the installable
        error handler used in class odbcBASE.  The function address
        passed in a call to the member function SetErrhandler must
        be of this form.

        */

typedef void (CALLBACK  *odbcERRHANDLER)(
    RETCODE         lastRet,
    UCHAR           *szSqlState,
    SDWORD          fNativeError,
    UCHAR           *szErrorMsg,
    odbcBASE        *pObj
    );

// the __LINEAGE macro could be defined to allow prior ancestors to
// odbcBASE; for instance,

// #define __LINEAGE    : cMyBaseClass

//  ...would allow you to set up the hierarchy as
// part of your own class hierarchy descended from cMyBaseClass.

// define __LINEAGE before including sql.hpp.

// if you do this, don't forget the colon!

class odbcEXPORTED odbcBASE  __LINEAGE
{
protected:
    /**********************************************************
            bGetErrorInfo

            Automatically get info if error from the last ODBC
            operation.
    **********************************************************/

    SWORD                   bGetErrorInfo;

    /**********************************************************
            bReportErrorInfo

            Automatically report info if error from the last ODBC
            operation.
    **********************************************************/

    SWORD                   bReportErrorInfo;

    /**********************************************************
            hwnd

            optional window handle for message boxes.
    **********************************************************/

    HWND            hwnd_;

    /**********************************************************
            flags

            flags for error message boxes.
    **********************************************************/

    UWORD           flags;

    /**********************************************************
            ErrHandler

            Error handler function address.
    **********************************************************/

    odbcERRHANDLER  ErrHandler;

private:
    /**********************************************************
            lastRet

            return code from the last ODBC operation.
    **********************************************************/

    RETCODE         lastRet;

    /**********************************************************
            szSqlState

            SQL State from the last ODBC operation.
    **********************************************************/

    UCHAR           szSqlState[odbcSTATE_SIZE]{};

    /**********************************************************
            fNativeError

            native return code from the last ODBC operation.
    **********************************************************/

    SDWORD          fNativeError;

    /**********************************************************
            szErrorMsg

            error message from the last ODBC operation.
    **********************************************************/

    LPUSTR          szErrorMsg;

    /**********************************************************
            cbErrorMsgActual

            actual size of error message from the last ODBC
            operation.
    **********************************************************/

    SWORD           cbErrorMsgActual;

protected:

    /**********************************************************
            ReportResponse

            Return code from the Report() message box.
    **********************************************************/

    int             ReportResponse{};

    /**********************************************************
            SetRC

            set last error return code; get info if error occurred.
    **********************************************************/

    virtual void SetRC(RETCODE rc);

public:
    /**********************************************************
            odbcBASE

            default constructor.
    **********************************************************/

    odbcBASE();

    /**********************************************************
            ~odbcBASE

            destructor.
    **********************************************************/

    virtual ~odbcBASE();

    /**********************************************************
            Error

            function that calls SQLError to load variables.
    **********************************************************/

    virtual RETCODE Error(
        HENV            henv,
        HDBC            hdbc,
        HSTMT           hstmt);

    /**********************************************************
            SqlState

            returns read-only pointer to state storage.
    **********************************************************/

    virtual LPUCSTR SqlState()
    {
        return szSqlState;
    }

    /**********************************************************
            NativeError

            returns native error code.
    **********************************************************/

    virtual UDWORD NativeError()
    {
        return fNativeError;
    }

    /**********************************************************
            ErrMsg

            returns read-only pointer to error message.
    **********************************************************/

    virtual LPUCSTR ErrMsg()
    {
        return ((szErrorMsg != NULL)
            ? (LPUCSTR)szErrorMsg
            : (LPUCSTR)"");
    }

    /**********************************************************
            SetWnd

            set window handle for message boxes.
    **********************************************************/

    virtual void SetWnd(HWND Wnd)
    {
        hwnd_ = Wnd;
    }

    /**********************************************************
            SetMBoxFlags

            set flags for message boxes.
    **********************************************************/

    virtual void SetMBoxFlags(UWORD f)
    {
        flags = f;
    }

    /**********************************************************
            Report

            display message box showing error.
    **********************************************************/

    virtual int Report();

    /**********************************************************
            ReportReturned

            display return code from message box showing error.
    **********************************************************/

    virtual int ReportReturned()
    {
        return ReportResponse;
    }

    /**********************************************************
            RegisterError

            Get error-handling info.
        // new in v2.0: returns RETCODE returned by Error().

    **********************************************************/

    virtual RETCODE RegisterError() = 0;

    /**********************************************************
            AutoRetrieve

            Turns on and off the automatic error handling
    **********************************************************/

    virtual SWORD AutoRetrieve(SWORD bOn)
    {
        SWORD bSave = bGetErrorInfo;
        bGetErrorInfo = bOn;
        return bSave;
    }

    /**********************************************************
            AutoReport

            Turns on and off the automatic error reporting
    **********************************************************/

    virtual SWORD AutoReport(SWORD bOn)
    {
        SWORD bSave = bReportErrorInfo;
        bReportErrorInfo = bOn;
        return bSave;
    }

    /**********************************************************

            sqlsuccess()

            returns true if the return code of the last ODBC operation
            is either SQL_SUCCESS or SQL_SUCCESS_WITH_INFO.
    **********************************************************/

    virtual bool sqlsuccess()
    {
        return (lastRet == SQL_SUCCESS
            || lastRet == SQL_SUCCESS_WITH_INFO);
    }

    /**********************************************************

            lastRC()

            returns the return code of the last ODBC operation.
            Use the member function sqlsuccess() to determine if the result
            is either SQL_SUCCESS or SQL_SUCCESS_WITH_INFO.
    **********************************************************/

    virtual RETCODE lastRC() { return lastRet; }


    /**********************************************************

            SetErrHandler

            Install a new error handler.
            Returns the old error handler address.
    **********************************************************/
    virtual odbcERRHANDLER SetErrHandler(odbcERRHANDLER New)
    {
        odbcERRHANDLER Temp = ErrHandler;

        ErrHandler = New;

        return Temp;
    }
    /**********************************************************

            TrimTrailingBlanks

            Trim trailing blanks from a string.

    **********************************************************/
    virtual void TrimTrailingBlanks(LPUSTR lpsz);
    virtual void TrimTrailingBlanks(LPSTR lpsz)
    {
        TrimTrailingBlanks((LPUSTR)lpsz);
    }

    // new in v2.0

    /**********************************************************

            GetConvertInfoFlag

            Given a SQL data type, get the flag to pass to SQLGetInfo
        to get its data type conversion support information.

    **********************************************************/
    virtual UWORD GetConvertInfoFlag(SWORD fDataType);

    /**********************************************************

            GetConvertInfoBitMask

            Given a data type, get the bitmask to use on the return
        for a SQLGetInfo call with a SQL_CONVERT_<type> flag
        to get its data type conversion support information.

    **********************************************************/
    virtual UDWORD GetConvertInfoBitMask(SWORD fDataType);

    /**********************************************************

            needData()

            returns true if the return code of the last ODBC operation
            is  SQL_NEED_DATA.
    **********************************************************/

    virtual bool needData()
    {
        return (lastRet == SQL_NEED_DATA);
    }

    /**********************************************************

            NoDataFound()

            returns true if the return code of the last ODBC operation
            is  SQL_NO_DATA_FOUND.
    **********************************************************/

    virtual bool NoDataFound()
    {
        return (lastRet == SQL_NO_DATA_FOUND);
    }

    /**********************************************************

            NextError()

        loads next error message until the return code is
        SQL_NO_DATA_FOUND.
    **********************************************************/

    virtual RETCODE NextError()
    {
        RETCODE rc = lastRC();
        if (rc != SQL_NO_DATA_FOUND)
        {
            rc = RegisterError();
        }
        else
            rc = SQL_NO_DATA_FOUND;

        return rc;
    }

    /***************************************************

        PrecisionForSqlType

        Returns the correct precision for a given SQL
        data type.

    ***************************************************/

    virtual UDWORD PrecisionForSqlType(SWORD fSqlType, UDWORD cbValueMax);

    /***************************************************

        LengthForCType

        Returns the correct size for a given C
        data type.

    ***************************************************/

    virtual UDWORD LengthForCType(SWORD fCType, UDWORD cbValueMax);

    /**********************************************************
            GetAutoRetrieve

            Gets flag value for automatic error handling
    **********************************************************/

    virtual SWORD GetAutoRetrieve()
    {
        return bGetErrorInfo;
    }

    /**********************************************************
            GetAutoReport

            Gets flag value for automatic error reporting
    **********************************************************/

    virtual SWORD GetAutoReport()
    {
        return bReportErrorInfo;
    }

    /**********************************************************
            GetWnd

            Get window handle for message boxes.
    **********************************************************/

    virtual HWND GetWnd()
    {
        return hwnd_;
    }

    // end new in v2.0

    UWORD GetMBoxFlags()
    {
        return flags;
    }

    odbcERRHANDLER GetErrHandler()
    {
        return ErrHandler;
    }

}; // end of class
