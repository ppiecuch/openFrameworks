/*
                            __  __            _
                         ___\ \/ /_ __   __ _| |_
                        / _ \\  /| '_ \ / _` | __|
                       |  __//  \| |_) | (_| | |_
                        \___/_/\_\ .__/ \__,_|\__|
                                 |_| XML parser

   Copyright (c) 1997-2000 Thai Open Source Software Center Ltd
   Copyright (c) 2000-2017 Expat development team
   Licensed under the MIT license:

   Permission is  hereby granted,  free of charge,  to any  person obtaining
   a  copy  of  this  software   and  associated  documentation  files  (the
   "Software"),  to  deal in  the  Software  without restriction,  including
   without  limitation the  rights  to use,  copy,  modify, merge,  publish,
   distribute, sublicense, and/or sell copies of the Software, and to permit
   persons  to whom  the Software  is  furnished to  do so,  subject to  the
   following conditions:

   The above copyright  notice and this permission notice  shall be included
   in all copies or substantial portions of the Software.

   THE  SOFTWARE  IS  PROVIDED  "AS  IS",  WITHOUT  WARRANTY  OF  ANY  KIND,
   EXPRESS  OR IMPLIED,  INCLUDING  BUT  NOT LIMITED  TO  THE WARRANTIES  OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
   NO EVENT SHALL THE AUTHORS OR  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
   DAMAGES OR  OTHER LIABILITY, WHETHER  IN AN  ACTION OF CONTRACT,  TORT OR
   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
   USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef Expat_External_INCLUDED
#define Expat_External_INCLUDED 1

/* External API definitions */

#if defined(_MSC_EXTENSIONS) && !defined(__BEOS__) && !defined(__CYGWIN__)
# define XML_USE_MSC_EXTENSIONS 1
#endif

/* Expat tries very hard to make the API boundary very specifically
   defined.  There are two macros defined to control this boundary;
   each of these can be defined before including this header to
   achieve some different behavior, but doing so it not recommended or
   tested frequently.

   XMLCALL    - The calling convention to use for all calls across the
                "library boundary."  This will default to cdecl, and
                try really hard to tell the compiler that's what we
                want.

   XMLIMPORT  - Whatever magic is needed to note that a function is
                to be imported from a dynamically loaded library
                (.dll, .so, or .sl, depending on your platform).

   The XMLCALL macro was added in Expat 1.95.7.  The only one which is
   expected to be directly useful in client code is XMLCALL.

   Note that on at least some Unix versions, the Expat library must be
   compiled with the cdecl calling convention as the default since
   system headers may assume the cdecl convention.
*/
#ifndef XMLCALL
# if defined(_MSC_VER)
#  define XMLCALL __cdecl
# elif defined(__GNUC__) && defined(__i386) && !defined(__INTEL_COMPILER)
#  define XMLCALL __attribute__((cdecl))
# else
/* For any platform which uses this definition and supports more than
   one calling convention, we need to extend this definition to
   declare the convention used on that platform, if it's possible to
   do so.

   If this is the case for your platform, please file a bug report
   with information on how to identify your platform via the C
   pre-processor and how to specify the same calling convention as the
   platform's malloc() implementation.
*/
#  define XMLCALL
# endif
#endif  /* not defined XMLCALL */


#if !defined(XML_STATIC) && !defined(XMLIMPORT)
# ifndef XML_BUILDING_EXPAT
/* using Expat from an application */

#  ifdef XML_USE_MSC_EXTENSIONS
#   define XMLIMPORT __declspec(dllimport)
#  endif

# endif
#endif  /* not defined XML_STATIC */

#if !defined(XMLIMPORT) && defined(__GNUC__) && (__GNUC__ >= 4)
# define XMLIMPORT __attribute__ ((visibility ("default")))
#endif

/* If we didn't define it above, define it away: */
#ifndef XMLIMPORT
# define XMLIMPORT
#endif

#if defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96))
# define XML_ATTR_MALLOC __attribute__((__malloc__))
#else
# define XML_ATTR_MALLOC
#endif

#if defined(__GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
# define XML_ATTR_ALLOC_SIZE(x)  __attribute__((__alloc_size__(x)))
#else
# define XML_ATTR_ALLOC_SIZE(x)
#endif

#define XMLPARSEAPI(type) XMLIMPORT type XMLCALL

#ifdef __cplusplus
extern "C" {
#endif

#ifdef XML_UNICODE_WCHAR_T
# ifndef XML_UNICODE
#  define XML_UNICODE
# endif
# if defined(__SIZEOF_WCHAR_T__) && (__SIZEOF_WCHAR_T__ != 2)
#  error "sizeof(wchar_t) != 2; Need -fshort-wchar for both Expat and libc"
# endif
#endif

#ifdef XML_UNICODE     /* Information is UTF-16 encoded. */
# ifdef XML_UNICODE_WCHAR_T
typedef wchar_t XML_Char;
typedef wchar_t XML_LChar;
# else
typedef unsigned short XML_Char;
typedef char XML_LChar;
# endif /* XML_UNICODE_WCHAR_T */
#else                  /* Information is UTF-8 encoded. */
typedef char XML_Char;
typedef char XML_LChar;
#endif /* XML_UNICODE */

#ifdef XML_LARGE_SIZE  /* Use large integers for file/stream positions. */
# if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
typedef __int64 XML_Index; 
typedef unsigned __int64 XML_Size;
# else
typedef long long XML_Index;
typedef unsigned long long XML_Size;
# endif
#else
typedef long XML_Index;
typedef unsigned long XML_Size;
#endif /* XML_LARGE_SIZE */

#ifdef __cplusplus
}
#endif

#endif /* not Expat_External_INCLUDED */
/* internal.h

   Internal definitions used by Expat.  This is not needed to compile
   client code.

   The following calling convention macros are defined for frequently
   called functions:

   FASTCALL    - Used for those internal functions that have a simple
                 body and a low number of arguments and local variables.

   PTRCALL     - Used for functions called though function pointers.

   PTRFASTCALL - Like PTRCALL, but for low number of arguments.

   inline      - Used for selected internal functions for which inlining
                 may improve performance on some platforms.

   Note: Use of these macros is based on judgement, not hard rules,
         and therefore subject to change.
                            __  __            _
                         ___\ \/ /_ __   __ _| |_
                        / _ \\  /| '_ \ / _` | __|
                       |  __//  \| |_) | (_| | |_
                        \___/_/\_\ .__/ \__,_|\__|
                                 |_| XML parser

   Copyright (c) 1997-2000 Thai Open Source Software Center Ltd
   Copyright (c) 2000-2017 Expat development team
   Licensed under the MIT license:

   Permission is  hereby granted,  free of charge,  to any  person obtaining
   a  copy  of  this  software   and  associated  documentation  files  (the
   "Software"),  to  deal in  the  Software  without restriction,  including
   without  limitation the  rights  to use,  copy,  modify, merge,  publish,
   distribute, sublicense, and/or sell copies of the Software, and to permit
   persons  to whom  the Software  is  furnished to  do so,  subject to  the
   following conditions:

   The above copyright  notice and this permission notice  shall be included
   in all copies or substantial portions of the Software.

   THE  SOFTWARE  IS  PROVIDED  "AS  IS",  WITHOUT  WARRANTY  OF  ANY  KIND,
   EXPRESS  OR IMPLIED,  INCLUDING  BUT  NOT LIMITED  TO  THE WARRANTIES  OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
   NO EVENT SHALL THE AUTHORS OR  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
   DAMAGES OR  OTHER LIABILITY, WHETHER  IN AN  ACTION OF CONTRACT,  TORT OR
   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
   USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#if defined(__GNUC__) && defined(__i386__) && !defined(__MINGW32__)
/* We'll use this version by default only where we know it helps.

   regparm() generates warnings on Solaris boxes.   See SF bug #692878.

   Instability reported with egcs on a RedHat Linux 7.3.
   Let's comment out:
   #define FASTCALL __attribute__((stdcall, regparm(3)))
   and let's try this:
*/
#define FASTCALL __attribute__((regparm(3)))
#define PTRFASTCALL __attribute__((regparm(3)))
#endif

/* Using __fastcall seems to have an unexpected negative effect under
   MS VC++, especially for function pointers, so we won't use it for
   now on that platform. It may be reconsidered for a future release
   if it can be made more effective.
   Likely reason: __fastcall on Windows is like stdcall, therefore
   the compiler cannot perform stack optimizations for call clusters.
*/

/* Make sure all of these are defined if they aren't already. */

#ifndef FASTCALL
#define FASTCALL
#endif

#ifndef PTRCALL
#define PTRCALL
#endif

#ifndef PTRFASTCALL
#define PTRFASTCALL
#endif

#ifndef XML_MIN_SIZE
#if !defined(__cplusplus) && !defined(inline)
#ifdef __GNUC__
#define inline __inline
#endif /* __GNUC__ */
#endif
#endif /* XML_MIN_SIZE */

#ifdef __cplusplus
#define inline inline
#else
#ifndef inline
#define inline
#endif
#endif

#ifndef UNUSED_P
# ifdef __GNUC__
#  define UNUSED_P(p) UNUSED_ ## p __attribute__((__unused__))
# else
#  define UNUSED_P(p) UNUSED_ ## p
# endif
#endif


#ifdef __cplusplus
extern "C" {
#endif


void
_INTERNAL_trim_to_complete_utf8_characters(const char * from, const char ** fromLimRef);


#ifdef __cplusplus
}
#endif
/*
                            __  __            _
                         ___\ \/ /_ __   __ _| |_
                        / _ \\  /| '_ \ / _` | __|
                       |  __//  \| |_) | (_| | |_
                        \___/_/\_\ .__/ \__,_|\__|
                                 |_| XML parser

   Copyright (c) 1997-2000 Thai Open Source Software Center Ltd
   Copyright (c) 2000-2017 Expat development team
   Licensed under the MIT license:

   Permission is  hereby granted,  free of charge,  to any  person obtaining
   a  copy  of  this  software   and  associated  documentation  files  (the
   "Software"),  to  deal in  the  Software  without restriction,  including
   without  limitation the  rights  to use,  copy,  modify, merge,  publish,
   distribute, sublicense, and/or sell copies of the Software, and to permit
   persons  to whom  the Software  is  furnished to  do so,  subject to  the
   following conditions:

   The above copyright  notice and this permission notice  shall be included
   in all copies or substantial portions of the Software.

   THE  SOFTWARE  IS  PROVIDED  "AS  IS",  WITHOUT  WARRANTY  OF  ANY  KIND,
   EXPRESS  OR IMPLIED,  INCLUDING  BUT  NOT LIMITED  TO  THE WARRANTIES  OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
   NO EVENT SHALL THE AUTHORS OR  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
   DAMAGES OR  OTHER LIABILITY, WHETHER  IN AN  ACTION OF CONTRACT,  TORT OR
   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
   USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#define ASCII_A 0x41
#define ASCII_B 0x42
#define ASCII_C 0x43
#define ASCII_D 0x44
#define ASCII_E 0x45
#define ASCII_F 0x46
#define ASCII_G 0x47
#define ASCII_H 0x48
#define ASCII_I 0x49
#define ASCII_J 0x4A
#define ASCII_K 0x4B
#define ASCII_L 0x4C
#define ASCII_M 0x4D
#define ASCII_N 0x4E
#define ASCII_O 0x4F
#define ASCII_P 0x50
#define ASCII_Q 0x51
#define ASCII_R 0x52
#define ASCII_S 0x53
#define ASCII_T 0x54
#define ASCII_U 0x55
#define ASCII_V 0x56
#define ASCII_W 0x57
#define ASCII_X 0x58
#define ASCII_Y 0x59
#define ASCII_Z 0x5A

#define ASCII_a 0x61
#define ASCII_b 0x62
#define ASCII_c 0x63
#define ASCII_d 0x64
#define ASCII_e 0x65
#define ASCII_f 0x66
#define ASCII_g 0x67
#define ASCII_h 0x68
#define ASCII_i 0x69
#define ASCII_j 0x6A
#define ASCII_k 0x6B
#define ASCII_l 0x6C
#define ASCII_m 0x6D
#define ASCII_n 0x6E
#define ASCII_o 0x6F
#define ASCII_p 0x70
#define ASCII_q 0x71
#define ASCII_r 0x72
#define ASCII_s 0x73
#define ASCII_t 0x74
#define ASCII_u 0x75
#define ASCII_v 0x76
#define ASCII_w 0x77
#define ASCII_x 0x78
#define ASCII_y 0x79
#define ASCII_z 0x7A

#define ASCII_0 0x30
#define ASCII_1 0x31
#define ASCII_2 0x32
#define ASCII_3 0x33
#define ASCII_4 0x34
#define ASCII_5 0x35
#define ASCII_6 0x36
#define ASCII_7 0x37
#define ASCII_8 0x38
#define ASCII_9 0x39

#define ASCII_TAB 0x09
#define ASCII_SPACE 0x20
#define ASCII_EXCL 0x21
#define ASCII_QUOT 0x22
#define ASCII_AMP 0x26
#define ASCII_APOS 0x27
#define ASCII_MINUS 0x2D
#define ASCII_PERIOD 0x2E
#define ASCII_COLON 0x3A
#define ASCII_SEMI 0x3B
#define ASCII_LT 0x3C
#define ASCII_EQUALS 0x3D
#define ASCII_GT 0x3E
#define ASCII_LSQB 0x5B
#define ASCII_RSQB 0x5D
#define ASCII_UNDERSCORE 0x5F
#define ASCII_LPAREN 0x28
#define ASCII_RPAREN 0x29
#define ASCII_FF 0x0C
#define ASCII_SLASH 0x2F
#define ASCII_HASH 0x23
#define ASCII_PIPE 0x7C
#define ASCII_COMMA 0x2C
/*
                            __  __            _
                         ___\ \/ /_ __   __ _| |_
                        / _ \\  /| '_ \ / _` | __|
                       |  __//  \| |_) | (_| | |_
                        \___/_/\_\ .__/ \__,_|\__|
                                 |_| XML parser

   Copyright (c) 1997-2000 Thai Open Source Software Center Ltd
   Copyright (c) 2000-2017 Expat development team
   Licensed under the MIT license:

   Permission is  hereby granted,  free of charge,  to any  person obtaining
   a  copy  of  this  software   and  associated  documentation  files  (the
   "Software"),  to  deal in  the  Software  without restriction,  including
   without  limitation the  rights  to use,  copy,  modify, merge,  publish,
   distribute, sublicense, and/or sell copies of the Software, and to permit
   persons  to whom  the Software  is  furnished to  do so,  subject to  the
   following conditions:

   The above copyright  notice and this permission notice  shall be included
   in all copies or substantial portions of the Software.

   THE  SOFTWARE  IS  PROVIDED  "AS  IS",  WITHOUT  WARRANTY  OF  ANY  KIND,
   EXPRESS  OR IMPLIED,  INCLUDING  BUT  NOT LIMITED  TO  THE WARRANTIES  OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
   NO EVENT SHALL THE AUTHORS OR  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
   DAMAGES OR  OTHER LIABILITY, WHETHER  IN AN  ACTION OF CONTRACT,  TORT OR
   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
   USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

static const unsigned namingBitmap[] = {
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
0x00000000, 0x04000000, 0x87FFFFFE, 0x07FFFFFE,
0x00000000, 0x00000000, 0xFF7FFFFF, 0xFF7FFFFF,
0xFFFFFFFF, 0x7FF3FFFF, 0xFFFFFDFE, 0x7FFFFFFF,
0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFE00F, 0xFC31FFFF,
0x00FFFFFF, 0x00000000, 0xFFFF0000, 0xFFFFFFFF,
0xFFFFFFFF, 0xF80001FF, 0x00000003, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0xFFFFD740, 0xFFFFFFFB, 0x547F7FFF, 0x000FFFFD,
0xFFFFDFFE, 0xFFFFFFFF, 0xDFFEFFFF, 0xFFFFFFFF,
0xFFFF0003, 0xFFFFFFFF, 0xFFFF199F, 0x033FCFFF,
0x00000000, 0xFFFE0000, 0x027FFFFF, 0xFFFFFFFE,
0x0000007F, 0x00000000, 0xFFFF0000, 0x000707FF,
0x00000000, 0x07FFFFFE, 0x000007FE, 0xFFFE0000,
0xFFFFFFFF, 0x7CFFFFFF, 0x002F7FFF, 0x00000060,
0xFFFFFFE0, 0x23FFFFFF, 0xFF000000, 0x00000003,
0xFFF99FE0, 0x03C5FDFF, 0xB0000000, 0x00030003,
0xFFF987E0, 0x036DFDFF, 0x5E000000, 0x001C0000,
0xFFFBAFE0, 0x23EDFDFF, 0x00000000, 0x00000001,
0xFFF99FE0, 0x23CDFDFF, 0xB0000000, 0x00000003,
0xD63DC7E0, 0x03BFC718, 0x00000000, 0x00000000,
0xFFFDDFE0, 0x03EFFDFF, 0x00000000, 0x00000003,
0xFFFDDFE0, 0x03EFFDFF, 0x40000000, 0x00000003,
0xFFFDDFE0, 0x03FFFDFF, 0x00000000, 0x00000003,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0xFFFFFFFE, 0x000D7FFF, 0x0000003F, 0x00000000,
0xFEF02596, 0x200D6CAE, 0x0000001F, 0x00000000,
0x00000000, 0x00000000, 0xFFFFFEFF, 0x000003FF,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0xFFFFFFFF, 0xFFFF003F, 0x007FFFFF,
0x0007DAED, 0x50000000, 0x82315001, 0x002C62AB,
0x40000000, 0xF580C900, 0x00000007, 0x02010800,
0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
0x0FFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x03FFFFFF,
0x3F3FFFFF, 0xFFFFFFFF, 0xAAFF3F3F, 0x3FFFFFFF,
0xFFFFFFFF, 0x5FDFFFFF, 0x0FCF1FDC, 0x1FDC1FFF,
0x00000000, 0x00004C40, 0x00000000, 0x00000000,
0x00000007, 0x00000000, 0x00000000, 0x00000000,
0x00000080, 0x000003FE, 0xFFFFFFFE, 0xFFFFFFFF,
0x001FFFFF, 0xFFFFFFFE, 0xFFFFFFFF, 0x07FFFFFF,
0xFFFFFFE0, 0x00001FFF, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
0xFFFFFFFF, 0x0000003F, 0x00000000, 0x00000000,
0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
0xFFFFFFFF, 0x0000000F, 0x00000000, 0x00000000,
0x00000000, 0x07FF6000, 0x87FFFFFE, 0x07FFFFFE,
0x00000000, 0x00800000, 0xFF7FFFFF, 0xFF7FFFFF,
0x00FFFFFF, 0x00000000, 0xFFFF0000, 0xFFFFFFFF,
0xFFFFFFFF, 0xF80001FF, 0x00030003, 0x00000000,
0xFFFFFFFF, 0xFFFFFFFF, 0x0000003F, 0x00000003,
0xFFFFD7C0, 0xFFFFFFFB, 0x547F7FFF, 0x000FFFFD,
0xFFFFDFFE, 0xFFFFFFFF, 0xDFFEFFFF, 0xFFFFFFFF,
0xFFFF007B, 0xFFFFFFFF, 0xFFFF199F, 0x033FCFFF,
0x00000000, 0xFFFE0000, 0x027FFFFF, 0xFFFFFFFE,
0xFFFE007F, 0xBBFFFFFB, 0xFFFF0016, 0x000707FF,
0x00000000, 0x07FFFFFE, 0x0007FFFF, 0xFFFF03FF,
0xFFFFFFFF, 0x7CFFFFFF, 0xFFEF7FFF, 0x03FF3DFF,
0xFFFFFFEE, 0xF3FFFFFF, 0xFF1E3FFF, 0x0000FFCF,
0xFFF99FEE, 0xD3C5FDFF, 0xB080399F, 0x0003FFCF,
0xFFF987E4, 0xD36DFDFF, 0x5E003987, 0x001FFFC0,
0xFFFBAFEE, 0xF3EDFDFF, 0x00003BBF, 0x0000FFC1,
0xFFF99FEE, 0xF3CDFDFF, 0xB0C0398F, 0x0000FFC3,
0xD63DC7EC, 0xC3BFC718, 0x00803DC7, 0x0000FF80,
0xFFFDDFEE, 0xC3EFFDFF, 0x00603DDF, 0x0000FFC3,
0xFFFDDFEC, 0xC3EFFDFF, 0x40603DDF, 0x0000FFC3,
0xFFFDDFEC, 0xC3FFFDFF, 0x00803DCF, 0x0000FFC3,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0xFFFFFFFE, 0x07FF7FFF, 0x03FF7FFF, 0x00000000,
0xFEF02596, 0x3BFF6CAE, 0x03FF3F5F, 0x00000000,
0x03000000, 0xC2A003FF, 0xFFFFFEFF, 0xFFFE03FF,
0xFEBF0FDF, 0x02FE3FFF, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x1FFF0000, 0x00000002,
0x000000A0, 0x003EFFFE, 0xFFFFFFFE, 0xFFFFFFFF,
0x661FFFFF, 0xFFFFFFFE, 0xFFFFFFFF, 0x77FFFFFF,
};
static const unsigned char nmstrtPages[] = {
0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00,
0x00, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
0x10, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x13,
0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x15, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x17,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x18,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static const unsigned char namePages[] = {
0x19, 0x03, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x00,
0x00, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
0x10, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x13,
0x26, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x27, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x17,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x18,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
/* ==========================================================================
 * siphash.h - SipHash-2-4 in a single header file
 * --------------------------------------------------------------------------
 * Derived by William Ahern from the reference implementation[1] published[2]
 * by Jean-Philippe Aumasson and Daniel J. Berstein.
 * Minimal changes by Sebastian Pipping and Victor Stinner on top, see below.
 * Licensed under the CC0 Public Domain Dedication license.
 *
 * 1. https://www.131002.net/siphash/siphash24.c
 * 2. https://www.131002.net/siphash/
 * --------------------------------------------------------------------------
 * HISTORY:
 *
 * 2017-07-25  (Vadim Zeitlin)
 *   - Fix use of SIPHASH_MAIN macro
 *
 * 2017-07-05  (Sebastian Pipping)
 *   - Use _SIP_ULL macro to not require a C++11 compiler if compiled as C++
 *   - Add const qualifiers at two places
 *   - Ensure <=80 characters line length (assuming tab width 4)
 *
 * 2017-06-23  (Victor Stinner)
 *   - Address Win64 compile warnings
 *
 * 2017-06-18  (Sebastian Pipping)
 *   - Clarify license note in the header
 *   - Address C89 issues:
 *     - Stop using inline keyword (and let compiler decide)
 *     - Replace _Bool by int
 *     - Turn macro siphash24 into a function
 *     - Address invalid conversion (void pointer) by explicit cast
 *   - Address lack of stdint.h for Visual Studio 2003 to 2008
 *   - Always expose sip24_valid (for self-tests)
 *
 * 2012-11-04 - Born.  (William Ahern)
 * --------------------------------------------------------------------------
 * USAGE:
 *
 * SipHash-2-4 takes as input two 64-bit words as the key, some number of
 * message bytes, and outputs a 64-bit word as the message digest. This
 * implementation employs two data structures: a struct sipkey for
 * representing the key, and a struct siphash for representing the hash
 * state.
 *
 * For converting a 16-byte unsigned char array to a key, use either the
 * macro sip_keyof or the routine sip_tokey. The former instantiates a
 * compound literal key, while the latter requires a key object as a
 * parameter.
 *
 * 	unsigned char secret[16];
 * 	arc4random_buf(secret, sizeof secret);
 * 	struct sipkey *key = sip_keyof(secret);
 *
 * For hashing a message, use either the convenience macro siphash24 or the
 * routines sip24_init, sip24_update, and sip24_final.
 *
 * 	struct siphash state;
 * 	void *msg;
 * 	size_t len;
 * 	uint64_t hash;
 *
 * 	sip24_init(&state, key);
 * 	sip24_update(&state, msg, len);
 * 	hash = sip24_final(&state);
 *
 * or
 *
 * 	hash = siphash24(msg, len, key);
 *
 * To convert the 64-bit hash value to a canonical 8-byte little-endian
 * binary representation, use either the macro sip_binof or the routine
 * sip_tobin. The former instantiates and returns a compound literal array,
 * while the latter requires an array object as a parameter.
 * --------------------------------------------------------------------------
 * NOTES:
 *
 * o Neither sip_keyof, sip_binof, nor siphash24 will work with compilers
 *   lacking compound literal support. Instead, you must use the lower-level
 *   interfaces which take as parameters the temporary state objects.
 *
 * o Uppercase macros may evaluate parameters more than once. Lowercase
 *   macros should not exhibit any such side effects.
 * ==========================================================================
 */
#ifndef SIPHASH_H
#define SIPHASH_H

#include <stddef.h> /* size_t */

#if defined(_WIN32) && defined(_MSC_VER) && (_MSC_VER < 1600)
  /* For vs2003/7.1 up to vs2008/9.0; _MSC_VER 1600 is vs2010/10.0 */
  typedef unsigned __int8   uint8_t;
  typedef unsigned __int32 uint32_t;
  typedef unsigned __int64 uint64_t;
#else
 #include <stdint.h> /* uint64_t uint32_t uint8_t */
#endif


/*
 * Workaround to not require a C++11 compiler for using ULL suffix
 * if this code is included and compiled as C++; related GCC warning is:
 * warning: use of C++11 long long integer constant [-Wlong-long]
 */
#define _SIP_ULL(high, low)  (((uint64_t)high << 32) | low)


#define SIP_ROTL(x, b) (uint64_t)(((x) << (b)) | ( (x) >> (64 - (b))))

#define SIP_U32TO8_LE(p, v) \
	(p)[0] = (uint8_t)((v) >>  0); (p)[1] = (uint8_t)((v) >>  8); \
	(p)[2] = (uint8_t)((v) >> 16); (p)[3] = (uint8_t)((v) >> 24);

#define SIP_U64TO8_LE(p, v) \
	SIP_U32TO8_LE((p) + 0, (uint32_t)((v) >>  0)); \
	SIP_U32TO8_LE((p) + 4, (uint32_t)((v) >> 32));

#define SIP_U8TO64_LE(p) \
	(((uint64_t)((p)[0]) <<  0) | \
	 ((uint64_t)((p)[1]) <<  8) | \
	 ((uint64_t)((p)[2]) << 16) | \
	 ((uint64_t)((p)[3]) << 24) | \
	 ((uint64_t)((p)[4]) << 32) | \
	 ((uint64_t)((p)[5]) << 40) | \
	 ((uint64_t)((p)[6]) << 48) | \
	 ((uint64_t)((p)[7]) << 56))


#define SIPHASH_INITIALIZER { 0, 0, 0, 0, { 0 }, 0, 0 }

struct siphash {
	uint64_t v0, v1, v2, v3;

	unsigned char buf[8], *p;
	uint64_t c;
}; /* struct siphash */


#define SIP_KEYLEN 16

struct sipkey {
	uint64_t k[2];
}; /* struct sipkey */

#define sip_keyof(k) sip_tokey(&(struct sipkey){ { 0 } }, (k))

static struct sipkey *sip_tokey(struct sipkey *key, const void *src) {
	key->k[0] = SIP_U8TO64_LE((const unsigned char *)src);
	key->k[1] = SIP_U8TO64_LE((const unsigned char *)src + 8);
	return key;
} /* sip_tokey() */


#define sip_binof(v) sip_tobin((unsigned char[8]){ 0 }, (v))

static void *sip_tobin(void *dst, uint64_t u64) {
	SIP_U64TO8_LE((unsigned char *)dst, u64);
	return dst;
} /* sip_tobin() */


static void sip_round(struct siphash *H, const int rounds) {
	int i;

	for (i = 0; i < rounds; i++) {
		H->v0 += H->v1;
		H->v1 = SIP_ROTL(H->v1, 13);
		H->v1 ^= H->v0;
		H->v0 = SIP_ROTL(H->v0, 32);

		H->v2 += H->v3;
		H->v3 = SIP_ROTL(H->v3, 16);
		H->v3 ^= H->v2;

		H->v0 += H->v3;
		H->v3 = SIP_ROTL(H->v3, 21);
		H->v3 ^= H->v0;

		H->v2 += H->v1;
		H->v1 = SIP_ROTL(H->v1, 17);
		H->v1 ^= H->v2;
		H->v2 = SIP_ROTL(H->v2, 32);
	}
} /* sip_round() */


static struct siphash *sip24_init(struct siphash *H,
		const struct sipkey *key) {
	H->v0 = _SIP_ULL(0x736f6d65U, 0x70736575U) ^ key->k[0];
	H->v1 = _SIP_ULL(0x646f7261U, 0x6e646f6dU) ^ key->k[1];
	H->v2 = _SIP_ULL(0x6c796765U, 0x6e657261U) ^ key->k[0];
	H->v3 = _SIP_ULL(0x74656462U, 0x79746573U) ^ key->k[1];

	H->p = H->buf;
	H->c = 0;

	return H;
} /* sip24_init() */


#define sip_endof(a) (&(a)[sizeof (a) / sizeof *(a)])

static struct siphash *sip24_update(struct siphash *H, const void *src,
		size_t len) {
	const unsigned char *p = (const unsigned char *)src, *pe = p + len;
	uint64_t m;

	do {
		while (p < pe && H->p < sip_endof(H->buf))
			*H->p++ = *p++;

		if (H->p < sip_endof(H->buf))
			break;

		m = SIP_U8TO64_LE(H->buf);
		H->v3 ^= m;
		sip_round(H, 2);
		H->v0 ^= m;

		H->p = H->buf;
		H->c += 8;
	} while (p < pe);

	return H;
} /* sip24_update() */


static uint64_t sip24_final(struct siphash *H) {
	const char left = (char)(H->p - H->buf);
	uint64_t b = (H->c + left) << 56;

	switch (left) {
	case 7: b |= (uint64_t)H->buf[6] << 48;
	case 6: b |= (uint64_t)H->buf[5] << 40;
	case 5: b |= (uint64_t)H->buf[4] << 32;
	case 4: b |= (uint64_t)H->buf[3] << 24;
	case 3: b |= (uint64_t)H->buf[2] << 16;
	case 2: b |= (uint64_t)H->buf[1] << 8;
	case 1: b |= (uint64_t)H->buf[0] << 0;
	case 0: break;
	}

	H->v3 ^= b;
	sip_round(H, 2);
	H->v0 ^= b;
	H->v2 ^= 0xff;
	sip_round(H, 4);

	return H->v0 ^ H->v1 ^ H->v2  ^ H->v3;
} /* sip24_final() */


static uint64_t siphash24(const void *src, size_t len,
		const struct sipkey *key) {
	struct siphash state = SIPHASH_INITIALIZER;
	return sip24_final(sip24_update(sip24_init(&state, key), src, len));
} /* siphash24() */


/*
 * SipHash-2-4 output with
 * k = 00 01 02 ...
 * and
 * in = (empty string)
 * in = 00 (1 byte)
 * in = 00 01 (2 bytes)
 * in = 00 01 02 (3 bytes)
 * ...
 * in = 00 01 02 ... 3e (63 bytes)
 */
static int sip24_valid(void) {
	static const unsigned char vectors[64][8] = {
		{ 0x31, 0x0e, 0x0e, 0xdd, 0x47, 0xdb, 0x6f, 0x72, },
		{ 0xfd, 0x67, 0xdc, 0x93, 0xc5, 0x39, 0xf8, 0x74, },
		{ 0x5a, 0x4f, 0xa9, 0xd9, 0x09, 0x80, 0x6c, 0x0d, },
		{ 0x2d, 0x7e, 0xfb, 0xd7, 0x96, 0x66, 0x67, 0x85, },
		{ 0xb7, 0x87, 0x71, 0x27, 0xe0, 0x94, 0x27, 0xcf, },
		{ 0x8d, 0xa6, 0x99, 0xcd, 0x64, 0x55, 0x76, 0x18, },
		{ 0xce, 0xe3, 0xfe, 0x58, 0x6e, 0x46, 0xc9, 0xcb, },
		{ 0x37, 0xd1, 0x01, 0x8b, 0xf5, 0x00, 0x02, 0xab, },
		{ 0x62, 0x24, 0x93, 0x9a, 0x79, 0xf5, 0xf5, 0x93, },
		{ 0xb0, 0xe4, 0xa9, 0x0b, 0xdf, 0x82, 0x00, 0x9e, },
		{ 0xf3, 0xb9, 0xdd, 0x94, 0xc5, 0xbb, 0x5d, 0x7a, },
		{ 0xa7, 0xad, 0x6b, 0x22, 0x46, 0x2f, 0xb3, 0xf4, },
		{ 0xfb, 0xe5, 0x0e, 0x86, 0xbc, 0x8f, 0x1e, 0x75, },
		{ 0x90, 0x3d, 0x84, 0xc0, 0x27, 0x56, 0xea, 0x14, },
		{ 0xee, 0xf2, 0x7a, 0x8e, 0x90, 0xca, 0x23, 0xf7, },
		{ 0xe5, 0x45, 0xbe, 0x49, 0x61, 0xca, 0x29, 0xa1, },
		{ 0xdb, 0x9b, 0xc2, 0x57, 0x7f, 0xcc, 0x2a, 0x3f, },
		{ 0x94, 0x47, 0xbe, 0x2c, 0xf5, 0xe9, 0x9a, 0x69, },
		{ 0x9c, 0xd3, 0x8d, 0x96, 0xf0, 0xb3, 0xc1, 0x4b, },
		{ 0xbd, 0x61, 0x79, 0xa7, 0x1d, 0xc9, 0x6d, 0xbb, },
		{ 0x98, 0xee, 0xa2, 0x1a, 0xf2, 0x5c, 0xd6, 0xbe, },
		{ 0xc7, 0x67, 0x3b, 0x2e, 0xb0, 0xcb, 0xf2, 0xd0, },
		{ 0x88, 0x3e, 0xa3, 0xe3, 0x95, 0x67, 0x53, 0x93, },
		{ 0xc8, 0xce, 0x5c, 0xcd, 0x8c, 0x03, 0x0c, 0xa8, },
		{ 0x94, 0xaf, 0x49, 0xf6, 0xc6, 0x50, 0xad, 0xb8, },
		{ 0xea, 0xb8, 0x85, 0x8a, 0xde, 0x92, 0xe1, 0xbc, },
		{ 0xf3, 0x15, 0xbb, 0x5b, 0xb8, 0x35, 0xd8, 0x17, },
		{ 0xad, 0xcf, 0x6b, 0x07, 0x63, 0x61, 0x2e, 0x2f, },
		{ 0xa5, 0xc9, 0x1d, 0xa7, 0xac, 0xaa, 0x4d, 0xde, },
		{ 0x71, 0x65, 0x95, 0x87, 0x66, 0x50, 0xa2, 0xa6, },
		{ 0x28, 0xef, 0x49, 0x5c, 0x53, 0xa3, 0x87, 0xad, },
		{ 0x42, 0xc3, 0x41, 0xd8, 0xfa, 0x92, 0xd8, 0x32, },
		{ 0xce, 0x7c, 0xf2, 0x72, 0x2f, 0x51, 0x27, 0x71, },
		{ 0xe3, 0x78, 0x59, 0xf9, 0x46, 0x23, 0xf3, 0xa7, },
		{ 0x38, 0x12, 0x05, 0xbb, 0x1a, 0xb0, 0xe0, 0x12, },
		{ 0xae, 0x97, 0xa1, 0x0f, 0xd4, 0x34, 0xe0, 0x15, },
		{ 0xb4, 0xa3, 0x15, 0x08, 0xbe, 0xff, 0x4d, 0x31, },
		{ 0x81, 0x39, 0x62, 0x29, 0xf0, 0x90, 0x79, 0x02, },
		{ 0x4d, 0x0c, 0xf4, 0x9e, 0xe5, 0xd4, 0xdc, 0xca, },
		{ 0x5c, 0x73, 0x33, 0x6a, 0x76, 0xd8, 0xbf, 0x9a, },
		{ 0xd0, 0xa7, 0x04, 0x53, 0x6b, 0xa9, 0x3e, 0x0e, },
		{ 0x92, 0x59, 0x58, 0xfc, 0xd6, 0x42, 0x0c, 0xad, },
		{ 0xa9, 0x15, 0xc2, 0x9b, 0xc8, 0x06, 0x73, 0x18, },
		{ 0x95, 0x2b, 0x79, 0xf3, 0xbc, 0x0a, 0xa6, 0xd4, },
		{ 0xf2, 0x1d, 0xf2, 0xe4, 0x1d, 0x45, 0x35, 0xf9, },
		{ 0x87, 0x57, 0x75, 0x19, 0x04, 0x8f, 0x53, 0xa9, },
		{ 0x10, 0xa5, 0x6c, 0xf5, 0xdf, 0xcd, 0x9a, 0xdb, },
		{ 0xeb, 0x75, 0x09, 0x5c, 0xcd, 0x98, 0x6c, 0xd0, },
		{ 0x51, 0xa9, 0xcb, 0x9e, 0xcb, 0xa3, 0x12, 0xe6, },
		{ 0x96, 0xaf, 0xad, 0xfc, 0x2c, 0xe6, 0x66, 0xc7, },
		{ 0x72, 0xfe, 0x52, 0x97, 0x5a, 0x43, 0x64, 0xee, },
		{ 0x5a, 0x16, 0x45, 0xb2, 0x76, 0xd5, 0x92, 0xa1, },
		{ 0xb2, 0x74, 0xcb, 0x8e, 0xbf, 0x87, 0x87, 0x0a, },
		{ 0x6f, 0x9b, 0xb4, 0x20, 0x3d, 0xe7, 0xb3, 0x81, },
		{ 0xea, 0xec, 0xb2, 0xa3, 0x0b, 0x22, 0xa8, 0x7f, },
		{ 0x99, 0x24, 0xa4, 0x3c, 0xc1, 0x31, 0x57, 0x24, },
		{ 0xbd, 0x83, 0x8d, 0x3a, 0xaf, 0xbf, 0x8d, 0xb7, },
		{ 0x0b, 0x1a, 0x2a, 0x32, 0x65, 0xd5, 0x1a, 0xea, },
		{ 0x13, 0x50, 0x79, 0xa3, 0x23, 0x1c, 0xe6, 0x60, },
		{ 0x93, 0x2b, 0x28, 0x46, 0xe4, 0xd7, 0x06, 0x66, },
		{ 0xe1, 0x91, 0x5f, 0x5c, 0xb1, 0xec, 0xa4, 0x6c, },
		{ 0xf3, 0x25, 0x96, 0x5c, 0xa1, 0x6d, 0x62, 0x9f, },
		{ 0x57, 0x5f, 0xf2, 0x8e, 0x60, 0x38, 0x1b, 0xe5, },
		{ 0x72, 0x45, 0x06, 0xeb, 0x4c, 0x32, 0x8a, 0x95, }
	};
	unsigned char in[64];
	struct sipkey k;
	size_t i;

	sip_tokey(&k, "\000\001\002\003\004\005\006\007\010\011"
			"\012\013\014\015\016\017");

	for (i = 0; i < sizeof in; ++i) {
		in[i] = (unsigned char)i;

		if (siphash24(in, i, &k) != SIP_U8TO64_LE(vectors[i]))
			return 0;
	}

	return 1;
} /* sip24_valid() */


#ifdef SIPHASH_MAIN

#include <stdio.h>

int main(void) {
	const int ok = sip24_valid();

	if (ok)
		puts("OK");
	else
		puts("FAIL");

	return !ok;
} /* main() */

#endif /* SIPHASH_MAIN */


#endif /* SIPHASH_H */
/*
                            __  __            _
                         ___\ \/ /_ __   __ _| |_
                        / _ \\  /| '_ \ / _` | __|
                       |  __//  \| |_) | (_| | |_
                        \___/_/\_\ .__/ \__,_|\__|
                                 |_| XML parser

   Copyright (c) 1997-2000 Thai Open Source Software Center Ltd
   Copyright (c) 2000-2017 Expat development team
   Licensed under the MIT license:

   Permission is  hereby granted,  free of charge,  to any  person obtaining
   a  copy  of  this  software   and  associated  documentation  files  (the
   "Software"),  to  deal in  the  Software  without restriction,  including
   without  limitation the  rights  to use,  copy,  modify, merge,  publish,
   distribute, sublicense, and/or sell copies of the Software, and to permit
   persons  to whom  the Software  is  furnished to  do so,  subject to  the
   following conditions:

   The above copyright  notice and this permission notice  shall be included
   in all copies or substantial portions of the Software.

   THE  SOFTWARE  IS  PROVIDED  "AS  IS",  WITHOUT  WARRANTY  OF  ANY  KIND,
   EXPRESS  OR IMPLIED,  INCLUDING  BUT  NOT LIMITED  TO  THE WARRANTIES  OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
   NO EVENT SHALL THE AUTHORS OR  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
   DAMAGES OR  OTHER LIABILITY, WHETHER  IN AN  ACTION OF CONTRACT,  TORT OR
   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
   USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef XmlTok_INCLUDED
#define XmlTok_INCLUDED 1

#ifdef __cplusplus
extern "C" {
#endif

/* The following token may be returned by XmlContentTok */
#define XML_TOK_TRAILING_RSQB -5 /* ] or ]] at the end of the scan; might be
                                    start of illegal ]]> sequence */
/* The following tokens may be returned by both XmlPrologTok and
   XmlContentTok.
*/
#define XML_TOK_NONE -4          /* The string to be scanned is empty */
#define XML_TOK_TRAILING_CR -3   /* A CR at the end of the scan;
                                    might be part of CRLF sequence */
#define XML_TOK_PARTIAL_CHAR -2  /* only part of a multibyte sequence */
#define XML_TOK_PARTIAL -1       /* only part of a token */
#define XML_TOK_INVALID 0

/* The following tokens are returned by XmlContentTok; some are also
   returned by XmlAttributeValueTok, XmlEntityTok, XmlCdataSectionTok.
*/
#define XML_TOK_START_TAG_WITH_ATTS 1
#define XML_TOK_START_TAG_NO_ATTS 2
#define XML_TOK_EMPTY_ELEMENT_WITH_ATTS 3 /* empty element tag <e/> */
#define XML_TOK_EMPTY_ELEMENT_NO_ATTS 4
#define XML_TOK_END_TAG 5
#define XML_TOK_DATA_CHARS 6
#define XML_TOK_DATA_NEWLINE 7
#define XML_TOK_CDATA_SECT_OPEN 8
#define XML_TOK_ENTITY_REF 9
#define XML_TOK_CHAR_REF 10               /* numeric character reference */

/* The following tokens may be returned by both XmlPrologTok and
   XmlContentTok.
*/
#define XML_TOK_PI 11                     /* processing instruction */
#define XML_TOK_XML_DECL 12               /* XML decl or text decl */
#define XML_TOK_COMMENT 13
#define XML_TOK_BOM 14                    /* Byte order mark */

/* The following tokens are returned only by XmlPrologTok */
#define XML_TOK_PROLOG_S 15
#define XML_TOK_DECL_OPEN 16              /* <!foo */
#define XML_TOK_DECL_CLOSE 17             /* > */
#define XML_TOK_NAME 18
#define XML_TOK_NMTOKEN 19
#define XML_TOK_POUND_NAME 20             /* #name */
#define XML_TOK_OR 21                     /* | */
#define XML_TOK_PERCENT 22
#define XML_TOK_OPEN_PAREN 23
#define XML_TOK_CLOSE_PAREN 24
#define XML_TOK_OPEN_BRACKET 25
#define XML_TOK_CLOSE_BRACKET 26
#define XML_TOK_LITERAL 27
#define XML_TOK_PARAM_ENTITY_REF 28
#define XML_TOK_INSTANCE_START 29

/* The following occur only in element type declarations */
#define XML_TOK_NAME_QUESTION 30          /* name? */
#define XML_TOK_NAME_ASTERISK 31          /* name* */
#define XML_TOK_NAME_PLUS 32              /* name+ */
#define XML_TOK_COND_SECT_OPEN 33         /* <![ */
#define XML_TOK_COND_SECT_CLOSE 34        /* ]]> */
#define XML_TOK_CLOSE_PAREN_QUESTION 35   /* )? */
#define XML_TOK_CLOSE_PAREN_ASTERISK 36   /* )* */
#define XML_TOK_CLOSE_PAREN_PLUS 37       /* )+ */
#define XML_TOK_COMMA 38

/* The following token is returned only by XmlAttributeValueTok */
#define XML_TOK_ATTRIBUTE_VALUE_S 39

/* The following token is returned only by XmlCdataSectionTok */
#define XML_TOK_CDATA_SECT_CLOSE 40

/* With namespace processing this is returned by XmlPrologTok for a
   name with a colon.
*/
#define XML_TOK_PREFIXED_NAME 41

#ifdef XML_DTD
#define XML_TOK_IGNORE_SECT 42
#endif /* XML_DTD */

#ifdef XML_DTD
#define XML_N_STATES 4
#else /* not XML_DTD */
#define XML_N_STATES 3
#endif /* not XML_DTD */

#define XML_PROLOG_STATE 0
#define XML_CONTENT_STATE 1
#define XML_CDATA_SECTION_STATE 2
#ifdef XML_DTD
#define XML_IGNORE_SECTION_STATE 3
#endif /* XML_DTD */

#define XML_N_LITERAL_TYPES 2
#define XML_ATTRIBUTE_VALUE_LITERAL 0
#define XML_ENTITY_VALUE_LITERAL 1

/* The size of the buffer passed to XmlUtf8Encode must be at least this. */
#define XML_UTF8_ENCODE_MAX 4
/* The size of the buffer passed to XmlUtf16Encode must be at least this. */
#define XML_UTF16_ENCODE_MAX 2

typedef struct position {
  /* first line and first column are 0 not 1 */
  XML_Size lineNumber;
  XML_Size columnNumber;
} POSITION;

typedef struct {
  const char *name;
  const char *valuePtr;
  const char *valueEnd;
  char normalized;
} ATTRIBUTE;

struct encoding;
typedef struct encoding ENCODING;

typedef int (PTRCALL *SCANNER)(const ENCODING *,
                               const char *,
                               const char *,
                               const char **);

enum XML_Convert_Result {
  XML_CONVERT_COMPLETED = 0,
  XML_CONVERT_INPUT_INCOMPLETE = 1,
  XML_CONVERT_OUTPUT_EXHAUSTED = 2  /* and therefore potentially input remaining as well */
};

struct encoding {
  SCANNER scanners[XML_N_STATES];
  SCANNER literalScanners[XML_N_LITERAL_TYPES];
  int (PTRCALL *nameMatchesAscii)(const ENCODING *,
                                  const char *,
                                  const char *,
                                  const char *);
  int (PTRFASTCALL *nameLength)(const ENCODING *, const char *);
  const char *(PTRFASTCALL *skipS)(const ENCODING *, const char *);
  int (PTRCALL *getAtts)(const ENCODING *enc,
                         const char *ptr,
                         int attsMax,
                         ATTRIBUTE *atts);
  int (PTRFASTCALL *charRefNumber)(const ENCODING *enc, const char *ptr);
  int (PTRCALL *predefinedEntityName)(const ENCODING *,
                                      const char *,
                                      const char *);
  void (PTRCALL *updatePosition)(const ENCODING *,
                                 const char *ptr,
                                 const char *end,
                                 POSITION *);
  int (PTRCALL *isPublicId)(const ENCODING *enc,
                            const char *ptr,
                            const char *end,
                            const char **badPtr);
  enum XML_Convert_Result (PTRCALL *utf8Convert)(const ENCODING *enc,
                              const char **fromP,
                              const char *fromLim,
                              char **toP,
                              const char *toLim);
  enum XML_Convert_Result (PTRCALL *utf16Convert)(const ENCODING *enc,
                               const char **fromP,
                               const char *fromLim,
                               unsigned short **toP,
                               const unsigned short *toLim);
  int minBytesPerChar;
  char isUtf8;
  char isUtf16;
};

/* Scan the string starting at ptr until the end of the next complete
   token, but do not scan past eptr.  Return an integer giving the
   type of token.

   Return XML_TOK_NONE when ptr == eptr; nextTokPtr will not be set.

   Return XML_TOK_PARTIAL when the string does not contain a complete
   token; nextTokPtr will not be set.

   Return XML_TOK_INVALID when the string does not start a valid
   token; nextTokPtr will be set to point to the character which made
   the token invalid.

   Otherwise the string starts with a valid token; nextTokPtr will be
   set to point to the character following the end of that token.

   Each data character counts as a single token, but adjacent data
   characters may be returned together.  Similarly for characters in
   the prolog outside literals, comments and processing instructions.
*/


#define XmlTok(enc, state, ptr, end, nextTokPtr) \
  (((enc)->scanners[state])(enc, ptr, end, nextTokPtr))

#define XmlPrologTok(enc, ptr, end, nextTokPtr) \
   XmlTok(enc, XML_PROLOG_STATE, ptr, end, nextTokPtr)

#define XmlContentTok(enc, ptr, end, nextTokPtr) \
   XmlTok(enc, XML_CONTENT_STATE, ptr, end, nextTokPtr)

#define XmlCdataSectionTok(enc, ptr, end, nextTokPtr) \
   XmlTok(enc, XML_CDATA_SECTION_STATE, ptr, end, nextTokPtr)

#ifdef XML_DTD

#define XmlIgnoreSectionTok(enc, ptr, end, nextTokPtr) \
   XmlTok(enc, XML_IGNORE_SECTION_STATE, ptr, end, nextTokPtr)

#endif /* XML_DTD */

/* This is used for performing a 2nd-level tokenization on the content
   of a literal that has already been returned by XmlTok.
*/
#define XmlLiteralTok(enc, literalType, ptr, end, nextTokPtr) \
  (((enc)->literalScanners[literalType])(enc, ptr, end, nextTokPtr))

#define XmlAttributeValueTok(enc, ptr, end, nextTokPtr) \
   XmlLiteralTok(enc, XML_ATTRIBUTE_VALUE_LITERAL, ptr, end, nextTokPtr)

#define XmlEntityValueTok(enc, ptr, end, nextTokPtr) \
   XmlLiteralTok(enc, XML_ENTITY_VALUE_LITERAL, ptr, end, nextTokPtr)

#define XmlNameMatchesAscii(enc, ptr1, end1, ptr2) \
  (((enc)->nameMatchesAscii)(enc, ptr1, end1, ptr2))

#define XmlNameLength(enc, ptr) \
  (((enc)->nameLength)(enc, ptr))

#define XmlSkipS(enc, ptr) \
  (((enc)->skipS)(enc, ptr))

#define XmlGetAttributes(enc, ptr, attsMax, atts) \
  (((enc)->getAtts)(enc, ptr, attsMax, atts))

#define XmlCharRefNumber(enc, ptr) \
  (((enc)->charRefNumber)(enc, ptr))

#define XmlPredefinedEntityName(enc, ptr, end) \
  (((enc)->predefinedEntityName)(enc, ptr, end))

#define XmlUpdatePosition(enc, ptr, end, pos) \
  (((enc)->updatePosition)(enc, ptr, end, pos))

#define XmlIsPublicId(enc, ptr, end, badPtr) \
  (((enc)->isPublicId)(enc, ptr, end, badPtr))

#define XmlUtf8Convert(enc, fromP, fromLim, toP, toLim) \
  (((enc)->utf8Convert)(enc, fromP, fromLim, toP, toLim))

#define XmlUtf16Convert(enc, fromP, fromLim, toP, toLim) \
  (((enc)->utf16Convert)(enc, fromP, fromLim, toP, toLim))

typedef struct {
  ENCODING initEnc;
  const ENCODING **encPtr;
} INIT_ENCODING;

int XmlParseXmlDecl(int isGeneralTextEntity,
                    const ENCODING *enc,
                    const char *ptr,
                    const char *end,
                    const char **badPtr,
                    const char **versionPtr,
                    const char **versionEndPtr,
                    const char **encodingNamePtr,
                    const ENCODING **namedEncodingPtr,
                    int *standalonePtr);

int XmlInitEncoding(INIT_ENCODING *, const ENCODING **, const char *name);
const ENCODING *XmlGetUtf8InternalEncoding(void);
const ENCODING *XmlGetUtf16InternalEncoding(void);
int FASTCALL XmlUtf8Encode(int charNumber, char *buf);
int FASTCALL XmlUtf16Encode(int charNumber, unsigned short *buf);
int XmlSizeOfUnknownEncoding(void);


typedef int (XMLCALL *CONVERTER) (void *userData, const char *p);

ENCODING *
XmlInitUnknownEncoding(void *mem,
                       int *table,
                       CONVERTER convert,
                       void *userData);

int XmlParseXmlDeclNS(int isGeneralTextEntity,
                      const ENCODING *enc,
                      const char *ptr,
                      const char *end,
                      const char **badPtr,
                      const char **versionPtr,
                      const char **versionEndPtr,
                      const char **encodingNamePtr,
                      const ENCODING **namedEncodingPtr,
                      int *standalonePtr);

int XmlInitEncodingNS(INIT_ENCODING *, const ENCODING **, const char *name);
const ENCODING *XmlGetUtf8InternalEncodingNS(void);
const ENCODING *XmlGetUtf16InternalEncodingNS(void);
ENCODING *
XmlInitUnknownEncodingNS(void *mem,
                         int *table,
                         CONVERTER convert,
                         void *userData);
#ifdef __cplusplus
}
#endif

#endif /* not XmlTok_INCLUDED */
/*
                            __  __            _
                         ___\ \/ /_ __   __ _| |_
                        / _ \\  /| '_ \ / _` | __|
                       |  __//  \| |_) | (_| | |_
                        \___/_/\_\ .__/ \__,_|\__|
                                 |_| XML parser

   Copyright (c) 1997-2000 Thai Open Source Software Center Ltd
   Copyright (c) 2000-2017 Expat development team
   Licensed under the MIT license:

   Permission is  hereby granted,  free of charge,  to any  person obtaining
   a  copy  of  this  software   and  associated  documentation  files  (the
   "Software"),  to  deal in  the  Software  without restriction,  including
   without  limitation the  rights  to use,  copy,  modify, merge,  publish,
   distribute, sublicense, and/or sell copies of the Software, and to permit
   persons  to whom  the Software  is  furnished to  do so,  subject to  the
   following conditions:

   The above copyright  notice and this permission notice  shall be included
   in all copies or substantial portions of the Software.

   THE  SOFTWARE  IS  PROVIDED  "AS  IS",  WITHOUT  WARRANTY  OF  ANY  KIND,
   EXPRESS  OR IMPLIED,  INCLUDING  BUT  NOT LIMITED  TO  THE WARRANTIES  OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
   NO EVENT SHALL THE AUTHORS OR  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
   DAMAGES OR  OTHER LIABILITY, WHETHER  IN AN  ACTION OF CONTRACT,  TORT OR
   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
   USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef XmlRole_INCLUDED
#define XmlRole_INCLUDED 1

#ifdef __VMS
/*      0        1         2         3      0        1         2         3
        1234567890123456789012345678901     1234567890123456789012345678901 */
#define XmlPrologStateInitExternalEntity    XmlPrologStateInitExternalEnt
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
  XML_ROLE_ERROR = -1,
  XML_ROLE_NONE = 0,
  XML_ROLE_XML_DECL,
  XML_ROLE_INSTANCE_START,
  XML_ROLE_DOCTYPE_NONE,
  XML_ROLE_DOCTYPE_NAME,
  XML_ROLE_DOCTYPE_SYSTEM_ID,
  XML_ROLE_DOCTYPE_PUBLIC_ID,
  XML_ROLE_DOCTYPE_INTERNAL_SUBSET,
  XML_ROLE_DOCTYPE_CLOSE,
  XML_ROLE_GENERAL_ENTITY_NAME,
  XML_ROLE_PARAM_ENTITY_NAME,
  XML_ROLE_ENTITY_NONE,
  XML_ROLE_ENTITY_VALUE,
  XML_ROLE_ENTITY_SYSTEM_ID,
  XML_ROLE_ENTITY_PUBLIC_ID,
  XML_ROLE_ENTITY_COMPLETE,
  XML_ROLE_ENTITY_NOTATION_NAME,
  XML_ROLE_NOTATION_NONE,
  XML_ROLE_NOTATION_NAME,
  XML_ROLE_NOTATION_SYSTEM_ID,
  XML_ROLE_NOTATION_NO_SYSTEM_ID,
  XML_ROLE_NOTATION_PUBLIC_ID,
  XML_ROLE_ATTRIBUTE_NAME,
  XML_ROLE_ATTRIBUTE_TYPE_CDATA,
  XML_ROLE_ATTRIBUTE_TYPE_ID,
  XML_ROLE_ATTRIBUTE_TYPE_IDREF,
  XML_ROLE_ATTRIBUTE_TYPE_IDREFS,
  XML_ROLE_ATTRIBUTE_TYPE_ENTITY,
  XML_ROLE_ATTRIBUTE_TYPE_ENTITIES,
  XML_ROLE_ATTRIBUTE_TYPE_NMTOKEN,
  XML_ROLE_ATTRIBUTE_TYPE_NMTOKENS,
  XML_ROLE_ATTRIBUTE_ENUM_VALUE,
  XML_ROLE_ATTRIBUTE_NOTATION_VALUE,
  XML_ROLE_ATTLIST_NONE,
  XML_ROLE_ATTLIST_ELEMENT_NAME,
  XML_ROLE_IMPLIED_ATTRIBUTE_VALUE,
  XML_ROLE_REQUIRED_ATTRIBUTE_VALUE,
  XML_ROLE_DEFAULT_ATTRIBUTE_VALUE,
  XML_ROLE_FIXED_ATTRIBUTE_VALUE,
  XML_ROLE_ELEMENT_NONE,
  XML_ROLE_ELEMENT_NAME,
  XML_ROLE_CONTENT_ANY,
  XML_ROLE_CONTENT_EMPTY,
  XML_ROLE_CONTENT_PCDATA,
  XML_ROLE_GROUP_OPEN,
  XML_ROLE_GROUP_CLOSE,
  XML_ROLE_GROUP_CLOSE_REP,
  XML_ROLE_GROUP_CLOSE_OPT,
  XML_ROLE_GROUP_CLOSE_PLUS,
  XML_ROLE_GROUP_CHOICE,
  XML_ROLE_GROUP_SEQUENCE,
  XML_ROLE_CONTENT_ELEMENT,
  XML_ROLE_CONTENT_ELEMENT_REP,
  XML_ROLE_CONTENT_ELEMENT_OPT,
  XML_ROLE_CONTENT_ELEMENT_PLUS,
  XML_ROLE_PI,
  XML_ROLE_COMMENT,
#ifdef XML_DTD
  XML_ROLE_TEXT_DECL,
  XML_ROLE_IGNORE_SECT,
  XML_ROLE_INNER_PARAM_ENTITY_REF,
#endif /* XML_DTD */
  XML_ROLE_PARAM_ENTITY_REF
};

typedef struct prolog_state {
  int (PTRCALL *handler) (struct prolog_state *state,
                          int tok,
                          const char *ptr,
                          const char *end,
                          const ENCODING *enc);
  unsigned level;
  int role_none;
#ifdef XML_DTD
  unsigned includeLevel;
  int documentEntity;
  int inEntityValue;
#endif /* XML_DTD */
} PROLOG_STATE;

void XmlPrologStateInit(PROLOG_STATE *);
#ifdef XML_DTD
void XmlPrologStateInitExternalEntity(PROLOG_STATE *);
#endif /* XML_DTD */

#define XmlTokenRole(state, tok, ptr, end, enc) \
 (((state)->handler)(state, tok, ptr, end, enc))

#ifdef __cplusplus
}
#endif

#endif /* not XmlRole_INCLUDED */
/*
                            __  __            _
                         ___\ \/ /_ __   __ _| |_
                        / _ \\  /| '_ \ / _` | __|
                       |  __//  \| |_) | (_| | |_
                        \___/_/\_\ .__/ \__,_|\__|
                                 |_| XML parser

   Copyright (c) 1997-2000 Thai Open Source Software Center Ltd
   Copyright (c) 2000-2017 Expat development team
   Licensed under the MIT license:

   Permission is  hereby granted,  free of charge,  to any  person obtaining
   a  copy  of  this  software   and  associated  documentation  files  (the
   "Software"),  to  deal in  the  Software  without restriction,  including
   without  limitation the  rights  to use,  copy,  modify, merge,  publish,
   distribute, sublicense, and/or sell copies of the Software, and to permit
   persons  to whom  the Software  is  furnished to  do so,  subject to  the
   following conditions:

   The above copyright  notice and this permission notice  shall be included
   in all copies or substantial portions of the Software.

   THE  SOFTWARE  IS  PROVIDED  "AS  IS",  WITHOUT  WARRANTY  OF  ANY  KIND,
   EXPRESS  OR IMPLIED,  INCLUDING  BUT  NOT LIMITED  TO  THE WARRANTIES  OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
   NO EVENT SHALL THE AUTHORS OR  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
   DAMAGES OR  OTHER LIABILITY, WHETHER  IN AN  ACTION OF CONTRACT,  TORT OR
   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
   USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

enum {
  BT_NONXML,
  BT_MALFORM,
  BT_LT,
  BT_AMP,
  BT_RSQB,
  BT_LEAD2,
  BT_LEAD3,
  BT_LEAD4,
  BT_TRAIL,
  BT_CR,
  BT_LF,
  BT_GT,
  BT_QUOT,
  BT_APOS,
  BT_EQUALS,
  BT_QUEST,
  BT_EXCL,
  BT_SOL,
  BT_SEMI,
  BT_NUM,
  BT_LSQB,
  BT_S,
  BT_NMSTRT,
  BT_COLON,
  BT_HEX,
  BT_DIGIT,
  BT_NAME,
  BT_MINUS,
  BT_OTHER, /* known not to be a name or name start character */
  BT_NONASCII, /* might be a name or name start character */
  BT_PERCNT,
  BT_LPAR,
  BT_RPAR,
  BT_AST,
  BT_PLUS,
  BT_COMMA,
  BT_VERBAR
};

#include <stddef.h>
