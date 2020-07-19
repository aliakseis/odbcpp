#pragma once

#include <stddef.h>

typedef UCHAR *             LPUSTR;
typedef const UCHAR    *   LPUCSTR;



#if !defined( WIN32 ) && !defined( __WIN32__ )
    #if defined( _MSC_VER )
        // for external classes
        #ifdef _DLL
            #define odbcEXPORTED _export
        #else
            #if defined( _WINDLL ) && !defined( IMPORT_ODBCLASSLIB )
                #define odbcEXPORTED _export
            #else
                #define odbcEXPORTED far
            #endif
        #endif

    #else // or other compilers

        // for external classes
        #define odbcEXPORTED _export

#endif
#else // WIN32
    #if defined(_MSC_VER) // MS VC++ 2.0

        #if !defined( IMPORT_ODBCLASSLIB )
//          #pragma message ( "#define odbcEXPORTED _declspec( dllexport )" )
            #define odbcEXPORTED _declspec( dllexport )
        #else
//          #pragma message ( "#define odbcEXPORTED" )
            #define odbcEXPORTED
        #endif

    #else  // other compilers

        #define odbcEXPORTED _export

    #endif

#endif // if !defined( WIN32 )


// classes in v1.0
class odbcEXPORTED odbcBASE;
class odbcEXPORTED odbcCONNELEM;
class odbcEXPORTED odbcCURSOR;
class odbcEXPORTED odbcSTMT;
class odbcEXPORTED odbcCONNECT;
class odbcEXPORTED odbcENV;
// iterator classes in v1.0
class odbcEXPORTED odbcTableIterator;
class odbcEXPORTED odbcColumnIterator;
class odbcEXPORTED odbcStatisticsIterator;
class odbcEXPORTED odbcSpecialColumnIterator;
class odbcEXPORTED odbcTypeInfoIterator;

// new in v2.0
class odbcEXPORTED odbcTABLECREATOR;
class odbcEXPORTED odbcRECORD;
class odbcEXPORTED odbcRECINSERTER;
class odbcEXPORTED odbcRECUPDATER;
class odbcEXPORTED odbcBLOB; 
// iterator classes added in v2.0
class odbcEXPORTED odbcColumnPrivilegesIterator;
class odbcEXPORTED odbcForeignKeysIterator;
class odbcEXPORTED odbcPrimaryKeysIterator;
class odbcEXPORTED odbcProcedureColumnsIterator;
class odbcEXPORTED odbcProceduresIterator;
class odbcEXPORTED odbcTablePrivilegesIterator;

// end new in v2.0

const int odbcSTATE_SIZE = (SQL_SQLSTATE_SIZE+1);
const int odbcERRMSG_SIZE = SQL_MAX_MESSAGE_LENGTH;

const int odbcNOREPORT              = 0;
const int odbcREPERRS               = 1;
const int odbcREPSUCCESSWITHINFO    = 2;
const int STRING1_MAX               = 128;
const int TABLE_QUALIFIER_MAX       = STRING1_MAX;
const int TABLE_OWNER_MAX           = STRING1_MAX;
const int TABLE_NAME_MAX            = STRING1_MAX;
const int TABLE_TYPE_MAX            = STRING1_MAX;
const int COLUMN_NAME_MAX           = STRING1_MAX;
const int TYPE_NAME_MAX             = STRING1_MAX;
const int REMARKS_MAX               = 254;
const int DSN_DESC_MAX              = STRING1_MAX;
const int INDEX_QUALIFIER_MAX       = STRING1_MAX;
const int INDEX_NAME_MAX            = STRING1_MAX;
const int COLLATION_SIZE            = 1;
const int LIBERRMSGSIZE             = 32;

// new in v2.0

const int STRING2_MAX               = 256;
const int SQLSTMT_MAX               = 5000;

#if (ODBCVER >= 0x0200)
const int DRIVER_MAX                = STRING1_MAX;
const int DRIVER_ATTRIBS_MAX        = STRING2_MAX;
#endif // if (ODBCVER >= 0x0200)

// additional error code defines

const RETCODE SQL_ALLOC_FAILED          = -1000;

// new in v2.0

const RETCODE SQL_BUFFER_TOO_SMALL = -1001;
const RETCODE SQL_BAD_PARAMETER = -1002;
const RETCODE SQL_NOT_EXTSETUP = -1003;
const RETCODE SQL_ILLFORMED_STMT = -1004;
const RETCODE SQL_CANT_CONVERT_DATATYPE = -1005;
const RETCODE SQL_BAD_CURSOR = -1006;
const RETCODE SQL_STMT_CTOR_FAILED = -1007;
const RETCODE SQL_INTERNAL_INSERT_FAILED = -1008;
const RETCODE SQL_UPDATE_STMT_CTOR_FAILED = -1009;
const RETCODE SQL_INTERNAL_UPDATE_FAILED = -1010;
const RETCODE SQL_INTERNAL_DELETE_FAILED = -1011;
const RETCODE SQL_INTERNAL_SETPOS_FAILED = -1012;
const RETCODE SQL_NULL_TBL_NAME = -1013;
const RETCODE SQL_NO_SELECT = -1014;
const RETCODE SQL_INTERNAL_PREPARE_FAILED = -1015;
const RETCODE SQL_NO_DATA_IN_BLOB = -1016;
const RETCODE SQL_CORRUPT_DATA_IN_BLOB = -1017;
const RETCODE SQL_DATA_CONVERT_ERROR = -1018;
const RETCODE SQL_BAD_DISP_COLUMN = -1019;
const RETCODE SQL_BAD_ATTACH_COLUMN = -1020;
const RETCODE SQL_ENTRY_FMT_ERROR = -1021;
const RETCODE SQL_ENTRY_RANGE_ERROR = -1022;
const RETCODE SQL_NO_EXT_OPS = -1023;
const RETCODE SQL_APPEND_WHERE_CLAUSE_FAILED = -1024;
const RETCODE SQL_BLOB_TYPE_MISMATCH = -1025;


const RETCODE SQL_INTERNAL_ERR_MAX   =       SQL_ALLOC_FAILED;
const RETCODE SQL_INTERNAL_ERR_MIN   =       SQL_BLOB_TYPE_MISMATCH;


// Procedures and ProcedureColumns
const unsigned PROCEDURE_QUALIFIER_SIZE         = 255 ;
const unsigned PROCEDURE_OWNER_SIZE             = 129 ;
const unsigned PROCEDURE_NAME_SIZE              = 129 ;
const unsigned REMARKS_SIZE                     = 255 ;

// ProcedureColumns
const unsigned COLUMN_NAME_SIZE                 = 129 ;
const unsigned DATA_TYPE_SIZE                   = 9 ;
const unsigned TYPE_NAME_SIZE                   = 8 ;

// ForeignKeys
const unsigned PKTABLE_QUALIFIER_SIZE           = 512 ;
const unsigned PKTABLE_OWNER_SIZE               = 129 ;
const unsigned PKTABLE_NAME_SIZE                = 129 ;
const unsigned PKCOLUMN_NAME_SIZE               = 129 ;
const unsigned FKTABLE_QUALIFIER_SIZE           = 512 ;
const unsigned FKTABLE_OWNER_SIZE               = 129 ;
const unsigned FKTABLE_NAME_SIZE                = 129 ;
const unsigned FKCOLUMN_NAME_SIZE               = 129 ;
const unsigned ROLE_NAME_SIZE                   = 129 ;

// ColumnPrivileges and TablePrivileges and PrimaryKeys
const unsigned TABLE_QUALIFIER_SIZE             = 255 ;
const unsigned TABLE_OWNER_SIZE                 = 129 ;
const unsigned TABLE_NAME_SIZE                  = 129 ;
const unsigned GRANTOR_SIZE                     = 129 ;
const unsigned GRANTEE_SIZE                     = 129 ;
const unsigned PRIVILEGE_SIZE                   = 11 ;
const unsigned IS_GRANTABLE_SIZE                = 4 ;

// macros to determine type category for SQL types
inline bool TypeIsInteger(SWORD t)
{
    return ((t)==SQL_BIT || (t)==SQL_INTEGER ||
        (t)==SQL_SMALLINT || (t)==SQL_BIGINT || (t)==SQL_TINYINT);
}

inline bool TypeIsCurrency(SWORD t)
{
    return ((t)==SQL_NUMERIC || (t)==SQL_DECIMAL);
}

inline bool TypeIsDate(SWORD t)
{
    return ((t)==SQL_DATE);
}

inline bool TypeIsTime(SWORD t)
{
    return ((t)==SQL_TIME);
}

inline bool TypeIsTimestamp(SWORD t)
{
    return ((t)==SQL_TIMESTAMP);
}

inline bool TypeIsNumeric(SWORD t)
{
    return ((t)==SQL_REAL || (t)==SQL_FLOAT || (t)==SQL_DOUBLE
         || TypeIsCurrency((t)));
}

inline bool TypeIsCharacter(SWORD t)
{
    return ((t)==SQL_CHAR || (t)==SQL_VARCHAR || (t)==SQL_LONGVARCHAR);
}

inline bool TypeIsBinary(SWORD t)
{
    return ((t)==SQL_BINARY || (t)==SQL_VARBINARY || (t)==SQL_LONGVARBINARY);
}


// these are the default sizes for BLOB data transfers 
// in PutData() and GetData() calls respectively - they work
// fine with MS Access but may need to be optimized for other
// data sources.
const UDWORD BLOB_CHUNK_PUT_SIZE = 65536L;
const UDWORD BLOB_CHUNK_GET_SIZE = 32768L;

// special return codes from odbcBLOB::GetData()
const RETCODE SQL_NULL_DATA_FOUND = 32000;
const RETCODE SQL_EMPTY_DATA_FOUND = 32001;

const UDWORD MAX_REASONABLE_C_CHAR_SIZE   = (UDWORD)4096L;
const UDWORD MAX_REASONABLE_C_BINARY_SIZE = (UDWORD)4096L;
const SDWORD MAX_REASONABLE_C_BLOB_SIZE   = (SDWORD)0x3fffefL; // ~4MB

// end new in v2.0
