#pragma once

// definitions of pointers, references, and pointers and references to const
struct odbcEXPORTED sCOLBIND;
struct odbcEXPORTED sPARMBIND;
struct odbcEXPORTED sSQLCOLUMNSRESULTSET;
struct odbcEXPORTED sSQLTABLESRESULTSET;
struct odbcEXPORTED sSQLSTATISTICSRESULTSET;
struct odbcEXPORTED sSQLTYPEINFORESULTSET;
struct odbcEXPORTED sSQLSPECIALCOLRESULTSET;
struct odbcEXPORTED sCOLUMNPRIVILEGESRESULTSET;
struct odbcEXPORTED sPROCEDURESRESULTSET;
struct odbcEXPORTED sPROCEDURECOLUMNSRESULTSET;
struct odbcEXPORTED sFOREIGNKEYSRESULTSET;
struct odbcEXPORTED sTABLEPRIVILEGESRESULTSET;
struct odbcEXPORTED sPRIMARYKEYSRESULTSET;

// manifest constant for sPARMBIND iOffset member

const int odbcUSE_RGBVALUE = ((SWORD)(UWORD)(0xfffe));
/*
///////////////////////////////////////////////////////
//////// sCOLBIND /////////////////////////////////////
///////////////////////////////////////////////////////

Column binding structure, for use in odbcCURSOR::BindCols.
The address of an array of these will be passed to the member
function (along with a count and the address of an output
structure).

Reminders:
        Inputs:
        =======
iCol:           Column numbers start with 1, not zero.
fCType:         See ODBC docs for SQLBindCol for legal constants
iOffset:        Use the FLDOFFSET macro from windows.h to get the
            offset of the receiving member in the output structure.
cbValueMax:     Use sizeof() to be accurate.

            Outputs
            =======
cbValue         After fetch, will contain the actual bytes transferred
            or if truncation occurred the number of bytes that could
            have been transferred.
*/

struct odbcEXPORTED sCOLBIND {
    UWORD           iCol;           // col number in query
    SWORD           fCType;         // C data type constant
    unsigned        iOffset;        // offset in result struct: use offsetof
    SDWORD          cbValueMax;     // size of buffer
    SDWORD          cbValue;        // size of actual result (output)
    UWORD           fPtr;           // if non-zero, struct member contains 
                                    // address of buffer, not the buffer itself
    UCHAR           szColName[COLUMN_NAME_MAX];
    // column name used in auto-binding
// new in v2.0
    SWORD           fSqlType;       // actual SQL type
    SWORD           ibScale;        // scale (only used for some types)
    SWORD           fNullable;      // non-zero if null values are possible
    void*             pConstraints;   // if non-NULL, column constraints
                                    //   to be used in CreateTable()
    // end new in v2.0
};

/*
The following macros make it easier to initialize arrays  of sCOLBIND
structs.
*/

#define _colbindChar(num,struc,memb,msize)\
    {num,SQL_C_CHAR,offsetof(struc,memb),msize,}

#define _colbindShort(num,struc,memb){num,SQL_C_SHORT,offsetof(struc,memb),0,}

#define _colbindLong(num,struc,memb){num,SQL_C_LONG,offsetof(struc,memb),0,}

#define _colbindFloat(num,struc,memb){num,SQL_C_FLOAT,offsetof(struc,memb),0,}

#define _colbindDouble(num,struc,memb){num,SQL_C_DOUBLE,offsetof(struc,memb),0,}

#define _colbindDate(num,struc,memb){num,SQL_C_DATE,offsetof(struc,memb),0,}

#define _colbindTime(num,struc,memb){num,SQL_C_TIME,offsetof(struc,memb),0,}

#define _colbindTimestamp(num,struc,memb)\
    {num,SQL_C_TIMESTAMP,offsetof(struc,memb),0,}

#define _colbindBit(num,struc,memb){num,SQL_C_BIT,offsetof(struc,memb),0,}

#define _colbindTinyint(num,struc,memb){num,SQL_C_TINYINT,offsetof(struc,memb),0,}

#define _colbindBinary(num,struc,memb,msize)\
    {num,SQL_C_BINARY,offsetof(struc,memb),msize,}

// new in version 2 to handle the additional struct members

#define _colbindCharV2(num,struc,memb,msize,name,type,nullable,constraint)\
    {num,SQL_C_CHAR,offsetof(struc,memb),msize,0,0,name,type,0,nullable,constraint}

#define _colbindShortV2(num,struc,memb,name,type,nullable,constraint)\
         {num,SQL_C_SHORT,offsetof(struc,memb),0,0,0,name,type,0,nullable,constraint }

#define _colbindLongV2(num,struc,memb,name,type,nullable,constraint)\
        {num,SQL_C_LONG,offsetof(struc,memb),0,0,0,name,type,0,nullable,constraint }

#define _colbindFloatV2(num,struc,memb,name,type,scale,nullable,constraint)\
        {num,SQL_C_FLOAT,offsetof(struc,memb),0,0,0,name,type,scale,nullable,constraint }

#define _colbindDoubleV2(num,struc,memb,name,type, scale,nullable,constraint)\
        {num,SQL_C_DOUBLE,offsetof(struc,memb),0,0,0,name,type,scale,nullable,constraint }

#define _colbindDateV2(num,struc,memb,name,type, nullable,constraint)\
        {num,SQL_C_DATE,offsetof(struc,memb),0,0,0,name,type,0,nullable,constraint }

#define _colbindTimeV2(num,struc,memb,name,type,nullable,constraint)\
        {num,SQL_C_TIME,offsetof(struc,memb),0,0,0,name,type,0,nullable,constraint }

#define _colbindTimestampV2(num,struc,memb,name,type,nullable,constraint)\
    {num,SQL_C_TIMESTAMP,offsetof(struc,memb),0,0,0,name,type,0,nullable,constraint }

#define _colbindBitV2(num,struc,memb,name,type,nullable,constraint)\
        {num,SQL_C_BIT,offsetof(struc,memb),0,0,0,name,type,0,nullable,constraint }

#define _colbindTinyintV2(num,struc,memb,name,type,nullable,constraint)\
        {num,SQL_C_TINYINT,offsetof(struc,memb),0,0,0,name,type,0,nullable,constraint }

#define _colbindBinaryV2(num,struc,memb,msize,name,type,nullable,constraint)\
    {num,SQL_C_BINARY,offsetof(struc,memb),msize,0,0,name,type,0,nullable,constraint }

#define _colbindLongBinaryV2(num,struc,memb,msize, name,nullable,constraint)\
{num,SQL_C_CHAR,offsetof(struc,memb),msize,0,0,name,SQL_LONGVARBINARY,0,nullable,constraint }

#define _colbindLongVarcharV2(num,struc,memb,msize, name,nullable,constraint)\
{num,SQL_C_CHAR,offsetof(struc,memb),msize,0,0,name,SQL_LONGVARCHAR,0,nullable,constraint }

/*
///////////////////////////////////////////////////////
//////// sPARMBIND /////////////////////////////////////
///////////////////////////////////////////////////////

Parameter binding structure, for use in odbcSTMT::SetParams.
The address of an array of these will be passed to the member
function (along with a count and the address of an output
structure).

This parameter binding mechanism only works for single-value
bindings.

Reminders:
        Inputs:
        =======
iParm:          Parameter markers start with 1, not zero.
fCType:         See ODBC docs for SQLSetParam for legal constants
fSqlType:       See ODBC docs for SQLSetParam for legal constants
iOffset:        Use the FLDOFFSET macro from windows.h to get the
                offset of the sending member in the input structure.
                This value can also be the constant odbcUSE_RGBVALUE
                to cause the structure address to be ignored and the
                rgbValue member value to be passed directly. This is
                used in conjunction with SQL_DATA_AT_EXEC in the
                variable cbValue and the ParamData
                member function to pass parameter values at
                statement execution time rather than at parameter
                inding time.
rgbValue:       32-bit application-defined token used in conjunction
                with SQL_DATA_AT_EXEC in cbValue.
cbValue:        Variable containing size of parameter.

                Use sizeof() to be accurate.  The variable can
                also be set to one of three special constants:

                SQL_NULL_DATA       Parameter value is NULL

                SQL_NTS             Parameter is null-terminated
                                    string

                SQL_DATA_AT_EXEC    Data for the parameter value
                                    will be supplied at run-time.
                                    In this case, rgbValue will be
                                    treated as a 32-bit token that
                                    wll be returned by the ParamData
                                    member function as an application-
                                    defined parameter token.  See
                                    ParamData for more details.

*/

struct odbcEXPORTED sPARMBIND {
    UWORD           iParm;          // col number in query
    SWORD           fCType;         // C data type constant
    SWORD           fSqlType;       // SQL data type constant
    UDWORD          cbColDef;       // Max value (INPUT size of parm or
                                    //  size of available buffer for
                                    //  output and i/o parms)
    SWORD           ibScale;        // Scale (set to 0 for non-numeric 
                                    //    and integer types)
    SWORD           iOffset;        // offset into parameter structure,
                                    //    for use in composing address of
                                    //    parameter.
    void*             rgbValue;       // 32-bit app-defined token for
                                    //    SQL_DATA_AT_EXEC, or NULL 
                                    //    otherwise.
    SDWORD          cbValue;        // Variable containing size of actual
                                    //  data; can be set to SQL_NTS, 
                                    //  SQL_NULL_DATA, or SQL_DATA_AT_EXEC
    SWORD           fParamType;     // new in v2.0; see SQLBindParameter
                                    // fParam argument for this value.
};

// macros for initializing sPARMBIND structures
#define _parmbind(num,ctype,sqltype,precision,scale,struc,memb,user32,msize)\
   {num,ctype,sqltype,precision,scale,offsetof(struc,memb),user32,msize,}

#define _parmbindV2(num,ctype,sqltype,precision,scale,struc,memb,user32,msize,parmtype)\
   {num,ctype,sqltype,precision,scale,offsetof(struc,memb),user32,msize,parmtype,}
/*
///////////////////////////////////////////////////////
//////// sSQLCOLUMNSRESULTSET /////////////////////////////
///////////////////////////////////////////////////////

Receiver struct for result set of call to SQLColumns.

*/

struct odbcEXPORTED sSQLCOLUMNSRESULTSET {
    UCHAR szTableQualifier[TABLE_QUALIFIER_MAX];
    UCHAR szTableOwner[TABLE_OWNER_MAX];
    UCHAR szTableName[TABLE_NAME_MAX];
    UCHAR szColumnName[COLUMN_NAME_MAX];
    SWORD fDataType;
    UCHAR szTypeName[TYPE_NAME_MAX];
    SDWORD fPrecision;
    SDWORD fLength;
    SWORD Scale;
    SWORD Radix;
    SWORD Nullable;
    UCHAR Remarks[REMARKS_MAX];
};


// enumerator function typedef for callback to EnumColumns
typedef RETCODE(*pfENUMCOLUMNS)(const sSQLCOLUMNSRESULTSET&, void*);

/*
///////////////////////////////////////////////////////
//////// sSQLTABLESRESULTSET //////////////////////////
///////////////////////////////////////////////////////

Receiver struct for result set of call to SQLTables.

*/

struct odbcEXPORTED sSQLTABLESRESULTSET {
    UCHAR szTableQualifier[TABLE_QUALIFIER_MAX];
    UCHAR szTableOwner[TABLE_OWNER_MAX];
    UCHAR szTableName[TABLE_NAME_MAX];
    UCHAR szTableType[TABLE_TYPE_MAX];
    UCHAR Remarks[REMARKS_MAX];
};

// enumerator function typedef for callback to EnumTables
typedef RETCODE(*pfENUMTABLES)(const sSQLTABLESRESULTSET&, void*);

/*
///////////////////////////////////////////////////////
//////// sSQLSTATISTICSRESULTSET //////////////////////////
///////////////////////////////////////////////////////

Receiver struct for result set of call to SQLStatistics.

*/

struct odbcEXPORTED sSQLSTATISTICSRESULTSET {
    UCHAR szTableQualifier[TABLE_QUALIFIER_MAX];
    UCHAR szTableOwner[TABLE_OWNER_MAX];
    UCHAR szTableName[TABLE_NAME_MAX];
    SWORD fNonUnique;
    UCHAR szIndexQualifier[INDEX_QUALIFIER_MAX];
    UCHAR szIndexName[INDEX_NAME_MAX];
    SWORD fType;
    SWORD fSeqInIndex;
    UCHAR szColumnName[COLUMN_NAME_MAX];
    UCHAR cCollation;
    UCHAR uFiller;
    SDWORD nCardinality;
    SDWORD nPages;
};


// enumerator function typedef for callback to EnumStatistics
typedef RETCODE(*pfENUMSTATISTICS)(const sSQLSTATISTICSRESULTSET&, void*);

/*
///////////////////////////////////////////////////////
//////// sSQLTYPEINFORESULTSET //////////////////////////
///////////////////////////////////////////////////////

Receiver struct for result set of call to SQLTypeInfo.

*/

struct odbcEXPORTED sSQLTYPEINFORESULTSET {
    UCHAR szTypeName[STRING1_MAX];
    SWORD fDataType;
    SDWORD fPrecision;
    UCHAR szLiteralPrefix[STRING1_MAX];
    UCHAR szLiteralSuffix[STRING1_MAX];
    UCHAR szCreateParams[STRING1_MAX];
    SWORD fNullable;
    SWORD fCaseSensitive;
    SWORD fSearchable;
    SWORD fUnsigned;
    SWORD fMoney;
    SWORD fAutoIncrement;
    UCHAR szLocalTypeName[STRING1_MAX];
};

// enumerator function typedef for callback to EnumTypeInfo
typedef RETCODE(*pfENUMTYPEINFO)(const sSQLTYPEINFORESULTSET&, void*);

/*
///////////////////////////////////////////////////////
//////// sSQLSPECIALCOLRESULTSET //////////////////////////
///////////////////////////////////////////////////////

Receiver struct for result set of call to SQLSpecialColumns

*/

struct odbcEXPORTED sSQLSPECIALCOLRESULTSET {
    SWORD fScope;
    UCHAR szColumnName[COLUMN_NAME_MAX];
    SWORD fDataType;
    UCHAR szTypeName[TYPE_NAME_MAX];
    SDWORD fPrecision;
    SDWORD fLength;
    SWORD Scale;
};

// enumerator function typedef for callback to EnumSpecialColumns
typedef RETCODE(*pfENUMSPECIALCOL)(const sSQLSPECIALCOLRESULTSET&, void*);

// new in v2.0

/*
///////////////////////////////////////////////////////
//////// sCOLUMNPRIVILEGESRESULTSET ///////////////////
///////////////////////////////////////////////////////

Receiver struct for result set of call to SQLColumnPrivileges.

*/

struct odbcEXPORTED sCOLUMNPRIVILEGESRESULTSET {
    char                 szTable_qualifier[TABLE_QUALIFIER_SIZE];
    char                 szTable_owner[TABLE_OWNER_SIZE];
    char                 szTable_name[TABLE_NAME_SIZE];
    char                 szColumn_name[COLUMN_NAME_SIZE];
    char                 szGrantor[GRANTOR_SIZE];
    char                 szGrantee[GRANTEE_SIZE];
    char                 szPrivilege[PRIVILEGE_SIZE];
    char                 szIs_grantable[IS_GRANTABLE_SIZE];
};

// enumerator function typedef for callback to EnumColumnPrivileges
typedef RETCODE(*pfENUMCOLPRIVS)(const sCOLUMNPRIVILEGESRESULTSET&, void*);

/*
///////////////////////////////////////////////////////
//////// sPROCEDURESRESULTSET ///////////////////
///////////////////////////////////////////////////////

Receiver struct for result set of call to SQLProcedures.

*/

struct odbcEXPORTED sPROCEDURESRESULTSET {
    char                 szProcedure_qualifier[PROCEDURE_QUALIFIER_SIZE];
    char                 szProcedure_owner[PROCEDURE_OWNER_SIZE];
    char                 szProcedure_name[PROCEDURE_NAME_SIZE];
    long                 lNum_input_params;
    long                 lNum_output_params;
    long                 lNum_result_sets;
    char                 szRemarks[REMARKS_SIZE];
};

// enumerator function typedef for callback to EnumProcedures
typedef RETCODE(*pfENUMPROCEDURES)(const sPROCEDURESRESULTSET&, void*);

/*
///////////////////////////////////////////////////////
//////// sPROCEDURECOLUMNSRESULTSET ///////////////////
///////////////////////////////////////////////////////

Receiver struct for result set of call to SQLProcedureColumnss.

*/

struct odbcEXPORTED sPROCEDURECOLUMNSRESULTSET {
    char                 szProcedure_qualifier[PROCEDURE_QUALIFIER_SIZE];
    char                 szProcedure_owner[PROCEDURE_OWNER_SIZE];
    char                 szProcedure_name[PROCEDURE_NAME_SIZE];
    char                 szColumn_name[COLUMN_NAME_SIZE];
    short                nColumn_type;
    char                 szData_type[DATA_TYPE_SIZE];
    char                 szType_name[TYPE_NAME_SIZE];
    long                 lPrecision;
    long                 lLength;
    short                nScale;
    short                nRadix;
    short                nNullable;
    char                 szRemarks[REMARKS_SIZE];
};

// enumerator function typedef for callback to EnumProcedureColumns
typedef RETCODE(*pfENUMPROCEDURECOL)(const sPROCEDURECOLUMNSRESULTSET&, void*);

/*
///////////////////////////////////////////////////////
//////// sFOREIGNKEYSRESULTSET ///////////////////
///////////////////////////////////////////////////////

Receiver struct for result set of call to SQLForeignKeys.

*/

struct odbcEXPORTED sFOREIGNKEYSRESULTSET {
    char                 szPktable_qualifier[PKTABLE_QUALIFIER_SIZE];
    char                 szPktable_owner[PKTABLE_OWNER_SIZE];
    char                 szPktable_name[PKTABLE_NAME_SIZE];
    char                 szPkcolumn_name[PKCOLUMN_NAME_SIZE];
    char                 szFktable_qualifier[FKTABLE_QUALIFIER_SIZE];
    char                 szFktable_owner[FKTABLE_OWNER_SIZE];
    char                 szFktable_name[FKTABLE_NAME_SIZE];
    char                 szFkcolumn_name[FKCOLUMN_NAME_SIZE];
    short                nKey_seq;
    short                nUpdate_rule;
    short                nDelete_rule;
    char                 szRole_name[ROLE_NAME_SIZE];
};

// enumerator function typedef for callback to EnumForeignKeys
typedef RETCODE(*pfENUMFOREIGNKEYS)(const sFOREIGNKEYSRESULTSET&, void*);

/*
///////////////////////////////////////////////////////
//////// sTABLEPRIVILEGESRESULTSET  ///////////////////
///////////////////////////////////////////////////////

Receiver struct for result set of call to SQLTablePrivileges.

*/

struct odbcEXPORTED sTABLEPRIVILEGESRESULTSET {
    char                 szTable_qualifier[TABLE_QUALIFIER_SIZE];
    char                 szTable_owner[TABLE_OWNER_SIZE];
    char                 szTable_name[TABLE_NAME_SIZE];
    char                 szGrantor[GRANTOR_SIZE];
    char                 szGrantee[GRANTEE_SIZE];
    char                 szPrivilege[PRIVILEGE_SIZE];
    char                 szIs_grantable[IS_GRANTABLE_SIZE];
};

// enumerator function typedef for callback to EnumTablePrivileges
typedef RETCODE(*pfENUMTABLEPRIVS)(const sTABLEPRIVILEGESRESULTSET&, void*);

/*
///////////////////////////////////////////////////////
//////// sPRIMARYKEYSRESULTSET ///////////////////
///////////////////////////////////////////////////////

Receiver struct for result set of call to SQLPrimaryKeys.

*/

struct odbcEXPORTED sPRIMARYKEYSRESULTSET {
    char                 szTable_qualifier[TABLE_QUALIFIER_SIZE];
    char                 szTable_owner[TABLE_OWNER_SIZE];
    char                 szTable_name[TABLE_NAME_SIZE];
    char                 szColumn_name[COLUMN_NAME_SIZE];
    short                nKey_seq;
};

// enumerator function typedef for callback to EnumPrimaryKeys
typedef RETCODE(*pfENUMPRIMARYKEYS)(const sPRIMARYKEYSRESULTSET&, void*);

// end new in v2.0
