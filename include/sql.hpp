#if !defined(__SQL_HPP)
#define __SQL_HPP

#if defined(WIN32)
#include <windows.h>
#endif

#include "sql.h"
#include "sqlext.h"

// allow support for prior ancestors of odbcBASE
#if !defined( __LINEAGE )
#define __LINEAGE
#endif


#include "odbclslb/sqldefs.hpp"

#include "odbclslb/sqlstruc.hpp"

#include "odbclslb/base.hpp"

#include "odbclslb/stmt.hpp"

#include "odbclslb/cursor.hpp"

#include "odbclslb/blobs.hpp"

#include "odbclslb/cretable.hpp"

#include "odbclslb/record.hpp"

#include "odbclslb/connect.hpp"

#include "odbclslb/queriter.hpp"

#include "odbclslb/env.hpp"


#endif  // #if !defined(__SQL_HPP)
