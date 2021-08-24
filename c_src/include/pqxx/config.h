/* include/pqxx/config.h.  Generated from config.h.in by configure.  */
/* include/pqxx/config.h.in.  Generated from configure.ac by autoheader.  */

/* define if the compiler supports basic C++17 syntax */
#define HAVE_CXX17 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `pq' library (-lpq). */
#define HAVE_LIBPQ 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "libpqxx"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "Jeroen T. Vermeulen"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libpqxx"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libpqxx 7.7.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libpqxx"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "7.7.0"

/* Define if <charconv> supports floating-point conversion. */
/* #undef PQXX_HAVE_CHARCONV_FLOAT */

/* Define if <charconv> supports integer conversion. */
#define PQXX_HAVE_CHARCONV_INT 1

/* Define if compiler supports Concepts and <ranges> header. */
/* #undef PQXX_HAVE_CONCEPTS */

/* Define if compiler supports __cxa_demangle */
#define PQXX_HAVE_CXA_DEMANGLE 1

/* Define if g++ supports pure attribute */
#define PQXX_HAVE_GCC_PURE 1

/* Define if g++ supports visibility attribute. */
#define PQXX_HAVE_GCC_VISIBILITY 1

/* Define if likely & unlikely work. */
/* #undef PQXX_HAVE_LIKELY */

/* Define if poll() is available. */
#define PQXX_HAVE_POLL 1

/* Define if libpq has PQencryptPasswordConn (since pg 10). */
#define PQXX_HAVE_PQENCRYPTPASSWORDCONN 1

/* Define if libpq has pipeline mode (since pg 14). */
/* #undef PQXX_HAVE_PQ_PIPELINE */

/* Define if std::this_thread::sleep_for works. */
#define PQXX_HAVE_SLEEP_FOR 1

/* Define if compiler has std::span. */
/* #undef PQXX_HAVE_SPAN */

/* Define if thread_local is fully supported. */
#define PQXX_HAVE_THREAD_LOCAL 1

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "7.7.0"
