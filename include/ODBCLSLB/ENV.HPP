#pragma once

///////////////////////////////////////////////////////////
//////////////////////// environment
///////////////////////////////////////////////////////////

class odbcEXPORTED odbcENV : public odbcBASE {

/**********************************************************
        friend classes
        
        Connection must be friend class so it
        can call RegisterConnection() to register its instances.
**********************************************************/

friend class odbcEXPORTED odbcCONNECT;

private:
/**********************************************************
    henv
    
    ODBC environment handle.
**********************************************************/

        HENV henv{};
        
/**********************************************************
    pConnList
    
    list of odbcCONNECT connection objects.
**********************************************************/

//        odbcCONNLIST *pConnList;
        
/**********************************************************
        RegisterConnection
        
        Register an odbcCONNECT object.
**********************************************************/

        virtual void RegisterConnection(odbcCONNECT* pConnection);
                
/**********************************************************
        UnregisterConnection
        
        Unregister an odbcCONNECT object.
**********************************************************/

        virtual void UnregisterConnection(odbcCONNECT* pConnection);
                
/**********************************************************
    szDSN
    
    Data source name returned from FirstDataSource or
    NextDataSource.
**********************************************************/

        LPSTR szDSN_;

/**********************************************************
    szDescription
    
    Data source description returned from FirstDataSource or
    NextDataSource.
**********************************************************/

        LPSTR szDescription_;

#if (ODBCVER >= 0x0200)
/**********************************************************
    szDriver
    
    Data source name returned from FirstDriver or
    NextDriver.
**********************************************************/

        LPSTR szDriver_;

/**********************************************************
    szDriverAttributes
    
    Driver attributes returned from FirstDriver or
    NextDriver.
**********************************************************/

        LPSTR szDriverAttributes_;

#endif // if (ODBCVER >= 0x0200)

public:

/**********************************************************
    nCursorLibUsage
    
    Constant to define cursor library usage; can be one of
    
    SQL_CUR_USE_IF_NEEDED Use if needed
    SQL_CUR_USE_ODBC      Use cursor library always
    SQL_CUR_USE_DRIVER    Never use cursor library
    SQL_CUR_DEFAULT       Default is never use cursor library

**********************************************************/

        int nCursorLibUsage;

/**********************************************************

        odbcENV()
        
        default constructor. Allocates an ODBC environment
        handle.
**********************************************************/

        odbcENV();
           
           
/**********************************************************

        ~odbcENV()
        
        destructor. De-allocates ODBC environment handle.
**********************************************************/

        virtual ~odbcENV();

/**********************************************************

        GetHenv()
        
        Returns ODBC environment handle.
**********************************************************/

        virtual HENV    GetHenv() { return henv;}
        ////////////////////////////////////////////////////////
        ////////////////////// core functions //////////////////
        ////////////////////////////////////////////////////////


/**********************************************************

        AllocConnect
        
        Allocates and optionally opens a connection on an
        odbcCONNECT object.
        
        Connections should always be allocated this way and 
        deallocated using DeAllocConnect.
        
        If the return value is NULL, call lastRC() to determine
        the return code from the ODBC function that failed.
**********************************************************/

        virtual odbcCONNECT* 
                AllocConnect(
                        LPUSTR szDSN,
                        LPUSTR szUID       = NULL,
                        LPUSTR szAuthStr   = NULL,
                        UDWORD udTimeout   = 0
                        );
                                                       
                                                       
        virtual odbcCONNECT* 
                AllocConnect(
                        LPSTR  szDSN       = NULL,
                        LPSTR  szUID       = NULL,
                        LPSTR  szAuthStr   = NULL,
                        UDWORD udTimeout   = 0
                        )
                        {
                        return
                                AllocConnect(
                                        (LPUSTR)szDSN,
                                        (LPUSTR)szUID,
                                        (LPUSTR)szAuthStr,
                                        udTimeout
                        );
                        }
                                                       
/**********************************************************

        DeAllocConnect
        
        Deallocates an odbcCONNECT object.
        
**********************************************************/

        virtual void DeAllocConnect(
                        odbcCONNECT* pConn
                        );
                                                       
                                                       
/**********************************************************

        RegisterError
        
        Get more information on the most recent error code
        from an ODBC operation. Results can be retrieved using
        member functions in the parent odbcBASE class.
        
        This function calls the base class member function Error()
        with arguments appropriate for this object type.
**********************************************************/

        RETCODE RegisterError() override;

/**********************************************************
    Commit
    
    Call to SQLTransact to commit a transaction across multiple 
    connections.
**********************************************************/

        virtual RETCODE Commit();

/**********************************************************
    RollBack
    
    Call to SQLTransact to roll back a transaction across multiple 
    connections.
**********************************************************/

        virtual RETCODE RollBack();

        ////////////////////////////////////////////////////////
        ////////////////////// level 1 functions ///////////////
        ////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////
        ////////////////////// level 2 functions ///////////////
        ////////////////////////////////////////////////////////
        
/**********************************************************

        DataSources
        
        Translates into direct call to SQLDataSources.
**********************************************************/

        virtual RETCODE DataSources(
                                        UWORD           fDirection,
                                        LPUSTR          szDSN,
                                        WORD            cbDSNMax,
                                        SWORD      *pcbDSN,
                                        LPUSTR          szDescription,
                                        SWORD           cbDescriptionMax,
                                        SWORD      *pcbDescription);
    
        virtual RETCODE DataSources(
                                        UWORD           fDirection,
                                        LPSTR           szDSN,
                                        WORD            cbDSNMax,
                                        SWORD      *pcbDSN,
                                        LPSTR           szDescription,
                                        SWORD           cbDescriptionMax,
                                        SWORD      *pcbDescription)
                                        {
                                        return
                                                DataSources(
                                                        fDirection,
                                                        (LPUSTR)szDSN,
                                                        cbDSNMax,
                                                        pcbDSN,
                                                        (LPUSTR)szDescription,
                                                        cbDescriptionMax,
                                                        pcbDescription);
                                        }
    
/***************************************************

    FirstDataSource
    
    Invokes DataSources member function to get the first
    data source name and description.  These can be accessed
    via the DSN() and DSNDesc() member functions.
***************************************************/

        virtual RETCODE FirstDataSource();

/***************************************************

    NextDataSource
    
    Invokes DataSources member function to get the next
    data source name and description.  These can be accessed
    via the DSN() and DSNDesc() member functions.
***************************************************/

        virtual RETCODE NextDataSource();
    
    /***************************************************

    DSN
    
    Pointer to data source name returned by FirstDataSource
    and NextDataSource.  NULL if these have never been invoked
    or did not succeed!
    ***************************************************/
    virtual LPCSTR DSN() 
                {
                return szDSN_;
                }
                
    /***************************************************

    Description
    
    Pointer to data source description returned by FirstDataSource
    and NextDataSource.  NULL if these have never been invoked
    or did not succeed!
    ***************************************************/
    virtual LPCSTR Description() 
                {
                return szDescription_;
                }

// new in v2.0

#if (ODBCVER >= 0x0200)
/**********************************************************

        Drivers
        
        Translates into direct call to SQLDrivers.
**********************************************************/

        virtual RETCODE Drivers(
                                        UWORD           fDirection,
                                        LPUSTR          szDriver,
                                        WORD            cbDriverMax,
                                        SWORD      *pcbDriver,
                                        LPUSTR          szDriverAttributes,
                                        SWORD           cbDriverAttributesMax,
                                        SWORD      *pcbDriverAttributes);
    
        virtual RETCODE Drivers(
                                        UWORD           fDirection,
                                        LPSTR           szDriver,
                                        WORD            cbDriverMax,
                                        SWORD      *pcbDriver,
                                        LPSTR           szDriverAttributes,
                                        SWORD           cbDriverAttributesMax,
                                        SWORD      *pcbDriverAttributes)
                                        {
                                        return
                                                Drivers(
                                                        fDirection,
                                                        (LPUSTR)szDriver,
                                                        cbDriverMax,
                                                        pcbDriver,
                                                        (LPUSTR)szDriverAttributes,
                                                        cbDriverAttributesMax,
                                                        pcbDriverAttributes);
                                        }
    
/***************************************************

    FirstDriver
    
    Invokes Drivers member function to get the first
    data source name and description.  These can be accessed
    via the Driver() and DriverAttributes() member functions.
***************************************************/

        virtual RETCODE FirstDriver();

/***************************************************

    NextDriver
    
    Invokes Drivers member function to get the next
    data source name and attributes.  These can be accessed
    via the Driver() and DriverAttributes() member functions.
***************************************************/

        virtual RETCODE NextDriver();
    
    /***************************************************

    Driver
    
    Pointer to data source name returned by FirstDriver
    and NextDriver.  NULL if these have never been invoked
    or did not succeed!
    ***************************************************/
    virtual LPCSTR Driver()
                {
                return szDriver_;
                }
                
    /***************************************************

    DriverAttributes
    
    Pointer to driver attributes returned by FirstDriver
    and NextDriver.  NULL if these have never been invoked
    or did not succeed!
    ***************************************************/
    virtual LPCSTR DriverAttributes()
                {
                return szDriverAttributes_;
                }

#endif // if (ODBCVER >= 0x0200)

// end new in v2.0

};
