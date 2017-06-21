/*
	File: cdzf.h

	Structures used for $ZF callout functions

	Copyright (c) 1986-2013 by InterSystems.
	Cambridge, Massachusetts, U.S.A.  All rights reserved.

	NOTE - because this file is included in C programs and the
	C compiler on some platforms (PowerPC) does not support
	C++ style comments by default, the // style of comments
	should not be used here.

Modification History: (most recent first)

SOH392	 8/26/13 Steven Hobbs, Support CC/NAME=AS_IS on VMS
RFD587  11/15/10 Fix PRIVS feature for VMS $zf() commands
SAP1195 6/12/09 Provide long string list datatype
CDS1087	6/07/07 Fix GetZFTable export for c as well as c++
CDS917 11/07/06 Make GetZFTable export visible on macosx
SAP653  7/30/06 Change ccallin to callin
LRS1034 3/21/06 Implement 4-byte wchar_t wrappers and functions (2-byte wchar_t
		functions equate to Unicode)
CDS738	1/11/06 Native IEEE support
LRS1013 1/03/06 Prototype functions better under Windows, export GetZFTable
KMK357 12/05/05	Define long string paraphernalia.
JLC782 10/19/05	Don't use pointer_size #pragmas on VMS for C++ modules.
LRS793	7/08/04	change ZF_FAILURE from 3 to 2 under VMS to throw an error
LRS755	2/17/04	change some argument prototypes, avoid repeated #include
LRS666	2/21/03	change macro UNICODE to CACHE_UNICODE to avoid conflict
LRS605	6/03/02	add defines ZF_SUCCESS and ZF_FAILURE
LRS582	4/09/02	add types 'h', 'q', and 'l', for Unicode boundary translation
JLC334	4/18/01	adapt to 64-bit VMS. Make all pointers short (32-bit).
CDS188	4/12/01 system oref type
CLN040	9/06/00	Cleaned
CDS073	5/03/00 add type 'k' for argstk pointer.  This is for internal
		use only!
LRS421	3/13/00 add tests for C++ compilation, add 'const' to struct zfestr
LRS412	1/28/00 add zfresjob() to callback function table, modified some
		declarations to allow compiling with C++
LRS409	1/27/00 add callback function table for DLL's and shared objects,
		restructure this source file
CDS057 12/08/99 Add type 'y' for generic void pointer.  This is for
		internal use only!
LRS302	1/29/99 modify procedure for defining a DLL or shared object. The
		ZFDLL macro invocation will no longer be used, instead just
		define ZF_DLL for any platform. ZFDLLBEGIN is replaced by
		the standard ZFBEGIN call.
RYF091	7/29/98 add type 'z' for call-in linkage vector. This is a pseudo
	        argument in that it does not correspond to an argument in
		the $ZF call (although the position must be filled in in
		the $ZF call) but it passes a pointer to a CACHEEFT
		(see callin header files) structure which is the set of
		vectors you need to call back into Cache. This is used by
		DLL's which do not link against the image and so cannot
		call these routine directly. Change #if OSVMS to #ifdef __VMS
		as this must build w/out our standard include files.
RAM631	9/08/97 added the definition of the ZFDLL macro, which generates
		the GetZFTable function for a DLL containing user-written
		routines which are to be invoked via the
		$ZF(-3,"dll_name","routine_name") function.
MEB040	6/02/97	add support for translated character strings
MEB027	3/20/97	add support for Unicode character strings
CFL001  7/29/96 added prototypes for signal handling functions.
RJW050	7/03/96	create dll version of openm. 
AHS069	2/25/93	Support for byte arrays.
JAO552  1/12/93 port to VMS.
SPJ169	9/14/88	Rewrote to fix bugs and make portable
KSO212  8/11/88	Part of $ZF for common code.

***********************************************************************/

#ifndef	_cdzf_h_						/* LRS755 */
#define	_cdzf_h_						/* LRS755 */

#ifdef	__VMS							/* SOH392+ */
/* #define definitions to support VMS compiles with the CC/NAMES=AS_IS
   qualifier.  Mixed case external names are replaced with names containg
   all upper case.

   This header file supports both CC/NAMES=AS_IS and CC/NAMES=UPPERCASE 
   on VMS */

#ifndef	ZF_DLL
#define zfstrsize	ZFSTRSIZE
#define zferror		ZFERROR
#define zfresjob	ZFRESJOB
#define CacheMgrDirA	CACHEMGRDIRA
#define CacheMgrDirW	CACHEMGRDIRW

#if	CACHE_ZHARR
#define CacheMgrDirH	CACHEMGRDIRH
#endif

#else /* #ifdef ZF_DLL */

#define cachefunctabp	CACHEFUNCTABP
#define cachecallbackp	CACHECALLBACKP
#define zfedll		ZFEDLL
#define GetZFTable	GETZFTABLE
#endif /* ZF_DLL */

#define zfe		ZFE
#endif	/* __VMS */						/* SOH392- */

#ifdef	__cplusplus						/* LRS421+ */
extern	"C" {
#endif								/* LRS421- */

/*	Flag indicating 4-byte wchar_t support */		/* LRS1034+ */
#if	defined(_WIN32)
#define	CACHE_ZHARR	0
#else
#define	CACHE_ZHARR	1
#endif								/* LRS1034- */

#if	defined(__VMS) && !defined(__cplusplus)		/* JLC782, JLC334+ */
#pragma	__required_pointer_size	__save
#pragma	__required_pointer_size	__short
#endif							/* JLC782, JLC334- */

/*
	Counted string argument descriptors
*/
typedef struct zarray {						/* AHS069+ */
	unsigned short	len;
	unsigned char	data[1];	/* 1 is a dummy value */
	} *ZARRAYP;						/* AHS069- */

typedef struct zwarray {					/* MEB027+ */
	unsigned short	len;
	unsigned short	data[1];	/* 1 is a dummy value */
	} *ZWARRAYP;						/* MEB027- */

#if	CACHE_ZHARR						/* LRS1034+ */
#include <wchar.h>
typedef struct zharray {
	unsigned int	len;
	/* if sizeof(wchar_t) > sizeof(int), pad for alignment here */
	wchar_t		data[1];	/* 1 is a dummy value */
	} *ZHARRAYP;
#else
typedef	ZWARRAYP	ZHARRAYP;
#endif	/* CACHE_ZHARR */					/* LRS1034- */

/*	Argument descriptor values 

        Argument descriptor characters may be either lowercase or uppercase.
        The lowercase value is used in switch statement to distinguish
        argument types. Uppercase counterparts indicate that the argument
        is an output argument.

*/
#define	DPBYTE	  'b'	/* Counted byte array */		/* AHS069 */
#define DPCHAR	  'c'	/* Zero terminated string (8 bit characters) */
#define DPDBL	  'd'	/* Double */
#define DPFLOAT	  'f'	/* Float */
#define	DCVLLIST  'g'	/* Long string $LIST object*/           /* SAP1195 */
#define	DCVWSTR   'h'	/* Counted wide string with translation */
#define DINT	  'i'	/* Integer */
#define	DLSTR	  'j'	/* long string (8-bit characters, counted) */
#define DPARGSTK  'k'	/* Pointer to argstk element */		/* CDS073 */
#define	DCVLIST	  'l'	/* $LIST object with translation */
#define	DTLSTR	  'm'	/* translated long string (8-bit characters, counted) */
#define	DWLSTR	  'n'	/* wide long string (counted) */
#define DOREF	  'o'	/* Object reference */			/* CDS188 */
#define DPINT	  'p'	/* Pointer to integer */
#define	DCVWCHR	  'q'	/* Zero terminated string with translation */
#define	DTWLSTR	  'r'	/* translated wide long string (counted)   KMK357 */
#define	DPCWSTR   's'	/* Counted string (16 bit characters) *//* MEB027+ */
#define	DPTSTR	  't'	/* Translated string (in byte array) */	/* MEB040 */
#define DPVDESC	  'v'	/* VMS fixed length string descriptor */
#define	DPWCHAR	  'w'	/* Zero terminated string (16 bit characters) */
#define DPVOID	  'y'	/* Generic void pointer */		/* CDS057 */
#define DCILINK   'z'	/* Call-in linkage vector. input only. Need */
			/* to include callin.h as well */
								/* LRS582- */
/* The following macros work with an argument descriptor character
   that may be either upper or lower case.
*/
#define ZFARGTYPE(x)  ((x) | 0x20)	/* OR in bit coerces to lowercase */
#define ISZFOUTARG(x) !((x) & 0x20)	/* Check for absence of bit that
					   indicates uppercase character */
/* Prefix character designating IEEE format */
#define	DIEEENUM  '#'	/* See next char for real type */	/* CDS738 */

/* Expanded argument descriptor prototypes:			   LRS1034+
	Since new data types are being introduced at a faster
	rate than characters are being added to the alphabet,
	the following two-character convention (similar to that
	employed for IEEE number formats) has been adopted for
	various string data types. Both prior and new syntaxes,
	where applicable, are accepted:

	prior	new	meaning
	 'b'	'1b'	counted 8-bit character array
	 'c'	'1c'	zero-terminated 8-bit character string
	 's'	'2b'	counted 16-bit character array
	 'w'	'2c'	zero-terminated 16-bit character string
	 	'4b'	counted wchar_t array
	 	'4c'	zero-terminated wchar_t string
	 	'1h'	counted 8-bit character array w/ translation*
	 	'1q'	zero-terminated 8-bit character string w/ translation*
	 'h'	'2h'	counted 16-bit character array w/ translation
	 'q'	'2q'	zero-terminated 16-bit character string w/ translation
	 	'4h'	counted wchar_t array w/ translation
	 	'4q'	zero-terminated wchar_t string w/ translation
	 'j'	'1j'	counted 8-bit character long string array
	 'n'	'2j'	counted 16-bit character long string array
	 	'4j'	counted wchar_t long string array

	The size of wide character type wchar_t is system dependent.

	* Types '1h' and '1q' are not currently implemented.
*/								/* LRS1034- */

#ifdef	__VMS							/* JAO552, RFD587+ */
#define NOPRIV 0	/* Execute $ZF function at user's priv level */
#define PRIV   1	/* Execute $ZF function at elevated priv level */
#endif								/* JAO552, RFD587- */

typedef	int	(*zffunc)();

/*
	Table of remote routine descriptors
*/
struct zfestr {
	const char  *zfename;	/* Address of function name string */
	const char  *zfead;	/* Address of argument descriptor string */
	zffunc	     zfeep;	/* Function entry point address */
#ifdef	__VMS							/* JAO552+ */
	int	     zfprivs;	/* Set if we drop privs for this $ZF func */
#endif								/* JAO552- */
};

/*	To allow czf.c files to be moved unchanged between VMS and other
	systems (eg. Unix and NT), the following macros have the following
	effects:
		ZFENTRY on VMS defines a $ZF function that executes at
			the caller's normal privilege level. This is the
			way $ZF functions behave on Unix.
		ZFPRIV allows the programmer to specify whether the $ZF
			function should execute at the caller's privilege
			level or at the elevated privilege level processes
			have when they're in Cache (all privs except share).
			For Unix this function is identical to ZFENTRY as
			all $ZF functions execute at the caller's level.

	Note: The original default behavior on the VAX was to execute
	      $ZF functions at an elevated privilege level. This module
	      defaults (via ZFENTRY) to a normal privilege level.
*/

								/* LRS409+ */
/*
	Function Prototypes
*/

#ifdef	_WIN32							/* LRS1013+ */
#define	ZFCALL			__cdecl
#define	ZFEXPO			__declspec( dllexport )
#elif defined(__APPLE__) && defined(__GNUC__) && (__GNUC__ >= 4)    /* CDS1087 */
#define	ZFCALL			
#define	ZFEXPO		__attribute__((visibility("default")))	/* CDS917 */
#else
#define	ZFCALL
#define	ZFEXPO
#endif

#if	defined(__STDC__) || defined(__cplusplus)		/* LRS412 */
#define	ZFPROTOD(func,args)	ZFCALL func args
#define	ZFPROTOI(func,args)	(ZFCALL *func) args
#else
#define	ZFPROTOD(func,args)	ZFCALL func ()
#define	ZFPROTOI(func,args)	(ZFCALL *func) ()
#endif

typedef const struct  CacheCallbackTab {
   int	ZFPROTOI(ZFSTRSIZE,(void));
   int	ZFPROTOI(ZFERROR,(int,int));
   int	ZFPROTOI(CACHEMGRDIRA,(int,char *));
   int	ZFPROTOI(CACHEMGRDIRW,(int,unsigned short *));
#if	!defined(__VMS) && !defined(_WIN32)	/* These are only for UNIX */
   int	ZFPROTOI(SIGRTCHK,(void));
   int	ZFPROTOI(SIGRTCLR,(void));
   int	ZFPROTOI(DZFALARM,());
#endif
   int	ZFPROTOI(ZFRESJOB,(void));				/* LRS412 */
#if	CACHE_ZHARR						/* LRS1034+ */
   int	ZFPROTOI(CACHEMGRDIRH,(int,wchar_t *));
#endif	/* CACHE_ZHARR */					/* LRS1034- */
} CACHECBT, *CACHECBTP;

#ifndef	ZF_DLL		/*==============================*/

/*
	Direct (linked-in) Callback Function Declarations

	zfstrsize - returns max length of $ZF reply string
	zferror - used to report errors as though from within Cache
	CacheMgrDirA - obtains Cache manager's directory (narrow)
	CacheMgrDirW - obtains Cache manager's directory (wide)
	CacheMgrDirH - obtains Cache manager's directory (wchar_t)
*/
extern	int	ZFPROTOD(zfstrsize,(void));
extern	int	ZFPROTOD(zferror,(int,int));
extern	int	ZFPROTOD(zfresjob,(void));			/* LRS412 */
extern	int	ZFPROTOD(CacheMgrDirA,(int,char *));
extern	int	ZFPROTOD(CacheMgrDirW,(int,unsigned short *));
#if	CACHE_ZHARR						/* LRS1034+ */
extern	int	ZFPROTOD(CacheMgrDirH,(int,wchar_t *));
#else	/* CACHE_ZHARR */
#define CacheMgrDirH(len,namep)	CacheMgrDirW(len,namep)
#endif	/* CACHE_ZHARR */					/* LRS1034- */
								/* CFL001+ */
/*
	Prototypes for signal handling helper functions in $ZF
	for UNIX systems only.
*/

/*
*  sigrtchk() checks for asynchronous events.
*      Should be called whenever a system call fails because of a
*      received signal (usually open(), close(), read(), write(),
*      ioctl() and others that may take an unpredictable amount
*      of time to complete). It returns a code indicating the action
*      the user should take:
*
*     Return code   Cause                Action
*     ----------    -----------------    -------------------------
*         -1        Not a signal         Check for I/O error.
*          1        SIGINT or SIGTERM    Exit $ZF.
*          0        Other signal         Restart operation.
*/
extern	int	ZFPROTOD(sigrtchk,(void));

/*
*  sigrtclr() clears retry flag.
*      Should be called once before using sigrtchk()
*/
extern	int	ZFPROTOD(sigrtclr,(void));

/*
*  dzfalarm() establishes new SIGALRM handler.
*      On entry to $ZF, the previous handler is saved automatically.
*      On exit, it's restored automatically.
*/
extern	int	ZFPROTOD(dzfalarm,());				/* LRS1013- */
								/* CFL001- */
#else	/* ZF_DLL */	/*==============================*/

/*
	Indirect (DLL) Callback Function Table
*/

typedef struct CacheExportFuncTab *CACHEEFTP;

extern	CACHEEFTP	cachefunctabp;
extern	CACHECBTP	cachecallbackp;

/*
	Interface Function Definition
*/
#define	zfstrsize()		(*cachecallbackp->ZFSTRSIZE)()
#define	zferror(errnum,mode)	(*cachecallbackp->ZFERROR)(errnum,mode)
#define	zfresjob()		(*cachecallbackp->ZFRESJOB)()
#define CacheMgrDirA(len,namep)	(*cachecallbackp->CACHEMGRDIRA)(len,namep)
#define CacheMgrDirW(len,namep)	(*cachecallbackp->CACHEMGRDIRW)(len,namep)
#if	CACHE_ZHARR						/* LRS1034+ */
#define CacheMgrDirH(len,namep)	(*cachecallbackp->CACHEMGRDIRH)(len,namep)
#else	/* CACHE_ZHARR */
#define CacheMgrDirH(len,namep)	(*cachecallbackp->CACHEMGRDIRW)(len,namep)
#endif	/* CACHE_ZHARR */					/* LRS1034- */
#if	!defined(__VMS) && !defined(_WIN32)
#define	sigrtchk()		(*cachecallbackp->SIGRTCHK)();
#define	sigrtclr()		(*cachecallbackp->SIGRTCLR)();
#define	dzfalarm()		(*cachecallbackp->DZFALARM)();
#endif

#endif	/* ZF_DLL */	/*==============================*/

#if	defined(CACHE_UNICODE)
#define	CacheMgrDir(len,namep)	CacheMgrDirW(len,namep)
#elif	defined(CACHE_WCHART)	/* wchar_t character strings */	/* LRS1034+ */
#define	CacheMgrDir(len,namep)	CacheMgrDirH(len,namep)		/* LRS1034- */
#else
#define	CacheMgrDir(len,namep)	CacheMgrDirA(len,namep)
#endif
								/* LRS409- */
#ifdef  OPENM_DLL_IMPORT					/* RJW050+ */
_declspec( dllimport )	struct zfestr zfe[];
#else								/* RJW050- */
#ifdef	ZF_DLL							/* RAM631+ */
extern struct zfestr zfedll[];
#else								/* LRS409 */
extern struct zfestr zfe[];
#endif								/* RAM631- */
#endif								/* RJW050 */

#ifdef	ZF_DLL							/* LRS409+ */
#ifdef	__cplusplus						/* LRS1013+ */
#define	ZFDECL		extern "C" ZFEXPO 
#else
#define	ZFDECL		ZFEXPO
#endif								/* LRS1013- */
#define	ZFBEGIN				\
	CACHECBTP	cachecallbackp;	\
	CACHEEFTP	cachefunctabp;	\
ZFDECL struct zfestr * ZFCALL GetZFTable(	\
	CACHECBTP	cbtp,		\
	CACHEEFTP	eftp)		\
{					\
	cachecallbackp = cbtp;		\
	cachefunctabp = eftp;		\
	return zfedll;			\
}					\
struct zfestr zfedll[] = {
#else	/* ZF_DLL */						/* LRS409- */
#define	ZFBEGIN	struct zfestr zfe[] = {
#endif	/* ZF_DLL */						/* LRS302 */

#ifndef	__VMS							/* JAO552+ */
#define	ZFENTRY(n,a,f)	{n,a,(zffunc)f},			/* LRS1013 */
#define	ZFPRIV		ZFENTRY
#define	ZFEND		{(char *) 0, (char *) 0, (zffunc) 0}};
#define	ZF_SUCCESS	0	/* Return code for success */	/* LRS605 */
#define	ZF_FAILURE	1	/* Return code for failure */	/* LRS605 */
#else								/* JAO552= */
#define	ZFPRIV(n,a,f,p) {n,a,(zffunc)f,p},
#define	ZFENTRY(n,a,f)	ZFPRIV(n,a,f,NOPRIV)
#define	ZFEND		{(char *) 0, (char *) 0, (zffunc) 0,0}};
#define	ZF_SUCCESS	1	/* Return code for success */	/* LRS605 */
#define	ZF_FAILURE	2	/* Return code for failure */	/* LRS793 */
#ifndef	__cplusplus						/* JLC782+ */
#pragma	__required_pointer_size	__restore			/* JLC334 */
#endif								/* JLC782- */
#endif								/* JAO552- */

#ifdef	__cplusplus						/* LRS421+ */
}
#endif								/* LRS421- */

#include "callin.h"						/* KMK409 */

#endif	/* _cdzf_h_ */						/* LRS755 */
