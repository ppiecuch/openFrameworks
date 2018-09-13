/* Thu Sep 13 14:22:21 CEST 2018 */
#pragma once
#define TARGET_QT
#include "ofMain.h"

#line 0 "../addons-all/ofxLua/libs/lua/luaconf.h"
/*
** $Id: luaconf.h,v 1.259 2016/12/22 13:08:50 roberto Exp $
** Configuration file for Lua
** See Copyright Notice in lua.h
*/


#ifndef luaconf_h
#define luaconf_h

#include <limits.h>
#include <stddef.h>


/*
** ===================================================================
** Search for "@@" to find all configurable definitions.
** ===================================================================
*/


/*
** {====================================================================
** System Configuration: macros to adapt (if needed) Lua to some
** particular platform, for instance compiling it with 32-bit numbers or
** restricting it to C89.
** =====================================================================
*/

/*
@@ LUA_32BITS enables Lua with 32-bit integers and 32-bit floats. You
** can also define LUA_32BITS in the make file, but changing here you
** ensure that all software connected to Lua will be compiled with the
** same configuration.
*/
/* #define LUA_32BITS */


/*
@@ LUA_USE_C89 controls the use of non-ISO-C89 features.
** Define it if you want Lua to avoid the use of a few C99 features
** or Windows-specific features on Windows.
*/
/* #define LUA_USE_C89 */


/*
** By default, Lua on Windows use (some) specific Windows features
*/
#if !defined(LUA_USE_C89) && defined(_WIN32) && !defined(_WIN32_WCE)
#define LUA_USE_WINDOWS  /* enable goodies for regular Windows */
#endif


#if defined(LUA_USE_WINDOWS)
#define LUA_DL_DLL	/* enable support for DLL */
#define LUA_USE_C89	/* broadly, Windows is C89 */
#endif


#if defined(LUA_USE_LINUX)
#define LUA_USE_POSIX
#define LUA_USE_DLOPEN		/* needs an extra library: -ldl */
#define LUA_USE_READLINE	/* needs some extra libraries */
#endif


#if defined(LUA_USE_MACOSX)
#define LUA_USE_POSIX
#define LUA_USE_DLOPEN		/* MacOS does not need -ldl */
#define LUA_USE_READLINE	/* needs an extra library: -lreadline */
#endif


/*
@@ LUA_C89_NUMBERS ensures that Lua uses the largest types available for
** C89 ('long' and 'double'); Windows always has '__int64', so it does
** not need to use this case.
*/
#if defined(LUA_USE_C89) && !defined(LUA_USE_WINDOWS)
#define LUA_C89_NUMBERS
#endif



/*
@@ LUAI_BITSINT defines the (minimum) number of bits in an 'int'.
*/
/* avoid undefined shifts */
#if ((INT_MAX >> 15) >> 15) >= 1
#define LUAI_BITSINT	32
#else
/* 'int' always must have at least 16 bits */
#define LUAI_BITSINT	16
#endif


/*
@@ LUA_INT_TYPE defines the type for Lua integers.
@@ LUA_FLOAT_TYPE defines the type for Lua floats.
** Lua should work fine with any mix of these options (if supported
** by your C compiler). The usual configurations are 64-bit integers
** and 'double' (the default), 32-bit integers and 'float' (for
** restricted platforms), and 'long'/'double' (for C compilers not
** compliant with C99, which may not have support for 'long long').
*/

/* predefined options for LUA_INT_TYPE */
#define LUA_INT_INT		1
#define LUA_INT_LONG		2
#define LUA_INT_LONGLONG	3

/* predefined options for LUA_FLOAT_TYPE */
#define LUA_FLOAT_FLOAT		1
#define LUA_FLOAT_DOUBLE	2
#define LUA_FLOAT_LONGDOUBLE	3

#if defined(LUA_32BITS)		/* { */
/*
** 32-bit integers and 'float'
*/
#if LUAI_BITSINT >= 32  /* use 'int' if big enough */
#define LUA_INT_TYPE	LUA_INT_INT
#else  /* otherwise use 'long' */
#define LUA_INT_TYPE	LUA_INT_LONG
#endif
#define LUA_FLOAT_TYPE	LUA_FLOAT_FLOAT

#elif defined(LUA_C89_NUMBERS)	/* }{ */
/*
** largest types available for C89 ('long' and 'double')
*/
#define LUA_INT_TYPE	LUA_INT_LONG
#define LUA_FLOAT_TYPE	LUA_FLOAT_DOUBLE

#endif				/* } */


/*
** default configuration for 64-bit Lua ('long long' and 'double')
*/
#if !defined(LUA_INT_TYPE)
#define LUA_INT_TYPE	LUA_INT_LONGLONG
#endif

#if !defined(LUA_FLOAT_TYPE)
#define LUA_FLOAT_TYPE	LUA_FLOAT_DOUBLE
#endif

/* }================================================================== */




/*
** {==================================================================
** Configuration for Paths.
** ===================================================================
*/

/*
** LUA_PATH_SEP is the character that separates templates in a path.
** LUA_PATH_MARK is the string that marks the substitution points in a
** template.
** LUA_EXEC_DIR in a Windows path is replaced by the executable's
** directory.
*/
#define LUA_PATH_SEP            ";"
#define LUA_PATH_MARK           "?"
#define LUA_EXEC_DIR            "!"


/*
@@ LUA_PATH_DEFAULT is the default path that Lua uses to look for
** Lua libraries.
@@ LUA_CPATH_DEFAULT is the default path that Lua uses to look for
** C libraries.
** CHANGE them if your machine has a non-conventional directory
** hierarchy or if you want to install your libraries in
** non-conventional directories.
*/
#define LUA_VDIR	LUA_VERSION_MAJOR "." LUA_VERSION_MINOR
#if defined(_WIN32)	/* { */
/*
** In Windows, any exclamation mark ('!') in the path is replaced by the
** path of the directory of the executable file of the current process.
*/
#define LUA_LDIR	"!\\lua\\"
#define LUA_CDIR	"!\\"
#define LUA_SHRDIR	"!\\..\\share\\lua\\" LUA_VDIR "\\"
#define LUA_PATH_DEFAULT  \
		LUA_LDIR"?.lua;"  LUA_LDIR"?\\init.lua;" \
		LUA_CDIR"?.lua;"  LUA_CDIR"?\\init.lua;" \
		LUA_SHRDIR"?.lua;" LUA_SHRDIR"?\\init.lua;" \
		".\\?.lua;" ".\\?\\init.lua"
#define LUA_CPATH_DEFAULT \
		LUA_CDIR"?.dll;" \
		LUA_CDIR"..\\lib\\lua\\" LUA_VDIR "\\?.dll;" \
		LUA_CDIR"loadall.dll;" ".\\?.dll"

#else			/* }{ */

#define LUA_ROOT	"/usr/local/"
#define LUA_LDIR	LUA_ROOT "share/lua/" LUA_VDIR "/"
#define LUA_CDIR	LUA_ROOT "lib/lua/" LUA_VDIR "/"
#define LUA_PATH_DEFAULT  \
		LUA_LDIR"?.lua;"  LUA_LDIR"?/init.lua;" \
		LUA_CDIR"?.lua;"  LUA_CDIR"?/init.lua;" \
		"./?.lua;" "./?/init.lua"
#define LUA_CPATH_DEFAULT \
		LUA_CDIR"?.so;" LUA_CDIR"loadall.so;" "./?.so"
#endif			/* } */


/*
@@ LUA_DIRSEP is the directory separator (for submodules).
** CHANGE it if your machine does not use "/" as the directory separator
** and is not Windows. (On Windows Lua automatically uses "\".)
*/
#if defined(_WIN32)
#define LUA_DIRSEP	"\\"
#else
#define LUA_DIRSEP	"/"
#endif

/* }================================================================== */


/*
** {==================================================================
** Marks for exported symbols in the C code
** ===================================================================
*/

/*
@@ LUA_API is a mark for all core API functions.
@@ LUALIB_API is a mark for all auxiliary library functions.
@@ LUAMOD_API is a mark for all standard library opening functions.
** CHANGE them if you need to define those functions in some special way.
** For instance, if you want to create one Windows DLL with the core and
** the libraries, you may want to use the following definition (define
** LUA_BUILD_AS_DLL to get it).
*/
#if defined(LUA_BUILD_AS_DLL)	/* { */

#if defined(LUA_CORE) || defined(LUA_LIB)	/* { */
#define LUA_API __declspec(dllexport)
#else						/* }{ */
#define LUA_API __declspec(dllimport)
#endif						/* } */

#else				/* }{ */

#define LUA_API		extern

#endif				/* } */


/* more often than not the libs go together with the core */
#define LUALIB_API	LUA_API
#define LUAMOD_API	LUALIB_API


/*
@@ LUAI_FUNC is a mark for all extern functions that are not to be
** exported to outside modules.
@@ LUAI_DDEF and LUAI_DDEC are marks for all extern (const) variables
** that are not to be exported to outside modules (LUAI_DDEF for
** definitions and LUAI_DDEC for declarations).
** CHANGE them if you need to mark them in some special way. Elf/gcc
** (versions 3.2 and later) mark them as "hidden" to optimize access
** when Lua is compiled as a shared library. Not all elf targets support
** this attribute. Unfortunately, gcc does not offer a way to check
** whether the target offers that support, and those without support
** give a warning about it. To avoid these warnings, change to the
** default definition.
*/
#if defined(__GNUC__) && ((__GNUC__*100 + __GNUC_MINOR__) >= 302) && \
    defined(__ELF__)		/* { */
#define LUAI_FUNC	__attribute__((visibility("hidden"))) extern
#else				/* }{ */
#define LUAI_FUNC	extern
#endif				/* } */

#define LUAI_DDEC	LUAI_FUNC
#define LUAI_DDEF	/* empty */

/* }================================================================== */


/*
** {==================================================================
** Compatibility with previous versions
** ===================================================================
*/

/*
@@ LUA_COMPAT_5_2 controls other macros for compatibility with Lua 5.2.
@@ LUA_COMPAT_5_1 controls other macros for compatibility with Lua 5.1.
** You can define it to get all options, or change specific options
** to fit your specific needs.
*/
#if defined(LUA_COMPAT_5_2)	/* { */

/*
@@ LUA_COMPAT_MATHLIB controls the presence of several deprecated
** functions in the mathematical library.
*/
#define LUA_COMPAT_MATHLIB

/*
@@ LUA_COMPAT_BITLIB controls the presence of library 'bit32'.
*/
#define LUA_COMPAT_BITLIB

/*
@@ LUA_COMPAT_IPAIRS controls the effectiveness of the __ipairs metamethod.
*/
#define LUA_COMPAT_IPAIRS

/*
@@ LUA_COMPAT_APIINTCASTS controls the presence of macros for
** manipulating other integer types (lua_pushunsigned, lua_tounsigned,
** luaL_checkint, luaL_checklong, etc.)
*/
#define LUA_COMPAT_APIINTCASTS

#endif				/* } */


#if defined(LUA_COMPAT_5_1)	/* { */

/* Incompatibilities from 5.2 -> 5.3 */
#define LUA_COMPAT_MATHLIB
#define LUA_COMPAT_APIINTCASTS

/*
@@ LUA_COMPAT_UNPACK controls the presence of global 'unpack'.
** You can replace it with 'table.unpack'.
*/
#define LUA_COMPAT_UNPACK

/*
@@ LUA_COMPAT_LOADERS controls the presence of table 'package.loaders'.
** You can replace it with 'package.searchers'.
*/
#define LUA_COMPAT_LOADERS

/*
@@ macro 'lua_cpcall' emulates deprecated function lua_cpcall.
** You can call your C function directly (with light C functions).
*/
#define lua_cpcall(L,f,u)  \
	(lua_pushcfunction(L, (f)), \
	 lua_pushlightuserdata(L,(u)), \
	 lua_pcall(L,1,0,0))


/*
@@ LUA_COMPAT_LOG10 defines the function 'log10' in the math library.
** You can rewrite 'log10(x)' as 'log(x, 10)'.
*/
#define LUA_COMPAT_LOG10

/*
@@ LUA_COMPAT_LOADSTRING defines the function 'loadstring' in the base
** library. You can rewrite 'loadstring(s)' as 'load(s)'.
*/
#define LUA_COMPAT_LOADSTRING

/*
@@ LUA_COMPAT_MAXN defines the function 'maxn' in the table library.
*/
#define LUA_COMPAT_MAXN

/*
@@ The following macros supply trivial compatibility for some
** changes in the API. The macros themselves document how to
** change your code to avoid using them.
*/
#define lua_strlen(L,i)		lua_rawlen(L, (i))

#define lua_objlen(L,i)		lua_rawlen(L, (i))

#define lua_equal(L,idx1,idx2)		lua_compare(L,(idx1),(idx2),LUA_OPEQ)
#define lua_lessthan(L,idx1,idx2)	lua_compare(L,(idx1),(idx2),LUA_OPLT)

/*
@@ LUA_COMPAT_MODULE controls compatibility with previous
** module functions 'module' (Lua) and 'luaL_register' (C).
*/
#define LUA_COMPAT_MODULE

#endif				/* } */


/*
@@ LUA_COMPAT_FLOATSTRING makes Lua format integral floats without a
@@ a float mark ('.0').
** This macro is not on by default even in compatibility mode,
** because this is not really an incompatibility.
*/
/* #define LUA_COMPAT_FLOATSTRING */

/* }================================================================== */



/*
** {==================================================================
** Configuration for Numbers.
** Change these definitions if no predefined LUA_FLOAT_* / LUA_INT_*
** satisfy your needs.
** ===================================================================
*/

/*
@@ LUA_NUMBER is the floating-point type used by Lua.
@@ LUAI_UACNUMBER is the result of a 'default argument promotion'
@@ over a floating number.
@@ l_mathlim(x) corrects limit name 'x' to the proper float type
** by prefixing it with one of FLT/DBL/LDBL.
@@ LUA_NUMBER_FRMLEN is the length modifier for writing floats.
@@ LUA_NUMBER_FMT is the format for writing floats.
@@ lua_number2str converts a float to a string.
@@ l_mathop allows the addition of an 'l' or 'f' to all math operations.
@@ l_floor takes the floor of a float.
@@ lua_str2number converts a decimal numeric string to a number.
*/


/* The following definitions are good for most cases here */

#define l_floor(x)		(l_mathop(floor)(x))

#define lua_number2str(s,sz,n)  \
	l_sprintf((s), sz, LUA_NUMBER_FMT, (LUAI_UACNUMBER)(n))

/*
@@ lua_numbertointeger converts a float number to an integer, or
** returns 0 if float is not within the range of a lua_Integer.
** (The range comparisons are tricky because of rounding. The tests
** here assume a two-complement representation, where MININTEGER always
** has an exact representation as a float; MAXINTEGER may not have one,
** and therefore its conversion to float may have an ill-defined value.)
*/
#define lua_numbertointeger(n,p) \
  ((n) >= (LUA_NUMBER)(LUA_MININTEGER) && \
   (n) < -(LUA_NUMBER)(LUA_MININTEGER) && \
      (*(p) = (LUA_INTEGER)(n), 1))


/* now the variable definitions */

#if LUA_FLOAT_TYPE == LUA_FLOAT_FLOAT		/* { single float */

#define LUA_NUMBER	float

#define l_mathlim(n)		(FLT_##n)

#define LUAI_UACNUMBER	double

#define LUA_NUMBER_FRMLEN	""
#define LUA_NUMBER_FMT		"%.7g"

#define l_mathop(op)		op##f

#define lua_str2number(s,p)	strtof((s), (p))


#elif LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE	/* }{ long double */

#define LUA_NUMBER	long double

#define l_mathlim(n)		(LDBL_##n)

#define LUAI_UACNUMBER	long double

#define LUA_NUMBER_FRMLEN	"L"
#define LUA_NUMBER_FMT		"%.19Lg"

#define l_mathop(op)		op##l

#define lua_str2number(s,p)	strtold((s), (p))

#elif LUA_FLOAT_TYPE == LUA_FLOAT_DOUBLE	/* }{ double */

#define LUA_NUMBER	double

#define l_mathlim(n)		(DBL_##n)

#define LUAI_UACNUMBER	double

#define LUA_NUMBER_FRMLEN	""
#define LUA_NUMBER_FMT		"%.14g"

#define l_mathop(op)		op

#define lua_str2number(s,p)	strtod((s), (p))

#else						/* }{ */

#error "numeric float type not defined"

#endif					/* } */



/*
@@ LUA_INTEGER is the integer type used by Lua.
**
@@ LUA_UNSIGNED is the unsigned version of LUA_INTEGER.
**
@@ LUAI_UACINT is the result of a 'default argument promotion'
@@ over a lUA_INTEGER.
@@ LUA_INTEGER_FRMLEN is the length modifier for reading/writing integers.
@@ LUA_INTEGER_FMT is the format for writing integers.
@@ LUA_MAXINTEGER is the maximum value for a LUA_INTEGER.
@@ LUA_MININTEGER is the minimum value for a LUA_INTEGER.
@@ lua_integer2str converts an integer to a string.
*/


/* The following definitions are good for most cases here */

#define LUA_INTEGER_FMT		"%" LUA_INTEGER_FRMLEN "d"

#define LUAI_UACINT		LUA_INTEGER

#define lua_integer2str(s,sz,n)  \
	l_sprintf((s), sz, LUA_INTEGER_FMT, (LUAI_UACINT)(n))

/*
** use LUAI_UACINT here to avoid problems with promotions (which
** can turn a comparison between unsigneds into a signed comparison)
*/
#define LUA_UNSIGNED		unsigned LUAI_UACINT


/* now the variable definitions */

#if LUA_INT_TYPE == LUA_INT_INT		/* { int */

#define LUA_INTEGER		int
#define LUA_INTEGER_FRMLEN	""

#define LUA_MAXINTEGER		INT_MAX
#define LUA_MININTEGER		INT_MIN

#elif LUA_INT_TYPE == LUA_INT_LONG	/* }{ long */

#define LUA_INTEGER		long
#define LUA_INTEGER_FRMLEN	"l"

#define LUA_MAXINTEGER		LONG_MAX
#define LUA_MININTEGER		LONG_MIN

#elif LUA_INT_TYPE == LUA_INT_LONGLONG	/* }{ long long */

/* use presence of macro LLONG_MAX as proxy for C99 compliance */
#if defined(LLONG_MAX)		/* { */
/* use ISO C99 stuff */

#define LUA_INTEGER		long long
#define LUA_INTEGER_FRMLEN	"ll"

#define LUA_MAXINTEGER		LLONG_MAX
#define LUA_MININTEGER		LLONG_MIN

#elif defined(LUA_USE_WINDOWS) /* }{ */
/* in Windows, can use specific Windows types */

#define LUA_INTEGER		__int64
#define LUA_INTEGER_FRMLEN	"I64"

#define LUA_MAXINTEGER		_I64_MAX
#define LUA_MININTEGER		_I64_MIN

#else				/* }{ */

#error "Compiler does not support 'long long'. Use option '-DLUA_32BITS' \
  or '-DLUA_C89_NUMBERS' (see file 'luaconf.h' for details)"

#endif				/* } */

#else				/* }{ */

#error "numeric integer type not defined"

#endif				/* } */

/* }================================================================== */


/*
** {==================================================================
** Dependencies with C99 and other C details
** ===================================================================
*/

/*
@@ l_sprintf is equivalent to 'snprintf' or 'sprintf' in C89.
** (All uses in Lua have only one format item.)
*/
#if !defined(LUA_USE_C89)
#define l_sprintf(s,sz,f,i)	snprintf(s,sz,f,i)
#else
#define l_sprintf(s,sz,f,i)	((void)(sz), sprintf(s,f,i))
#endif


/*
@@ lua_strx2number converts an hexadecimal numeric string to a number.
** In C99, 'strtod' does that conversion. Otherwise, you can
** leave 'lua_strx2number' undefined and Lua will provide its own
** implementation.
*/
#if !defined(LUA_USE_C89)
#define lua_strx2number(s,p)		lua_str2number(s,p)
#endif


/*
@@ lua_number2strx converts a float to an hexadecimal numeric string.
** In C99, 'sprintf' (with format specifiers '%a'/'%A') does that.
** Otherwise, you can leave 'lua_number2strx' undefined and Lua will
** provide its own implementation.
*/
#if !defined(LUA_USE_C89)
#define lua_number2strx(L,b,sz,f,n)  \
	((void)L, l_sprintf(b,sz,f,(LUAI_UACNUMBER)(n)))
#endif


/*
** 'strtof' and 'opf' variants for math functions are not valid in
** C89. Otherwise, the macro 'HUGE_VALF' is a good proxy for testing the
** availability of these variants. ('math.h' is already included in
** all files that use these macros.)
*/
#if defined(LUA_USE_C89) || (defined(HUGE_VAL) && !defined(HUGE_VALF))
#undef l_mathop  /* variants not available */
#undef lua_str2number
#define l_mathop(op)		(lua_Number)op  /* no variant */
#define lua_str2number(s,p)	((lua_Number)strtod((s), (p)))
#endif


/*
@@ LUA_KCONTEXT is the type of the context ('ctx') for continuation
** functions.  It must be a numerical type; Lua will use 'intptr_t' if
** available, otherwise it will use 'ptrdiff_t' (the nearest thing to
** 'intptr_t' in C89)
*/
#define LUA_KCONTEXT	ptrdiff_t

#if !defined(LUA_USE_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
#include <stdint.h>
#if defined(INTPTR_MAX)  /* even in C99 this type is optional */
#undef LUA_KCONTEXT
#define LUA_KCONTEXT	intptr_t
#endif
#endif


/*
@@ lua_getlocaledecpoint gets the locale "radix character" (decimal point).
** Change that if you do not want to use C locales. (Code using this
** macro must include header 'locale.h'.)
*/
#if !defined(lua_getlocaledecpoint)
#define lua_getlocaledecpoint()		(localeconv()->decimal_point[0])
#endif

/* }================================================================== */


/*
** {==================================================================
** Language Variations
** =====================================================================
*/

/*
@@ LUA_NOCVTN2S/LUA_NOCVTS2N control how Lua performs some
** coercions. Define LUA_NOCVTN2S to turn off automatic coercion from
** numbers to strings. Define LUA_NOCVTS2N to turn off automatic
** coercion from strings to numbers.
*/
/* #define LUA_NOCVTN2S */
/* #define LUA_NOCVTS2N */


/*
@@ LUA_USE_APICHECK turns on several consistency checks on the C API.
** Define it as a help when debugging C code.
*/
#if defined(LUA_USE_APICHECK)
#include <assert.h>
#define luai_apicheck(l,e)	assert(e)
#endif

/* }================================================================== */


/*
** {==================================================================
** Macros that affect the API and must be stable (that is, must be the
** same when you compile Lua and when you compile code that links to
** Lua). You probably do not want/need to change them.
** =====================================================================
*/

/*
@@ LUAI_MAXSTACK limits the size of the Lua stack.
** CHANGE it if you need a different limit. This limit is arbitrary;
** its only purpose is to stop Lua from consuming unlimited stack
** space (and to reserve some numbers for pseudo-indices).
*/
#if LUAI_BITSINT >= 32
#define LUAI_MAXSTACK		1000000
#else
#define LUAI_MAXSTACK		15000
#endif


/*
@@ LUA_EXTRASPACE defines the size of a raw memory area associated with
** a Lua state with very fast access.
** CHANGE it if you need a different size.
*/
#define LUA_EXTRASPACE		(sizeof(void *))


/*
@@ LUA_IDSIZE gives the maximum size for the description of the source
@@ of a function in debug information.
** CHANGE it if you want a different size.
*/
#define LUA_IDSIZE	60


/*
@@ LUAL_BUFFERSIZE is the buffer size used by the lauxlib buffer system.
** CHANGE it if it uses too much C-stack space. (For long double,
** 'string.format("%.99f", -1e4932)' needs 5034 bytes, so a
** smaller buffer would force a memory allocation for each call to
** 'string.format'.)
*/
#if LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE
#define LUAL_BUFFERSIZE		8192
#else
#define LUAL_BUFFERSIZE   ((int)(0x80 * sizeof(void*) * sizeof(lua_Integer)))
#endif

/* }================================================================== */


/*
@@ LUA_QL describes how error messages quote program elements.
** Lua does not use these macros anymore; they are here for
** compatibility only.
*/
#define LUA_QL(x)	"'" x "'"
#define LUA_QS		LUA_QL("%s")




/* =================================================================== */

/*
** Local configuration. You can use this space to add your redefinitions
** without modifying the main part of the file.
*/





#endif


#line 0 "../addons-all/ofxLua/libs/lua/lua.hpp"
// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

extern "C" {
// inline lua.h
/*
** $Id: lua.h,v 1.332 2016/12/22 15:51:20 roberto Exp $
** Lua - A Scripting Language
** Lua.org, PUC-Rio, Brazil (http://www.lua.org)
** See Copyright Notice at the end of this file
*/


#ifndef lua_h
#define lua_h

#include <stdarg.h>
#include <stddef.h>


/* #include "luaconf.h" */


#define LUA_VERSION_MAJOR	"5"
#define LUA_VERSION_MINOR	"3"
#define LUA_VERSION_NUM		503
#define LUA_VERSION_RELEASE	"4"

#define LUA_VERSION	"Lua " LUA_VERSION_MAJOR "." LUA_VERSION_MINOR
#define LUA_RELEASE	LUA_VERSION "." LUA_VERSION_RELEASE
#define LUA_COPYRIGHT	LUA_RELEASE "  Copyright (C) 1994-2017 Lua.org, PUC-Rio"
#define LUA_AUTHORS	"R. Ierusalimschy, L. H. de Figueiredo, W. Celes"


/* mark for precompiled code ('<esc>Lua') */
#define LUA_SIGNATURE	"\x1bLua"

/* option for multiple returns in 'lua_pcall' and 'lua_call' */
#define LUA_MULTRET	(-1)


/*
** Pseudo-indices
** (-LUAI_MAXSTACK is the minimum valid index; we keep some free empty
** space after that to help overflow detection)
*/
#define LUA_REGISTRYINDEX	(-LUAI_MAXSTACK - 1000)
#define lua_upvalueindex(i)	(LUA_REGISTRYINDEX - (i))


/* thread status */
#define LUA_OK		0
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRGCMM	5
#define LUA_ERRERR	6


typedef struct lua_State lua_State;


/*
** basic types
*/
#define LUA_TNONE		(-1)

#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8

#define LUA_NUMTAGS		9



/* minimum Lua stack available to a C function */
#define LUA_MINSTACK	20


/* predefined values in the registry */
#define LUA_RIDX_MAINTHREAD	1
#define LUA_RIDX_GLOBALS	2
#define LUA_RIDX_LAST		LUA_RIDX_GLOBALS


/* type of numbers in Lua */
typedef LUA_NUMBER lua_Number;


/* type for integer functions */
typedef LUA_INTEGER lua_Integer;

/* unsigned integer type */
typedef LUA_UNSIGNED lua_Unsigned;

/* type for continuation-function contexts */
typedef LUA_KCONTEXT lua_KContext;


/*
** Type for C functions registered with Lua
*/
typedef int (*lua_CFunction) (lua_State *L);

/*
** Type for continuation functions
*/
typedef int (*lua_KFunction) (lua_State *L, int status, lua_KContext ctx);


/*
** Type for functions that read/write blocks when loading/dumping Lua chunks
*/
typedef const char * (*lua_Reader) (lua_State *L, void *ud, size_t *sz);

typedef int (*lua_Writer) (lua_State *L, const void *p, size_t sz, void *ud);


/*
** Type for memory-allocation functions
*/
typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);



/*
** generic extra include file
*/
#if defined(LUA_USER_H)
#include LUA_USER_H
#endif


/*
** RCS ident string
*/
extern const char lua_ident[];


/*
** state manipulation
*/
LUA_API lua_State *(lua_newstate) (lua_Alloc f, void *ud);
LUA_API void       (lua_close) (lua_State *L);
LUA_API lua_State *(lua_newthread) (lua_State *L);

LUA_API lua_CFunction (lua_atpanic) (lua_State *L, lua_CFunction panicf);


LUA_API const lua_Number *(lua_version) (lua_State *L);


/*
** basic stack manipulation
*/
LUA_API int   (lua_absindex) (lua_State *L, int idx);
LUA_API int   (lua_gettop) (lua_State *L);
LUA_API void  (lua_settop) (lua_State *L, int idx);
LUA_API void  (lua_pushvalue) (lua_State *L, int idx);
LUA_API void  (lua_rotate) (lua_State *L, int idx, int n);
LUA_API void  (lua_copy) (lua_State *L, int fromidx, int toidx);
LUA_API int   (lua_checkstack) (lua_State *L, int n);

LUA_API void  (lua_xmove) (lua_State *from, lua_State *to, int n);


/*
** access functions (stack -> C)
*/

LUA_API int             (lua_isnumber) (lua_State *L, int idx);
LUA_API int             (lua_isstring) (lua_State *L, int idx);
LUA_API int             (lua_iscfunction) (lua_State *L, int idx);
LUA_API int             (lua_isinteger) (lua_State *L, int idx);
LUA_API int             (lua_isuserdata) (lua_State *L, int idx);
LUA_API int             (lua_type) (lua_State *L, int idx);
LUA_API const char     *(lua_typename) (lua_State *L, int tp);

LUA_API lua_Number      (lua_tonumberx) (lua_State *L, int idx, int *isnum);
LUA_API lua_Integer     (lua_tointegerx) (lua_State *L, int idx, int *isnum);
LUA_API int             (lua_toboolean) (lua_State *L, int idx);
LUA_API const char     *(lua_tolstring) (lua_State *L, int idx, size_t *len);
LUA_API size_t          (lua_rawlen) (lua_State *L, int idx);
LUA_API lua_CFunction   (lua_tocfunction) (lua_State *L, int idx);
LUA_API void	       *(lua_touserdata) (lua_State *L, int idx);
LUA_API lua_State      *(lua_tothread) (lua_State *L, int idx);
LUA_API const void     *(lua_topointer) (lua_State *L, int idx);


/*
** Comparison and arithmetic functions
*/

#define LUA_OPADD	0	/* ORDER TM, ORDER OP */
#define LUA_OPSUB	1
#define LUA_OPMUL	2
#define LUA_OPMOD	3
#define LUA_OPPOW	4
#define LUA_OPDIV	5
#define LUA_OPIDIV	6
#define LUA_OPBAND	7
#define LUA_OPBOR	8
#define LUA_OPBXOR	9
#define LUA_OPSHL	10
#define LUA_OPSHR	11
#define LUA_OPUNM	12
#define LUA_OPBNOT	13

LUA_API void  (lua_arith) (lua_State *L, int op);

#define LUA_OPEQ	0
#define LUA_OPLT	1
#define LUA_OPLE	2

LUA_API int   (lua_rawequal) (lua_State *L, int idx1, int idx2);
LUA_API int   (lua_compare) (lua_State *L, int idx1, int idx2, int op);


/*
** push functions (C -> stack)
*/
LUA_API void        (lua_pushnil) (lua_State *L);
LUA_API void        (lua_pushnumber) (lua_State *L, lua_Number n);
LUA_API void        (lua_pushinteger) (lua_State *L, lua_Integer n);
LUA_API const char *(lua_pushlstring) (lua_State *L, const char *s, size_t len);
LUA_API const char *(lua_pushstring) (lua_State *L, const char *s);
LUA_API const char *(lua_pushvfstring) (lua_State *L, const char *fmt,
                                                      va_list argp);
LUA_API const char *(lua_pushfstring) (lua_State *L, const char *fmt, ...);
LUA_API void  (lua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
LUA_API void  (lua_pushboolean) (lua_State *L, int b);
LUA_API void  (lua_pushlightuserdata) (lua_State *L, void *p);
LUA_API int   (lua_pushthread) (lua_State *L);


/*
** get functions (Lua -> stack)
*/
LUA_API int (lua_getglobal) (lua_State *L, const char *name);
LUA_API int (lua_gettable) (lua_State *L, int idx);
LUA_API int (lua_getfield) (lua_State *L, int idx, const char *k);
LUA_API int (lua_geti) (lua_State *L, int idx, lua_Integer n);
LUA_API int (lua_rawget) (lua_State *L, int idx);
LUA_API int (lua_rawgeti) (lua_State *L, int idx, lua_Integer n);
LUA_API int (lua_rawgetp) (lua_State *L, int idx, const void *p);

LUA_API void  (lua_createtable) (lua_State *L, int narr, int nrec);
LUA_API void *(lua_newuserdata) (lua_State *L, size_t sz);
LUA_API int   (lua_getmetatable) (lua_State *L, int objindex);
LUA_API int  (lua_getuservalue) (lua_State *L, int idx);


/*
** set functions (stack -> Lua)
*/
LUA_API void  (lua_setglobal) (lua_State *L, const char *name);
LUA_API void  (lua_settable) (lua_State *L, int idx);
LUA_API void  (lua_setfield) (lua_State *L, int idx, const char *k);
LUA_API void  (lua_seti) (lua_State *L, int idx, lua_Integer n);
LUA_API void  (lua_rawset) (lua_State *L, int idx);
LUA_API void  (lua_rawseti) (lua_State *L, int idx, lua_Integer n);
LUA_API void  (lua_rawsetp) (lua_State *L, int idx, const void *p);
LUA_API int   (lua_setmetatable) (lua_State *L, int objindex);
LUA_API void  (lua_setuservalue) (lua_State *L, int idx);


/*
** 'load' and 'call' functions (load and run Lua code)
*/
LUA_API void  (lua_callk) (lua_State *L, int nargs, int nresults,
                           lua_KContext ctx, lua_KFunction k);
#define lua_call(L,n,r)		lua_callk(L, (n), (r), 0, NULL)

LUA_API int   (lua_pcallk) (lua_State *L, int nargs, int nresults, int errfunc,
                            lua_KContext ctx, lua_KFunction k);
#define lua_pcall(L,n,r,f)	lua_pcallk(L, (n), (r), (f), 0, NULL)

LUA_API int   (lua_load) (lua_State *L, lua_Reader reader, void *dt,
                          const char *chunkname, const char *mode);

LUA_API int (lua_dump) (lua_State *L, lua_Writer writer, void *data, int strip);


/*
** coroutine functions
*/
LUA_API int  (lua_yieldk)     (lua_State *L, int nresults, lua_KContext ctx,
                               lua_KFunction k);
LUA_API int  (lua_resume)     (lua_State *L, lua_State *from, int narg);
LUA_API int  (lua_status)     (lua_State *L);
LUA_API int (lua_isyieldable) (lua_State *L);

#define lua_yield(L,n)		lua_yieldk(L, (n), 0, NULL)


/*
** garbage-collection function and options
*/

#define LUA_GCSTOP		0
#define LUA_GCRESTART		1
#define LUA_GCCOLLECT		2
#define LUA_GCCOUNT		3
#define LUA_GCCOUNTB		4
#define LUA_GCSTEP		5
#define LUA_GCSETPAUSE		6
#define LUA_GCSETSTEPMUL	7
#define LUA_GCISRUNNING		9

LUA_API int (lua_gc) (lua_State *L, int what, int data);


/*
** miscellaneous functions
*/

LUA_API int   (lua_error) (lua_State *L);

LUA_API int   (lua_next) (lua_State *L, int idx);

LUA_API void  (lua_concat) (lua_State *L, int n);
LUA_API void  (lua_len)    (lua_State *L, int idx);

LUA_API size_t   (lua_stringtonumber) (lua_State *L, const char *s);

LUA_API lua_Alloc (lua_getallocf) (lua_State *L, void **ud);
LUA_API void      (lua_setallocf) (lua_State *L, lua_Alloc f, void *ud);



/*
** {==============================================================
** some useful macros
** ===============================================================
*/

#define lua_getextraspace(L)	((void *)((char *)(L) - LUA_EXTRASPACE))

#define lua_tonumber(L,i)	lua_tonumberx(L,(i),NULL)
#define lua_tointeger(L,i)	lua_tointegerx(L,(i),NULL)

#define lua_pop(L,n)		lua_settop(L, -(n)-1)

#define lua_newtable(L)		lua_createtable(L, 0, 0)

#define lua_register(L,n,f) (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))

#define lua_pushcfunction(L,f)	lua_pushcclosure(L, (f), 0)

#define lua_isfunction(L,n)	(lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L,n)	(lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)		(lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L,n)	(lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L,n)	(lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n)		(lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n)	(lua_type(L, (n)) <= 0)

#define lua_pushliteral(L, s)	lua_pushstring(L, "" s)

#define lua_pushglobaltable(L)  \
	((void)lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS))

#define lua_tostring(L,i)	lua_tolstring(L, (i), NULL)


#define lua_insert(L,idx)	lua_rotate(L, (idx), 1)

#define lua_remove(L,idx)	(lua_rotate(L, (idx), -1), lua_pop(L, 1))

#define lua_replace(L,idx)	(lua_copy(L, -1, (idx)), lua_pop(L, 1))

/* }============================================================== */


/*
** {==============================================================
** compatibility macros for unsigned conversions
** ===============================================================
*/
#if defined(LUA_COMPAT_APIINTCASTS)

#define lua_pushunsigned(L,n)	lua_pushinteger(L, (lua_Integer)(n))
#define lua_tounsignedx(L,i,is)	((lua_Unsigned)lua_tointegerx(L,i,is))
#define lua_tounsigned(L,i)	lua_tounsignedx(L,(i),NULL)

#endif
/* }============================================================== */

/*
** {======================================================================
** Debug API
** =======================================================================
*/


/*
** Event codes
*/
#define LUA_HOOKCALL	0
#define LUA_HOOKRET	1
#define LUA_HOOKLINE	2
#define LUA_HOOKCOUNT	3
#define LUA_HOOKTAILCALL 4


/*
** Event masks
*/
#define LUA_MASKCALL	(1 << LUA_HOOKCALL)
#define LUA_MASKRET	(1 << LUA_HOOKRET)
#define LUA_MASKLINE	(1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT	(1 << LUA_HOOKCOUNT)

typedef struct lua_Debug lua_Debug;  /* activation record */


/* Functions to be called by the debugger in specific events */
typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);


LUA_API int (lua_getstack) (lua_State *L, int level, lua_Debug *ar);
LUA_API int (lua_getinfo) (lua_State *L, const char *what, lua_Debug *ar);
LUA_API const char *(lua_getlocal) (lua_State *L, const lua_Debug *ar, int n);
LUA_API const char *(lua_setlocal) (lua_State *L, const lua_Debug *ar, int n);
LUA_API const char *(lua_getupvalue) (lua_State *L, int funcindex, int n);
LUA_API const char *(lua_setupvalue) (lua_State *L, int funcindex, int n);

LUA_API void *(lua_upvalueid) (lua_State *L, int fidx, int n);
LUA_API void  (lua_upvaluejoin) (lua_State *L, int fidx1, int n1,
                                               int fidx2, int n2);

LUA_API void (lua_sethook) (lua_State *L, lua_Hook func, int mask, int count);
LUA_API lua_Hook (lua_gethook) (lua_State *L);
LUA_API int (lua_gethookmask) (lua_State *L);
LUA_API int (lua_gethookcount) (lua_State *L);


struct lua_Debug {
  int event;
  const char *name;	/* (n) */
  const char *namewhat;	/* (n) 'global', 'local', 'field', 'method' */
  const char *what;	/* (S) 'Lua', 'C', 'main', 'tail' */
  const char *source;	/* (S) */
  int currentline;	/* (l) */
  int linedefined;	/* (S) */
  int lastlinedefined;	/* (S) */
  unsigned char nups;	/* (u) number of upvalues */
  unsigned char nparams;/* (u) number of parameters */
  char isvararg;        /* (u) */
  char istailcall;	/* (t) */
  char short_src[LUA_IDSIZE]; /* (S) */
  /* private part */
  struct CallInfo *i_ci;  /* active function */
};

/* }====================================================================== */


/******************************************************************************
* Copyright (C) 1994-2017 Lua.org, PUC-Rio.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/


#endif
// inline lualib.h
/*
** $Id: lualib.h,v 1.45 2017/01/12 17:14:26 roberto Exp $
** Lua standard libraries
** See Copyright Notice in lua.h
*/


#ifndef lualib_h
#define lualib_h

/* #include "lua.h" */


/* version suffix for environment variable names */
#define LUA_VERSUFFIX          "_" LUA_VERSION_MAJOR "_" LUA_VERSION_MINOR


LUAMOD_API int (luaopen_base) (lua_State *L);

#define LUA_COLIBNAME	"coroutine"
LUAMOD_API int (luaopen_coroutine) (lua_State *L);

#define LUA_TABLIBNAME	"table"
LUAMOD_API int (luaopen_table) (lua_State *L);

#define LUA_IOLIBNAME	"io"
LUAMOD_API int (luaopen_io) (lua_State *L);

#define LUA_OSLIBNAME	"os"
LUAMOD_API int (luaopen_os) (lua_State *L);

#define LUA_STRLIBNAME	"string"
LUAMOD_API int (luaopen_string) (lua_State *L);

#define LUA_UTF8LIBNAME	"utf8"
LUAMOD_API int (luaopen_utf8) (lua_State *L);

#define LUA_BITLIBNAME	"bit32"
LUAMOD_API int (luaopen_bit32) (lua_State *L);

#define LUA_MATHLIBNAME	"math"
LUAMOD_API int (luaopen_math) (lua_State *L);

#define LUA_DBLIBNAME	"debug"
LUAMOD_API int (luaopen_debug) (lua_State *L);

#define LUA_LOADLIBNAME	"package"
LUAMOD_API int (luaopen_package) (lua_State *L);


/* open all previous libraries */
LUALIB_API void (luaL_openlibs) (lua_State *L);



#if !defined(lua_assert)
#define lua_assert(x)	((void)0)
#endif


#endif
// inline lauxlib.h
/*
** $Id: lauxlib.h,v 1.131 2016/12/06 14:54:31 roberto Exp $
** Auxiliary functions for building Lua libraries
** See Copyright Notice in lua.h
*/


#ifndef lauxlib_h
#define lauxlib_h


#include <stddef.h>
#include <stdio.h>

/* #include "lua.h" */



/* extra error code for 'luaL_loadfilex' */
#define LUA_ERRFILE     (LUA_ERRERR+1)


/* key, in the registry, for table of loaded modules */
#define LUA_LOADED_TABLE	"_LOADED"


/* key, in the registry, for table of preloaded loaders */
#define LUA_PRELOAD_TABLE	"_PRELOAD"


typedef struct luaL_Reg {
  const char *name;
  lua_CFunction func;
} luaL_Reg;


#define LUAL_NUMSIZES	(sizeof(lua_Integer)*16 + sizeof(lua_Number))

LUALIB_API void (luaL_checkversion_) (lua_State *L, lua_Number ver, size_t sz);
#define luaL_checkversion(L)  \
	  luaL_checkversion_(L, LUA_VERSION_NUM, LUAL_NUMSIZES)

LUALIB_API int (luaL_getmetafield) (lua_State *L, int obj, const char *e);
LUALIB_API int (luaL_callmeta) (lua_State *L, int obj, const char *e);
LUALIB_API const char *(luaL_tolstring) (lua_State *L, int idx, size_t *len);
LUALIB_API int (luaL_argerror) (lua_State *L, int arg, const char *extramsg);
LUALIB_API const char *(luaL_checklstring) (lua_State *L, int arg,
                                                          size_t *l);
LUALIB_API const char *(luaL_optlstring) (lua_State *L, int arg,
                                          const char *def, size_t *l);
LUALIB_API lua_Number (luaL_checknumber) (lua_State *L, int arg);
LUALIB_API lua_Number (luaL_optnumber) (lua_State *L, int arg, lua_Number def);

LUALIB_API lua_Integer (luaL_checkinteger) (lua_State *L, int arg);
LUALIB_API lua_Integer (luaL_optinteger) (lua_State *L, int arg,
                                          lua_Integer def);

LUALIB_API void (luaL_checkstack) (lua_State *L, int sz, const char *msg);
LUALIB_API void (luaL_checktype) (lua_State *L, int arg, int t);
LUALIB_API void (luaL_checkany) (lua_State *L, int arg);

LUALIB_API int   (luaL_newmetatable) (lua_State *L, const char *tname);
LUALIB_API void  (luaL_setmetatable) (lua_State *L, const char *tname);
LUALIB_API void *(luaL_testudata) (lua_State *L, int ud, const char *tname);
LUALIB_API void *(luaL_checkudata) (lua_State *L, int ud, const char *tname);

LUALIB_API void (luaL_where) (lua_State *L, int lvl);
LUALIB_API int (luaL_error) (lua_State *L, const char *fmt, ...);

LUALIB_API int (luaL_checkoption) (lua_State *L, int arg, const char *def,
                                   const char *const lst[]);

LUALIB_API int (luaL_fileresult) (lua_State *L, int stat, const char *fname);
LUALIB_API int (luaL_execresult) (lua_State *L, int stat);

/* predefined references */
#define LUA_NOREF       (-2)
#define LUA_REFNIL      (-1)

LUALIB_API int (luaL_ref) (lua_State *L, int t);
LUALIB_API void (luaL_unref) (lua_State *L, int t, int ref);

LUALIB_API int (luaL_loadfilex) (lua_State *L, const char *filename,
                                               const char *mode);

#define luaL_loadfile(L,f)	luaL_loadfilex(L,f,NULL)

LUALIB_API int (luaL_loadbufferx) (lua_State *L, const char *buff, size_t sz,
                                   const char *name, const char *mode);
LUALIB_API int (luaL_loadstring) (lua_State *L, const char *s);

LUALIB_API lua_State *(luaL_newstate) (void);

LUALIB_API lua_Integer (luaL_len) (lua_State *L, int idx);

LUALIB_API const char *(luaL_gsub) (lua_State *L, const char *s, const char *p,
                                                  const char *r);

LUALIB_API void (luaL_setfuncs) (lua_State *L, const luaL_Reg *l, int nup);

LUALIB_API int (luaL_getsubtable) (lua_State *L, int idx, const char *fname);

LUALIB_API void (luaL_traceback) (lua_State *L, lua_State *L1,
                                  const char *msg, int level);

LUALIB_API void (luaL_requiref) (lua_State *L, const char *modname,
                                 lua_CFunction openf, int glb);

/*
** ===============================================================
** some useful macros
** ===============================================================
*/


#define luaL_newlibtable(L,l)	\
  lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1)

#define luaL_newlib(L,l)  \
  (luaL_checkversion(L), luaL_newlibtable(L,l), luaL_setfuncs(L,l,0))

#define luaL_argcheck(L, cond,arg,extramsg)	\
		((void)((cond) || luaL_argerror(L, (arg), (extramsg))))
#define luaL_checkstring(L,n)	(luaL_checklstring(L, (n), NULL))
#define luaL_optstring(L,n,d)	(luaL_optlstring(L, (n), (d), NULL))

#define luaL_typename(L,i)	lua_typename(L, lua_type(L,(i)))

#define luaL_dofile(L, fn) \
	(luaL_loadfile(L, fn) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define luaL_dostring(L, s) \
	(luaL_loadstring(L, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define luaL_getmetatable(L,n)	(lua_getfield(L, LUA_REGISTRYINDEX, (n)))

#define luaL_opt(L,f,n,d)	(lua_isnoneornil(L,(n)) ? (d) : f(L,(n)))

#define luaL_loadbuffer(L,s,sz,n)	luaL_loadbufferx(L,s,sz,n,NULL)


/*
** {======================================================
** Generic Buffer manipulation
** =======================================================
*/

typedef struct luaL_Buffer {
  char *b;  /* buffer address */
  size_t size;  /* buffer size */
  size_t n;  /* number of characters in buffer */
  lua_State *L;
  char initb[LUAL_BUFFERSIZE];  /* initial buffer */
} luaL_Buffer;


#define luaL_addchar(B,c) \
  ((void)((B)->n < (B)->size || luaL_prepbuffsize((B), 1)), \
   ((B)->b[(B)->n++] = (c)))

#define luaL_addsize(B,s)	((B)->n += (s))

LUALIB_API void (luaL_buffinit) (lua_State *L, luaL_Buffer *B);
LUALIB_API char *(luaL_prepbuffsize) (luaL_Buffer *B, size_t sz);
LUALIB_API void (luaL_addlstring) (luaL_Buffer *B, const char *s, size_t l);
LUALIB_API void (luaL_addstring) (luaL_Buffer *B, const char *s);
LUALIB_API void (luaL_addvalue) (luaL_Buffer *B);
LUALIB_API void (luaL_pushresult) (luaL_Buffer *B);
LUALIB_API void (luaL_pushresultsize) (luaL_Buffer *B, size_t sz);
LUALIB_API char *(luaL_buffinitsize) (lua_State *L, luaL_Buffer *B, size_t sz);

#define luaL_prepbuffer(B)	luaL_prepbuffsize(B, LUAL_BUFFERSIZE)

/* }====================================================== */



/*
** {======================================================
** File handles for IO library
** =======================================================
*/

/*
** A file handle is a userdata with metatable 'LUA_FILEHANDLE' and
** initial structure 'luaL_Stream' (it may contain other fields
** after that initial structure).
*/

#define LUA_FILEHANDLE          "FILE*"


typedef struct luaL_Stream {
  FILE *f;  /* stream (NULL for incompletely created streams) */
  lua_CFunction closef;  /* to close stream (NULL for closed streams) */
} luaL_Stream;

/* }====================================================== */



/* compatibility with old module system */
#if defined(LUA_COMPAT_MODULE)

LUALIB_API void (luaL_pushmodule) (lua_State *L, const char *modname,
                                   int sizehint);
LUALIB_API void (luaL_openlib) (lua_State *L, const char *libname,
                                const luaL_Reg *l, int nup);

#define luaL_register(L,n,l)	(luaL_openlib(L,(n),(l),0))

#endif


/*
** {==================================================================
** "Abstraction Layer" for basic report of messages and errors
** ===================================================================
*/

/* print a string */
#if !defined(lua_writestring)
#define lua_writestring(s,l)   fwrite((s), sizeof(char), (l), stdout)
#endif

/* print a newline and flush the output */
#if !defined(lua_writeline)
#define lua_writeline()        (lua_writestring("\n", 1), fflush(stdout))
#endif

/* print an error message */
#if !defined(lua_writestringerror)
#define lua_writestringerror(s,p) \
        (fprintf(stderr, (s), (p)), fflush(stderr))
#endif

/* }================================================================== */


/*
** {============================================================
** Compatibility with deprecated conversions
** =============================================================
*/
#if defined(LUA_COMPAT_APIINTCASTS)

#define luaL_checkunsigned(L,a)	((lua_Unsigned)luaL_checkinteger(L,a))
#define luaL_optunsigned(L,a,d)	\
	((lua_Unsigned)luaL_optinteger(L,a,(lua_Integer)(d)))

#define luaL_checkint(L,n)	((int)luaL_checkinteger(L, (n)))
#define luaL_optint(L,n,d)	((int)luaL_optinteger(L, (n), (d)))

#define luaL_checklong(L,n)	((long)luaL_checkinteger(L, (n)))
#define luaL_optlong(L,n,d)	((long)luaL_optinteger(L, (n), (d)))

#endif
/* }============================================================ */



#endif


}

#line 0 "../addons-all/ofxLua/src/ofxLuaFileWriter.h"
/*
 * Copyright (c) 2012 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxLua for documentation
 *
 */
#pragma once

/* #include "lua.hpp" */

/* #include "ofLog.h" */
/* #include "ofConstants.h" */

/// writes variables out to a lua text file
///
/// this is a not a scripting replacement, but a quick way to store data
///
/// this class is largely derived from the Allacrost scripting system:
/// http://allacrost.sourceforge.net
class ofxLuaFileWriter {

	public:

		ofxLuaFileWriter();
		~ofxLuaFileWriter();
		
		/// save the current buffer to a file,
		/// best to name it with the ".lua" ext
		bool saveToFile(const std::string& filename);
		
		/// clear the buffer
		void clear();

	/// \section Write elements
	
		/// insert a new line char
		void newLine();
	
		/// writes a single line "--" comment
		void writeComment(const std::string& comment);
		
		/// begin a multi-line comment block
		///
		/// --[[
		/// 	some commments, etc
		/// --]]
		///
		void beginCommentBlock();

		/// end a multi-line comment block
		void endCommentBlock();
		
		/// write a single line of text, not variable data
		///
		/// note: don't write close comments "--]]" when in a comment block
		void writeLine(const std::string& comment);
		
		/// begin a table with a name,
		/// subsequent data is written as variables inside
		void beginTable(const std::string& tableName);

		/// begin a table as an index, must bewithin a table
		/// subsequent data is written as variables inside
		void beginTable(const unsigned int tableIndex);

		/// end a table
		void endTable();

	/// \section Write variables

		/// write a boolean value with a name
		void writeBool(const std::string& name, bool value);

		/// write a boolean value with an index, must be within a table
		void writeBool(const unsigned int index, bool value);

		/// write a number value with a name
		void writeNumber(const std::string& name, lua_Number value);

		/// write a number value with a name, must be within a table
		void writeNumber(const unsigned int index, lua_Number value);

		/// write a string value with a name
		void writeString(const std::string& name, std::string value);

		/// write a string value with a name, must be within a table
		void writeString(const unsigned int index, std::string value);

		/// write a vector of boolean values with a name
		void writeBoolVector(const std::string& tableName, std::vector<bool>& v);

		/// write a vector of boolean values with a name, must be within a table
		void writeBoolVector(const unsigned int index, std::vector<bool>& v);

		/// write a vector of number values with a name
		void writeNumberVector(const std::string& tableName, std::vector<lua_Number>& v);

		/// write a vector of number values with a name, must be within a table
		void writeNumberVector(const unsigned int index, std::vector<lua_Number>& v);

		/// write a vector of string values with a name
		void writeStringVector(const std::string& tableName, std::vector<std::string>& v);

		/// write a vector of string values with a name, must be within a table
		void writeStringVector(const unsigned int index, std::vector<std::string>& v);

	private:

		/// templated write
		template <class T> void writetype(int type, T value);

		/// templated write by name
		template <class T> void write(const std::string& name, int type, T value);

		/// templated write by index
		template <class T> void write(const unsigned int index, int type, T value);

		/// templated vector write by name
		template <class T> void writeVector(const std::string& tableName, int type, std::vector<T> &v);

		/// templated vector write by index
		template <class T> void writeVector(const unsigned int index, int type, std::vector<T> &v);
		
		/// write the currently nested table paths
		void writeTablePath();

		/// nested table stack index, via name or index
		struct TableIndex {
			int type;           ///< LUA_TSTRING or LUA_TNUMBER
			std::string name;   ///< name index
			unsigned int index; ///< number index
			operator std::string() {
				if(type == LUA_TNUMBER) {
					return std::to_string(index);
				}
				return name;
			}
		};

		std::vector<TableIndex> tables; ///< the currently open table stack
	
		bool commentBlock;        ///< currently in a comment block?
		std::stringstream buffer; ///< string buffer
};

// TEMPLATE FUNCTIONS

template <class T>
void ofxLuaFileWriter::writetype(int type, T value) {}

template <class T>
void ofxLuaFileWriter::write(const std::string& name, int type, T value) {
	if(tables.empty()) {
		buffer << name << " = ";
	}
	else {
		writeTablePath();
		buffer << "." << name << " = ";
	}
	writetype(type, value);
	buffer << std::endl;
}

template <class T>
void ofxLuaFileWriter::write(const unsigned int index, int type, T value) {
	if(tables.empty()) {
		ofLogWarning("ofxLua") << "Couldn't write var to file by index, no open tables";
		return;
	}
	writeTablePath();
	buffer << "[" << index << "] = ";
	writetype(type, value);
	buffer << std::endl;
}

template <class T>
void ofxLuaFileWriter::writeVector(const std::string& tableName, int type, std::vector<T> &v) {
	if(v.empty()) {
		ofLogWarning("ofxLua") << "Couldn't write empty vector to file";
		return;
	}
	
	// write name
	if(tables.empty()) {
		buffer << tableName << " = { ";
	}
	else {
		writeTablePath();
		buffer << "." << tableName << " = { ";
	}
	
	// write vector
	writetype(type, v[0]);
	for(size_t i = 1; i < v.size(); ++i) {
		buffer << ", ";
		writetype(type, v[i]);
	}
	buffer << " }" << std::endl;
}

template <class T>
void ofxLuaFileWriter::writeVector(const unsigned int index, int type, std::vector<T> &v) {
	if(tables.empty()) {
		ofLogWarning("ofxLua") << "Couldn't write vector to file by index, no open tables";
		return;
	}
	
	// write name
	writeTablePath();
	buffer << "[" << index << "] = { ";
	
	// write vector
	writetype(type, v[0]);
	for(size_t i = 1; i < v.size(); ++i) {
		buffer << ", ";
		writetype(type, v[i]);
	}
	buffer << " }" << std::endl;
}

#line 0 "../addons-all/ofxLua/src/ofxLua.h"
/*
 * Copyright (c) 2012 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxLua for documentation
 *
 * This project uses the following library:
 *
 * Lua, Copyright (c) 1994â2011 Lua.org, PUC-Rio using the MIT License.
 * See the file "COPYRIGHT" in src/lua.
 * See http://www.lua.org/docs.html for documentation
 *
 */
#pragma once

/* #include "ofConstants.h" */
/* #include "ofEvents.h" */

/* #include "lua.hpp" */
/* #include "ofxLuaFileWriter.h" */

// define pua_pushglobaltable for Lua 5.1 api
#if LUA_VERSION_NUM < 502
	#define lua_pushglobaltable(L) lua_pushvalue(L, LUA_GLOBALSINDEX)
#endif

/// a baseclass to receieve Lua error messages,
/// useful for displaying an error message on the screen, etc
class ofxLuaListener {
	public :
		virtual void errorReceived(std::string& message) = 0;
};

///	a Lua interpreter instance
///
///	references:
///     - Lua api http://www.lua.org/manual/5.1/manual.html
///		- SWIG and Lua http://swig.org/Doc1.3/Lua.html
///
/// read/write algos are largely derived from the Allacrost scripting system:
/// http://allacrost.sourceforge.net/
class ofxLua {
	
	public :

		ofxLua();
		virtual ~ofxLua();
		
		/// init the Lua state
		///
		/// set abortOnError to true if you want the Lua state to be cleared
		/// when an error occurs
		///
		/// set openLibs to false if you don't want to load the standard Lua
		/// libs (math, io, std::string, etc)
		///
		/// set ofBindings to false if you don't want to load the openFrameworks
		/// api Lua bindings
		///
		/// note: calls clear if the state has already been inited
		bool init(bool abortOnError=false, bool openLibs=true, bool ofBindings=true);
		
		/// clears current state
		///
		/// note: this also clears all loaded modules
		void clear();
		
		/// is this state valid? (inited, not aborted, etc)
		bool isValid();
		
		/// get abort on error
		/// if abort on error is true, the state is closed when an error ocurrs
		bool getAbortOnError();

		/// set abort on error
		/// if abort on error is true, the state is closed when an error ocurrs
		void setAbortOnError(bool abort);
		
	/// \section Running Lua code
		
		/// run a Lua script string, returns false on script error
		bool doString(const std::string& text);
		
		/// run a Lua script, returns false on script error
		///
		/// set changeDir = true to change the current working dir
		/// to the script's parent dir, use this if Lua's require command
		/// cannot find local scripts
		///
		/// note: changeDir does not affect the current OF data path
		bool doScript(const std::string& script, bool changeDir=false);
		
	/// \section Listeners
		
		/// add a listener, ignores any duplicates
		void addListener(ofxLuaListener* listener);
		
		/// remove a listener
		void removeListener(ofxLuaListener* listener);

		/// set the error callback function directly as an altervative to using
		/// a listener, clear by setting to nulltpr
		///
		/// example:
		///
		///    lua.setErrorCallback([](std::string& message) {
		///         // this is called when a Lua script error occurrs
		///        ofLogWarning() << "Lua script error: " << message;
		///     });
		void setErrorCallback(std::function<void(std::string& message)> const &callback);
		
	/// \section Script Callbacks
	///
	/// these are default script callbacks which call a global function
	/// of the same name and arguments within the current Lua state
	///
	/// they fail silently if the function does not exist

		/// call Lua script setup() function
		void scriptSetup();

		/// call Lua script update() function
		void scriptUpdate();

		/// call Lua script draw() function
		void scriptDraw();

		/// call Lua script exit() function
		void scriptExit();

		/// call Lua script windowResized() function
		void scriptWindowResized(int w, int h);

		/// call Lua script keyPressed() function
		void scriptKeyPressed(int key);

		/// call Lua script keyReleased() function
		void scriptKeyReleased(int key);

		/// call Lua script mosueMoved() function
		void scriptMouseMoved(int x, int y );

		/// call Lua script mosueDragged() function
		void scriptMouseDragged(int x, int y, int button);

		/// call Lua script mousePressed() function
		void scriptMousePressed(int x, int y, int button);

		/// call Lua script mouseReleased() function
		void scriptMouseReleased(int x, int y, int button);

		/// call Lua script mouseScrolled() function
		void scriptMouseScrolled(int x, int y, float scrollX, float scrollY);

		/// call Lua script mouseEntered() function
		void scriptMouseEntered(int x, int y);

		/// call Lua script mouseExited() function
		void scriptMouseExited(int x, int y);

		/// call Lua script dragEvent() function
		void scriptDragEvent(ofDragInfo dragInfo);

		/// call Lua script gotMessage() function,
		/// note: sends msg contents as a std::string
		void scriptGotMessage(ofMessage msg);

		/// call Lua script touchDown() function
		void scriptTouchDown(ofTouchEventArgs &touch);

		/// call Lua script touchMoved() function
		void scriptTouchMoved(ofTouchEventArgs &touch);

		/// call Lua script touchUp() function
		void scriptTouchUp(ofTouchEventArgs &touch);

		/// call Lua script touchDoubleTap() function
		void scriptTouchDoubleTap(ofTouchEventArgs &touch);

		/// call Lua script touchCancelled() function
		void scriptTouchCancelled(ofTouchEventArgs &touch);
		
	/// \section Variables
	///
	/// check if a variable exists as a certain type in the Lua state,
	/// note: pushTable() must have been called when using the table index

		/// returns true if variable exists by name and is a bool
		bool isBool(const std::string& name);

		/// returns true if variable exists by index and is a bool,
		/// must be within a table
		/// note: integer indices start with 1!
		bool isBool(const unsigned int index);

		/// returns true if variable exists by name and is a number
		bool isNumber(const std::string& name);

		/// returns true if variable exists by index and is a number,
		/// must be within a table
		/// note: integer indices start with 1!
		bool isNumber(const unsigned int index);

		/// returns true if variable exists by name and is a string
		bool isString(const std::string& name);

		/// returns true if variable exists by index and is a string,
		/// must be within a table
		/// note: integer indices start with 1!
		bool isString(const unsigned int index);

		/// returns true if variable exists by name and is a function
		bool isFunction(const std::string& name);

		/// returns true if variable exists by index and is a function,
		/// must be within a table
		/// note: integer indices start with 1!
		bool isFunction(const unsigned int index);

		/// returns true if variable exists by name and is a table
		bool isTable(const std::string& name);

		/// returns true if variable exists by index and is a table,
		/// must be within a table
		/// note: integer indices start with 1!
		bool isTable(const unsigned int index);

		/// returns true if a variable *does not* exist by name,
		/// nil is the Lua equivalent of NULL
		bool isNil(const std::string& name);

		/// returns true if a variable *does not* exist by index,
		/// must be within a table, nil is the Lua equivalent of NULL
		/// note: integer indices start with 1!
		bool isNil(const unsigned int index);
	
	/// \section Table Operations
	
		/// create a new table with a given name
		void newTable(const std::string& tableName);

		/// create a new table at a given index, must be within a table
		/// note: integer indices start with 1!
		void newTable(const unsigned int& tableIndex);
	
		/// push table one level by name
		bool pushTable(const std::string& tableName);

		/// push table one level by index, must be within a table
		/// note: integer indices start with 1!
		bool pushTable(const unsigned int& tableIndex);

		/// pop table one level
		void popTable();

		/// pop all table levels
		void popAllTables();
		
		/// get the size of the current table,
		/// undefined if the table is not a sequence aka has a nil value somewhere
		unsigned int tableSize();

		/// get the size of a table with a given name,
		/// undefined if the table is not a sequence aka has a nil value somewhere
		unsigned int tableSize(const std::string& tableName);

		/// get the size of a table at a given index, must be within a table,
		/// undefined if the table is not a sequence aka has a nil value somewhere
		/// note: integer indices start with 1!
		unsigned int tableSize(const unsigned int& tableIndex);
		
		/// print the current table
		void printTable();

		/// print a table with a given name
		void printTable(const std::string& tableName);

		/// print a table at a given index, must be within a table
		/// note: integer indices start with 1!
		void printTable(const unsigned int& tableIndex);
		
		/// clear current table, removes all objects in the table
		void clearTable();

		/// clear table with a given name, removes all objects in the table
		void clearTable(const std::string& tableName);

		/// clear tabel at a given index, must be within a table,
		/// removes all objects in the table
		/// note: integer indices start with 1!
		void clearTable(const unsigned int& tableIndex);
	
	/// \section Reading
		
		/// get bool value by name,
		/// returns value or defaultValue if not found
		bool getBool(const std::string& name, bool defaultValue=false);

		/// get bool value by index, must be within a table
		/// returns value or defaultValue if not found
		/// note: integer indices start with 1!
		bool getBool(const unsigned int index, bool defaultValue=false);

		/// get number value by name
		/// returns value or defaultValue if not found
		lua_Number getNumber(const std::string& name, lua_Number devaultValue=0);

		/// get number value by index, must be within a table
		/// returns value or defaultValue if not found
		/// note: integer indices start with 1!
		lua_Number getNumber(const unsigned int index, lua_Number devaultValue=0);

		/// get string value by name
		/// returns value or defaultValue if not found
		std::string getString(const std::string& name, const std::string& defaultValue="");

		/// get string value by index, must be within a table
		/// returns value or defaultValue if not found
		/// note: integer indices start with 1!
		std::string getString(const unsigned int index, const std::string& defaultValue="");
		
		/// get a vector of bool values by table name,
		/// clears and copies values into v on success
		void getBoolVector(const std::string& tableName, std::vector<bool>& v);

		/// get a vector of bool values by table index, must be within a table
		/// clears and copies values into v on success
		/// note: integer indices start with 1!
		void getBoolVector(const unsigned int tableIndex, std::vector<bool>& v);

		/// get a vector of number values by table name,
		/// clears and copies values into v on success
		void getNumberVector(const std::string& tableName, std::vector<lua_Number>& v);

		/// get a vector of number values by table name, must be within a table
		/// clears and copies values into v on success
		/// note: integer indices start with 1!
		void getNumberVector(const unsigned int tableIndex, std::vector<lua_Number>& v);

		/// get a vector of string values by table name,
		/// clears and copies values into v on success
		void getStringVector(const std::string& tableName, std::vector<std::string>& v);

		/// get a vector of string values by table name, must be within a table
		/// clears and copies values into v on success
		/// note: integer indices start with 1!
		void getStringVector(const unsigned int tableIndex, std::vector<std::string>& v);
	
	/// \section Writing
		
		/// set bool value by name, creates if not existing
		void setBool(const std::string& name, bool value);

		/// set bool value by index, creates if not existing
		/// must be within a table
		/// note: integer indices start with 1!
		void setBool(const unsigned int index, bool value);

		/// set number value by name, creates if not existing
		void setNumber(const std::string& name, lua_Number value);

		/// set number value by index, creates if not existing
		/// must be within a table
		/// note: integer indices start with 1!
		void setNumber(const unsigned int index, lua_Number value);

		/// set string value by name, creates if not existing
		void setString(const std::string& name, const std::string value);

		/// set string value by index, creates if not existing
		/// must be within a table
		/// note: integer indices start with 1!
		void setString(const unsigned int index, const std::string value);

		/// set bool values by table name, table name must exist
		void setBoolVector(const std::string& tableName, std::vector<bool>& v);

		/// set bool values by table index, table index must exist
		/// must be within a table
		/// note: integer indices start with 1!
		void setBoolVector(const unsigned int tableIndex, std::vector<bool>& v);

		/// set number values by table name, table name must exist
		void setNumberVector(const std::string& tableName, std::vector<lua_Number>& v);

		/// set number values by table index, table index must exist
		/// must be within a table
		/// note: integer indices start with 1!
		void setNumberVector(const unsigned int tableIndex, std::vector<lua_Number>& v);

		/// set string values by table name, table name must exist
		void setStringVector(const std::string& tableName, std::vector<std::string>& v);

		/// set string values by table index, table index must exist
		/// must be within a table
		/// note: integer indices start with 1!
		void setStringVector(const unsigned int tableIndex, std::vector<std::string>& v);
	
		/// set a variable or table name to nil,
		/// essentially deletes it from the state
		void setNil(const std::string& name);

		/// set a variable or table index to nil, must be within a table
		/// essentially deletes it from the state
		/// note: integer indices start with 1!
		void setNil(const unsigned int index);
	
	/// \section Writing to a File
	///
	/// write simple variables and tables to a text file,
	/// this does not save actual scripts or the current state, only data in
	/// a given table
	
		/// write the current table to a given file writer,
		/// set recursive to false to only write data at the current level
		///
		/// note: cannot save the global table, you must call pushTable if you
	    /// don't save using a tableName
		void writeTable(ofxLuaFileWriter& writer, bool recursive=true);

		/// write named table to a given file writer,
		/// set recursive to false to only write data at the current level
		void writeTable(const std::string& tableName, ofxLuaFileWriter& writer, bool recursive=true);

		/// write the current table to a file,
		/// set recursive to false to only write data at the current level
		///
		/// note: cannot save the global table, you must call pushTable if you
	    /// don't save using a tableName
		bool writeTableToFile(const std::string& filename, bool recursive=true);

		/// write named table to a file,
		/// set recursive to false to only write data at the current level
		bool writeTableToFile(const std::string& tableName, const std::string& filename, bool recursive=true);
	
	/// \section Util

		/// send a Lua error message to any listeners and
		/// clear lua state if abortOnError is set
		virtual void errorOccurred(std::string& msg);
	
		/// get the current error message, an alternative to ofxLuaListener
		std::string getErrorMessage();
	
		/// print current stack length and contents from left to right,
		/// useful for debugging
		void printStack();
	
		/// get the raw Lua state, useful for custom Lua api code
		///
		/// example, call "myFunction(x, y)" in the Lua state:
		///
		///     int x = 20, y = 10;
		///     ofxLua lua;
		///     lua.init();
		///     
		///     lua_getglobal(lua, "myFunction");
		///     lua_pushinteger(lua, x);
		///     lua_pushinteger(lua, y);
		///     if(lua_pcall(lua, 2, 0, 0) != 0) {
		///         cout << "error running myFunction" << endl;
		///     }
		///
		///	note: make sure to call lua.init() before using the Lua state!
		///
		operator lua_State*() const {return L;}
	
		/// push a custom wrapped SWIG object pointer onto the stack,
		/// similar to lua_pushinteger, lua_pushstring, etc
		///
		/// typeName: name of the wrapped *orginal* C/C++ type as a std::string
		/// object: the pointer
		/// manageMemory: set to true if you want the Lua garbage collector to
		///               manage the memory pointed to
		///
		/// returns true if the pointer type was found and pushed
		///
		/// example, call a global Lua function "touchDown" with a touch event
		///
		///     ofTouchEventArgs *t = new ofTouchEventArgs(touch);
		///     lua_getglobal(lua, "touchDown");
		///     lua.pushobject("ofTouchArgEvents", t, true); // true: let Lua delete it
		///     lua_pcall(lua, 1, 0, 0);
		///
		bool pushobject(const std::string &typeName, void *object, bool manageMemory=true);
	
    protected:
		
		/// Lua stack top index
		static const int LUA_STACK_TOP = -1;
		
		/// returns true if an object exists by name
		bool exists(const std::string& name, int type);

		/// returns true if an object exists by index
		bool exists(const unsigned int index, int type);
		
		/// returns true is an object is of a certain type
		bool checkType(int stackIndex, int type);
	
		/// try to get value of a given type off of the top of the stack
		template <class T> T totype(int stackIndex, int type, T defaultValue);
	
		/// read a templated value from the state by name
		template <class T> T read(const std::string& name, int type, T defaultVal);

		/// read a templated value from the state by index
		template <class T> T read(const unsigned int index, int type, T defaultVal);

		/// read a table into a templated vector by name
		template <class T> void readVector(const std::string& name, std::vector<T>& v, int type, T defaultValue);

		/// read a table into a templated vector by index
		template <class T> void readVector(const unsigned int index, std::vector<T>& v, int type, T defaultValue);

		/// read the current table into a templated vector
		template <class T> void readVectorHelper(std::vector<T>& v, int type, T defaultValue);
	
		/// set a value of a given type by name
		template <class T> void settype(const std::string& name, int type, T value);

		/// set a value of a given type by index
		template <class T> void settype(unsigned int index, int type, T value);
	
		/// write a value to the state by name
		template <class T> void write(const std::string& name, int type, T value);

		/// write a value to the state by index
		template <class T> void write(const unsigned int index, int type, T value);
		
		/// write a vector into a table name
		template <class T> void writeVector(const std::string& name, int type, std::vector<T>& v);

		/// write a vector into a table index
		template <class T> void writeVector(const unsigned int index, int type, std::vector<T>& v);

		/// read a templated vector to the current table
		template <class T> void writeVectorHelper(int type, std::vector<T>& v);
	
		/// print current table
		void printTable(int stackIndex, int numTabs);
		
		/// writer current table to a buffer (values only)
		void writeTable(int stackIndex, ofxLuaFileWriter& writer, bool recursive);
	
		/// called when Lua state panics (hard crash)
		static int atPanic(lua_State *L);
	
		lua_State* L = NULL;       ///< the Lua state object
		bool abortOnError = false; ///< close the Lua state on error?

		/// nested table stack index, via name or index
		struct TableIndex {
			int type;           ///< LUA_TSTRING or LUA_TNUMBER
			std::string name;   ///< name index
			unsigned int index; ///< number index
			operator std::string() {
				if(type == LUA_TNUMBER) {
					return std::to_string(index);
				}
				return name;
			}
		};
		std::vector<TableIndex> tables; ///< the currently open table stack

		/// error event callback function pointer
		std::function<void(std::string& message)> errorCallback = nullptr;

		ofEvent<std::string> errorEvent; ///< error event object, std::string is error msg
		std::string errorMessage = "";   ///< current error message
};

// TEMPLATE FUNCTIONS

// TYPES

template <class T>
T ofxLua::totype(int stackIndex, int type, T defaultValue) { return defaultValue; }

template <class T>
void ofxLua::settype(const std::string& name, int type, T value) {}

template <class T>
void ofxLua::settype(unsigned int index, int type, T value) {}

// READ

template <class T>
T ofxLua::read(const std::string& name, int type, T defaultValue) {
	if(!isValid()) {
		return defaultValue;
	}
	
	// global variable?
	T ret = defaultValue;
	if(tables.size() == 0) {
		lua_getglobal(L, name.c_str());
		if(lua_type(L, LUA_STACK_TOP) == type) {
			ret = totype(LUA_STACK_TOP, type, defaultValue);
		}
		else {
			ofLogWarning("ofxLua") << "Couldn't read global var: \"" << name << "\"";
		}
		lua_pop(L, 1);
	}
	
	// in a table namespace
	else {
		if(!lua_istable(L, LUA_STACK_TOP)) {
			ofLogWarning("ofxLua") << "Couldn't read var: \"" << name << "\""
				<< ", top of stack is not a table";
			return ret;
		}
		lua_getfield(L, LUA_STACK_TOP, name.c_str());
		if(lua_type(L, LUA_STACK_TOP) == type) {
			ret = totype(LUA_STACK_TOP, type, defaultValue);
		}
		else {
			ofLogWarning("ofxLua") << "Couldn't read table var: \"" << name << "\"";
		}
		lua_pop(L, 1);
	}
	
	return ret;
}

template <class T>
T ofxLua::read(const unsigned int index, int type, T defaultValue) {
	if(!isValid()) {
		return defaultValue;
	}
	
	// global variable?
	T ret = defaultValue;
	if(tables.empty()) {
		ofLogWarning("ofxLua") << "Couldn't read var by index, no open tables";
		return defaultValue;
	}
	
	// in a table namespace
	if(!lua_istable(L, LUA_STACK_TOP)) {
		ofLogWarning("ofxLua") << "Couldn't read var " << index
			<< ", top of stack is not a table";
		return defaultValue;
	}
	lua_pushinteger(L, index);
	lua_gettable(L, LUA_STACK_TOP-1);
	if(lua_type(L, LUA_STACK_TOP) == type) {
		ret = totype(LUA_STACK_TOP, type, defaultValue);
	}
	else {
		ofLogWarning("ofxLua") << "Couldn't read table var " << index;
	}
	lua_pop(L, 1);
	
	return ret;
}

template <class T>
void ofxLua::readVector(const std::string& name, std::vector<T>& v, int type, T defaultValue) {
	if(!pushTable(name)) {
		return;
	}
	readVectorHelper(v, type, defaultValue);
	popTable();
}

template <class T>
void ofxLua::readVector(const unsigned int index, std::vector<T>& v, int type, T defaultValue) {
	if(!pushTable(index)) {
		return;
	}
	readVectorHelper(v, type, defaultValue);
	popTable();
}

template <class T>
void ofxLua::readVectorHelper(std::vector<T>& v, int type, T defaultValue) {
	if(!isValid()) {
		return;
	}
	
	v.clear();
	
	if(!lua_istable(L, LUA_STACK_TOP)) {
		std::string tname = "unknown";
		if(!tables.empty()) {
			tname = tables.back();
		}
		ofLogWarning("ofxLua") << "Couldn't read table \"" << tname << "\", stack var is not a table";
		ofLogWarning("ofxLua") << "Did you forget to call pushTable()?";
	}
	
	// push temp key & value onto the stack since lua_next will overwrite them
	lua_pushvalue(L, LUA_STACK_TOP); // stack: -1 => table
	lua_pushnil(L); // stack : -2 => table; -1 => nil;
	
	// iterate through elements, pushes key and value onto stack
	// stack: -3 => table; -2 => key; -1 => value
	while(lua_next(L, -2)) {
		if(lua_type(L, -1) == type) {
			v.push_back(totype(-1, type, defaultValue));
		}
		else {
			std::string tname = "unknown";
			if(!tables.empty()) {
				tname = tables.back();
			}
			ofLogWarning("ofxLua") << "Couldn't convert type when reading table \"" << tname << "\"";
		}
		lua_pop(L, 1); // stack: -2 => table; -1 => key
	}
	
	// stack: -1 => table
	lua_pop(L, 1); // stack:
}

// WRITE

template <class T>
void ofxLua::write(const std::string& name, int type, T value) {
	if(!isValid()) {
		return;
	}
	
	// global variable?
	if(tables.empty()) {
		lua_pushglobaltable(L);
		settype<T>(name, type, value);
		lua_pop(L, 1);
	}
	
	// in a table namespace
	else {
		if(!lua_istable(L, LUA_STACK_TOP)) {
			ofLogWarning("ofxLua") << "Couldn't write var: \"" << name << "\""
				<< ", top of stack is not a table";
			return;
		}
		settype<T>(name, type, value);
	}
}

template <class T>
void ofxLua::write(const unsigned int index, int type, T value) {
	
	// global variable?
	if(tables.empty()) {
		ofLogWarning("ofxLua") << "Couldn't write global var by index, no open tables";
		return;
	}
	
	// in a table namespace
	if(!lua_istable(L, LUA_STACK_TOP)) {
		ofLogWarning("ofxLua") << "Couldn't write var at index " << index
			<< ", top of stack is not a table";
		return;
	}
	
	settype<T>(index, type, value);
}

template <class T>
void ofxLua::writeVector(const std::string& name, int type, std::vector<T>& v) {
	if(!pushTable(name)) {
		return;
	}
	writeVectorHelper(type, v);
	popTable();
}

template <class T>
void ofxLua::writeVector(const unsigned int index, int type, std::vector<T>& v) {
	if(!pushTable(index)) {
		return;
	}
	writeVectorHelper(type, v);
	popTable();
}

template <class T>
void ofxLua::writeVectorHelper(int type, std::vector<T>& v) {

	// global variable?
	if(tables.empty()) {
		ofLogWarning("ofxLua") << "Couldn't write table vector, no open tables";
		return;
	}
	std::string tname = tables.back();
	
	if(v.empty()) {
		ofLogWarning("ofxLua") << "Couldn't write table \"" << tname << "\", vector is empty";
		return;
	}

	if(!lua_istable(L, LUA_STACK_TOP)) {
		ofLogWarning("ofxLua") << "Couldn't write table \"" << tname << "\", stack var is not a table";
	}
	
	// remove all elements
	clearTable();
	
	// add new variables
	for(size_t i = 0; i < v.size(); ++i) {
		settype<T>(i+1, type, v[i]);
	}
}
