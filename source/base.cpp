#include <sql.hpp>
#include <cstring>
#include <cstdio>

namespace {

    const char UNKNOWN_ERR_MSG[] = "Unknown error.";
    const char MEM_ERR_MSG[] = "Memory allocation error.";

    const char BUF_WARN_MSG[] = "Buffer too small.";
    const char BADPARM_ERR_MSG[] = "Bad parameter.";
    const char NOTEXT_ERR_MSG[] = "Not set up for extended fetch operations.";
    const char ILLSTMT_ERR_MSG[] = "Ill-formed statement.";
    const char CVT_ERR_MSG[] = "Can't convert data type.";
    const char BADCUR_ERR_MSG[] = "Bad cursor pointer encountered.";
    const char INSCTOR_ERR_MSG[] = "Internal secondary statement constructor failed.";
    const char INTINS_ERR_MSG[] = "Internal insert operation failed.";
    const char UPDCTOR_ERR_MSG[] = "Internal update/delete statement failed.";
    const char INTUPD_ERR_MSG[] = "Internal update operation failed.";
    const char INTDEL_ERR_MSG[] = "Internal delete operation failed.";
    const char INTSETPOS_ERR_MSG[] = "Internal SetPos operation failed.";
    const char NULLTBL_ERR_MSG[] = "InternalodbcRECORD class requires table name.";
    const char NOT_SELECT_MSG[] = "InternalStatement passed to odbcRECORD was not a SELECT statement.";
    const char INTPREP_ERR_MSG[] = "Internal prepare operation failed.";
    const char EMPTYBLOB_ERR_MSG[] = "BLOB did not contain data.";
    const char CORRUPTBLOB_ERR_MSG[] = "BLOB contains corrupt data.";
    const char DATACVT_ERR_MSG[] = "Error converting data into BLOB format.";
    const char DISPCOL_ERR_MSG[] = "Unknown or non-character display column in dialog operation.";
    const char ATTCOL_ERR_MSG[] = "Unknown attach column in list box operation.";
    const char ENTRYFMT_ERR_MSG[] = "Data entry formatting error.";
    const char ENTRYRANGE_ERR_MSG[] = "Data entry range error.";
    const char NOEXTOPS_ERR_MSG[] = "Extended ins/upd/del operations not supported.";
    const char APPENDWHERE_ERR_MSG[] = "WHERE clause append operation failed.";

    // internal error reporting information
    struct sINTERNALERROR {
        RETCODE  nErr;
        const char *szSqlState;
        const char *szMsg;
        RETCODE nNewRet;
    } sInternalErr[] =
    {
        {
            SQL_ALLOC_FAILED,
            "S1001",
            MEM_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_BUFFER_TOO_SMALL,
            "S1000",
            BUF_WARN_MSG,
            SQL_SUCCESS_WITH_INFO,
        },
        {
            SQL_BAD_PARAMETER,
            "S1000",
            BADPARM_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_NOT_EXTSETUP,
            "S1000",
            NOTEXT_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_ILLFORMED_STMT,
            "S1000",
            ILLSTMT_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_CANT_CONVERT_DATATYPE,
            "S1000",
            CVT_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_BAD_CURSOR,
            "S1000",
            BADCUR_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_STMT_CTOR_FAILED,
            "S1000",
            INSCTOR_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_INTERNAL_INSERT_FAILED,
            "S1000",
            INTINS_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_UPDATE_STMT_CTOR_FAILED,
            "S1000",
            UPDCTOR_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_INTERNAL_UPDATE_FAILED,
            "S1000",
            INTUPD_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_INTERNAL_DELETE_FAILED,
            "S1000",
            INTDEL_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_INTERNAL_SETPOS_FAILED,
            "S1000",
            INTSETPOS_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_NULL_TBL_NAME,
            "S1000",
            NULLTBL_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_NO_SELECT,
            "S1000",
            NOT_SELECT_MSG,
            SQL_ERROR,
        },
        {
            SQL_INTERNAL_PREPARE_FAILED,
            "S1000",
            INTPREP_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_NO_DATA_IN_BLOB,
            "S1000",
            EMPTYBLOB_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_CORRUPT_DATA_IN_BLOB,
            "S1000",
            CORRUPTBLOB_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_DATA_CONVERT_ERROR,
            "S1000",
            DATACVT_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_BAD_DISP_COLUMN,
            "S1000",
            DISPCOL_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_BAD_ATTACH_COLUMN,
            "S1000",
            ATTCOL_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_ENTRY_RANGE_ERROR,
            "S1000",
            ENTRYRANGE_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_ENTRY_FMT_ERROR,
            "S1000",
            ENTRYFMT_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_NO_EXT_OPS,
            "S1000",
            NOEXTOPS_ERR_MSG,
            SQL_ERROR,
        },
        {
            SQL_APPEND_WHERE_CLAUSE_FAILED,
            "S1000",
            APPENDWHERE_ERR_MSG,
            SQL_ERROR,
        },
    };

    const int nInternalErrs
        = sizeof(sInternalErr) / sizeof(sInternalErr[0]);

} // namespace

/**********************************************************
        odbcBASE

        default constructor.
**********************************************************/

odbcBASE::odbcBASE()
{
    ErrHandler = nullptr;
    lastRet = 0;
    bReportErrorInfo = odbcNOREPORT;
    bGetErrorInfo = odbcREPERRS;
    szSqlState[0] = 0;
    fNativeError = 0;
    szErrorMsg = nullptr;
    cbErrorMsgActual = 0;
    hwnd_ = HWND_DESKTOP;
    flags = MB_OK | MB_ICONASTERISK;
}

/**********************************************************
        ~odbcBASE

        destructor.
**********************************************************/

odbcBASE::~odbcBASE()
{
    if (szErrorMsg)
    {
        delete[] szErrorMsg;
        szErrorMsg = nullptr;
    }
}


///////////////////////////////////////////////////////////
//////////////////////// base class for error handling
///////////////////////////////////////////////////////////

void odbcBASE::SetRC(RETCODE rc)
{
    int i = nInternalErrs;
    lastRet = rc;

    // clear buffers
    szSqlState[0] = 0;
    if (szErrorMsg)
        szErrorMsg[0] = 0;

    if (rc <= SQL_INTERNAL_ERR_MAX
        && rc >= SQL_INTERNAL_ERR_MIN)
        for (i = 0; i < nInternalErrs; i++)
            if (lastRet == sInternalErr[i].nErr)
                break;

    if (i < nInternalErrs)
    {
        // save internal error code
        fNativeError = rc;

        if ((bGetErrorInfo == odbcREPSUCCESSWITHINFO) ||
            ((bGetErrorInfo == odbcREPERRS)))
        {
            // install our own error message
            strcpy((LPSTR)szSqlState, sInternalErr[i].szSqlState);
            fNativeError = 0;
            if (!szErrorMsg)
            {
                szErrorMsg = new UCHAR[odbcERRMSG_SIZE];
            }
            if (szErrorMsg)
                strcpy((LPSTR)szErrorMsg, sInternalErr[i].szMsg);
            lastRet = sInternalErr[i].nNewRet;
            if ((bReportErrorInfo == odbcREPSUCCESSWITHINFO) ||
                ((bReportErrorInfo == odbcREPERRS)))
            {
                // is there an object-level error
                // handler?
                if (ErrHandler)
                    do
                    {
                        (*ErrHandler)
                            (
                                lastRet,
                                (UCHAR     *)szSqlState,
                                fNativeError,
                                (UCHAR     *)sInternalErr[i].szMsg,
                                this
                                );
                    } while (NextError() != SQL_NO_DATA_FOUND);

                    // else, just call Report()
                else
                    do
                    {
                        Report();
                    } while (NextError() != SQL_NO_DATA_FOUND);
            }
        }
    }
    else if (lastRet == SQL_ERROR ||
        lastRet == SQL_SUCCESS_WITH_INFO)
    {
        if ((bGetErrorInfo == odbcREPSUCCESSWITHINFO) ||
            ((bGetErrorInfo == odbcREPERRS) &&
            (lastRet != SQL_SUCCESS_WITH_INFO)))
        {
            RegisterError();
            if ((bReportErrorInfo == odbcREPSUCCESSWITHINFO) ||
                ((bReportErrorInfo == odbcREPERRS) &&
                (lastRet != SQL_SUCCESS_WITH_INFO)))
            {
                // is there an object-level error
                // handler?
                if (ErrHandler)
                    do
                    {
                        (*ErrHandler)
                            (
                                lastRet,
                                szSqlState,
                                fNativeError,
                                szErrorMsg ? (UCHAR     *)szErrorMsg : (UCHAR     *)"",
                                this
                                );
                    } while (NextError() != SQL_NO_DATA_FOUND);
                    // else, just call Report()
                else
                    do
                    {
                        Report();
                    } while (NextError() != SQL_NO_DATA_FOUND);
            }
        }
    }
}

RETCODE odbcBASE::Error(
    HENV            henv,
    HDBC            hdbc,
    HSTMT           hstmt
)
{
    if (!szErrorMsg)
    {
        szErrorMsg = new UCHAR[odbcERRMSG_SIZE];
        if (!szErrorMsg)
        {
            SetRC(SQL_ALLOC_FAILED);
            return lastRC();
        }
    }
    szSqlState[0] = 0;
    szErrorMsg[0] = 0;
    cbErrorMsgActual = 0;
    fNativeError = 0;

    RETCODE ret = SQLError(
        henv,
        hdbc,
        hstmt,
        szSqlState,
        &fNativeError,
        szErrorMsg,
        odbcERRMSG_SIZE,
        &cbErrorMsgActual
    );

    return ret;
}

int odbcBASE::Report()
{
    char buf[sizeof(szSqlState) + 10];
    wsprintf(buf, "%s(%05d)", szSqlState, lastRet);
    return
        (
            ReportResponse =
            MessageBox(
                hwnd_,
                szErrorMsg ?
                (const char *)szErrorMsg :
                UNKNOWN_ERR_MSG,
                (const char *)buf,
                flags)
            );
}

/**********************************************************

        TrimTrailingBlanks

        Trim trailing blanks from a string.

**********************************************************/
void odbcBASE::TrimTrailingBlanks(LPUSTR lpsz)
{
    int i = lstrlen((LPSTR)lpsz);

    if (i > 0)
        i--;
    else
        return;

    for (; i >= 0 && lpsz[i] == ' '; i--)
        lpsz[i] = 0;
}

// new in v2.0

/**********************************************************

        GetConvertInfoFlag

        Given a data type, get the flag to pass to SQLGetInfo
    to get its data type conversion support information.

**********************************************************/
UWORD odbcBASE::GetConvertInfoFlag(SWORD fDataType)
{
    UWORD uRet;

    switch (fDataType)
    {
    case SQL_CHAR:
        uRet = SQL_CONVERT_CHAR;
        break;

    case SQL_NUMERIC:
        uRet = SQL_CONVERT_NUMERIC;
        break;

    case SQL_DECIMAL:
        uRet = SQL_CONVERT_DECIMAL;
        break;

    case SQL_INTEGER:
        uRet = SQL_CONVERT_INTEGER;
        break;

    case SQL_SMALLINT:
        uRet = SQL_CONVERT_SMALLINT;
        break;

    case SQL_FLOAT:
        uRet = SQL_CONVERT_FLOAT;
        break;

    case SQL_REAL:
        uRet = SQL_CONVERT_REAL;
        break;

    case SQL_DOUBLE:
        uRet = SQL_CONVERT_DOUBLE;
        break;

    case SQL_VARCHAR:
        uRet = SQL_CONVERT_VARCHAR;
        break;

    case SQL_DATE:
        uRet = SQL_CONVERT_DATE;
        break;

    case SQL_TIME:
        uRet = SQL_CONVERT_TIME;
        break;

    case SQL_TIMESTAMP:
        uRet = SQL_CONVERT_TIMESTAMP;
        break;

    case SQL_LONGVARCHAR:
        uRet = SQL_CONVERT_LONGVARCHAR;
        break;

    case SQL_BINARY:
        uRet = SQL_CONVERT_BINARY;
        break;

    case SQL_VARBINARY:
        uRet = SQL_CONVERT_VARBINARY;
        break;

    case SQL_LONGVARBINARY:
        uRet = SQL_CONVERT_LONGVARBINARY;
        break;

    case SQL_BIGINT:
        uRet = SQL_CONVERT_BIGINT;
        break;

    case SQL_TINYINT:
        uRet = SQL_CONVERT_TINYINT;
        break;

    case SQL_BIT:
        uRet = SQL_CONVERT_BIT;
        break;

    default:
        uRet = 0;
        break;

    }

    return uRet;
}

/**********************************************************

        GetConvertInfoBitMask

        Given a data type, get the bitmask to use on the return
    for a SQLGetInfo call with a SQL_CONVERT_<type> flag
    to get its data type conversion support information.

**********************************************************/
UDWORD odbcBASE::GetConvertInfoBitMask(SWORD fDataType)
{
    UDWORD uRet;

    switch (fDataType)
    {
    case SQL_CHAR:
        uRet = SQL_CVT_CHAR;
        break;

    case SQL_NUMERIC:
        uRet = SQL_CVT_NUMERIC;
        break;

    case SQL_DECIMAL:
        uRet = SQL_CVT_DECIMAL;
        break;

    case SQL_INTEGER:
        uRet = SQL_CVT_INTEGER;
        break;

    case SQL_SMALLINT:
        uRet = SQL_CVT_SMALLINT;
        break;

    case SQL_FLOAT:
        uRet = SQL_CVT_FLOAT;
        break;

    case SQL_REAL:
        uRet = SQL_CVT_REAL;
        break;

    case SQL_DOUBLE:
        uRet = SQL_CVT_DOUBLE;
        break;

    case SQL_VARCHAR:
        uRet = SQL_CVT_VARCHAR;
        break;

    case SQL_DATE:
        uRet = SQL_CVT_DATE;
        break;

    case SQL_TIME:
        uRet = SQL_CVT_TIME;
        break;

    case SQL_TIMESTAMP:
        uRet = SQL_CVT_TIMESTAMP;
        break;

    case SQL_LONGVARCHAR:
        uRet = SQL_CVT_LONGVARCHAR;
        break;

    case SQL_BINARY:
        uRet = SQL_CVT_BINARY;
        break;

    case SQL_VARBINARY:
        uRet = SQL_CVT_VARBINARY;
        break;

    case SQL_LONGVARBINARY:
        uRet = SQL_CVT_LONGVARBINARY;
        break;

    case SQL_BIGINT:
        uRet = SQL_CVT_BIGINT;
        break;

    case SQL_TINYINT:
        uRet = SQL_CVT_TINYINT;
        break;

    case SQL_BIT:
        uRet = SQL_CVT_BIT;
        break;

    default:
        uRet = 0;
        break;

    }

    return uRet;
}

/***************************************************

    PrecisionForSqlType

    Returns the correct precision for a given SQL
    data type.

***************************************************/

UDWORD odbcBASE::PrecisionForSqlType(SWORD fSqlType, UDWORD cbValueMax)
{
    switch (fSqlType)
    {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
        return cbValueMax;

    case SQL_BIGINT:
        return 20;

    case SQL_BIT:
        return 1;

    case SQL_TINYINT:
        return 3;

    case SQL_SMALLINT:
        return 5;

    case SQL_INTEGER:
        return 10;

    case SQL_REAL:
        return 7;

    case SQL_DECIMAL:
    case SQL_NUMERIC:
    case SQL_FLOAT:
    case SQL_DOUBLE:
        return 15;

    case SQL_DATE:
        return 10;


    case SQL_TIME:
        return 8;


    case SQL_TIMESTAMP:
        return 23;

    default:
        break;
    } // end switch

    return 0;
}

/***************************************************

    LengthForCType

    Returns the correct length for a given C language
    data type.

***************************************************/

UDWORD odbcBASE::LengthForCType(SWORD fCType, UDWORD cbValueMax)
{
    switch (fCType)
    {
    case SQL_C_CHAR:
    case SQL_C_BINARY:
    case SQL_C_DEFAULT:
        return cbValueMax;

    case SQL_C_SSHORT:
    case SQL_C_SHORT:
        return sizeof(short int);

    case SQL_C_USHORT:
        return sizeof(unsigned short int);

    case SQL_C_LONG:
    case SQL_C_SLONG:
        return sizeof(long int);

    case SQL_C_ULONG:
        return sizeof(unsigned long int);

    case SQL_C_FLOAT:
        return sizeof(float);

    case SQL_C_DOUBLE:
        return sizeof(double);

    case SQL_C_BIT:
        return sizeof(unsigned char);

    case SQL_C_STINYINT:
        return sizeof(signed char);

    case SQL_C_UTINYINT:
        return sizeof(unsigned char);

    case SQL_C_DATE:
        return sizeof(DATE_STRUCT);

    case SQL_C_TIME:
        return sizeof(TIME_STRUCT);

    case SQL_C_TIMESTAMP:
        return sizeof(TIMESTAMP_STRUCT);

    default:
        break;
    } // end switch

    return 0;
}
