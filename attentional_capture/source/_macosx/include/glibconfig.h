/* glibconfig.h
 *
 * This is a generated file.  Please modify 'configure.in'
 */

#ifndef __G_LIBCONFIG_H__
#define __G_LIBCONFIG_H__

#include <glib/gmacros.h>

#include <limits.h>
#include <float.h>
#define GLIB_HAVE_ALLOCA_H
#define GLIB_HAVE_SYS_POLL_H

/* Specifies that GLib's g_print*() functions wrap the
 * system printf functions.  This is useful to know, for example,
 * when using glibc's register_printf_function().
 */
#define GLIB_USING_SYSTEM_PRINTF

G_BEGIN_DECLS

#define G_MINFLOAT	FLT_MIN
#define G_MAXFLOAT	FLT_MAX
#define G_MINDOUBLE	DBL_MIN
#define G_MAXDOUBLE	DBL_MAX
#define G_MINSHORT	SHRT_MIN
#define G_MAXSHORT	SHRT_MAX
#define G_MAXUSHORT	USHRT_MAX
#define G_MININT	INT_MIN
#define G_MAXINT	INT_MAX
#define G_MAXUINT	UINT_MAX
#define G_MINLONG	LONG_MIN
#define G_MAXLONG	LONG_MAX
#define G_MAXULONG	ULONG_MAX

typedef signed char gint8;
typedef unsigned char guint8;
typedef signed short gint16;
typedef unsigned short guint16;
#define G_GINT16_MODIFIER "h"
#define G_GINT16_FORMAT "hi"
#define G_GUINT16_FORMAT "hu"
typedef signed int gint32;
typedef unsigned int guint32;
#define G_GINT32_MODIFIER ""
#define G_GINT32_FORMAT "i"
#define G_GUINT32_FORMAT "u"
#define G_HAVE_GINT64 1          /* deprecated, always true */

#ifdef __LP64__
G_GNUC_EXTENSION typedef signed long gint64;
G_GNUC_EXTENSION typedef unsigned long guint64;

#define G_GINT64_CONSTANT(val)	(val##L)
#define G_GUINT64_CONSTANT(val)	(val##UL)
#else
G_GNUC_EXTENSION typedef signed long long gint64;
G_GNUC_EXTENSION typedef unsigned long long guint64;

#define G_GINT64_CONSTANT(val)	(G_GNUC_EXTENSION (val##LL))
#define G_GUINT64_CONSTANT(val)	(G_GNUC_EXTENSION (val##ULL))
#endif
#ifdef __LP64__
#define G_GINT64_MODIFIER "l"
#define G_GINT64_FORMAT "li"
#define G_GUINT64_FORMAT "lu"
#else
#define G_GINT64_MODIFIER "ll"
#define G_GINT64_FORMAT "lli"
#define G_GUINT64_FORMAT "llu"
#endif

#ifdef __LP64__
#define GLIB_SIZEOF_VOID_P 8
#define GLIB_SIZEOF_LONG   8
#define GLIB_SIZEOF_SIZE_T 8
#else
#define GLIB_SIZEOF_VOID_P 4
#define GLIB_SIZEOF_LONG   4
#define GLIB_SIZEOF_SIZE_T 4
#endif

typedef signed long gssize;
typedef unsigned long gsize;
#define G_GSIZE_MODIFIER "l"
#define G_GSSIZE_FORMAT "li"
#define G_GSIZE_FORMAT "lu"

#define G_MAXSIZE	G_MAXULONG
#define G_MINSSIZE	G_MINLONG
#define G_MAXSSIZE	G_MAXLONG

typedef gint64 goffset;
#define G_MINOFFSET	G_MININT64
#define G_MAXOFFSET	G_MAXINT64

#define G_GOFFSET_MODIFIER      G_GINT64_MODIFIER
#define G_GOFFSET_FORMAT        G_GINT64_FORMAT
#define G_GOFFSET_CONSTANT(val) G_GINT64_CONSTANT(val)


#ifdef __LP64__
#define GPOINTER_TO_INT(p)	((gint)  (glong) (p))
#define GPOINTER_TO_UINT(p)	((guint) (gulong) (p))

#define GINT_TO_POINTER(i)	((gpointer) (glong) (i))
#define GUINT_TO_POINTER(u)	((gpointer) (gulong) (u))

typedef signed long gintptr;
typedef unsigned long guintptr;

#define G_GINTPTR_MODIFIER      "l"
#define G_GINTPTR_FORMAT        "li"
#define G_GUINTPTR_FORMAT       "lu"
#else
#define GPOINTER_TO_INT(p)	((gint)   (p))
#define GPOINTER_TO_UINT(p)	((guint)  (p))

#define GINT_TO_POINTER(i)	((gpointer)  (i))
#define GUINT_TO_POINTER(u)	((gpointer)  (u))

typedef signed int gintptr;
typedef unsigned int guintptr;

#define G_GINTPTR_MODIFIER      ""
#define G_GINTPTR_FORMAT        "i"
#define G_GUINTPTR_FORMAT       "u"
#endif

#ifdef NeXT /* @#%@! NeXTStep */
# define g_ATEXIT(proc)	(!atexit (proc))
#else
# define g_ATEXIT(proc)	(atexit (proc))
#endif

#define g_memmove(dest,src,len) G_STMT_START { memmove ((dest), (src), (len)); } G_STMT_END

#define GLIB_MAJOR_VERSION 2
#define GLIB_MINOR_VERSION 22
#define GLIB_MICRO_VERSION 1

#define G_OS_UNIX


#define G_VA_COPY	va_copy
#ifdef __LP64__
#define G_VA_COPY_AS_ARRAY 1
#endif

#ifdef	__cplusplus
#define	G_HAVE_INLINE	1
#else	/* !__cplusplus */
#define G_HAVE_INLINE 1
#define G_HAVE___INLINE 1
#define G_HAVE___INLINE__ 1
#endif	/* !__cplusplus */

#ifdef	__cplusplus
#define G_CAN_INLINE	1
#else	/* !__cplusplus */
#define G_CAN_INLINE	1
#endif

#ifndef __cplusplus
# define G_HAVE_ISO_VARARGS 1
#endif
#ifdef __cplusplus
# define G_HAVE_ISO_VARARGS 1
#endif

/* gcc-2.95.x supports both gnu style and ISO varargs, but if -ansi
 * is passed ISO vararg support is turned off, and there is no work
 * around to turn it on, so we unconditionally turn it off.
 */
#if __GNUC__ == 2 && __GNUC_MINOR__ == 95
#  undef G_HAVE_ISO_VARARGS
#endif

#define G_HAVE_GNUC_VARARGS 1
#define G_HAVE_GROWING_STACK 0

#if defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590)
#define G_GNUC_INTERNAL __attribute__((visibility("hidden")))
#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x550)
#define G_GNUC_INTERNAL __hidden
#elif defined (__GNUC__) && defined (G_HAVE_GNUC_VISIBILITY)
#define G_GNUC_INTERNAL __attribute__((visibility("hidden")))
#else
#define G_GNUC_INTERNAL
#endif

#define G_THREADS_ENABLED
#define G_THREADS_IMPL_POSIX
typedef struct _GStaticMutex GStaticMutex;
struct _GStaticMutex
{
  struct _GMutex *runtime_mutex;
  union {
#ifdef __LP64__
    char   pad[64];
#else
    char   pad[44];
#endif
    double dummy_double;
    void  *dummy_pointer;
    long   dummy_long;
  } static_mutex;
};
#ifdef __LP64__
#ifdef __BIG_ENDIAN__
#define	G_STATIC_MUTEX_INIT	{ NULL, { { 50,-86,-85,-89,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} } }
#else
#define	G_STATIC_MUTEX_INIT	{ NULL, { { -89,-85,-86,50,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} } }
#endif
#else
#ifdef __BIG_ENDIAN__
#define	G_STATIC_MUTEX_INIT	{ NULL, { { 50,-86,-85,-89,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} } }
#else
#define	G_STATIC_MUTEX_INIT	{ NULL, { { -89,-85,-86,50,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} } }
#endif
#endif
#define	g_static_mutex_get_mutex(mutex) \
  (g_thread_use_default_impl ? ((GMutex*)(gpointer) ((mutex)->static_mutex.pad)) : \
   g_static_mutex_get_mutex_impl_shortcut (&((mutex)->runtime_mutex)))
/* This represents a system thread as used by the implementation. An
 * alien implementaion, as loaded by g_thread_init can only count on
 * "sizeof (gpointer)" bytes to store their info. We however need more
 * for some of our native implementations. */
typedef union _GSystemThread GSystemThread;
union _GSystemThread
{
#ifdef __LP64__
  char   data[8];
#else
  char   data[4];
#endif
  double dummy_double;
  void  *dummy_pointer;
  long   dummy_long;
};

#ifdef __BIG_ENDIAN__
#define G_ATOMIC_OP_MEMORY_BARRIER_NEEDED 1
#endif

#ifdef __BIG_ENDIAN__
#define GINT16_TO_BE(val)	((gint16) (val))
#define GUINT16_TO_BE(val)	((guint16) (val))
#define GINT16_TO_LE(val)	((gint16) GUINT16_SWAP_LE_BE (val))
#define GUINT16_TO_LE(val)	(GUINT16_SWAP_LE_BE (val))
#else
#define GINT16_TO_LE(val)	((gint16) (val))
#define GUINT16_TO_LE(val)	((guint16) (val))
#define GINT16_TO_BE(val)	((gint16) GUINT16_SWAP_LE_BE (val))
#define GUINT16_TO_BE(val)	(GUINT16_SWAP_LE_BE (val))
#endif
#ifdef __BIG_ENDIAN__
#define GINT32_TO_BE(val)	((gint32) (val))
#define GUINT32_TO_BE(val)	((guint32) (val))
#define GINT32_TO_LE(val)	((gint32) GUINT32_SWAP_LE_BE (val))
#define GUINT32_TO_LE(val)	(GUINT32_SWAP_LE_BE (val))
#else
#define GINT32_TO_LE(val)	((gint32) (val))
#define GUINT32_TO_LE(val)	((guint32) (val))
#define GINT32_TO_BE(val)	((gint32) GUINT32_SWAP_LE_BE (val))
#define GUINT32_TO_BE(val)	(GUINT32_SWAP_LE_BE (val))
#endif
#ifdef __BIG_ENDIAN__
#define GINT64_TO_BE(val)	((gint64) (val))
#define GUINT64_TO_BE(val)	((guint64) (val))
#define GINT64_TO_LE(val)	((gint64) GUINT64_SWAP_LE_BE (val))
#define GUINT64_TO_LE(val)	(GUINT64_SWAP_LE_BE (val))
#else
#define GINT64_TO_LE(val)	((gint64) (val))
#define GUINT64_TO_LE(val)	((guint64) (val))
#define GINT64_TO_BE(val)	((gint64) GUINT64_SWAP_LE_BE (val))
#define GUINT64_TO_BE(val)	(GUINT64_SWAP_LE_BE (val))
#endif
#ifdef __LP64__
#define GLONG_TO_LE(val)	((glong) GINT64_TO_LE (val))
#define GULONG_TO_LE(val)	((gulong) GUINT64_TO_LE (val))
#define GLONG_TO_BE(val)	((glong) GINT64_TO_BE (val))
#define GULONG_TO_BE(val)	((gulong) GUINT64_TO_BE (val))
#else
#define GLONG_TO_LE(val)	((glong) GINT32_TO_LE (val))
#define GULONG_TO_LE(val)	((gulong) GUINT32_TO_LE (val))
#define GLONG_TO_BE(val)	((glong) GINT32_TO_BE (val))
#define GULONG_TO_BE(val)	((gulong) GUINT32_TO_BE (val))
#endif
#define GINT_TO_LE(val)		((gint) GINT32_TO_LE (val))
#define GUINT_TO_LE(val)	((guint) GUINT32_TO_LE (val))
#define GINT_TO_BE(val)		((gint) GINT32_TO_BE (val))
#define GUINT_TO_BE(val)	((guint) GUINT32_TO_BE (val))
#ifdef __BIG_ENDIAN__
#define G_BYTE_ORDER G_BIG_ENDIAN
#else
#define G_BYTE_ORDER G_LITTLE_ENDIAN
#endif

#define GLIB_SYSDEF_POLLIN =1
#define GLIB_SYSDEF_POLLOUT =4
#define GLIB_SYSDEF_POLLPRI =2
#define GLIB_SYSDEF_POLLHUP =16
#define GLIB_SYSDEF_POLLERR =8
#define GLIB_SYSDEF_POLLNVAL =32

#define G_MODULE_SUFFIX "so"

/* A GPid is an abstraction for a process "handle". It is *not* an
 * abstraction for a process identifier in general. GPid is used in
 * GLib only for descendant processes spawned with the g_spawn*
 * functions. On POSIX there is no "process handle" concept as such,
 * but on Windows a GPid is a handle to a process, a kind of pointer,
 * not a process identifier.
 */
typedef int GPid;

#define GLIB_SYSDEF_AF_UNIX 1
#define GLIB_SYSDEF_AF_INET 2
#define GLIB_SYSDEF_AF_INET6 30

#define GLIB_SYSDEF_MSG_OOB 1
#define GLIB_SYSDEF_MSG_PEEK 2
#define GLIB_SYSDEF_MSG_DONTROUTE 4

G_END_DECLS

#endif /* GLIBCONFIG_H */