#include <sql.hpp>
#include <cstdlib>
#include <cstring>
#include <memory.h>
#include <malloc.h>
#include <algorithm>


// types used only in this file
namespace {

    typedef struct tagBITMAP32 {
        LONG   bmType;
        LONG   bmWidth;
        LONG   bmHeight;
        LONG   bmWidthBytes;
        WORD   bmPlanes;
        WORD   bmBitsPixel;
        LPVOID bmBits;
    } BITMAP32;

    typedef struct tagMETAFILEPICT32 { /* mfp */
        LONG      mm;
        LONG      xExt;
        LONG      yExt;
        LPVOID    mfh32Bit;
    } METAFILEPICT32;

} // namespace

/****************************************************************

    odbcBLOB

    Constructor. Pass address of owning cursor, number of the
    associated column, and maximum size of the parameter;
    optionally also pass the column's SQL data type and
    put- and get-chunk granularities.
****************************************************************/
odbcBLOB::odbcBLOB(
    odbcCURSOR* pCurs,
    UWORD iSentCol,
    UDWORD cbSentMaxSize,
    SWORD  fSentSqlType,
    UWORD  iSentParam,
    SDWORD cbSentPutChunkSize,
    SDWORD cbSentGetChunkSize
)
{
    pPutCursor = pGetCursor = pCurs;
    cbMaxBlobSize = MAX_REASONABLE_C_BLOB_SIZE;
    iCol = iSentCol;
    if (iSentParam == 0)
        iParam = iCol;
    else
        iParam = iSentParam;

    fSqlType = fSentSqlType;
    cbMaxSize = cbSentMaxSize;
    pData_ = nullptr;
    cbValue = SQL_NULL_DATA;
    cbAllocatedSize = 0;
    cbPutChunkSize = cbSentPutChunkSize;
    cbGetChunkSize = cbSentGetChunkSize;
}

/****************************************************************

    ~odbcBLOB

    Destructor.
****************************************************************/
odbcBLOB::~odbcBLOB()
{
    ReleaseMem();
}

/****************************************************************

    ReleaseMem

    Free memory on the internal pointer.
****************************************************************/
void odbcBLOB::ReleaseMem()
{
    if (pData_)
    {
        free(pData_);
        pData_ = nullptr;
    }

    cbAllocatedSize = 0;
}

/****************************************************************

    GetMem

    Allocate memory to the internal pointer. We only ever have
    one pointer in use, so free it first if it is not null. If
    zero is passed, just return the current pointer.
****************************************************************/
void*    odbcBLOB::GetMem(UDWORD cbSize)
{
    // if just asking for current pointer, give it to them
    if (!cbSize)
        return pData_;

    // if already allocated, free memory
    if (pData_)
        ReleaseMem();

    // There appeared to be a bug in the BORLAND 4.0 version of the compiler
    // where by halloc is defined to call _farmalloc (with an underscore) and
    // this beast doesn't exist so for the borland compiler on 16 bit we call
    // farmalloc directly.

    pData_ = malloc(cbSize + 1);// Got rid of heap damage with SQL Server

    if (pData_)
        cbValue = cbAllocatedSize = cbSize;
    else
    {
        pGetCursor->SetRC(SQL_ALLOC_FAILED);
        if (pGetCursor != pPutCursor)
            pPutCursor->SetRC(SQL_ALLOC_FAILED);
    }

    return pData_;
}

/****************************************************************

    SetData

    Allocate memory to the internal pointer and move data into it.
****************************************************************/
RETCODE odbcBLOB::SetData(void* pSentData, UDWORD cbSentSize)
{
    void* pNew = GetMem();
    UDWORD i;

    // reallocate if needed
    if (cbSentSize != cbAllocatedSize)
    {
        pNew = GetMem(cbSentSize);
        if (!pNew)
            return SQL_ERROR;
    }

    // copy data to buffer
    for (i = 0; i < cbAllocatedSize; i++)
        ((char  *)pNew)[i] = ((char  *)pSentData)[i];

    return SQL_SUCCESS;
}

/****************************************************************

    PutData

    Call odbcSTMT::PutData() to pass data in chunkwise fashion.
****************************************************************/
RETCODE odbcBLOB::PutData(void* pSentData, UDWORD cbSentSize)
{
    RETCODE ret = SQL_SUCCESS;
    UDWORD i;

    if (cbSentSize && pSentData)
    {
        ret = SetData(pSentData, cbSentSize);
        if (ret != SQL_SUCCESS)
            return ret;
    }

    // call PutData in a loop

    i = 0;

    do
    {
        ret = pPutCursor->PutData(
            &((char  *)pData_)[i],
            (std::min)(cbPutChunkSize,
                cbAllocatedSize - i));
    } while
        (
        ((i += cbPutChunkSize)
            < cbAllocatedSize)
            && pPutCursor->sqlsuccess()
            );

    return ret;
}

/****************************************************************

    GetData

    Call odbcCURSOR::GetData() to retrieve data in chunkwise fashion.
****************************************************************/
RETCODE odbcBLOB::GetData()
{
    RETCODE ret = SQL_SUCCESS;
    SDWORD i = 0; // loop indexes
    SDWORD cbRemaining; SDWORD cbRemainingReturned; SDWORD len;
    char c[2];
    auto GetChunkSize = (SDWORD)cbGetChunkSize;
    auto fBlobCType = (SWORD)(fSqlType == SQL_LONGVARBINARY ?
        SQL_C_BINARY : SQL_C_CHAR);
    auto cbBuf = (SDWORD)(fSqlType == SQL_LONGVARBINARY ?
        1 : 2);

    // empty already allocated memory
    ReleaseMem();

    cbValue = 0;

    // see if we can get one byte and also find out how much there is.
    // we can't pass a zero because Watcom SQL barfs on an empty buffer.
    ret = pGetCursor->GetData(
        iCol,
        fBlobCType,
        c,
        cbBuf,  // to find out how much there is.
        &cbValue
    );

    if (!pGetCursor->sqlsuccess())
    {
        return ret;
    }

    if (cbValue == SQL_NO_TOTAL)
    {
        cbValue = cbMaxBlobSize;
    }

    if (cbValue == SQL_NULL_DATA)
    {
        pGetCursor->SetRC(SQL_SUCCESS);
        return SQL_NULL_DATA_FOUND;
    }
    if (cbValue == 0)
    {
        pGetCursor->SetRC(SQL_SUCCESS);
        return SQL_EMPTY_DATA_FOUND;
    }


    if (TypeIsCharacter(fSqlType) && !pGetCursor->GetNoExtraByteOnStrings())
        cbValue++;

    GetMem(cbValue + 1);


    if (!pData_)
    {
        return SQL_ERROR;
    }


    // copy the first byte over.
    ((char  *)pData_)[0] = c[0];
    ((char  *)pData_)[1] = 0;


    // do get data loop: remember we already got the first byte.
    for (
        i = 1,
        len = 1,
        GetChunkSize = (SDWORD)cbGetChunkSize - 1,
        cbRemaining = cbValue;

        pGetCursor->lastRC() == SQL_SUCCESS_WITH_INFO
        && i < cbValue;

        i += GetChunkSize,
        cbRemaining -= GetChunkSize,
        GetChunkSize = (SDWORD)cbGetChunkSize
        )
    {
        ret = pGetCursor->GetData(
            iCol,
            (SWORD)(fSqlType == SQL_LONGVARBINARY ?
                SQL_C_BINARY : SQL_C_CHAR),
                (void*)(&((char  *)pData_)[i]),
            (SDWORD)((std::min) (
                GetChunkSize,
                cbRemaining)),
            &cbRemainingReturned
        );
        if (cbRemainingReturned == SQL_NO_TOTAL)
        {
            // if there's no total to work with, set up so
            // after the subtraction is done we'll still be
            // trying to get another chunk.
            cbRemaining = cbGetChunkSize * 2;
            len += GetChunkSize;
        }
        else
        {
            // cbRemainingReturned contains the total before
            // we did the last GetData call. If we were doing
            // SQL_NO_TOTAL handling, this is the size of the
            // last chunk. Otherwise it's the size of the
            // remaining BLOB before we did our processing.
            // in either case it's our remainder, and we
            // add it to our length (or the chunk size if
            // it's smaller).
            cbRemaining = cbRemainingReturned;
            len += (std::min) (
                GetChunkSize,
                cbRemaining);
        }

    } // end for

// make the length be real if a string BLOB.
    if (fSqlType == SQL_LONGVARCHAR)
    {
        // find the index of the null terminator byte.
        // it is also the length of the string, so it's
        // one less than the size of the buffer.
        for (i = 0; i < cbValue && ((char *)pData_)[i] != 0; i++)
            ;

        // in a perfect world, i would be equal to cbValue - 1.
        // Are we perfect, or what?
        if (i < (cbValue - 1))
        {
            // guess not. Add one for the null terminator and
            // adjust our size to fit. We won't shrink the memory,
            // just adjust what we say the size is, for performance'
            // sake.
            ++i;
            cbAllocatedSize = cbValue = i;
        }
    }
    else // SQL_LONGVARBINARY
    {
        // set to the actual returned length
        cbAllocatedSize = cbValue = len;
    }

    if (!pGetCursor->sqlsuccess())
        ReleaseMem();

    return ret;
}

/****************************************************************

    BindParameter

    Bind a parameter.
****************************************************************/

RETCODE odbcBLOB::BindParameter()
{
    RETCODE ret = SQL_SUCCESS;
    SWORD iValueReturned;
    UDWORD cbColDef = cbAllocatedSize;

    // don't have error reporting temporarily...
    UWORD bSaveAutoRetrieve
        = pPutCursor->pConn->AutoRetrieve(odbcNOREPORT);
    UWORD bSaveAutoReport
        = pPutCursor->pConn->AutoReport(odbcNOREPORT);

    if (cbValue != SQL_NULL_DATA)
    {
        // if a string, get the actual number of characters
        // we are storing.
        if (fSqlType == SQL_LONGVARCHAR)
        {
            char  *lpstr;
            UDWORD i = 0;
            for (lpstr = (char  *)GetMem(),
                i = 0; i < cbAllocatedSize; i++)
                if (!lpstr[i])
                    break;
            cbColDef = i + 1;
        }

        // if the driver is a version 1.0 driver that doesn't
        // recognize SQL_NEED_LONG_DATA_LEN, use SQL_DATA_AT_EXEC.
        // if the driver doesn't have to have the data length up
        // front, use SQL_LEN_DATA_AT_EXEC(0). Otherwise we must use
        // the SQL_LEN_DATA_AT_EXEC macro to pass the entire length.

        char szValue[4];
        pPutCursor->pConn->GetInfoString(
            SQL_NEED_LONG_DATA_LEN,
            szValue,
            sizeof(szValue),
            &iValueReturned);

        if (!pPutCursor->pConn->sqlsuccess())
        {
            // bind the BLOB parameter
            cbValue = SQL_DATA_AT_EXEC;
        }
        else if (_stricmp(szValue, "Y") != 0)
        {
            // bind the BLOB parameter - driver doesn't need
            // advance warning of maximum size.
            cbValue = SQL_LEN_DATA_AT_EXEC(0);
        }
        else
        {
            // bind the BLOB parameter - driver must know the
            // maximum size.
            cbValue = SQL_LEN_DATA_AT_EXEC(((SDWORD)cbColDef));
        }
    }
    else
        cbColDef = 0; // NULL value - no length

    pPutCursor->pConn->AutoReport(bSaveAutoReport);
    pPutCursor->pConn->AutoRetrieve(bSaveAutoRetrieve);

#if (ODBCVER >= 0x0200)
    // see if we can use new ODBC API function SQLBindParameter via odbcSTMT
    // member function BindParameter().
    if (static_cast<bool>(pPutCursor->pConn->GetFunctions(SQL_API_SQLBINDPARAMETER)))
    {
        ret = pPutCursor->BindParameter(
            iParam,             // parameter number
            SQL_PARAM_INPUT,    // parm bind type - BLOBs are input only
            (SWORD)(fSqlType == SQL_LONGVARBINARY ?
                SQL_C_BINARY :
                SQL_C_CHAR),    // C type of our storage location
            fSqlType,
            // SQL type of the column
            (UDWORD)cbColDef,   // precision and scale are
            0,                  // not used for binary types
            (void*)this,          // address of our object
            (SDWORD)cbMaxSize,  // max length
            &cbValue            // data to be passed at execution
                                //   time.
        );
    }
    else
    {
        ret = pPutCursor->SetParam(
            iParam,             // parameter number
            (SWORD)(fSqlType == SQL_LONGVARBINARY ?
                SQL_C_BINARY :
                SQL_C_CHAR),    // C type of our storage location
            fSqlType,
            // SQL type of the column
            (UDWORD)cbMaxSize,  // precision
            0,                  // scale not used for binary types
            (void*)this,          // address of our object
            &cbValue            // data to be passed at execution
                                //   time.
        );
    }
#else
    ret = pPutCursor->SetParam(
        iParam,                 // parameter number
        fSqlType
        == SQL_LONGVARBINARY ?
        SQL_C_BINARY :
        SQL_C_CHAR,     // C type of our storage location
        fSqlType,
        // SQL type of the column
        (SDWORD)cbMaxSize,      // precision
        0,                      // scale not used for binary types
        (void*)this,              // address of our object
        &cbValue                // data to be passed at execution
                                //   time.
    );
#endif
    // set flag so odbcCURSOR::ExecDirect and odbcCURSOR::Execute
    // can know they should handle BLOBs.
    if (pPutCursor->sqlsuccess())
        pPutCursor->bBlobsBound = true;

    return ret;
}

/****************************************************************

    SetLPSTRInBlob

    Given a huge pointer to a null-terminated string, allocate
    the necessary amount of internal memory in the BLOB and
    copy the string to the BLOB's internal memory.
****************************************************************/
RETCODE    odbcBLOB::SetLPSTRInBlob(char  *lpstr)
{
    if (!lpstr)
    {
        SetNull();
        lpstr = (char  *)GetMem(1);
        if (lpstr)
            lpstr[0] = 0;
    }
    else
    {
        UDWORD len = 0;
        UDWORD i;
        while (lpstr[len++])
            ;

        // len now contains the length of the string including
        // the null-terminator byte. Allocate memory and copy
        // the string.

        char  *dest = (char  *)GetMem(len);

        if (!dest)
        {
            pGetCursor->SetRC(SQL_ALLOC_FAILED);
            if (pPutCursor != pGetCursor)
                pPutCursor->SetRC(SQL_ALLOC_FAILED);
            return pGetCursor->lastRC();
        }

        for (i = 0; i < len; i++)
            dest[i] = lpstr[i];
    }

    return SQL_SUCCESS;
}

/****************************************************************

    SettHGlobalInBlob

    Set the content of the internal memory from a global memory
    object, allocated via GlobalAlloc, which will be locked, copied, and
    unlocked.

****************************************************************/
RETCODE odbcBLOB::SetHGlobalInBlob(HGLOBAL hGlobal)
{
    UDWORD cbSize = 0;
    UDWORD i = 0;
    char  *pData;

    // if the handle's NULL, or its size is zero, or we can't
    // lock the handle, we are going to save NULL data for the
    // blob.
    if (hGlobal == nullptr)
    {
        SetNull();
    }
    else
    {
        cbSize = GlobalSize(hGlobal);
        if (cbSize == 0)
            SetNull();
        else if ((pData = (char  *)GlobalLock(hGlobal)) == nullptr)
            SetNull();
        else
        {
            // we get here, so we must have non-null data.
            // allocate a buffer and copy.
            char* pContent = (char  *)GetMem(cbSize);
            if (!pContent)
            {
                GlobalUnlock(hGlobal);
                pGetCursor->SetRC(SQL_ALLOC_FAILED);
                if (pPutCursor != pGetCursor)
                    pPutCursor->SetRC(SQL_ALLOC_FAILED);
                return pGetCursor->lastRC();
            }

            // whew! now we can copy the data.
            for (i = 0; i < cbSize; i++)
                pContent[i] = pData[i];

            // unlock the handle.
            GlobalUnlock(hGlobal);
        }
    }

    return SQL_SUCCESS;
}

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
HGLOBAL odbcBLOB::GetHGlobalFromBlob(UINT fuAlloc /* = GHND */)
{
    HGLOBAL hAlloc = nullptr;
    char  * pData = nullptr;
    char  * pContent = (char  *)GetMem();
    UDWORD i;

    if (IsEmpty() || IsNull())
        ;
    else
    {
        hAlloc = GlobalAlloc(fuAlloc, (UDWORD)cbValue);
        if (hAlloc)
        {
            pData = (char  *)GlobalLock(hAlloc);
        }
        if (!pData)
        {
            pGetCursor->SetRC(SQL_ALLOC_FAILED);
            if (pPutCursor != pGetCursor)
                pPutCursor->SetRC(SQL_ALLOC_FAILED);
        }
        else
        {
            for (i = 0; i < (UDWORD)cbValue; i++)
                pData[i] = pContent[i];
            GlobalUnlock(hAlloc);
        }
    }

    return hAlloc;
}

/****************************************************************
    HasFormat

    Given a BLOB that has been retrieved from the database,
    make sure it contains the correct type of data.

****************************************************************/
bool odbcBLOB::HasFormat(BLOBFORMAT bfFormatToCheck)
{
    void* pBlobData = GetMem();
    UDWORD i;

    // if it's empty, the answer is easy.
    if (!pBlobData || IsBadReadPtr(pBlobData, NUM_FMT_ENTRIES))
        return false;

    // look at BLOB for BLOBFORMAT
    for (i = 0; i < NUM_FMT_ENTRIES; i++)
        if ((BLOBFORMAT)((char  *)pBlobData)[i] != bfFormatToCheck)
            return false;

    return true;
}
/****************************************************************
    SetBitmapInBlob

    Given a HBITMAP, store it in the BLOB so that it can be
    used as insertion or update data.

****************************************************************/
RETCODE odbcBLOB::SetBitmapInBlob(HBITMAP hSentBitmap)
{
    void* pNew = GetMem();
    void far * pBitmapBits;
    UDWORD i;
    UDWORD j;
    UDWORD cbTotalSizeNeeded = 0;
    UDWORD cbSizeOfbmBits = 0;
    BITMAP bmpBitmap;
#if !defined( WIN32 ) && !defined( __WIN32__ )
    BITMAP32 bm32;
#endif

    if (!GetObject(hSentBitmap, sizeof(BITMAP), &bmpBitmap))
    {
        pGetCursor->SetRC(SQL_BAD_PARAMETER);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_BAD_PARAMETER);
        return pGetCursor->lastRC();
    }
#if !defined( WIN32 ) && !defined( __WIN32__ )
    bm32.bmType = (LONG)bmpBitmap.bmType;
    bm32.bmWidth = (LONG)bmpBitmap.bmWidth;
    bm32.bmHeight = (LONG)bmpBitmap.bmHeight;
    bm32.bmWidthBytes = (LONG)bmpBitmap.bmWidthBytes;
    bm32.bmPlanes = (WORD)bmpBitmap.bmPlanes;
    bm32.bmBitsPixel = (WORD)bmpBitmap.bmBitsPixel;
    cbSizeOfbmBits = (UDWORD)bm32.bmWidthBytes * bm32.bmHeight *
        bm32.bmPlanes;
    cbTotalSizeNeeded = (UDWORD)SIZE_FMT_ENTRIES
        + sizeof(BITMAP32)
        + cbSizeOfbmBits;
#else
    cbSizeOfbmBits = (UDWORD)bmpBitmap.bmWidthBytes * bmpBitmap.bmHeight *
        bmpBitmap.bmPlanes;
    cbTotalSizeNeeded = (UDWORD)SIZE_FMT_ENTRIES
        + sizeof(BITMAP)
        + cbSizeOfbmBits;
#endif

    // reallocate if needed
    if (cbTotalSizeNeeded != cbAllocatedSize)
    {
        pNew = GetMem(cbTotalSizeNeeded);
        if (!pNew)
        {
            pGetCursor->SetRC(SQL_ALLOC_FAILED);
            if (pPutCursor != pGetCursor)
                pPutCursor->SetRC(SQL_ALLOC_FAILED);
            return pGetCursor->lastRC();
        }
    }

    // copy BLOBFORMAT id to buffer
    for (i = 0; i < NUM_FMT_ENTRIES; i++)
        ((char  *)pNew)[i] = FMT_BITMAP;


    // copy BITMAP to buffer
    for (j = 0;
#if !defined( WIN32 ) && !defined( __WIN32__ )
        j < sizeof(BITMAP32);
#else
        j < sizeof(BITMAP);
#endif
        j++)
#if !defined( WIN32 ) && !defined( __WIN32__ )
        ((char  *)pNew)[i++] = ((char  *)&bm32)[j];
#else
        ((char  *)pNew)[i++] = ((char  *)&bmpBitmap)[j];
#endif

    // copy BITMAP bmBits to buffer
    pBitmapBits = (void far *)((char  *)pNew
        + SIZE_FMT_ENTRIES
#if !defined( WIN32 ) && !defined( __WIN32__ )
        +sizeof(BITMAP32));
#else
        + sizeof(BITMAP));
#endif
    if (cbSizeOfbmBits != (UDWORD)GetBitmapBits(hSentBitmap,
        cbSizeOfbmBits,
        pBitmapBits))
    {
        /* BLOB can't be created from bitmap */

        pGetCursor->SetRC(SQL_DATA_CONVERT_ERROR);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_DATA_CONVERT_ERROR);
        return pGetCursor->lastRC();
    }

    return SQL_SUCCESS;
}


/****************************************************************
    GetBitmapFromBlob

    Given a BLOB that has been retrieved from the database,
    translate it into a HBITMAP.

****************************************************************/
RETCODE odbcBLOB::GetBitmapFromBlob(HBITMAP* phBitmapToFill)
{
    RETCODE ret = SQL_SUCCESS;
    char  *pStoredBitmapData;
    const void far * pBitmapBits;

    if (!HasFormat(FMT_BITMAP))
    {
        pGetCursor->SetRC(SQL_BLOB_TYPE_MISMATCH);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_BLOB_TYPE_MISMATCH);
        return pGetCursor->lastRC();
    }

    pStoredBitmapData = (char  *)GetMem();

    if (pStoredBitmapData == nullptr
        || IsBadReadPtr(pStoredBitmapData,
            SIZE_FMT_ENTRIES
#if !defined( WIN32 ) && !defined( __WIN32__ )
            +sizeof(BITMAP32)))
#else
        + sizeof(BITMAP) ) )
#endif
    {
        // Bitmap was apparently stored, but must have somehow been
        // corrupted
        pGetCursor->SetRC(SQL_CORRUPT_DATA_IN_BLOB);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_CORRUPT_DATA_IN_BLOB);
        return pGetCursor->lastRC();
    }

    // skip over the blob format entries
    pStoredBitmapData += SIZE_FMT_ENTRIES;

    pBitmapBits = (const void far *)(pStoredBitmapData
#if !defined( WIN32 ) && !defined( __WIN32__ )
        +sizeof(BITMAP32));
#else
        + sizeof(BITMAP) );
#endif

#if !defined( WIN32 ) && !defined( __WIN32__ )
    *phBitmapToFill = CreateBitmap((int)(((BITMAP32 *)pStoredBitmapData)->bmWidth),
        (int)(((BITMAP32 *)pStoredBitmapData)->bmHeight),
        (BYTE)(((BITMAP32 *)pStoredBitmapData)->bmPlanes),
        (BYTE)(((BITMAP32 *)pStoredBitmapData)->bmBitsPixel),
        pBitmapBits);
#else
    *phBitmapToFill = CreateBitmap(((BITMAP*)pStoredBitmapData)->bmWidth,
        ((BITMAP*)pStoredBitmapData)->bmHeight,
        ((BITMAP*)pStoredBitmapData)->bmPlanes,
        ((BITMAP*)pStoredBitmapData)->bmBitsPixel,
        pBitmapBits);
#endif

    if (*phBitmapToFill == nullptr)
    {
        // Bitmap was apparently stored, but must have somehow been
        // corrupted
        pGetCursor->SetRC(SQL_CORRUPT_DATA_IN_BLOB);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_CORRUPT_DATA_IN_BLOB);
        return pGetCursor->lastRC();
    }


    return ret;
}

/****************************************************************
    SetMetaFilePictInBlob

    Given a METAFILEPICT, store it in the BLOB so that it can be
    used as insertion or update data.

****************************************************************/
RETCODE odbcBLOB::SetMetaFilePictInBlob(METAFILEPICT* pSentMetaFilePict)
{
    void* pNew = GetMem();
    RETCODE ret = SQL_SUCCESS;
    UDWORD cbTotalSizeNeeded = 0;
    UDWORD cbSizeOfMF = 0;
#if !defined( WIN32 ) && !defined( __WIN32__ )
    METAFILEPICT32 mfp32;
    HGLOBAL hMFBits = NULL;
    void far * fpMF = NULL;
#endif
    UDWORD i;
    UDWORD j;

    if (pSentMetaFilePict == nullptr)
    {
        pGetCursor->SetRC(SQL_BAD_PARAMETER);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_BAD_PARAMETER);
        return pGetCursor->lastRC();
    }

#if !defined( WIN32 ) && !defined( __WIN32__ )
    // use 32-bit structure for db storage
    mfp32.mm = (LONG)pSentMetaFilePict->mm;
    mfp32.xExt = (LONG)pSentMetaFilePict->xExt;
    mfp32.yExt = (LONG)pSentMetaFilePict->yExt;

    hMFBits = GetMetaFileBits(pSentMetaFilePict->hMF);
    cbSizeOfMF = GlobalSize(hMFBits);
#else
    cbSizeOfMF = GetMetaFileBitsEx(pSentMetaFilePict->hMF, 0, nullptr);
#endif

    if (cbSizeOfMF == NULL)
    {
        /* BLOB can't be created from metafile */

        pGetCursor->SetRC(SQL_DATA_CONVERT_ERROR);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_DATA_CONVERT_ERROR);
        return pGetCursor->lastRC();
    }

    cbTotalSizeNeeded = SIZE_FMT_ENTRIES
#if !defined( WIN32 ) && !defined( __WIN32__ )
        +sizeof(METAFILEPICT32)
#else
        + sizeof(METAFILEPICT)
#endif
        + sizeof(UDWORD)
        + cbSizeOfMF;

    // reallocate if needed
    if (cbTotalSizeNeeded != cbAllocatedSize)
    {
        pNew = GetMem(cbTotalSizeNeeded);
        if (!pNew)
        {
            pGetCursor->SetRC(SQL_ALLOC_FAILED);
            if (pPutCursor != pGetCursor)
                pPutCursor->SetRC(SQL_ALLOC_FAILED);
            return pGetCursor->lastRC();
        }
    }

    // copy BLOBFORMAT id to buffer
    // use i as index in the first round
    for (i = 0; i < NUM_FMT_ENTRIES; i++)
        ((char  *)pNew)[i] = FMT_METAFILEPICT;

    // copy METAFILEPICT to buffer - we always use the 32-bit version
    // of the structure for interoperability between 16- and 32-bit apps.
    // we continue to use i as the index for the destination, but now
    // we use j for the index to the source (for this and next copy
    //  operation).

#if !defined( WIN32 ) && !defined( __WIN32__ )
    for (j = 0; j < sizeof(METAFILEPICT32); j++)
        ((char  *)pNew)[i++] = ((char  *)&mfp32)[j];
#else
    for (j = 0; j < sizeof(METAFILEPICT); j++)
        ((char  *)pNew)[i++] = ((char  *)pSentMetaFilePict)[j];
#endif

    // copy size of metafile to buffer
    for (j = 0; j < sizeof(UDWORD); j++)
        ((char  *)pNew)[i++] = ((char  *)&cbSizeOfMF)[j];


#if !defined( WIN32 ) && !defined( __WIN32__ )
    fpMF = GlobalLock(hMFBits);
    if (fpMF == NULL)
    {
        /* BLOB can't be created from metafile */

        pGetCursor->SetRC(SQL_DATA_CONVERT_ERROR);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_DATA_CONVERT_ERROR);
        return pGetCursor->lastRC();
    }

    // copy metafile to buffer. i is still our index into the
    // destination buffer.
    for (j = 0; j < cbSizeOfMF; j++)
        ((char  *)pNew)[i++] = ((char  *)fpMF)[j];

    GlobalUnlock(hMFBits);
#else
    GetMetaFileBitsEx(
        pSentMetaFilePict->hMF, cbSizeOfMF,
        (LPVOID)(&((char  *)pNew)[i]));
#endif
    return ret;
}

/****************************************************************
    GetMetaFilePictFromBlob

    Given a BLOB that has been retrieved from the database,
    translate it into a METAFILEPICT.

****************************************************************/
RETCODE odbcBLOB::GetMetaFilePictFromBlob(METAFILEPICT* pMetaFilePictToFill)
{
    RETCODE ret = SQL_SUCCESS;
    UDWORD cbSizeOfMF = 0;
#if !defined( WIN32 ) && !defined( __WIN32__ )
    METAFILEPICT32 mfp32;
    HGLOBAL hgMetaFile = NULL;
    char  * pMetaFile = NULL;
#endif
    char  *pStoredMFData = nullptr;
    UDWORD i = 0;
    UDWORD j = 0;

    if (!HasFormat(FMT_METAFILEPICT))
    {
        pGetCursor->SetRC(SQL_BLOB_TYPE_MISMATCH);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_BLOB_TYPE_MISMATCH);
        return pGetCursor->lastRC();
    }

    pStoredMFData = (char  *)GetMem();

    if (pMetaFilePictToFill == nullptr
        || IsBadWritePtr(pMetaFilePictToFill,
            sizeof(METAFILEPICT)))
    {
        pGetCursor->SetRC(SQL_BAD_PARAMETER);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_BAD_PARAMETER);
        return pGetCursor->lastRC();
    }

    // skip over the blob format entries
    pStoredMFData += SIZE_FMT_ENTRIES;

    // copy buffer to METAFILEPICT
#if !defined( WIN32 ) && !defined( __WIN32__ )
    for (i = 0; i < sizeof(METAFILEPICT32); i++)
        ((char  *)&mfp32)[i] = pStoredMFData[i];
    // move data from 32-bit stored struct to 16-bit user's struct.
    pMetaFilePictToFill->mm = (int)mfp32.mm;
    pMetaFilePictToFill->xExt = (int)mfp32.xExt;
    pMetaFilePictToFill->yExt = (int)mfp32.yExt;
#else
    for (i = 0; i < sizeof(METAFILEPICT); i++)
        ((char  *)pMetaFilePictToFill)[i] = pStoredMFData[i];
#endif
    // copy size of metafile from buffer
    for (j = 0; j < sizeof(UDWORD); j++)
        ((char  *)(&cbSizeOfMF))[j]
        = pStoredMFData[i++];

#if !defined( WIN32 ) && !defined( __WIN32__ )

    hgMetaFile = GlobalAlloc(GMEM_SHARE, cbSizeOfMF);
    if (!hgMetaFile)
    {
        pGetCursor->SetRC(SQL_ALLOC_FAILED);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_ALLOC_FAILED);
        return pGetCursor->lastRC();
    }

    pMetaFile = (char  *)GlobalLock(hgMetaFile);
    if (!pMetaFile)
    {
        pGetCursor->SetRC(SQL_ALLOC_FAILED);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_ALLOC_FAILED);
        return pGetCursor->lastRC();
    }

    // copy METAFILE to buffer
    for (j = 0; j < cbSizeOfMF; j++)
        ((char  *)pMetaFile)[j] = pStoredMFData[i++];

    GlobalUnlock(hgMetaFile);

    pMetaFilePictToFill->hMF = SetMetaFileBits((HMETAFILE)hgMetaFile);
#else
    pMetaFilePictToFill->hMF
        = SetMetaFileBitsEx(cbSizeOfMF,
        (LPUCSTR)
            &pStoredMFData[i]);
#endif
    if (!pMetaFilePictToFill->hMF)
    {
        // Metafile was apparently stored, but must have somehow been
        // corrupted
        pGetCursor->SetRC(SQL_CORRUPT_DATA_IN_BLOB);
        if (pPutCursor != pGetCursor)
            pPutCursor->SetRC(SQL_CORRUPT_DATA_IN_BLOB);
        return pGetCursor->lastRC();
    }

    return ret;
}
