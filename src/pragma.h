#pragma once

#ifdef _MSC_VER
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4101) // unreferenced local variable
#pragma warning(disable: 4146) // unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4244) // 'initializing': conversion from 'int' to 'char', possible loss of data
#pragma warning(disable: 4245) // conversion from 'int' to 'uint64_t', signed / unsigned mismatch
#pragma warning(disable: 4267) // 'argument': conversion from 'size_t' to 'int', possible loss of data
#pragma warning(disable: 4305) // truncation from 'int' to 'bool'
#pragma warning(disable: 4389) // signed / unsigned mismatch
#pragma warning(disable: 4458) // declaration of 'wtime' hides class member
#pragma warning(disable: 4459) // declaration of 'tt' hides global declaration
#pragma warning(disable: 4505) // unreferenced function with internal linkage has been removed
#pragma warning(disable: 4701) // potentially uninitialized local variable
#pragma warning(disable: 4702) // unreachable code
#pragma warning(disable: 4706) // assignment within conditional expression
#pragma warning(disable: 4715) // not all control paths return a value
#else
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
