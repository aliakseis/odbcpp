#include <sql.hpp>

///////////////////////////////////////////////////////////
//////////////////////// environment
///////////////////////////////////////////////////////////


/**********************************************************

        odbcENV()

        default constructor. Allocates an ODBC environment
        handle.
**********************************************************/

odbcENV::odbcENV()
{
    AutoRetrieve(odbcREPERRS);
    SetRC(SQLAllocEnv(
        &henv));
    //        pConnList = 0;
    AutoRetrieve(odbcNOREPORT);
    szDSN_ = nullptr;
    szDescription_ = nullptr;
    nCursorLibUsage = SQL_CUR_DEFAULT;
#if (ODBCVER >= 0x0200)
    szDriver_ = nullptr;
    szDriverAttributes_ = nullptr;
#endif // if (ODBCVER >= 0x0200)
}

/**********************************************************

        ~odbcENV()

        destructor. De-allocates ODBC environment handle.
**********************************************************/

odbcENV::~odbcENV()
{
    // drop all connections
/*        if (pConnList)
            {
            delete pConnList;
            pConnList = NULL;
            }
*/
// free environment handle
    if (henv)
    {
        SQLFreeEnv(henv);
        henv = nullptr;
    }

    // free locally allocated memroy
    if (szDSN_)
    {
        delete[] szDSN_;
        szDSN_ = nullptr;
    }

    if (szDescription_)
    {
        delete[] szDescription_;
        szDescription_ = nullptr;
    }


#if (ODBCVER >= 0x0200)
    if (szDriver_)
    {
        delete[] szDriver_;
        szDriver_ = nullptr;
    }

    if (szDriverAttributes_)
    {
        delete[] szDriverAttributes_;
        szDriverAttributes_ = nullptr;
    }

#endif // if (ODBCVER >= 0x0200)
}

/**********************************************************

        AllocConnect

        Allocates and optionally opens a connection on an
        odbcCONNECT object.

        Connections should always be allocated this way and
        deallocated using DeAllocConnect.

        If the return value is NULL, call lastRC() to determine
        the return code from the ODBC function that failed.
**********************************************************/

odbcCONNECT* odbcENV::AllocConnect(
    LPUSTR szDSN,
    LPUSTR szUID,
    LPUSTR szAuthStr,
    UDWORD      udTimeout
)
{
    auto* pC = new odbcCONNECT(
        this,
        szDSN,
        szUID,
        szAuthStr,
        udTimeout);

    if (!pC)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pC;
}

/**********************************************************

        AllocConnect

        Deallocates an odbcCONNECT object.

**********************************************************/

void odbcENV::DeAllocConnect(
    odbcCONNECT* pC
)
{

    delete pC;
}

/**********************************************************
        RegisterConnection

        Register an odbcSTMT object.
**********************************************************/

void odbcENV::RegisterConnection(odbcCONNECT* /*pConnection*/)
{
    //	Has no use now
}


/**********************************************************
        UnregisterConnection

        Unregister an odbcSTMT object.
**********************************************************/

void odbcENV::UnregisterConnection(odbcCONNECT* /*pConnection*/)
{
    //	Has no use now
}


/**********************************************************

        RegisterError

        Get more information on the most recent error code
        from an ODBC operation. Results can be retrieved using
        member functions in the parent odbcBASE class.
**********************************************************/

RETCODE odbcENV::RegisterError()
{
    return Error(
        henv,
        SQL_NULL_HDBC,
        SQL_NULL_HSTMT);
}

/**********************************************************
    Commit

    Call to SQLTransact to commit a transaction across multiple
    connections.
**********************************************************/

RETCODE odbcENV::Commit()
{
    SetRC(SQLTransact(
        henv,
        SQL_NULL_HDBC,
        SQL_COMMIT
    ));

    return lastRC();
}

/**********************************************************
    RollBack

    Call to SQLTransact to roll back a transaction across multiple
    connections.
**********************************************************/

RETCODE odbcENV::RollBack()
{
    SetRC(SQLTransact(
        henv,
        SQL_NULL_HDBC,
        SQL_ROLLBACK
    ));

    return lastRC();
}

/**********************************************************

        DataSources

        Translates into direct call to SQLDataSources.
**********************************************************/

RETCODE odbcENV::DataSources(
    UWORD           fDirection,
    LPUSTR          szDSN,
    WORD            cbDSNMax,
    SWORD      *pcbDSN,
    LPUSTR          szDescription,
    SWORD           cbDescriptionMax,
    SWORD      *pcbDescription)
{
    SetRC(
        SQLDataSources(
            henv,
            fDirection,
            szDSN,
            cbDSNMax,
            pcbDSN,
            szDescription,
            cbDescriptionMax,
            pcbDescription));

    return lastRC();
}

/***************************************************

    FirstDataSource

    Invokes DataSources member function to get the first
    data source name and description.  These can be accessed
    via the DSN() and DSNDesc() member functions.
***************************************************/

RETCODE odbcENV::FirstDataSource()
{
    SWORD cbDSN; SWORD cbDescription;

    if (!szDSN_)
    {
        szDSN_ = (LPSTR)new UCHAR[SQL_MAX_DSN_LENGTH];
        if (!szDSN_)
        {
            SetRC(SQL_ALLOC_FAILED);
            return lastRC();
        }
    }

    if (!szDescription_)
    {
        szDescription_ = (LPSTR)new UCHAR[DSN_DESC_MAX];
        if (!szDescription_)
        {
            SetRC(SQL_ALLOC_FAILED);
            return lastRC();
        }
    }

    return
        DataSources(
            SQL_FETCH_FIRST,
            (LPUSTR)szDSN_,
            SQL_MAX_DSN_LENGTH,
            &cbDSN,
            (LPUSTR)szDescription_,
            DSN_DESC_MAX,
            &cbDescription);
}

/***************************************************

    NextDataSource

    Invokes DataSources member function to get the next
    data source name and description.  These can be accessed
    via the DSN() and DSNDesc() member functions.
***************************************************/

RETCODE odbcENV::NextDataSource()
{
    SWORD cbDSN; SWORD cbDescription;

    if (!szDSN_)
    {
        szDSN_ = (LPSTR)new UCHAR[SQL_MAX_DSN_LENGTH];
        if (!szDSN_)
        {
            SetRC(SQL_ALLOC_FAILED);
            return lastRC();
        }
    }

    if (!szDescription_)
    {
        szDescription_ = (LPSTR)new UCHAR[DSN_DESC_MAX];
        if (!szDescription_)
        {
            SetRC(SQL_ALLOC_FAILED);
            return lastRC();
        }
    }

    return
        DataSources(
            SQL_FETCH_NEXT,
            (LPUSTR)szDSN_,
            SQL_MAX_DSN_LENGTH,
            &cbDSN,
            (LPUSTR)szDescription_,
            DSN_DESC_MAX,
            &cbDescription);
}

#if (ODBCVER >= 0x0200)
/**********************************************************

        Drivers

        Translates into direct call to SQLDrivers.
**********************************************************/

RETCODE odbcENV::Drivers(
    UWORD           fDirection,
    LPUSTR          szDriver,
    WORD            cbDriverMax,
    SWORD      *pcbDriver,
    LPUSTR          szDriverAttributes,
    SWORD           cbDriverAttributesMax,
    SWORD      *pcbDriverAttributes)
{
    SetRC(
        SQLDrivers(
            henv,
            fDirection,
            szDriver,
            cbDriverMax,
            pcbDriver,
            szDriverAttributes,
            cbDriverAttributesMax,
            pcbDriverAttributes));

    return lastRC();
}

/***************************************************

    FirstDriver

    Invokes Drivers member function to get the first
    data source name and driver attributes.  These can be accessed
    via the Driver() and DriverDesc() member functions.
***************************************************/

RETCODE odbcENV::FirstDriver()
{
    SWORD cbDriver; SWORD cbDriverAttributes;

    if (!szDriver_)
    {
        szDriver_ = (LPSTR)new UCHAR[DRIVER_MAX];
        if (!szDriver_)
        {
            SetRC(SQL_ALLOC_FAILED);
            return lastRC();
        }
    }

    if (!szDriverAttributes_)
    {
        szDriverAttributes_ = (LPSTR)new UCHAR[DRIVER_ATTRIBS_MAX];
        if (!szDriverAttributes_)
        {
            SetRC(SQL_ALLOC_FAILED);
            return lastRC();
        }
    }

    return
        Drivers(
            SQL_FETCH_FIRST,
            (LPUSTR)szDriver_,
            DRIVER_MAX,
            &cbDriver,
            (LPUSTR)szDriverAttributes_,
            DRIVER_ATTRIBS_MAX,
            &cbDriverAttributes);
}

/***************************************************

    NextDriver

    Invokes Drivers member function to get the next
    data source name and driver attributes.  These can be accessed
    via the Driver() and DSNDesc() member functions.
***************************************************/

RETCODE odbcENV::NextDriver()
{
    SWORD cbDriver; SWORD cbDriverAttributes;

    if (!szDriver_)
    {
        szDriver_ = (LPSTR)new UCHAR[DRIVER_MAX];
        if (!szDriver_)
        {
            SetRC(SQL_ALLOC_FAILED);
            return lastRC();
        }
    }

    if (!szDriverAttributes_)
    {
        szDriverAttributes_ = (LPSTR)new UCHAR[DRIVER_ATTRIBS_MAX];
        if (!szDriverAttributes_)
        {
            SetRC(SQL_ALLOC_FAILED);
            return lastRC();
        }
    }

    return
        Drivers(
            SQL_FETCH_NEXT,
            (LPUSTR)szDriver_,
            DRIVER_MAX,
            &cbDriver,
            (LPUSTR)szDriverAttributes_,
            DRIVER_ATTRIBS_MAX,
            &cbDriverAttributes);
}

#endif // if (ODBCVER >= 0x0200)
