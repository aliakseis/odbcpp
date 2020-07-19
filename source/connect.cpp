#include <sql.hpp>

#include <mutex>

/**********************************************************
    _SQLStatisticsRSB

    Column bindings for result set for SQLStatistics.
**********************************************************/

static sCOLBIND _SQLStatisticsRSB[12] =
{
    _colbindChar(1,sSQLSTATISTICSRESULTSET,szTableQualifier,TABLE_QUALIFIER_MAX),
    _colbindChar(2,sSQLSTATISTICSRESULTSET,szTableOwner,TABLE_OWNER_MAX),
    _colbindChar(3,sSQLSTATISTICSRESULTSET,szTableName,TABLE_NAME_MAX),
    _colbindShort(4,sSQLSTATISTICSRESULTSET,fNonUnique),
    _colbindChar(5,sSQLSTATISTICSRESULTSET,szIndexQualifier,INDEX_QUALIFIER_MAX),
    _colbindChar(6,sSQLSTATISTICSRESULTSET,szIndexName,INDEX_NAME_MAX),
    _colbindShort(7,sSQLSTATISTICSRESULTSET,fType),
    _colbindShort(8,sSQLSTATISTICSRESULTSET,fSeqInIndex),
    _colbindChar(9,sSQLSTATISTICSRESULTSET,szColumnName,COLUMN_NAME_MAX),
    _colbindChar(10,sSQLSTATISTICSRESULTSET,cCollation,COLLATION_SIZE),
    _colbindLong(11,sSQLSTATISTICSRESULTSET,nCardinality),
    _colbindLong(12,sSQLSTATISTICSRESULTSET,nPages),

};

/**********************************************************
    _SQLTypeInfoRSB

    Column bindings for result set for SQLGetTypeInfo.
**********************************************************/

static sCOLBIND _SQLTypeInfoRSB[13] =
{
    {
    1,
    SQL_C_CHAR,
    offsetof(sSQLTYPEINFORESULTSET,szTypeName),
    STRING1_MAX,
    },

    {
    2,
    SQL_C_SHORT,
    offsetof(sSQLTYPEINFORESULTSET,fDataType),
    sizeof(SWORD),
    },

    {
    3,
    SQL_C_LONG,
    offsetof(sSQLTYPEINFORESULTSET,fPrecision),
    sizeof(SDWORD),
    },

    {
    4,
    SQL_C_CHAR,
    offsetof(sSQLTYPEINFORESULTSET,szLiteralPrefix),
    STRING1_MAX,
    },

    {
    5,
    SQL_C_CHAR,
    offsetof(sSQLTYPEINFORESULTSET,szLiteralSuffix),
    STRING1_MAX,
    },

    {
    6,
    SQL_C_CHAR,
    offsetof(sSQLTYPEINFORESULTSET,szCreateParams),
    STRING1_MAX,
    },

    {
    7,
    SQL_C_SHORT,
    offsetof(sSQLTYPEINFORESULTSET,fNullable),
    sizeof(SWORD),
    },

    {
    8,
    SQL_C_SHORT,
    offsetof(sSQLTYPEINFORESULTSET,fCaseSensitive),
    0,
    },

    {
    9,
    SQL_C_SHORT,
    offsetof(sSQLTYPEINFORESULTSET,fSearchable),
    sizeof(SWORD),
    },

    {
    10,
    SQL_C_SHORT,
    offsetof(sSQLTYPEINFORESULTSET,fUnsigned),
    0,
    },

    {
    11,
    SQL_C_SHORT,
    offsetof(sSQLTYPEINFORESULTSET,fMoney),
    sizeof(SWORD),
    },

    {
    12,
    SQL_C_SHORT,
    offsetof(sSQLTYPEINFORESULTSET,fAutoIncrement),
    sizeof(SWORD),
    },

    {
    13,
    SQL_C_CHAR,
    offsetof(sSQLTYPEINFORESULTSET,szLocalTypeName),
    STRING1_MAX,
    },

};

/**********************************************************
    _SQLTablesRSB

    Column bindings for result set for SQLTables.
**********************************************************/

static sCOLBIND _SQLTablesRSB[5] =
{
    {
    1,
    SQL_C_CHAR,
    offsetof(sSQLTABLESRESULTSET,szTableQualifier),
    TABLE_QUALIFIER_MAX,
    },

    {
    2,
    SQL_C_CHAR,
    offsetof(sSQLTABLESRESULTSET,szTableOwner),
    TABLE_OWNER_MAX,
    },

    {
    3,
    SQL_C_CHAR,
    offsetof(sSQLTABLESRESULTSET,szTableName),
    TABLE_NAME_MAX,
    },

    {
    4,
    SQL_C_CHAR,
    offsetof(sSQLTABLESRESULTSET,szTableType),
    TABLE_TYPE_MAX,
    },

    {
    5,
    SQL_C_CHAR,
    offsetof(sSQLTABLESRESULTSET,Remarks),
    REMARKS_MAX,
    },

};

/**********************************************************
    _SQLColumnsRSB

    Column bindings for result set for SQLColumns.
**********************************************************/

static sCOLBIND _SQLColumnsRSB[12] =
{
    {
    1,
    SQL_C_CHAR,
    offsetof(sSQLCOLUMNSRESULTSET,szTableQualifier),
    TABLE_QUALIFIER_MAX,
    },

    {
    2,
    SQL_C_CHAR,
    offsetof(sSQLCOLUMNSRESULTSET,szTableOwner),
    TABLE_OWNER_MAX,
    },

    {
    3,
    SQL_C_CHAR,
    offsetof(sSQLCOLUMNSRESULTSET,szTableName),
    TABLE_NAME_MAX,
    },

    {
    4,
    SQL_C_CHAR,
    offsetof(sSQLCOLUMNSRESULTSET,szColumnName),
    COLUMN_NAME_MAX,
    },

    {
    5,
    SQL_C_SHORT,
    offsetof(sSQLCOLUMNSRESULTSET,fDataType),
    sizeof(SWORD),
    },

    {
    6,
    SQL_C_CHAR,
    offsetof(sSQLCOLUMNSRESULTSET,szTypeName),
    TYPE_NAME_MAX,
    },

    {
    7,
    SQL_C_LONG,
    offsetof(sSQLCOLUMNSRESULTSET,fPrecision),
    sizeof(SDWORD),
    },

    {
    8,
    SQL_C_LONG,
    offsetof(sSQLCOLUMNSRESULTSET,fLength),
    sizeof(SDWORD),
    },

    {
    9,
    SQL_C_SHORT,
    offsetof(sSQLCOLUMNSRESULTSET,Scale),
    sizeof(SWORD),
    },

    {
    10,
    SQL_C_SHORT,
    offsetof(sSQLCOLUMNSRESULTSET,Radix),
    sizeof(SWORD),
    },

    {
    11,
    SQL_C_SHORT,
    offsetof(sSQLCOLUMNSRESULTSET,Nullable),
    sizeof(SWORD),
    },

    {
    12,
    SQL_C_CHAR,
    offsetof(sSQLCOLUMNSRESULTSET,Remarks),
    REMARKS_MAX,
    },

};


/**********************************************************
    _SQLSpecialColumnsRSB

    Column bindings for result set for SQLSpecialColumns.
**********************************************************/

static sCOLBIND _SQLSpecialColumnsRSB[7] =
{
    {
    1,
    SQL_C_SHORT,
    offsetof(sSQLSPECIALCOLRESULTSET,fScope),
    sizeof(SWORD),
    },

    {
    2,
    SQL_C_CHAR,
    offsetof(sSQLSPECIALCOLRESULTSET,szColumnName),
    COLUMN_NAME_MAX,
    },

    {
    3,
    SQL_C_SHORT,
    offsetof(sSQLSPECIALCOLRESULTSET,fDataType),
    sizeof(SWORD),
    },

    {
    4,
    SQL_C_CHAR,
    offsetof(sSQLSPECIALCOLRESULTSET,szTypeName),
    TYPE_NAME_MAX,
    },

    {
    5,
    SQL_C_LONG,
    offsetof(sSQLSPECIALCOLRESULTSET,fPrecision),
    sizeof(SDWORD),
    },

    {
    6,
    SQL_C_LONG,
    offsetof(sSQLSPECIALCOLRESULTSET,fLength),
    sizeof(SDWORD),
    },

    {
    7,
    SQL_C_SHORT,
    offsetof(sSQLSPECIALCOLRESULTSET,Scale),
    sizeof(SWORD),
    },

};

// new in v2.0

static sCOLBIND _SQLColumnPrivilegesRSB[8] =
{
{   1,
    SQL_C_CHAR,
    offsetof(sCOLUMNPRIVILEGESRESULTSET, szTable_qualifier),
    TABLE_QUALIFIER_SIZE,
    0,
    false,
    "TABLE_QUALIFIER",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   2,
    SQL_C_CHAR,
    offsetof(sCOLUMNPRIVILEGESRESULTSET, szTable_owner),
    TABLE_OWNER_SIZE,
    0,
    false,
    "TABLE_OWNER",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   3,
    SQL_C_CHAR,
    offsetof(sCOLUMNPRIVILEGESRESULTSET, szTable_name),
    TABLE_NAME_SIZE,
    0,
    false,
    "TABLE_NAME",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   4,
    SQL_C_CHAR,
    offsetof(sCOLUMNPRIVILEGESRESULTSET, szColumn_name),
    COLUMN_NAME_SIZE,
    0,
    false,
    "column_name",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   5,
    SQL_C_CHAR,
    offsetof(sCOLUMNPRIVILEGESRESULTSET, szGrantor),
    GRANTOR_SIZE,
    0,
    false,
    "GRANTOR",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   6,
    SQL_C_CHAR,
    offsetof(sCOLUMNPRIVILEGESRESULTSET, szGrantee),
    GRANTEE_SIZE,
    0,
    false,
    "GRANTEE",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   7,
    SQL_C_CHAR,
    offsetof(sCOLUMNPRIVILEGESRESULTSET, szPrivilege),
    PRIVILEGE_SIZE,
    0,
    false,
    "PRIVILEGE",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   8,
    SQL_C_CHAR,
    offsetof(sCOLUMNPRIVILEGESRESULTSET, szIs_grantable),
    IS_GRANTABLE_SIZE,
    0,
    false,
    "IS_GRANTABLE",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
};
const int _SQLColumnPrivilegesRSBCount
= sizeof(_SQLColumnPrivilegesRSB) / sizeof(_SQLColumnPrivilegesRSB[0]);

static sCOLBIND _SQLProceduresRSB[7] =
{
{   1,
    SQL_C_CHAR,
    offsetof(sPROCEDURESRESULTSET, szProcedure_qualifier),
    PROCEDURE_QUALIFIER_SIZE,
    0,
    false,
    "PROCEDURE_QUALIFIER",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   2,
    SQL_C_CHAR,
    offsetof(sPROCEDURESRESULTSET, szProcedure_owner),
    PROCEDURE_OWNER_SIZE,
    0,
    false,
    "PROCEDURE_OWNER",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   3,
    SQL_C_CHAR,
    offsetof(sPROCEDURESRESULTSET, szProcedure_name),
    PROCEDURE_NAME_SIZE,
    0,
    false,
    "PROCEDURE_NAME",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   4,
    SQL_C_LONG,
    offsetof(sPROCEDURESRESULTSET, lNum_input_params),
    sizeof(SDWORD),
    0,
    false,
    "NUM_INPUT_PARAMS",
    SQL_INTEGER,
    0,
    0,
    nullptr,

},
{   5,
    SQL_C_LONG,
    offsetof(sPROCEDURESRESULTSET, lNum_output_params),
    sizeof(SDWORD),
    0,
    false,
    "NUM_OUTPUT_PARAMS",
    SQL_INTEGER,
    0,
    0,
    nullptr,

},
{   6,
    SQL_C_LONG,
    offsetof(sPROCEDURESRESULTSET, lNum_result_sets),
    sizeof(SDWORD),
    0,
    false,
    "NUM_RESULT_SETS",
    SQL_INTEGER,
    0,
    0,
    nullptr,

},
{   7,
    SQL_C_CHAR,
    offsetof(sPROCEDURESRESULTSET, szRemarks),
    REMARKS_SIZE,
    0,
    false,
    "REMARKS",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
};
const int _SQLProceduresRSBCount
= sizeof(_SQLProceduresRSB) / sizeof(_SQLProceduresRSB[0]);

static sCOLBIND _SQLProcedureColumnsRSB[13] =
{
{   1,
    SQL_C_CHAR,
    offsetof(sPROCEDURECOLUMNSRESULTSET, szProcedure_qualifier),
    PROCEDURE_QUALIFIER_SIZE,
    0,
    false,
    "PROCEDURE_QUALIFIER",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   2,
    SQL_C_CHAR,
    offsetof(sPROCEDURECOLUMNSRESULTSET, szProcedure_owner),
    PROCEDURE_OWNER_SIZE,
    0,
    false,
    "PROCEDURE_OWNER",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   3,
    SQL_C_CHAR,
    offsetof(sPROCEDURECOLUMNSRESULTSET, szProcedure_name),
    PROCEDURE_NAME_SIZE,
    0,
    false,
    "PROCEDURE_NAME",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   4,
    SQL_C_CHAR,
    offsetof(sPROCEDURECOLUMNSRESULTSET, szColumn_name),
    COLUMN_NAME_SIZE,
    0,
    false,
    "COLUMN_NAME",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   5,
    SQL_C_SHORT,
    offsetof(sPROCEDURECOLUMNSRESULTSET, nColumn_type),
    sizeof(SWORD),
    0,
    false,
    "COLUMN_TYPE",
    SQL_SMALLINT,
    0,
    0,
    nullptr,

},
{   6,
    SQL_C_CHAR,
    offsetof(sPROCEDURECOLUMNSRESULTSET, szData_type),
    DATA_TYPE_SIZE,
    0,
    false,
    "DATA_TYPE",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   7,
    SQL_C_CHAR,
    offsetof(sPROCEDURECOLUMNSRESULTSET, szType_name),
    TYPE_NAME_SIZE,
    0,
    false,
    "TYPE_NAME",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   8,
    SQL_C_LONG,
    offsetof(sPROCEDURECOLUMNSRESULTSET, lPrecision),
    sizeof(SDWORD),
    0,
    false,
    "PRECISION",
    SQL_INTEGER,
    0,
    1,
    nullptr,

},
{   9,
    SQL_C_LONG,
    offsetof(sPROCEDURECOLUMNSRESULTSET, lLength),
    sizeof(SDWORD),
    0,
    false,
    "LENGTH",
    SQL_INTEGER,
    0,
    1,
    nullptr,

},
{   10,
    SQL_C_SHORT,
    offsetof(sPROCEDURECOLUMNSRESULTSET, nScale),
    sizeof(SWORD),
    0,
    false,
    "SCALE",
    SQL_SMALLINT,
    0,
    1,
    nullptr,

},
{   11,
    SQL_C_SHORT,
    offsetof(sPROCEDURECOLUMNSRESULTSET, nRadix),
    sizeof(SDWORD),
    0,
    false,
    "RADIX",
    SQL_SMALLINT,
    0,
    0,
    nullptr,

},
{   12,
    SQL_C_SHORT,
    offsetof(sPROCEDURECOLUMNSRESULTSET, nNullable),
    sizeof(SWORD),
    0,
    false,
    "NULLABLE",
    SQL_SMALLINT,
    0,
    1,
    nullptr,

},
{   13,
    SQL_C_CHAR,
    offsetof(sPROCEDURECOLUMNSRESULTSET, szRemarks),
    REMARKS_SIZE,
    0,
    false,
    "REMARKS",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
};
const int _SQLProcedureColumnsRSBCount
= sizeof(_SQLProcedureColumnsRSB) / sizeof(_SQLProcedureColumnsRSB[0]);

static sCOLBIND _SQLForeignKeysRSB[12] =
{
{   1,
    SQL_C_CHAR,
    offsetof(sFOREIGNKEYSRESULTSET, szPktable_qualifier),
    PKTABLE_QUALIFIER_SIZE,
    0,
    false,
    "PKTABLE_QUALIFIER",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   2,
    SQL_C_CHAR,
    offsetof(sFOREIGNKEYSRESULTSET, szPktable_owner),
    PKTABLE_OWNER_SIZE,
    0,
    false,
    "PKTABLE_OWNER",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   3,
    SQL_C_CHAR,
    offsetof(sFOREIGNKEYSRESULTSET, szPktable_name),
    PKTABLE_NAME_SIZE,
    0,
    false,
    "PKTABLE_NAME",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   4,
    SQL_C_CHAR,
    offsetof(sFOREIGNKEYSRESULTSET, szPkcolumn_name),
    PKCOLUMN_NAME_SIZE,
    0,
    false,
    "PKCOLUMN_NAME",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   5,
    SQL_C_CHAR,
    offsetof(sFOREIGNKEYSRESULTSET, szFktable_qualifier),
    FKTABLE_QUALIFIER_SIZE,
    0,
    false,
    "FKTABLE_QUALIFIER",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   6,
    SQL_C_CHAR,
    offsetof(sFOREIGNKEYSRESULTSET, szFktable_owner),
    FKTABLE_OWNER_SIZE,
    0,
    false,
    "FKTABLE_OWNER",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   7,
    SQL_C_CHAR,
    offsetof(sFOREIGNKEYSRESULTSET, szFktable_name),
    FKTABLE_NAME_SIZE,
    0,
    false,
    "FKTABLE_NAME",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   8,
    SQL_C_CHAR,
    offsetof(sFOREIGNKEYSRESULTSET, szFkcolumn_name),
    FKCOLUMN_NAME_SIZE,
    0,
    false,
    "FKCOLUMN_NAME",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   9,
    SQL_C_SHORT,
    offsetof(sFOREIGNKEYSRESULTSET, nKey_seq),
    sizeof(SWORD),
    0,
    false,
    "KEY_SEQ",
    SQL_SMALLINT,
    0,
    1,
    nullptr,

},
{   10,
    SQL_C_SHORT,
    offsetof(sFOREIGNKEYSRESULTSET, nUpdate_rule),
    sizeof(SWORD),
    0,
    false,
    "Update_Rule",
    SQL_SMALLINT,
    0,
    0,
    nullptr,

},
{   11,
    SQL_C_SHORT,
    offsetof(sFOREIGNKEYSRESULTSET, nDelete_rule),
    sizeof(SWORD),
    0,
    false,
    "Delete_Rule",
    SQL_SMALLINT,
    0,
    0,
    nullptr,

},
{   12,
    SQL_C_CHAR,
    offsetof(sFOREIGNKEYSRESULTSET, szRole_name),
    ROLE_NAME_SIZE,
    0,
    false,
    "ROLE_NAME",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
};
const int _SQLForeignKeysRSBCount
= sizeof(_SQLForeignKeysRSB) / sizeof(_SQLForeignKeysRSB[0]);

static sCOLBIND _SQLTablePrivilegesRSB[7] =
{
{   1,
    SQL_C_CHAR,
    offsetof(sTABLEPRIVILEGESRESULTSET, szTable_qualifier),
    TABLE_QUALIFIER_SIZE,
    0,
    false,
    "TABLE_QUALIFIER",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   2,
    SQL_C_CHAR,
    offsetof(sTABLEPRIVILEGESRESULTSET, szTable_owner),
    TABLE_OWNER_SIZE,
    0,
    false,
    "TABLE_OWNER",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   3,
    SQL_C_CHAR,
    offsetof(sTABLEPRIVILEGESRESULTSET, szTable_name),
    TABLE_NAME_SIZE,
    0,
    false,
    "TABLE_NAME",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   4,
    SQL_C_CHAR,
    offsetof(sTABLEPRIVILEGESRESULTSET, szGrantor),
    GRANTOR_SIZE,
    0,
    false,
    "GRANTOR",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   5,
    SQL_C_CHAR,
    offsetof(sTABLEPRIVILEGESRESULTSET, szGrantee),
    GRANTEE_SIZE,
    0,
    false,
    "GRANTEE",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   6,
    SQL_C_CHAR,
    offsetof(sTABLEPRIVILEGESRESULTSET, szPrivilege),
    PRIVILEGE_SIZE,
    0,
    false,
    "PRIVILEGE",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   7,
    SQL_C_CHAR,
    offsetof(sTABLEPRIVILEGESRESULTSET, szIs_grantable),
    IS_GRANTABLE_SIZE,
    0,
    false,
    "IS_GRANTABLE",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
};
const int _SQLTablePrivilegesRSBCount
= sizeof(_SQLTablePrivilegesRSB) / sizeof(_SQLTablePrivilegesRSB[0]);

static sCOLBIND _SQLPrimaryKeysRSB[5] =
{
{   1,
    SQL_C_CHAR,
    offsetof(sPRIMARYKEYSRESULTSET, szTable_qualifier),
    TABLE_QUALIFIER_SIZE,
    0,
    false,
    "TABLE_QUALIFIER",
    SQL_VARCHAR,
    0,
    1,
    nullptr,

},
{   2,
    SQL_C_CHAR,
    offsetof(sPRIMARYKEYSRESULTSET, szTable_owner),
    TABLE_OWNER_SIZE,
    0,
    false,
    "TABLE_OWNER",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   3,
    SQL_C_CHAR,
    offsetof(sPRIMARYKEYSRESULTSET, szTable_name),
    TABLE_NAME_SIZE,
    0,
    false,
    "TABLE_NAME",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   4,
    SQL_C_CHAR,
    offsetof(sPRIMARYKEYSRESULTSET, szColumn_name),
    COLUMN_NAME_SIZE,
    0,
    false,
    "column_name",
    SQL_VARCHAR,
    0,
    0,
    nullptr,

},
{   5,
    SQL_C_SHORT,
    offsetof(sPRIMARYKEYSRESULTSET, nKey_seq),
    sizeof(SWORD),
    0,
    false,
    "KEY_SEQ",
    SQL_SMALLINT,
    0,
    1,
    nullptr,

},
};
const int _SQLPrimaryKeysRSBCount
= sizeof(_SQLPrimaryKeysRSB) / sizeof(_SQLPrimaryKeysRSB[0]);

// end new in v2.0

static std::mutex g_AllocCritSec;

///////////////////////////////////////////////////////////
//////////////////////// connections
///////////////////////////////////////////////////////////

/**********************************************************
    odbcCONNECT

    constructor.  Environment handle is required; other
    arguments are optional.
**********************************************************/

odbcCONNECT::odbcCONNECT(
    odbcENV*    penv,
    LPUSTR      szDSN,
    LPUSTR      szUID,
    LPUSTR      szAuthStr,
    UDWORD      udTimeout
)
{
    pEnv = penv;
    //    pStmtList = 0;
    isConnected = 0;
    hdbc = nullptr;
    bTrimAllTrailingBlanks = false;
    AutoRetrieve(odbcREPERRS);

    {
        std::lock_guard<std::mutex> guard(g_AllocCritSec);
        RETCODE rc = SQLAllocConnect(
            pEnv->GetHenv(),
            &hdbc
        );
        SetRC(rc);
    }


    if (sqlsuccess() && udTimeout > 0)
    {
#if (ODBCVER >= 0x0300)
        SQLSetConnectAttr(
            hdbc,
            SQL_LOGIN_TIMEOUT,
            (SQLPOINTER)udTimeout,
            0);
#else
        SQLSetConnectOption(
            hdbc,
            SQL_LOGIN_TIMEOUT,
            udTimeout);
#endif
    }

    if (sqlsuccess())
    {
#if (ODBCVER >= 0x0200)

        SetRC(
            SetConnectOption(SQL_ODBC_CURSORS, pEnv->nCursorLibUsage)
        );

#endif  // ODBCVER >= 0x0200
        if (szDSN && szUID && szAuthStr && sqlsuccess())
            Connect(
                szDSN,
                szUID,
                szAuthStr
            );
    }
    // inherit error handling from environment
    ErrHandler = pEnv->ErrHandler;
    bGetErrorInfo = pEnv->bGetErrorInfo;
    bReportErrorInfo = pEnv->bReportErrorInfo;
    hwnd_ = pEnv->hwnd_;
    flags = pEnv->flags;

    pEnv->RegisterConnection(this);
}

odbcCONNECT::odbcCONNECT(
    odbcENV*    penv,
    LPSTR       szDSN,
    LPSTR       szUID,
    LPSTR       szAuthStr,
    UDWORD      udTimeout
)
{
    pEnv = penv;
    //    pStmtList = 0;
    isConnected = 0;
    hdbc = nullptr;
    bTrimAllTrailingBlanks = false;
    AutoRetrieve(odbcREPERRS);

    {
        std::lock_guard<std::mutex> guard(g_AllocCritSec);
        RETCODE rc = SQLAllocConnect(
            pEnv->GetHenv(),
            &hdbc
        );

        SetRC(rc);
    }

    if (sqlsuccess() && udTimeout > 0)
    {
#if (ODBCVER >= 0x0300)
        SQLSetConnectAttr(
            hdbc,
            SQL_LOGIN_TIMEOUT,
            (SQLPOINTER)udTimeout,
            0);
#else
        SQLSetConnectOption(
            hdbc,
            SQL_LOGIN_TIMEOUT,
            udTimeout);
#endif
    }

    if (sqlsuccess())
    {
#if (ODBCVER >= 0x0200)

        SetRC(
            SetConnectOption(SQL_ODBC_CURSORS, pEnv->nCursorLibUsage)
        );

#endif  // ODBCVER >= 0x0200

        if (szDSN && szUID && szAuthStr && sqlsuccess())
            Connect(
                szDSN,
                szUID,
                szAuthStr
            );
    }

    // inherit error handling from environment
    ErrHandler = pEnv->ErrHandler;
    bGetErrorInfo = pEnv->bGetErrorInfo;
    bReportErrorInfo = pEnv->bReportErrorInfo;
    hwnd_ = pEnv->hwnd_;
    flags = pEnv->flags;

    pEnv->RegisterConnection(this);
}

/**********************************************************
    ~odbcCONNECT

    Destructor.
**********************************************************/

odbcCONNECT::~odbcCONNECT()
{
    // close cursors first
/*    if (pStmtList)
        {
        delete pStmtList;
        pStmtList = NULL;
        }
*/
// then disconnect
    if (isConnected)
        Disconnect();

    // then free the connection handle
    if (hdbc)
    {
        std::lock_guard<std::mutex> guard(g_AllocCritSec);

        SQLFreeConnect(hdbc);

        hdbc = nullptr;
    }

    //    if ( pEnv && !IsBadReadPtr( pEnv, sizeof(*pEnv)))    
    //       pEnv->UnregisterConnection(this);
}

/**********************************************************
    Connect

    Call to SQLConnect, passing data set name, user ID,
    and password.
**********************************************************/

static std::mutex g_ConnectCritSec;

RETCODE odbcCONNECT::Connect(
    LPUSTR szDSN,
    LPUSTR szUID,
    LPUSTR szAuthStr
)
{
    if (!hdbc)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    // if already connected, disconnect
    if (isConnected)
        Disconnect();

    std::lock_guard<std::mutex> guard(g_ConnectCritSec);

    RETCODE rc = SQLConnect(
        hdbc,
        szDSN,
        SQL_NTS,
        szUID,
        SQL_NTS,
        szAuthStr,
        SQL_NTS
    );

    SetRC(rc);

    if (sqlsuccess())
    {
        isConnected = true;
    }

    return lastRC();
}

/**********************************************************
    Disconnect

    Call to SQLDisconnect.
**********************************************************/

RETCODE odbcCONNECT::Disconnect()
{
    if (!hdbc)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }
    if (isConnected)
    {
        std::lock_guard<std::mutex> guard(g_ConnectCritSec);
        RETCODE rc = SQLDisconnect(hdbc);
        SetRC(rc);

        if (sqlsuccess())
            isConnected = false;
    }
    return lastRC();
}

/**********************************************************
    Commit

    Call to SQLTransact to commit a transaction.
**********************************************************/

RETCODE odbcCONNECT::Commit()
{
    if (!hdbc)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    SetRC(SQLTransact(
        SQL_NULL_HENV,
        hdbc,
        SQL_COMMIT
    ));

    return lastRC();
}

/**********************************************************
    RollBack

    Call to SQLTransact to roll back a transaction.
**********************************************************/

RETCODE odbcCONNECT::RollBack()
{
    if (!hdbc)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    SetRC(SQLTransact(
        SQL_NULL_HENV,
        hdbc,
        SQL_ROLLBACK
    ));

    return lastRC();
}

/**********************************************************
    RegisterError

    Get more information on the most recent error code
    from an ODBC operation. Results can be retrieved using
    member functions in the parent odbcBASE class.

    This function calls the base class member function Error()
    with arguments appropriate for this object type.
**********************************************************/

RETCODE odbcCONNECT::RegisterError()
{
    return Error(
        henv,
        (hdbc != nullptr) ? hdbc : SQL_NULL_HDBC,
        SQL_NULL_HSTMT
    );
}


/**********************************************************
    AllocStmt

    Allocate a statement object.

    lpszStmt:       SQL statement to use.
    bPrepare:       if non-zero, call SQLPrepare.
    bExecute:       Call SQLExecDirect (or SQLExecute
                    if bPrepare was non-zero).
    psParmBindings: address of array of parameter bindings.
    uParmCount:     count of array elements.
    pvParmStruct:   Address of structure containing
                    parameter values.
**********************************************************/

odbcSTMT*      odbcCONNECT::AllocStmt
(
    LPUSTR      lpszSentStmt,
    sPARMBIND*  psParmBindings,
    UWORD       uParmCount,
    void*         pvParmStruct
)
{
    auto* pS = new odbcSTMT
    (
        this,
        lpszSentStmt,
        psParmBindings,
        uParmCount,
        pvParmStruct
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}


/**********************************************************
    RegisterStmt

    Register an odbcSTMT object.
**********************************************************/

void odbcCONNECT::RegisterStmt(odbcSTMT* /*pStmt*/)
{
    //	Has no use now
}


/**********************************************************
    UnregisterStmt

    Unregister an odbcSTMT object.
**********************************************************/

void odbcCONNECT::UnregisterStmt(odbcSTMT* /*pStmt*/)
{
    //	Has no use now
}


/**********************************************************
    AllocCursor

    Allocate a cursor object.

    lpszStmt:       SQL statement to use.
    bPrepare:       if non-zero, call SQLPrepare.
    bExecute:       Call SQLExecDirect (or SQLExecute
                    if bPrepare was non-zero).
    bAutoBind:      Automatically bind columns
                    to dynamically allocated struct?
    psParmBindings: Address of array of parameter bindings.
    uParmCount:     Count of array elements.
    pvParmStruct:   Address of structure containing
                    parameter values.
    pColBindings:   Address of array of column bindings.
    uColcount:      Count of array elements.
    pvColStruct:    Address of structure for column bindings.
**********************************************************/

odbcCURSOR*      odbcCONNECT::AllocCursor
(
    LPUSTR  lpszSentStmt,
    bool        bAutoBind,
    sPARMBIND*  psParmBindings,
    UWORD       uParmCount,
    void*         pvParmStruct,
    sCOLBIND*   psColBindings,
    UWORD       uColCount,
    void*         pvColStruct
)
{
    auto* pS = new odbcCURSOR
    (
        this,
        lpszSentStmt,
        bAutoBind,
        psParmBindings,
        uParmCount,
        pvParmStruct,
        psColBindings,
        uColCount,
        pvColStruct
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocStmt

    Deallocate a statement object.
**********************************************************/

void odbcCONNECT::DeAllocStmt(
    odbcSTMT* pS
)
{
    delete pS;
}


/**********************************************************
    AllocRecInserter

    Allocate a RecInserter object.

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

**********************************************************/

odbcRECINSERTER* odbcCONNECT::AllocRecInserter
(
    LPCSTR          lpszSentTblName,
    sCOLBIND*       pColBinds       /* = NULL */,
    UWORD           uNumColBindings /* = 0 */,
    void*             pRecord         /* = NULL */
)
{
    auto* pS = new odbcRECINSERTER
    (
        this,
        lpszSentTblName,
        pColBinds,
        uNumColBindings,
        pRecord
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocRecInserter

    Deallocate a statement object.
**********************************************************/

void odbcCONNECT::DeAllocRecInserter(
    odbcRECINSERTER* pS
)
{
    delete pS;
}

/**********************************************************
    AllocRecUpdater

    Allocate a RecUpdater object.

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

**********************************************************/

odbcRECUPDATER* odbcCONNECT::AllocRecUpdater
(
    LPCSTR          lpszSentTblName,
    LPCSTR          lpszSelectStmt,
    sCOLBIND*       pColBinds       /* = NULL*/,
    UWORD           uNumColBindings /* = 0*/,
    void*             pRecord         /* = NULL*/,
    bool            bExecDirect     /* = true*/,
    UWORD           fConcur         /* = SQL_CONCUR_VALUES*/,
    SDWORD          fKeyset         /* = SQL_CURSOR_STATIC*/,
    UWORD           fRowSet         /* = 1 */
)
{
    auto* pS = new odbcRECUPDATER
    (
        this,
        lpszSentTblName,
        lpszSelectStmt,
        pColBinds,
        uNumColBindings,
        pRecord,
        bExecDirect,
        fConcur,
        fKeyset,
        fRowSet
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocRecUpdater

    Deallocate a RecUpdater object.
**********************************************************/

void odbcCONNECT::DeAllocRecUpdater(
    odbcRECUPDATER* pS
)
{
    delete pS;
}

/**********************************************************
    AllocTableCreator

    Allocate a TableCreator object.
**********************************************************/

odbcTABLECREATOR* odbcCONNECT::AllocTableCreator()
{
    auto* pS = new odbcTABLECREATOR
    (
        this
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocTableCreator

    Deallocate a TableCreator object.
**********************************************************/

void odbcCONNECT::DeAllocTableCreator(
    odbcTABLECREATOR* pS
)
{
    delete pS;
}

/**********************************************************
    AllocBLOB

    Allocate a BLOB object.

    Pass address of owning cursor, number of the
    associated column, and maximum size of the parameter;
    optionally also pass the column's SQL data type and
    put- and get-chunk granularities.
**********************************************************/

odbcBLOB* odbcCONNECT::AllocBLOB(
    odbcCURSOR* pCurs,
    UWORD iSentCol,
    UDWORD cbSentMaxSize,
    SWORD  fSentSqlType,            /* = SQL_LONGVARBINARY */
    UWORD  iSentParam,              /* = 0 */
    SDWORD cbSentPutChunkSize,      /* = BLOB_CHUNK_PUT_SIZE */
    SDWORD cbSentGetChunkSize       /* = BLOB_CHUNK_GET_SIZE */
)
{
    auto* pS = new odbcBLOB
    (
        pCurs,
        iSentCol,
        cbSentMaxSize,
        fSentSqlType,
        iSentParam,
        cbSentPutChunkSize,
        cbSentGetChunkSize
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocBLOB

    Deallocate a BLOB object.

    pS      BLOB to deallocate.
**********************************************************/

void odbcCONNECT::DeAllocBLOB(
    odbcBLOB* pS
)
{
    delete pS;
}


/**********************************************************
    AllocColumnIterator

    Allocate a ColumnIterator object.
**********************************************************/

odbcColumnIterator* odbcCONNECT::AllocColumnIterator(
    LPSTR szTableQualifier,
    LPSTR szTableOwner,
    LPSTR szTableName,
    LPSTR szColumnName
)
{
    auto* pS = new odbcColumnIterator
    (
        this,
        szTableQualifier,
        szTableOwner,
        szTableName,
        szColumnName
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocColumnIterator

    Deallocate a ColumnIterator object.
**********************************************************/

void odbcCONNECT::DeAllocColumnIterator(
    odbcColumnIterator* pS
)
{
    delete pS;
}

/**********************************************************
    AllocColumnPrivilegesIterator

    Allocate a ColumnPrivilegesIterator object.
**********************************************************/

odbcColumnPrivilegesIterator* odbcCONNECT::AllocColumnPrivilegesIterator(
    LPSTR szTableQualifier,
    LPSTR szTableOwner,
    LPSTR szTableName,
    LPSTR szColumnName
)
{
    auto* pS = new odbcColumnPrivilegesIterator
    (
        this,
        szTableQualifier,
        szTableOwner,
        szTableName,
        szColumnName
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocColumnPrivilegesIterator

    Deallocate a ColumnPrivilegesIterator object.
**********************************************************/

void odbcCONNECT::DeAllocColumnPrivilegesIterator(
    odbcColumnPrivilegesIterator* pS
)
{
    delete pS;
}

/**********************************************************
    AllocForeignKeysIterator

    Allocate a ForeignKeysIterator object.
**********************************************************/

odbcForeignKeysIterator* odbcCONNECT::AllocForeignKeysIterator(
    LPSTR szPkTableQualifier,
    LPSTR szPkTableOwner,
    LPSTR szPkTableName,
    LPSTR szFkTableQualifier,
    LPSTR szFkTableOwner,
    LPSTR szFkTableName
)
{
    auto* pS = new odbcForeignKeysIterator
    (
        this,
        szPkTableQualifier,
        szPkTableOwner,
        szPkTableName,
        szFkTableQualifier,
        szFkTableOwner,
        szFkTableName
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocForeignKeysIterator

    Deallocate a ForeignKeysIterator object.
**********************************************************/

void odbcCONNECT::DeAllocForeignKeysIterator(
    odbcForeignKeysIterator* pS
)
{
    delete pS;
}

/**********************************************************
    AllocPrimaryKeysIterator

    Allocate a PrimaryKeysIterator object.
**********************************************************/

odbcPrimaryKeysIterator* odbcCONNECT::AllocPrimaryKeysIterator(
    LPSTR szTableQualifier,
    LPSTR szTableOwner,
    LPSTR szTableName
)
{
    auto* pS = new odbcPrimaryKeysIterator
    (
        this,
        szTableQualifier,
        szTableOwner,
        szTableName
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocPrimaryKeysIterator

    Deallocate a PrimaryKeysIterator object.
**********************************************************/

void odbcCONNECT::DeAllocPrimaryKeysIterator(
    odbcPrimaryKeysIterator* pS
)
{
    delete pS;
}

/**********************************************************
    AllocProcedureColumnsIterator

    Allocate a ProcedureColumnsIterator object.
**********************************************************/

odbcProcedureColumnsIterator* odbcCONNECT::AllocProcedureColumnsIterator(
    LPSTR szProcQualifier,
    LPSTR szProcOwner,
    LPSTR szProcName,
    LPSTR szProcColumn
)
{
    auto* pS = new odbcProcedureColumnsIterator
    (
        this,
        szProcQualifier,
        szProcOwner,
        szProcName,
        szProcColumn
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocProcedureColumnsIterator

    Deallocate a ProcedureColumnsIterator object.
**********************************************************/

void odbcCONNECT::DeAllocProcedureColumnsIterator(
    odbcProcedureColumnsIterator* pS
)
{
    delete pS;
}

/**********************************************************
    AllocProceduresIterator

    Allocate a ProceduresIterator object.
**********************************************************/

odbcProceduresIterator* odbcCONNECT::AllocProceduresIterator(
    LPSTR szProcQualifier,
    LPSTR szProcOwner,
    LPSTR szProcName
)
{
    auto* pS = new odbcProceduresIterator
    (
        this,
        szProcQualifier,
        szProcOwner,
        szProcName
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocProceduresIterator

    Deallocate a ProceduresIterator object.
**********************************************************/

void odbcCONNECT::DeAllocProceduresIterator(
    odbcProceduresIterator* pS
)
{
    delete pS;
}

/**********************************************************
    AllocSpecialColumnIterator

    Allocate a SpecialColumnIterator object.
**********************************************************/

odbcSpecialColumnIterator* odbcCONNECT::AllocSpecialColumnIterator(
    UWORD                   fColType,
    LPSTR                   szTableQualifier,
    LPSTR                   szTableOwner,
    LPSTR                   szTableName,
    UWORD                   fScope,
    UWORD                   fNullable
)
{
    auto* pS = new odbcSpecialColumnIterator
    (
        this,
        fColType,
        szTableQualifier,
        szTableOwner,
        szTableName,
        fScope,
        fNullable
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocSpecialColumnIterator

    Deallocate a SpecialColumnIterator object.
**********************************************************/

void odbcCONNECT::DeAllocSpecialColumnIterator(
    odbcSpecialColumnIterator* pS
)
{
    delete pS;
}

/**********************************************************
    AllocStatisticsIterator

    Allocate a StatisticsIterator object.
**********************************************************/

odbcStatisticsIterator* odbcCONNECT::AllocStatisticsIterator(
    LPSTR                   szTableQualifier,
    LPSTR                   szTableOwner,
    LPSTR                   szTableName,
    UWORD                   fTblUnique,
    UWORD                   fTblAccuracy
)
{
    auto* pS = new odbcStatisticsIterator
    (
        this,
        szTableQualifier,
        szTableOwner,
        szTableName,
        fTblUnique,
        fTblAccuracy
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocStatisticsIterator

    Deallocate a StatisticsIterator object.
**********************************************************/

void odbcCONNECT::DeAllocStatisticsIterator(
    odbcStatisticsIterator* pS
)
{
    delete pS;
}

/**********************************************************
    AllocTableIterator

    Allocate a TableIterator object.
**********************************************************/

odbcTableIterator* odbcCONNECT::AllocTableIterator(
    LPSTR                  szTableQualifier,
    LPSTR                  szTableOwner,
    LPSTR                  szTableName,
    LPSTR                  szTableType
)
{
    auto* pS = new odbcTableIterator
    (
        this,
        szTableQualifier,
        szTableOwner,
        szTableName,
        szTableType
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocTableIterator

    Deallocate a TableIterator object.
**********************************************************/

void odbcCONNECT::DeAllocTableIterator(
    odbcTableIterator* pS
)
{
    delete pS;
}

/**********************************************************
    AllocTablePrivilegesIterator

    Allocate a TablePrivilegesIterator object.
**********************************************************/

odbcTablePrivilegesIterator* odbcCONNECT::AllocTablePrivilegesIterator(
    LPSTR szTableQualifier,
    LPSTR szTableOwner,
    LPSTR szTableName
)
{
    auto* pS = new odbcTablePrivilegesIterator
    (
        this,
        szTableQualifier,
        szTableOwner,
        szTableName
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocTablePrivilegesIterator

    Deallocate a TablePrivilegesIterator object.
**********************************************************/

void odbcCONNECT::DeAllocTablePrivilegesIterator(
    odbcTablePrivilegesIterator* pS
)
{
    delete pS;
}

/**********************************************************
    AllocTypeInfoIterator

    Allocate a TypeInfoIterator object.
**********************************************************/

odbcTypeInfoIterator* odbcCONNECT::AllocTypeInfoIterator()
{
    auto* pS = new odbcTypeInfoIterator
    (
        this
    );

    if (!pS)
    {
        SetRC(SQL_ALLOC_FAILED);
        return nullptr;
    }

    return pS;
}

/**********************************************************
    DeAllocTypeInfoIterator

    Deallocate a TypeInfoIterator object.
**********************************************************/

void odbcCONNECT::DeAllocTypeInfoIterator(
    odbcTypeInfoIterator* pS
)
{
    delete pS;
}

/**********************************************************
    GetConnectOption

    Call SQLGetConnectOption.

    The various Get... member functions call the same
    function with the appropriate flag.
**********************************************************/

UDWORD odbcCONNECT::GetConnectOption(UWORD      fOption)
{
#if (ODBCVER >= 0x0300)
    SQLULEN Result = 0;
    SetRC(
        SQLGetConnectAttr(
            hdbc,
            fOption,
            &Result,
            SQL_IS_UINTEGER,
            nullptr
        ));
    return Result;
#else
    UDWORD Result = 0;
    SetRC(
        SQLGetConnectOption(
            hdbc,
            fOption,
            &Result
        ));
    return Result;
#endif
}


/**********************************************************
    SetConnectOption

    Call SQLSetConnectOption.

    The various Set... member functions call the same
    function with the appropriate flag.
**********************************************************/

RETCODE odbcCONNECT::SetConnectOption(
    UWORD       fOption,
    UDWORD      ulValue
)
{
#if (ODBCVER >= 0x0300)
    SetRC(
        SQLSetConnectAttr(
            hdbc,
            fOption,
            (SQLPOINTER)ulValue,
            0
        ));
#else
    SetRC(
        SQLSetConnectOption(
            hdbc,
            fOption,
            ulValue
        ));
#endif                
    return lastRC();
}


/**********************************************************
    DriverConnect

    Call SQLDriverConnect; pass-through function.

**********************************************************/

RETCODE odbcCONNECT::DriverConnect(
    HWND        hwnd,
    LPUCSTR     szConnStrIn,
    LPUSTR      szConnStrOut,
    SWORD       cbConnStrOutMax,
    SWORD      *pcbConnStrOut,
    UWORD       fDriverCompletion
)
{
    // if we didn't construct, don't try to connect
    if (!hdbc)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    // if already connected, disconnect
    if (isConnected)
        Disconnect();

    if (hwnd && !IsWindow(hwnd))
    {
        return SQL_ERROR;
    }

    SetRC(
        SQLDriverConnect(
            hdbc,
            hwnd,
            (LPUSTR)szConnStrIn,
            SQL_NTS,
            szConnStrOut,
            cbConnStrOutMax,
            pcbConnStrOut,
            fDriverCompletion
        ));

    if (sqlsuccess())
        isConnected = true;

    return lastRC();
}

/**********************************************************
    DriverConnectNoPrompt

    Call SQLDriverConnect, using contents of szConnStrIn;
    do not prompt with any dialog, but return an error if
    the connection information in the string is not enough.
    Use strlen() to obtain the length of szConnStrOut,
    but only if the function succeeded.
**********************************************************/

RETCODE odbcCONNECT::DriverConnectNoPrompt(
    LPUCSTR     szConnStrIn,
    LPUSTR      szConnStrOut,
    SWORD       cbConnStrOutMax
)
{
    SWORD  cbConnStrOut;

    DriverConnect(
        nullptr,
        szConnStrIn,
        szConnStrOut,
        cbConnStrOutMax,
        &cbConnStrOut,
        SQL_DRIVER_NOPROMPT
    );

    return lastRC();
}

/**********************************************************
    DriverConnectComplete

    Call SQLDriverConnect; prompt to complete the connection,
    if insufficient information is provided in szConnStrIn.
    Use strlen() to obtain the length of szConnStrOut,
    but only if the function succeeded.
**********************************************************/

RETCODE odbcCONNECT::DriverConnectComplete(
    HWND        hwnd,
    LPUCSTR     szConnStrIn,
    LPUSTR      szConnStrOut,
    SWORD       cbConnStrOutMax
)
{
    SWORD  cbConnStrOut;

    DriverConnect(
        hwnd,
        szConnStrIn,
        szConnStrOut,
        cbConnStrOutMax,
        &cbConnStrOut,
        SQL_DRIVER_COMPLETE
    );

    return lastRC();
}

/**********************************************************
    DriverConnectPrompt

    Call SQLDriverConnect; tells driver to prompt for
    connection information.
    Use strlen() to obtain the length of szConnStrOut,
    but only if the function succeeded.
**********************************************************/

RETCODE odbcCONNECT::DriverConnectPrompt(
    HWND        hwnd,
    LPUSTR      szConnStrOut,
    SWORD       cbConnStrOutMax
)
{
    SWORD   cbConnStrOut;
    UCHAR   szConnStrIn[255];

    szConnStrIn[0] = 0;

    DriverConnect(
        hwnd,
        szConnStrIn,
        szConnStrOut,
        cbConnStrOutMax,
        &cbConnStrOut,
        SQL_DRIVER_PROMPT
    );

    return lastRC();
}

/**********************************************************
    DriverConnectCompleteRequired

    Calls SQLDriverConnect; tells driver to complete any
    required items not in the szConnStrIn argument.
    Use strlen() to obtain the length of szConnStrOut,
    but only if the function succeeded.
**********************************************************/

RETCODE odbcCONNECT::DriverConnectCompleteRequired(
    HWND        hwnd,
    LPUCSTR     szConnStrIn,
    LPUSTR      szConnStrOut,
    SWORD       cbConnStrOutMax
)
{
    SWORD  cbConnStrOut;

    DriverConnect(
        hwnd,
        szConnStrIn,
        szConnStrOut,
        cbConnStrOutMax,
        &cbConnStrOut,
        SQL_DRIVER_COMPLETE_REQUIRED
    );

    return lastRC();
}

/**********************************************************
    GetFunctions

    Call SQLGetFunctions.

**********************************************************/

UWORD odbcCONNECT::GetFunctions(UWORD fFunction)
{
    UWORD uExists = 0;

    SetRC(
        SQLGetFunctions(
            hdbc,
            fFunction,
            &uExists
        ));

    return uExists;
}

/**********************************************************
    GetInfo

    Call SQLGetInfo.

**********************************************************/

RETCODE odbcCONNECT::GetInfo(
    UWORD      fInfoType,
    void*        rgbInfoValue,
    SWORD      cbInfoValueMax,
    SWORD      *pcbInfoValue)
{
    SetRC(
        SQLGetInfo(
            hdbc,
            fInfoType,
            rgbInfoValue,
            cbInfoValueMax,
            pcbInfoValue
        ));

    return lastRC();
}

/**********************************************************
    SQLColumnsRSBCount

    Count of structures in array of column bindings for the
    result set of SQLColumns.
**********************************************************/

UWORD odbcCONNECT::SQLColumnsRSBCount()
{
    return sizeof(_SQLColumnsRSB) / sizeof(_SQLColumnsRSB[0]);
}

/**********************************************************
    SQLColumnsRSB

    Address of array of structures describing column bindings
    for the result set of SQLColumns.
**********************************************************/

sCOLBIND* odbcCONNECT::SQLColumnsRSB()
{
    return _SQLColumnsRSB;
}

/**********************************************************
    SQLSpecialColumnsRSBCount

    Count of structures in array of column bindings for the
    result set of SQLSpecialColumns.
**********************************************************/

UWORD odbcCONNECT::SQLSpecialColumnsRSBCount()
{
    return sizeof(_SQLSpecialColumnsRSB) / sizeof(_SQLSpecialColumnsRSB[0]);
}

/**********************************************************
    SQLSpecialColumnsRSB

    Address of array of structures describing column bindings
    for the result set of SQLSpecialColumns.
**********************************************************/

sCOLBIND* odbcCONNECT::SQLSpecialColumnsRSB()
{
    return _SQLSpecialColumnsRSB;
}

/**********************************************************
    SQLTypeInfoRSBCount

    Count of structures in array of column bindings for the
    result set of SQLTypeInfo.
**********************************************************/

UWORD odbcCONNECT::SQLTypeInfoRSBCount()
{
    return sizeof(_SQLTypeInfoRSB) / sizeof(_SQLTypeInfoRSB[0]);
}

/**********************************************************
    SQLTypeInfoRSB

    Address of array of structures describing column bindings
    for the result set of SQLTypeInfo.
**********************************************************/

sCOLBIND* odbcCONNECT::SQLTypeInfoRSB()
{
    return _SQLTypeInfoRSB;
}

/**********************************************************
    SQLProcedureColumnsRSBCount

    Count of structures in array of column bindings for the
    result set of SQLProcedureColumns.
**********************************************************/

UWORD odbcCONNECT::SQLProcedureColumnsRSBCount()
{
    return sizeof(_SQLProcedureColumnsRSB) / sizeof(_SQLProcedureColumnsRSB[0]);
}

/**********************************************************
    SQLProcedureColumnsRSB

    Address of array of structures describing column bindings
    for the result set of SQLProcedureColumns.
**********************************************************/

sCOLBIND* odbcCONNECT::SQLProcedureColumnsRSB()
{
    return _SQLProcedureColumnsRSB;
}

/**********************************************************
    SQLTablesRSBCount

    Count of structures in array of column bindings for the
    result set of SQLTables.
**********************************************************/

UWORD odbcCONNECT::SQLTablesRSBCount()
{
    return sizeof(_SQLTablesRSB) / sizeof(_SQLTablesRSB[0]);
}

/**********************************************************
    SQLTablesRSB

    Address of array of structures describing column bindings
    for the result set of SQLTables.
**********************************************************/

sCOLBIND* odbcCONNECT::SQLTablesRSB()
{
    return _SQLTablesRSB;
}

/**********************************************************
    SQLStatisticsRSBCount

    Count of structures in array of column bindings for the
    result set of SQLStatistics.
**********************************************************/

UWORD odbcCONNECT::SQLStatisticsRSBCount()
{
    return sizeof(_SQLStatisticsRSB) / sizeof(_SQLStatisticsRSB[0]);
}

/**********************************************************
    SQLStatisticsRSB

    Address of array of structures describing column bindings
    for the result set of SQLStatistics.
**********************************************************/

sCOLBIND* odbcCONNECT::SQLStatisticsRSB()
{
    return _SQLStatisticsRSB;
}

/**********************************************************
    SQLColumnPrivilegesRSBCount

    Count of structures in array of column bindings for the
    result set of SQLColumnPrivileges.
**********************************************************/

UWORD odbcCONNECT::SQLColumnPrivilegesRSBCount()
{
    return sizeof(_SQLColumnPrivilegesRSB) / sizeof(_SQLColumnPrivilegesRSB[0]);
}

/**********************************************************
    SQLColumnPrivilegesRSB

    Address of array of structures describing column bindings
    for the result set of SQLColumnPrivileges.
**********************************************************/

sCOLBIND* odbcCONNECT::SQLColumnPrivilegesRSB()
{
    return _SQLColumnPrivilegesRSB;
}

/**********************************************************
    SQLProceduresRSBCount

    Count of structures in array of column bindings for the
    result set of SQLProcedures.
**********************************************************/

UWORD odbcCONNECT::SQLProceduresRSBCount()
{
    return sizeof(_SQLProceduresRSB) / sizeof(_SQLProceduresRSB[0]);
}

/**********************************************************
    SQLProceduresRSB

    Address of array of structures describing column bindings
    for the result set of SQLProcedures.
**********************************************************/

sCOLBIND* odbcCONNECT::SQLProceduresRSB()
{
    return _SQLProceduresRSB;
}

/**********************************************************
    SQLForeignKeysRSBCount

    Count of structures in array of column bindings for the
    result set of SQLForeignKeys.
**********************************************************/

UWORD odbcCONNECT::SQLForeignKeysRSBCount()
{
    return sizeof(_SQLForeignKeysRSB) / sizeof(_SQLForeignKeysRSB[0]);
}

/**********************************************************
    SQLForeignKeysRSB

    Address of array of structures describing column bindings
    for the result set of SQLForeignKeys.
**********************************************************/

sCOLBIND* odbcCONNECT::SQLForeignKeysRSB()
{
    return _SQLForeignKeysRSB;
}

/**********************************************************
    SQLTablePrivilegesRSBCount

    Count of structures in array of column bindings for the
    result set of SQLTablePrivileges.
**********************************************************/

UWORD odbcCONNECT::SQLTablePrivilegesRSBCount()
{
    return sizeof(_SQLTablePrivilegesRSB) / sizeof(_SQLTablePrivilegesRSB[0]);
}

/**********************************************************
    SQLTablePrivilegesRSB

    Address of array of structures describing column bindings
    for the result set of SQLTablePrivileges.
**********************************************************/

sCOLBIND* odbcCONNECT::SQLTablePrivilegesRSB()
{
    return _SQLTablePrivilegesRSB;
}

/**********************************************************
    SQLPrimaryKeysRSBCount

    Count of structures in array of column bindings for the
    result set of SQLPrimaryKeys.
**********************************************************/

UWORD odbcCONNECT::SQLPrimaryKeysRSBCount()
{
    return sizeof(_SQLPrimaryKeysRSB) / sizeof(_SQLPrimaryKeysRSB[0]);
}

/**********************************************************
    SQLPrimaryKeysRSB

    Address of array of structures describing column bindings
    for the result set of SQLPrimaryKeys.
**********************************************************/

sCOLBIND* odbcCONNECT::SQLPrimaryKeysRSB()
{
    return _SQLPrimaryKeysRSB;
}

/**********************************************************
    EnumColumns

    Calls SQLColumns and enumerates the result set, passing
    each column's data attributes as a structure to the
    callback function supplied by the caller.

**********************************************************/

RETCODE odbcCONNECT::EnumColumns(
    LPUSTR          szTableQualifier,
    LPUSTR          szTableOwner,
    LPUSTR          szTableName,
    LPUSTR          szColumnName,
    pfENUMCOLUMNS   pfEnum,
    void*             pUser
)
{
    // make a cursor
    odbcCURSOR* pCursor = AllocCursor();
    RETCODE ret;

    if (!pCursor)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    auto psResultSet = new sSQLCOLUMNSRESULTSET;

    if (!psResultSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        DeAllocCursor(pCursor);
        return lastRC();
    }

    // bind columns for result set
    pCursor->BindCol(
        SQLColumnsRSB(),
        SQLColumnsRSBCount(),
        psResultSet);

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // columns
    pCursor->Columns(
        szTableQualifier,
        szTableOwner,
        szTableName,
        szColumnName
    );

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // cycle through the result set.
    for (
        pCursor->Fetch();
        pCursor->sqlsuccess();
        pCursor->Fetch()
        )
    {
        ret = (*pfEnum)(*psResultSet, pUser);
        if (ret != SQL_SUCCESS)
        {
            DeAllocCursor(pCursor);
            delete psResultSet;
            return ret;
        }
    }

    delete psResultSet;
    DeAllocCursor(pCursor);
    return lastRC();
}


/**********************************************************
    EnumSpecialColumns

    Calls SQLSpecialColumns and enumerates the result set,
    passing each column's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

RETCODE odbcCONNECT::EnumSpecialColumns(
    UWORD               fColType,
    LPUSTR              szTableQualifier,
    LPUSTR              szTableOwner,
    LPUSTR              szTableName,
    UWORD               fScope,
    UWORD               fNullable,
    pfENUMSPECIALCOL    pfEnum,
    void*                 pUser
)
{
    // make a cursor
    odbcCURSOR* pCursor = AllocCursor();
    RETCODE ret;

    if (!pCursor)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    auto psResultSet = new sSQLSPECIALCOLRESULTSET;

    if (!psResultSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        DeAllocCursor(pCursor);
        return lastRC();
    }

    // bind columns for result set
    pCursor->BindCol(
        SQLSpecialColumnsRSB(),
        SQLSpecialColumnsRSBCount(),
        psResultSet);

    if (!pCursor->sqlsuccess())
    {
        SetRC(SQL_ALLOC_FAILED);
        delete psResultSet;
        DeAllocCursor(pCursor);
        return lastRC();
    }

    // columns
    pCursor->SpecialColumns(
        fColType,
        szTableQualifier,
        szTableOwner,
        szTableName,
        fScope,
        fNullable
    );

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // cycle through the result set.
    for (
        pCursor->Fetch();
        pCursor->sqlsuccess();
        pCursor->Fetch()
        )
    {
        ret = (*pfEnum)(*psResultSet, pUser);
        if (ret != SQL_SUCCESS)
        {
            DeAllocCursor(pCursor);
            delete psResultSet;
            return ret;
        }
    }

    delete psResultSet;
    DeAllocCursor(pCursor);
    return lastRC();
}


/**********************************************************
    EnumTables

    Calls SQLTables and enumerates the result set,
    passing each table's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

RETCODE odbcCONNECT::EnumTables(
    LPUSTR          szTableQualifier,
    LPUSTR          szTableOwner,
    LPUSTR          szTableName,
    LPUSTR          szTableType,
    pfENUMTABLES    pfEnum,
    void*             pUser
)
{
    // make a cursor
    odbcCURSOR* pCursor = AllocCursor();
    RETCODE ret;

    if (!pCursor)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    auto psResultSet = new sSQLTABLESRESULTSET;

    if (!psResultSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        DeAllocCursor(pCursor);
        return lastRC();
    }

    // bind columns for result set
    pCursor->BindCol(
        SQLTablesRSB(),
        SQLTablesRSBCount(),
        psResultSet);

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // columns
    pCursor->Tables(
        szTableQualifier,
        szTableOwner,
        szTableName,
        szTableType
    );

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // cycle through the result set.
    for (
        pCursor->Fetch();
        pCursor->sqlsuccess();
        pCursor->Fetch()
        )
    {
        ret = (*pfEnum)(*psResultSet, pUser);
        if (ret != SQL_SUCCESS)
        {
            delete psResultSet;
            DeAllocCursor(pCursor);
            return ret;
        }
    }

    delete psResultSet;
    DeAllocCursor(pCursor);
    return lastRC();
}

/**********************************************************
    EnumStatistics

    Calls SQLStatistics and enumerates the result set,
    passing each table's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

RETCODE odbcCONNECT::EnumStatistics(
    LPUSTR              szTableQualifier,
    LPUSTR              szTableOwner,
    LPUSTR              szTableName,
    UWORD               fUnique,
    UWORD               fAccuracy,
    pfENUMSTATISTICS    pfEnum,
    void*                 pUser
)
{
    // make a cursor
    odbcCURSOR* pCursor = AllocCursor();
    RETCODE ret;

    if (!pCursor)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    auto psResultSet = new sSQLSTATISTICSRESULTSET;

    if (!psResultSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        DeAllocCursor(pCursor);
        return lastRC();
    }

    // bind columns for result set
    pCursor->BindCol(
        SQLStatisticsRSB(),
        SQLStatisticsRSBCount(),
        psResultSet);

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // columns
    pCursor->Statistics(
        szTableQualifier,
        szTableOwner,
        szTableName,
        fUnique,
        fAccuracy
    );

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // cycle through the result set.
    for (
        pCursor->Fetch();
        pCursor->sqlsuccess();
        pCursor->Fetch()
        )
    {
        ret = (*pfEnum)(*psResultSet, pUser);
        if (ret != SQL_SUCCESS)
        {
            delete psResultSet;
            DeAllocCursor(pCursor);
            return ret;
        }
    }

    delete psResultSet;
    DeAllocCursor(pCursor);
    return lastRC();
}

/**********************************************************
    GetTypeInfo

    Calls SQLGetTypeInfo. Pass the flag of a specific type,
    and it returns the type's attributes in the result
    set structure.

**********************************************************/

RETCODE odbcCONNECT::GetTypeInfo(
    UWORD                   fSQLType,
    sSQLTYPEINFORESULTSET&  rsResultSet
)
{
    // make a cursor
    odbcCURSOR* pCursor = AllocCursor();
    RETCODE ret;

    auto psResultSet = new sSQLTYPEINFORESULTSET;

    if (!psResultSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    // bind columns for result set
    pCursor->BindCol(
        SQLTypeInfoRSB(),
        SQLTypeInfoRSBCount(),
        psResultSet);

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }


    // columns
    pCursor->GetTypeInfo(
        fSQLType
    );

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // get the result set (first one only; there should be only one).
    pCursor->Fetch();

    // return the result set to the user
    ret = pCursor->lastRC();
    if (pCursor->sqlsuccess())
    {
        rsResultSet = *psResultSet;
    }

    delete psResultSet;
    DeAllocCursor(pCursor);
    return ret;
}

/**********************************************************
    EnumTypeInfo

    Calls SQLGetTypeInfo with flag to get attributes of
    all types and enumerates the result set,
    passing each table's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

RETCODE odbcCONNECT::EnumTypeInfo(
    pfENUMTYPEINFO      pfEnum,
    void*                 pUser)
{
    // make a cursor
    odbcCURSOR* pCursor = AllocCursor();
    RETCODE ret;

    if (!pCursor)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    auto psResultSet = new sSQLTYPEINFORESULTSET;

    if (!psResultSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        DeAllocCursor(pCursor);
        return lastRC();
    }

    // bind columns for result set
    pCursor->BindCol(
        SQLTypeInfoRSB(),
        SQLTypeInfoRSBCount(),
        psResultSet);

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // get type info for all types
    pCursor->GetTypeInfo(
        SQL_ALL_TYPES
    );

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // cycle through the result set. 
    for (
        pCursor->Fetch();
        pCursor->sqlsuccess();
        pCursor->Fetch()
        )
    {
        ret = (*pfEnum)(*psResultSet, pUser);
        if (ret != SQL_SUCCESS)
        {
            delete psResultSet;
            DeAllocCursor(pCursor);
            return ret;
        }
    }

    delete psResultSet;
    DeAllocCursor(pCursor);
    return lastRC();
}

// new in v2.0

/**********************************************************
    EnumProcedureColumns

    Calls SQLProcedureColumns and enumerates the result set,
    passing each column's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

RETCODE odbcCONNECT::EnumProcedureColumns(
    LPUSTR              szProcQualifier,
    LPUSTR              szProcOwner,
    LPUSTR              szProcName,
    LPUSTR              szColumnName,
    pfENUMPROCEDURECOL  pfEnum,
    void*                 pUser
)
{
    // make a cursor
    odbcCURSOR* pCursor = AllocCursor();
    RETCODE ret;

    if (!pCursor)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    auto psResultSet = new sPROCEDURECOLUMNSRESULTSET;

    if (!psResultSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        DeAllocCursor(pCursor);
        return lastRC();
    }

    // bind columns for result set
    pCursor->BindCol(
        SQLProcedureColumnsRSB(),
        SQLProcedureColumnsRSBCount(),
        psResultSet);

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        DeAllocCursor(pCursor);
        delete psResultSet;
        return ret;
    }

    // columns
    pCursor->ProcedureColumns(
        szProcQualifier,
        szProcOwner,
        szProcName,
        szColumnName
    );

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        DeAllocCursor(pCursor);
        delete psResultSet;
        return ret;
    }

    // cycle through the result set. 
    for (
        pCursor->Fetch();
        pCursor->sqlsuccess();
        pCursor->Fetch()
        )
    {
        ret = (*pfEnum)(*psResultSet, pUser);
        if (ret != SQL_SUCCESS)
        {
            DeAllocCursor(pCursor);
            delete psResultSet;
            return ret;
        }
    }

    delete psResultSet;
    DeAllocCursor(pCursor);
    return lastRC();
}

/**********************************************************
    EnumProcedures

    Calls SQLProcedures and enumerates the result set,
    passing each column's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

RETCODE odbcCONNECT::EnumProcedures(
    LPUSTR              szProcQualifier,
    LPUSTR              szProcOwner,
    LPUSTR              szProcName,
    pfENUMPROCEDURES    pfEnum,
    void*                 pUser
)
{
    // make a cursor
    odbcCURSOR* pCursor = AllocCursor();
    RETCODE ret;

    if (!pCursor)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    auto psResultSet = new sPROCEDURESRESULTSET;

    if (!psResultSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        DeAllocCursor(pCursor);
        return lastRC();
    }

    // bind columns for result set
    pCursor->BindCol(
        SQLProceduresRSB(),
        SQLProceduresRSBCount(),
        psResultSet);

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        DeAllocCursor(pCursor);
        delete psResultSet;
        return ret;
    }

    // columns
    pCursor->Procedures(
        szProcQualifier,
        szProcOwner,
        szProcName
    );

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        DeAllocCursor(pCursor);
        delete psResultSet;
        return ret;
    }

    // cycle through the result set. 
    for (
        pCursor->Fetch();
        pCursor->sqlsuccess();
        pCursor->Fetch()
        )
    {
        ret = (*pfEnum)(*psResultSet, pUser);
        if (ret != SQL_SUCCESS)
        {
            DeAllocCursor(pCursor);
            delete psResultSet;
            return ret;
        }
    }

    delete psResultSet;
    DeAllocCursor(pCursor);
    return lastRC();
}

/**********************************************************
    EnumColumnPrivileges

    Calls SQLColumnPrivileges and enumerates the result set,
    passing each column's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

RETCODE odbcCONNECT::EnumColumnPrivileges(
    LPUSTR              szTableQualifier,
    LPUSTR              szTableOwner,
    LPUSTR              szTableName,
    LPUSTR              szColumnName,
    pfENUMCOLPRIVS      pfEnum,
    void*                 pUser
)
{
    // make a cursor
    odbcCURSOR* pCursor = AllocCursor();
    RETCODE ret;

    if (!pCursor)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    auto psResultSet = new sCOLUMNPRIVILEGESRESULTSET;

    if (!psResultSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        DeAllocCursor(pCursor);
        return lastRC();
    }

    // bind columns for result set
    pCursor->BindCol(
        SQLColumnPrivilegesRSB(),
        SQLColumnPrivilegesRSBCount(),
        psResultSet);

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // columns
    pCursor->ColumnPrivileges(
        szTableQualifier,
        szTableOwner,
        szTableName,
        szColumnName
    );

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // cycle through the result set.
    for (
        pCursor->Fetch();
        pCursor->sqlsuccess();
        pCursor->Fetch()
        )
    {
        ret = (*pfEnum)(*psResultSet, pUser);
        if (ret != SQL_SUCCESS)
        {
            DeAllocCursor(pCursor);
            delete psResultSet;
            return ret;
        }
    }

    delete psResultSet;
    DeAllocCursor(pCursor);
    return lastRC();
}

/**********************************************************
    EnumTablePrivileges

    Calls SQLTablePrivileges and enumerates the result set,
    passing each column's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

RETCODE odbcCONNECT::EnumTablePrivileges(
    LPUSTR              szTableQualifier,
    LPUSTR              szTableOwner,
    LPUSTR              szTableName,
    pfENUMTABLEPRIVS    pfEnum,
    void*                 pUser
)
{
    // make a cursor
    odbcCURSOR* pCursor = AllocCursor();
    RETCODE ret;

    if (!pCursor)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    auto psResultSet = new sTABLEPRIVILEGESRESULTSET;

    if (!psResultSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        DeAllocCursor(pCursor);
        return lastRC();
    }

    // bind columns for result set
    pCursor->BindCol(
        SQLTablePrivilegesRSB(),
        SQLTablePrivilegesRSBCount(),
        psResultSet);

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // columns
    pCursor->TablePrivileges(
        szTableQualifier,
        szTableOwner,
        szTableName
    );

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // cycle through the result set.
    for (
        pCursor->Fetch();
        pCursor->sqlsuccess();
        pCursor->Fetch()
        )
    {
        ret = (*pfEnum)(*psResultSet, pUser);
        if (ret != SQL_SUCCESS)
        {
            DeAllocCursor(pCursor);
            delete psResultSet;
            return ret;
        }
    }

    delete psResultSet;
    DeAllocCursor(pCursor);
    return lastRC();
}

/**********************************************************
    EnumForeignKeys

    Calls SQLForeignKeys and enumerates the result set,
    passing each column's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

RETCODE odbcCONNECT::EnumForeignKeys(
    LPUSTR              szPkTableQualifier,
    LPUSTR              szPkTableOwner,
    LPUSTR              szPkTableName,
    LPUSTR              szFkTableQualifier,
    LPUSTR              szFkTableOwner,
    LPUSTR              szFkTableName,
    pfENUMFOREIGNKEYS   pfEnum,
    void*                 pUser
)
{
    // make a cursor
    odbcCURSOR* pCursor = AllocCursor();
    RETCODE ret;

    if (!pCursor)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    auto psResultSet = new sFOREIGNKEYSRESULTSET;

    if (!psResultSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        DeAllocCursor(pCursor);
        return lastRC();
    }

    // bind columns for result set
    pCursor->BindCol(
        SQLForeignKeysRSB(),
        SQLForeignKeysRSBCount(),
        psResultSet);

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // columns
    pCursor->ForeignKeys(
        szPkTableQualifier,
        szPkTableOwner,
        szPkTableName,
        szFkTableQualifier,
        szFkTableOwner,
        szFkTableName
    );

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // cycle through the result set.
    for (
        pCursor->Fetch();
        pCursor->sqlsuccess();
        pCursor->Fetch()
        )
    {
        ret = (*pfEnum)(*psResultSet, pUser);
        if (ret != SQL_SUCCESS)
        {
            DeAllocCursor(pCursor);
            delete psResultSet;
            return ret;
        }
    }

    delete psResultSet;
    DeAllocCursor(pCursor);
    return lastRC();
}

/**********************************************************
    EnumPrimaryKeys

    Calls SQLPrimaryKeys and enumerates the result set,
    passing each column's data attributes as a structure to
    the callback function supplied by the caller.

**********************************************************/

RETCODE odbcCONNECT::EnumPrimaryKeys(
    LPUSTR              szTableQualifier,
    LPUSTR              szTableOwner,
    LPUSTR              szTableName,
    pfENUMPRIMARYKEYS   pfEnum,
    void*                 pUser
)
{
    // make a cursor
    odbcCURSOR* pCursor = AllocCursor();
    RETCODE ret;

    if (!pCursor)
    {
        SetRC(SQL_ALLOC_FAILED);
        return lastRC();
    }

    auto psResultSet = new sPRIMARYKEYSRESULTSET;

    if (!psResultSet)
    {
        SetRC(SQL_ALLOC_FAILED);
        DeAllocCursor(pCursor);
        return lastRC();
    }

    // bind columns for result set
    pCursor->BindCol(
        SQLPrimaryKeysRSB(),
        SQLPrimaryKeysRSBCount(),
        psResultSet);

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // columns
    pCursor->PrimaryKeys
    (
        szTableQualifier,
        szTableOwner,
        szTableName
    );

    if (!pCursor->sqlsuccess())
    {
        ret = pCursor->lastRC();
        delete psResultSet;
        DeAllocCursor(pCursor);
        return ret;
    }

    // cycle through the result set.
    for (
        pCursor->Fetch();
        pCursor->sqlsuccess();
        pCursor->Fetch()
        )
    {
        ret = (*pfEnum)(*psResultSet, pUser);
        if (ret != SQL_SUCCESS)
        {
            DeAllocCursor(pCursor);
            delete psResultSet;
            return ret;
        }
    }

    delete psResultSet;
    DeAllocCursor(pCursor);
    return lastRC();
}

// end new in v2.0

/**********************************************************
    NativeSql

    Calls SQLNativeSql to have the driver translate the
    given SQL statement into .

**********************************************************/

RETCODE odbcCONNECT::NativeSql(
    UCHAR      *szSqlStrIn,
    UCHAR      *szSqlStr,
    SDWORD     cbSqlStrMax,
    SDWORD     *pcbSqlStr)
{
    SetRC(
        SQLNativeSql(
            hdbc,
            szSqlStrIn,
            SQL_NTS,
            szSqlStr,
            cbSqlStrMax,
            pcbSqlStr));

    return lastRC();
}


