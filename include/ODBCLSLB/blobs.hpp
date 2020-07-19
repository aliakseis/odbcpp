#pragma once

typedef char BLOBFORMAT;
enum {
    FMT_BITMAP = 1,
    FMT_METAFILEPICT = 2,
    NUM_FMT_ENTRIES = 5,
    SIZE_FMT_ENTRIES = NUM_FMT_ENTRIES * sizeof(BLOBFORMAT)
};

class odbcEXPORTED odbcBLOB {
private:
    odbcCURSOR* pGetCursor;
    odbcCURSOR* pPutCursor;
    UWORD  iCol;
    UWORD  iParam;
    void*   pData_;
    UDWORD cbAllocatedSize;
    UDWORD cbMaxSize;
    SWORD  fSqlType;
    UDWORD cbPutChunkSize;
    UDWORD cbGetChunkSize;
    SDWORD cbValue;
public:
    SDWORD cbMaxBlobSize;
protected:
    /****************************************************************

        ReleaseMem

        Free memory on the internal pointer.
    ****************************************************************/
    void ReleaseMem();

public:
    /****************************************************************

        odbcBLOB

        Constructor. Pass address of owning cursor, number of the
        associated column, and maximum size of the parameter;
        optionally also pass the column's SQL data type and
        put- and get-chunk granularities.
    ****************************************************************/
    odbcBLOB(
        odbcCURSOR* pCurs,
        UWORD iSentCol,
        UDWORD cbSentMaxSize,
        SWORD  fSentSqlType = SQL_LONGVARBINARY,
        UWORD  iSentParam = 0,
        SDWORD cbSentPutChunkSize = BLOB_CHUNK_PUT_SIZE,
        SDWORD cbSentGetChunkSize = BLOB_CHUNK_GET_SIZE
    );

    /****************************************************************

        ~odbcBLOB

        Destructor.
    ****************************************************************/
    virtual ~odbcBLOB();

    /****************************************************************

        SetData

        Copy passed value (of passed size) into internal memory.
    ****************************************************************/
    virtual RETCODE SetData
    (
        void* pSentData,
        UDWORD cbSentSize
    );

    /****************************************************************

        PutData

        Invoke odbcSTMT::PutData in loop if necessary to pass entire
        value in chunks to the driver. Optionally set value before
        invoking odbcSTMT::PutData.
    ****************************************************************/
    virtual RETCODE PutData
    (
        void* pSentData = NULL,
        UDWORD cbSentSize = 0
    );

    /****************************************************************

        IsEmpty

        Returns true if the current value is empty.
    ****************************************************************/
    virtual bool IsEmpty()
    {
        return (cbValue == 0);
    }

    /****************************************************************

        IsNull

        Returns true if the current value is NULL.
    ****************************************************************/
    virtual bool IsNull()
    {
        return (cbValue == SQL_NULL_DATA);
    }

    /****************************************************************

        GetData

        Perform loop (if needed) to retrieve data for the column
        assigned to this odbcBLOB object.
    ****************************************************************/
    virtual RETCODE GetData();

    /****************************************************************

        GetMem

        Allocate memory to the internal pointer. We only ever have
        one pointer in use, so free it first if it is not null. If
        zero is passed, just return the current pointer.
    ****************************************************************/
    virtual void*    GetMem(UDWORD cbSize = 0);

    /****************************************************************

        GetcbValue

        Get current cbValue value (length of data returned or
        possibly SQL_NULL_DATA).
    ****************************************************************/
    virtual SDWORD GetcbValue()
    {
        return cbValue;
    }

    /****************************************************************

        SetNull

        Set up for a NULL PutValue.
    ****************************************************************/
    virtual void SetNull()
    {
        cbValue = SQL_NULL_DATA;
    }

    /****************************************************************

        GetCursor

        Get the associated cursor object used for GetData operations.
    ****************************************************************/
    virtual odbcCURSOR* GetCursor()
    {
        return pGetCursor;
    }

    /****************************************************************

        PutCursor

        Get the associated cursor object used for PutData and BindParameter
        operations.
    ****************************************************************/
    virtual odbcCURSOR* PutCursor()
    {
        return pPutCursor;
    }

    /****************************************************************

        SetPutCursor

        Set the associated cursor object used for PutData and BindParameter
        operations.
    ****************************************************************/
    virtual odbcCURSOR* SetPutCursor(odbcCURSOR* pNewPutCursor)
    {
        odbcCURSOR* pOldPutCursor = pPutCursor;
        pPutCursor = pNewPutCursor;
        return pOldPutCursor;
    }

    /****************************************************************

        SetGetCursor

        Set the associated cursor object used for GetData and BindParameter
        operations.
    ****************************************************************/
    virtual odbcCURSOR* SetGetCursor(odbcCURSOR* pNewGetCursor)
    {
        odbcCURSOR* pOldGetCursor = pGetCursor;
        pGetCursor = pNewGetCursor;
        return pOldGetCursor;
    }

    /****************************************************************

        BindParameter

        Bind a parameter.
    ****************************************************************/
    virtual RETCODE BindParameter();

    /****************************************************************

        SetParamNumber

        Set the parameter number for a parameter. Sets to column number
        if zero is sent.  Returns old value.

    ****************************************************************/
    virtual UWORD SetParamNumber(UWORD iSentParam)
    {
        UWORD iSaveParam = iParam;

        // set to column number if zero is sent
        if (iSentParam == 0)
            iParam = iCol;
        else
            iParam = iSentParam;

        return iSaveParam;
    }

    /****************************************************************

        SetGetChunkSize

        Set the chunk size for GetData() operations.  Returns old value.

    ****************************************************************/
    virtual UDWORD SetGetChunkSize(UDWORD cbSent)
    {
        UDWORD cbSave = cbSent;

        cbGetChunkSize = cbSent;

        return cbSave;
    }

    /****************************************************************

        SetPutChunkSize

        Set the chunk size for PutData() operations.  Returns old value.

    ****************************************************************/
    virtual UDWORD SetPutChunkSize(UDWORD cbSent)
    {
        UDWORD cbSave = cbSent;

        cbPutChunkSize = cbSent;

        return cbSave;
    }

    /****************************************************************

        SettHGlobalInBlob

        Set the content of the internal memory from a global memory
        object, allocated via GlobalAlloc, which will be locked, copied, and
        unlocked.

    ****************************************************************/
    virtual RETCODE SetHGlobalInBlob(HGLOBAL hGlobal);

    /****************************************************************

        GetHGlobalFromBlob

        Get the content of the internal memory as a global memory
        object, allocated via GlobalAlloc, locked, copied, and
        unlocked.

        Returns NULL without error setting if the BLOB is empty or
        contains null data. If memory allocation or locking fails,
        returns NULL and sets allocation error on get and put cursors.

        After copying the data it is wise to shrink its allocation in the
        BLOB's internal memory by calling GetMem(1).
    ****************************************************************/
    virtual HGLOBAL GetHGlobalFromBlob(UINT fuAlloc = GHND);

    /****************************************************************

        SetLPSTRInBlob

        Given a huge pointer to a null-terminated string, allocate
        the necessary amount of internal memory in the BLOB and
        copy the string to the BLOB's internal memory.
    ****************************************************************/
    virtual RETCODE    SetLPSTRInBlob(char  *lpstr);


    /****************************************************************

        GetLPSTRFromBlob

        Allocate memory to the internal pointer. We only ever have
        one pointer in use, so free it first if it is not null. If
        zero is passed, just return the current pointer. Return the
        poinster as an LPSTR.
    ****************************************************************/
    virtual LPSTR    GetLPSTRFromBlob(UDWORD cbSize = 0)
    {
        if (cbSize == 0 && IsNull())
            return NULL;

        return (LPSTR)GetMem(cbSize);
    }


    /****************************************************************
        HasFormat

        Given a BLOB that has been retrieved from the database,
        make sure it contains the correct type of data.

    ****************************************************************/

    bool HasFormat(BLOBFORMAT bfFormatToCheck);

    /****************************************************************
        SetBitmapInBlob

        Given a HBITMAP, store it in the BLOB so that it can be
        used as insertion or update data.

    ****************************************************************/

    virtual RETCODE SetBitmapInBlob(HBITMAP hSentBitmap);

    /****************************************************************
        GetBitmapFromBlob

        Given a BLOB that has been retrieved from the database,
        translate it into a HBITMAP.

    ****************************************************************/

    virtual RETCODE GetBitmapFromBlob(HBITMAP* pHBitmapToFill);

    /****************************************************************
        SetMetaFilePictInBlob

        Given a METAFILEPICT, store it in the BLOB so that it can be
        used as insertion or update data.

    ****************************************************************/
    virtual RETCODE SetMetaFilePictInBlob(METAFILEPICT* pSentMetaFilePict);

    /****************************************************************
        GetMetaFilePictFromBlob

        Given a BLOB that has been retrieved from the database,
        translate it into a METAFILEPICT.

    ****************************************************************/

    virtual RETCODE GetMetaFilePictFromBlob(METAFILEPICT* pMetaFilePictToFill);


}; // end odbcBLOB class
