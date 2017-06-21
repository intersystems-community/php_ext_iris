/*
	Definitions for the Call-In Interface to Cache

	Copyright (c) 1997-2016 by InterSystems.
	Cambridge, Massachusetts, U.S.A.  All rights reserved.
	Confidential, unpublished property of InterSystems.

	This module contains the definitions necessary to build a 'C'
	language program that utilizes the InterSystems Cache call-in
	interface.

	NOTE - because this file is included in C programs and the
	C compiler on some platforms (PowerPC) does not support
	C++ style comments by default, the // style of comments
	should not be used here.
*/

#ifndef	_ccallin_h_						/* LRS755 */
#define	_ccallin_h_						/* LRS755 */

#include "cdzf.h"						/* KMK409 */

/*
	By default, 8-bit strings and arrays used in the Callin interface
	are declared as type "unsigned char". Many C and C++ compilers
	expect string literals and arguments to string handling libraries
	to be of the type "char". Some compilers have option switches to
	specify the default type for "char", for example -signed and
	-unsigned. If you want the function argument prototypes in this
	header file to declare 8-bit string quantities as "char" instead of
	"unsigned char", to avoid compiler warnings on your system, define
	macro USE_CALLIN_CHAR before including it in your program. Doing so
	will not affect the way character strings are handled inside Cache.
*/

#ifdef	USE_CALLIN_CHAR						/* LRS758+ */
typedef	char		Callin_char_t;
#else
typedef	unsigned char	Callin_char_t;
#endif								/* LRS758- */

/*			 Data Structures
*/

/*	The following typedefs produce pointers to counted strings of the type
	the Cache interface routines expect as input and return as output.
	The length element for a string being passed to an interface routines
	is set to the actual length of the string.  When a string is to be
	passed back to the caller, the caller provides a counted string buffer
	with the length element set to the length of the buffer (i.e. the
	maximum possible length of the returned string).  Failure to set the
	length element can result in either errors from the interface if the
	length field is out of range, or overwriting other memory structures
	if a string larger than the allocated space is returned.
*/

#ifdef	__cplusplus
extern "C" {
#endif

/*	Flag indicating 4-byte wchar_t support */		/* LRS1034+ */
#if	defined(_WIN32)
#define	CACHE_HSTR	0
#else
#define	CACHE_HSTR	1
#endif								/* LRS1034- */

#if	defined(__VMS) && !defined(__cplusplus)	   /* GK525,JLC782 */
#pragma	__required_pointer_size	__save
#pragma	__required_pointer_size	__short
#endif

#ifndef	_mcallin_h_	/* In case mcallin.h also included */	/* LRS755+ */

#define	CACHE_MAXSTRLEN	32767U					/* CDS2287 */
#define	CACHE_MAXLOSTSZ	3641144U			/* SAP1198 CDS2287 */

/*	Structure to pass ASCII (one byte) character strings */
typedef struct {
	unsigned short	len;
	Callin_char_t	str[CACHE_MAXSTRLEN];
} CACHE_ASTR, *CACHE_ASTRP;

/*	Structure to pass Unicode (two byte) character strings */
typedef struct {
	unsigned short	len;
	unsigned short	str[CACHE_MAXSTRLEN];
} CACHEWSTR, *CACHEWSTRP;

#endif	/* _mcallin_h_ */					/* LRS755- */
								/* SOH106+ */
/*	CACHE_INT64 is a macro which defines a 64-bit integer type.  If
	it not already defined then we use "long long" which is a
	64-bit integer type supported by the C compilers used by 
	InterSystems on all current (2/1/2009) Cache platforms.  Note
	that "long long" is a data type supported by the 1999 C standard.

	If C90INT64 is defined then CACHE_INT64 is defined with a definition
	that meets the requirements of the older 1990 C standard (which does
	not allow "long long".)  Although defining C90INT64 will not
	produce a warning under C90 standards checking, the CACHE_INT64
	defined by C90INT64 probably will not work with the entry points that
	use CACHE_INT64 in callin.h.  The C90INT64 is for use by programs
	that do not have the "long long" type available *AND* do not intend
	to use any 64-bit interfaces in callin.h

	If your C compiler does not accept "long long" but does accept
	a data type name like __int64 then you should not define C90INT64
	but you should instead define CACHE_INT64 to be __int64, either
	using a -D compiler flag or a #define CACHE_INT64 __int64 directive.
*/
#ifndef CACHE_INT64
#ifdef C90INT64
   /* With C90 standards checking, use a conforming (but not portable) struct
      as CACHE_INT64.  The struct is not portable so uses of CACHE_INT64
      must be tested. */
typedef struct {int h1,h2;} cache_int64;
#define CACHE_INT64 cache_int64
#else
  /* long long is a 64-bit integer type on all supported platforms */
  /* long long conforms to the C99 standard and is mostly portable */
#define CACHE_INT64 long long
#endif
#endif
								/* SOH106- */

#if	CACHE_HSTR						/* LRS1034+ */
#include <wchar.h>
/*	Structure to pass extended (four byte) character strings */
typedef struct {
	unsigned int	len;
	wchar_t		str[CACHE_MAXSTRLEN];
} CACHEHSTR, *CACHEHSTRP;
#else
typedef	CACHEWSTR	CACHEHSTR, *CACHEHSTRP;
#endif	/* CACHE_HSTR */					/* LRS1034- */

/*	Cache' is able to manipulate "long strings", much longer than the
	former, traditional limit of 32,767 characters.  The CACHE_EXSTR
	descriptor is used when strings may exceed this old limit.

	As with the other string descriptors, in input, the len field gives
	the actual length of the data.  On output, it is irrelevant.

	Note that the str field is actually a pointer to the string.  It
	must be set for an input value.  On output, Cache sets it to the
	result value, which will have been allocated by malloc and which
	should be released with free, by the user, when no longer needed.
*/
								/* KMK409+ */
typedef struct {
	unsigned int	len;		/* length of string */
	union {
	   Callin_char_t * ch;		/* text of the 8-bit string */
	   unsigned short *wch;		/* text of the 16-bit string */
#if	CACHE_ZHARR						/* LRS1034+ */
	   wchar_t 	  *lch;		/* text of the wchar_t string */
#else
	   unsigned short *lch;
#endif								/* LRS1034- */
	}		str;
}	CACHE_EXSTR, *CACHE_EXSTRP;			/* KMK409- KMK497 */

/* A block of data, used by iKnow engine to store Knowledgebases and
   Languagebases */
typedef struct {
	int data_len;
	unsigned char* data;
} CACHE_IKNOWDATA, *CACHE_IKNOWDATAP;

/* An iKnow string, which is always UTF-16 with unsigned shorts */
typedef struct {
	int string_len;
	const unsigned short* string;
} CACHE_IKNOWSTRING, *CACHE_IKNOWSTRINGP;

/* Callbacks for a callin client (currently only the iKnow engine) to allocate and free iKnow knowledge bases */
/* All return error codes */
typedef int (*CACHE_IKNOWLOADFUNC)(void* udata, CACHE_IKNOWDATAP loaded_data);
typedef int (*CACHE_IKNOWRELEASEFUNC)(CACHE_IKNOWDATAP data);
typedef struct {
	/* Loads data if needed by calling load_func with user data udata, populates loaded_data with data description,
	   gives client the opportunity to release resources from the load via release_func */
	int (*LoadData)(CACHE_IKNOWSTRINGP id, void* udata, CACHE_IKNOWLOADFUNC load_func, CACHE_IKNOWRELEASEFUNC release_func, CACHE_IKNOWDATAP loaded_data);
	/* Release data that is no longer needed, takes the data pointer from the loaded_data struct of the LoadData function */
	int (*UnloadData)(unsigned char* data);
	/* Point at null terminated strings of the error message ID
	   (from Errors.xml) and a single error parameter for the last
	   iKnow data failure for this process. */
	void (*GetError)(const char** message_id, const char** message_val);
	/* Set the current error ID and parameter value */
	void (*SetError)(const char* message_id, const char* message_val);
} CACHE_IKNOWFUNCS, *CACHE_IKNOWFUNCSP;


/* 			Character type interface definitions
*/
#if	defined(CACHE_UNICODE)	/* Unicode character strings */
#define	CACHESTR	CACHEWSTR
#define	CACHE_STR	CACHEWSTR
#define	CACHESTRP	CACHEWSTRP
#define CACHE_STRP	CACHEWSTRP
#define	CACHE_STRING	CACHE_WSTRING
#elif	defined(CACHE_WCHART)	/* wchar_t character strings */	/* LRS1061 */
#define	CACHESTR	CACHEHSTR
#define	CACHE_STR	CACHEHSTR
#define	CACHESTRP	CACHEHSTRP
#define CACHE_STRP	CACHEHSTRP
#define	CACHE_STRING	CACHE_HSTRING
#else			/* ASCII character strings */
#define	CACHESTR	CACHE_ASTR
#define	CACHE_STR	CACHE_ASTR
#define	CACHESTRP	CACHE_ASTRP
#define CACHE_STRP	CACHE_ASTRP
#define	CACHE_STRING	CACHE_ASTRING
#endif

/* 			Constants
*/

/*	The following are the definitions that are used to form the flags
	argument to CacheStart() to control how Cache treats, or doesn't
	treat, the principal input device.

	Setting more than one of these flags (with the exception of the
	modifiers) will cause unpredictable results
*/
#define	CACHE_TTALL	1	/* Indicates that Cache should initialize the
				   terminal's settings and restore them across
				   each call into, and return from, the
				   interface.  */
#define CACHE_TTCALLIN	2	/* Indicates that Cache should initialize the
				   terminal each time its called but should
				   restore it only when the connection is
				   closed (egs. when CacheEnd() is called or
				   the connection is broken). */
#define CACHE_TTSTART	4	/* Indicates that Cache should initialize the
				   terminal when the connection is formed and
				   reset it when the connection is terminated.
				   */
#define	CACHE_TTNEVER	8	/* Indicates that Cache should not alter the
				   terminal's settings */
#define CACHE_TTNONE	16	/* Indicates that Cache should not do any
				   output or input from the principle input
				   device. This is equivilent to specifying
				   the null device for princin and princout.
				   Output will be discarded and reads will
				   complete with end-of-file */
#define CACHE_TTMASK	(CACHE_TTALL|CACHE_TTCALLIN|CACHE_TTSTART| \
			 CACHE_TTNEVER|CACHE_TTNONE)
#define CACHE_PROGMODE	32	/* Cache errors do not cause halt in the
				   absense of error handler */
/*	The following modifier is allowed in conjunction with
		CACHE_TTALL, CACHE_TTCALLIN and CACHE_TTSTART.
*/
#define CACHE_TTNOUSE	0x10000 /* If set this indicates that Cache OPEN and
				   USE commands are not allowed to alter the
				   terminal as well.  This is implicitly
				   set by CACHE_TTNEVER and CACHE_TTNONE.
				   The default is to allow Cache OPEN and
				   USE commands to alter the terminal in the
				   other modes. */

/*	The following are the definitions that can be used to form the
	arguments to CacheCtrl().  These flags are in pairs and if both
	members of a set are specified the results will be unpredictable.
*/
#define CACHE_DISACTRLC	 1	/* CTRL/C is ignored by Cache */
#define CACHE_ENABCTRLC	 2	/* CTRL/C in Cache generates an <INTERRUPT> */

/*	The following are the arguments to CacheAbort() that control the
	mechanism used to terminate Cache processing.

	CACHE_CTRLC simulates a CTRL/C at the keyboard and takes effect even
	if CacheCtrl() specified that CTRL/C is to be ignored. The Cache
	call that is interrupted returns CACHE_INTERRUPT.

	CACHE_RESJOB simulates a resjob. This results in the Cache connection
	being broken and the Cache call that's interrupted completes with
	a status of CACHE_CONBROKEN.
*/
#define	CACHE_CTRLC	 1
#define CACHE_RESJOB	 2

								/* JO1924+ */
/*	The following are the flag definitions for the flag argument
 *	to the CacheAcquireLock and/or CacheReleaseLock functions.
 *	These may be OR'd together to specify multiple flags for a
 *	single request.
 *
 *	CACHE_INCREMENTAL_LOCK is passed to CacheAcquireLock and indicates
 *	that this is a L + style request. Any locks currently held remain
 *	held. If this is omitted then all existing locks for the process
 *	are released before attempting to acquire the newly requested lock.
 *
 *	CACHE_SHARED_LOCK is passed to CacheAcquireLock and indicates that
 *	the requested lock should be acquired in SHARED access mode. If this
 *	is omitted the lock is acquired in EXCLUSIVE mode. This is equivilent
 *	to the #"S" argument to the lock command.
 *
 *	CACHE_IMMEDIATE_RELEASE is passed to CacheReleaseLock and indicates
 *	that the lock being released should be unlocked immediatly even if
 *	there is an open transaction. Normally within a transaction the
 *	actual releasing of a lock is delayed until the transaction ends.
 *	This is equivilent to the #"I" flag on an unlock command.
*/
#define	CACHE_INCREMENTAL_LOCK	1
#define	CACHE_SHARED_LOCK	2
#define CACHE_IMMEDIATE_RELEASE 4				/* JO1924- */

/* 	The following are the return values from CacheType() that describe
	the internal datatype of the result from the call to CacheEval()
*/
#define	CACHE_INT	 1	/* native (32-bit on the VAX) integer */
#define CACHE_DOUBLE	 2	/* Cache 64-bit floating point */
#define CACHE_ASTRING	 3	/* ASCII character string */

/*	In addition to the above the following can be used as the type
	argument to the CacheConvert() function

	Note that floating point types (CACHE_FLOAT, CACHE_HFLOAT and
	CACHE_DOUBLE) that have decimal parts (including negative exponents),
	as well as the 64 bit integer types CACHE_INT8 and CACHE_UINT8,
	involve division when calculating the return value. Therefore the
	results may not be 100% accurate.  CACHE_STRINT, CACHE_INT8 and
	CACHE_UINT8 can return the status CACHE_RETTRUNC if the result of the
	last CacheEval() call will not fit in the space allocated in
	CacheConvert(). In the case of CACHE_INT8 and CACHE_UINT8 this means
	that the expression resulted in a floating point # that could not be
	normalized to fit within 64 bits.
*/
#define	CACHE_CHAR	 4	/*   8 bit signed integer */
#define	CACHE_INT2	 5	/*  16 bit signed integer */
#define	CACHE_INT4	 6	/*  32 bit signed integer */
#define	CACHE_INT8	 7	/*  64 bit signed integer */
#define	CACHE_UCHAR	 8	/*   8 bit unsigned integer */
#define	CACHE_UINT2	 9	/*  16 bit unsigned integer */
#define	CACHE_UINT4	10	/*  32 bit unsigned integer */
#define	CACHE_UINT8	11	/*  64 bit unsigned integer */
#define CACHE_FLOAT	12	/*  32 bit floating point */
#define CACHE_HFLOAT	13	/* 128 bit floating point */
#define	CACHE_UINT	14	/* native size unsigned integer
				   (egs. 32 bit on the VAX) */
#define CACHE_WSTRING	15	/* Unicode character string */
#define	CACHE_OREF	16	/* object reference */
#define	CACHE_LASTRING	17	/* ASCII long string */
#define	CACHE_LWSTRING	18	/* Unicode long string */
#define	CACHE_IEEE_DBL	19	/* IEEE 64-bit floating point */
#define CACHE_HSTRING	20	/* wchar_t character string */
#define	CACHE_UNDEF	21	/* an undefined argument */


/*	The following are error codes that are generated by the call-in
	interface as opposed to error codes that are generated by
	Cache and returned by the call-in interface. */

#define CACHE_INVALID_INSTANCE  -17 /* invalid shdir */
#define	CACHE_CHANGEPASSWORD -16 /* Password change required */	/* STC1114+ */
#define	CACHE_ACCESSDENIED -15	/* Process not authenticated */ /* STC1114- */
#define	CACHE_EXSTR_INUSE  -14	/* an output EXSTR must be null */
#define CACHE_NORES	  -13	/* CacheConvert() was called to get a result
				   but there's no result to return */
#define CACHE_BADARG	  -12	/* A function was called with an argument
				   that was out of range or otherwise
				   invalid */
#define CACHE_NOTINCACHE  -11	/* Returned from CacheAbort() if the process
			  	   wasn't executing on the Cache side */
#define CACHE_RETTRUNC 	  -10	/* The return buffer was too small so only
				   part of the requested item was returned */
#define CACHE_ERUNKNOWN	   -9	/* Unknown error code in persave */
#define CACHE_RETTOOSMALL  -8	/* return buffer too small, nothing returned */
#define CACHE_NOCON 	   -7	/* not connected to Cache */
#define CACHE_INTERRUPT    -6	/* Failed due to CTRL/C (special case of
				   CacheError() */
#define CACHE_CONBROKEN    -4	/* We broke the connection */
#define CACHE_STRTOOLONG   -3	/* A string argument is too long */
#define CACHE_ALREADYCON   -2
#define CACHE_FAILURE	   -1
#define CACHE_SUCCESS 	    0

/*	The following are possible errors that Cache could return. To get
	the text associated with a particular error pass one of the following
	codes to CacheErrxlate().  After a call to one of the Cache interface
	routines returns one of these errors, calling CacheError() will return
	the contents of $ZE.  Note that $ZE is not set for all errors (egs.
	the <BREAK> generated from an argumentless break).

	Calling CacheErrxlate() with an error code out of range results in an
	CACHE_ERUNKNOWN return status

	Some Cache errors (CACHE_ERINTERRUPT and CACHE_ERCONBROKEN) are of
	special interest and are translated to one of the negative codes
	above.
*/

#define	CACHE_ERMAXNUM		  1	/* max number */
#define	CACHE_ERSELECT		  2	/* select */
#define	CACHE_ERSYNTAX		  3	/* syntax */
#define	CACHE_ERLABELREDEF	  4	/* labelredef */
#define CACHE_ERMXSTR             5     /* max string */        /* SAP1188 */
#define	CACHE_ERFUNCTION	  6	/* function */
#define	CACHE_ERINTERRUPT	  7	/* interrupt */
#define	CACHE_ERNOLINE		  8	/* noline */
#define	CACHE_ERUNDEF		  9	/* undefined */
#define	CACHE_ERSYSTEM		 10	/* system */
#define	CACHE_EREDITED		 11	/* edited */
#define	CACHE_ERCOMMAND  	 12	/* command */
#define	CACHE_ERMAXARRAY	 13	/* maxarray */
#define	CACHE_ERNAME		 14	/* name */
#define	CACHE_ERSTORE		 15	/* store */
#define	CACHE_ERSUBSCR		 16	/* subscript */
#define	CACHE_ERNOROUTINE	 17	/* noroutine */
#define	CACHE_ERDIVIDE		 18	/* divide */
#define	CACHE_ERSWIZZLEFAIL	 19	/* <SWIZZLE FAIL> */	/* CDS1988 */
#define	CACHE_ERSTRINGSTACK	 20	/* string stack */
#define	CACHE_ERFRAMESTACK	 21	/* framestack */
#define	CACHE_ERUNIMPLEMENTED	 22	/* unimplemented */
#define	CACHE_EREXQUOTA		 23	/* exceeded quota (wsextent) */
#define	CACHE_ERNOTOPEN		 24	/* not open */
#define	CACHE_ERARGSTACK	 25	/* argstack */
#define	CACHE_ERM11TAPE		 26	/* m11 tape error */
#define	CACHE_ERPROTECT		 27	/* protect */
#define	CACHE_ERDATABASE	 28	/* database */
#define	CACHE_ERFILEFULL	 29	/* file full */
#define	CACHE_ERNAKED		 30	/* naked */
#define	CACHE_ERROUTINELOAD	 31 	/* routineload */
#define	CACHE_ERTERMINATOR	 32	/* terminator */
#define	CACHE_ERDISKHARD	 33	/* disk hard */
#define	CACHE_ERBLOCKNUM	 34	/* block # */
#define	CACHE_ERDEVNAME		 35	/* device name */
#define	CACHE_ERNOJOB		 36	/* nojob */
#define	CACHE_ERREAD		 37	/* read */
#define	CACHE_ERWRITE		 38	/* write */
#define	CACHE_EREOF		 39	/* end of file */
#define	CACHE_ERPARAMETER	 40	/* parameter */
#define	CACHE_ERNETWORK		 41	/* network */
#define	CACHE_ERRECOMPILE	 42	/* recompile */
#define	CACHE_ERDIR		 43	/* directory */
#define	CACHE_ERMAXSCOPE	 44	/* max scope */
#define	CACHE_ERCLUSTERFAIL	 45	/* cluster failed */
#define	CACHE_ERMAXERROR	 46	/* MAXERROR (internal problem) */
#define	CACHE_ERCONBROKEN	 47	/* lost an external connection */
#define	CACHE_ERNOMBX		 48
#define	CACHE_ERNOCP		 49
#define	CACHE_ERNOWD		 50
#define	CACHE_ERNOGC		 51
#define	CACHE_ERMTERR		 52	/* <MAGTAPE> */
#define	CACHE_ERDOWN		 54
#define	CACHE_ERCLNUP		 55
#define	CACHE_ERMAXPID		 56
#define	CACHE_ERNOSIGNON	 57	/* <SIGNONS INHIBITED> */
#define	CACHE_ERHALT		 58	/* <HALTED> */
#define	CACHE_ERWAITINIT	 59	/* <WAIT FOR M/xx INIT.. TO COMPLT> */
#define	CACHE_ERPRIMARY		 60	/* <NOT PRIMARY VOLUME>	  */
#define	CACHE_ERCORRUPT		 61	/* <CORRUPT VOLUME SET>	  */
#define	CACHE_ERMAPS		 62	/* <INVALID MAP NUMBER>	  */
#define	CACHE_ERVOLSET1		 63	/* <VOLUME SET ALREADY CREATED> */
#define	CACHE_ERFORMAT		 64	/* <VOLUME IS NOT FORMATTED>	   */
#define	CACHE_ERMAPRSV		 65	/* <CONFLICTING BLOCK NUMBERS>  */
#define	CACHE_ERCOLLATE		 66	/* <COLLATION> */
#define	CACHE_ERZTRP		 67	/* <ZTRAP> */
#define	CACHE_ERNODEV		 68	/* <NODEV> */
#define	CACHE_ERLANG		 69	/* <LANGUAGE MISMATCH> */
#define	CACHE_ERNOSYS		 70	/* <NOSYS> */
#define	CACHE_ERGIMPL		 71	/* <BADIMPLICIT> */
#define	CACHE_ERNGLOREF		 72	/* <NETGLOREF> */
#define	CACHE_ERNFORMAT		 73	/* <NETFORMAT> */
#define	CACHE_ERDSCON		 74	/* <DSCON> */
#define	CACHE_ERNVERSION	 75	/* <NETVERSION> */
#define CACHE_ERNJOBMAX		 76	/* <NETJOBMAX> */
#define CACHE_ERNRETRY		 77	/* <NETRETRY> */
#define	CACHE_ERNAMADD		 78	/* <NAMEADD> */
#define	CACHE_ERNETLOCK		 79	/* <NETLOCK> */
#define	CACHE_ERVALUE		 80	/* <ILLEGAL VALUE> */
#define	CACHE_ERNUMFIL		 81	/* <TOOMANYFILES> */
#define	CACHE_ERLIST		 82	/* <LIST> */
#define	CACHE_ERNAMSP		 83	/* <NAMESPACE> */
#define	CACHE_ERCMTFAIL		 84	/* <COMMITFAIL> */
#define	CACHE_ERROLLFAIL	 85	/* <ROLLFAIL> */
#define	CACHE_ERNSRVFAIL	 86	/* <NETSRVFAIL> */
#define	CACHE_ERCOLMISMAT	 87	/* <COLLATEMISMATCH> */
#define	CACHE_ERSLMSPAN		 88	/* <SLMSPAN> */
#define	CACHE_ERWIDECHAR	 89	/* <WIDE CHAR> */
#define	CACHE_ERLINELEV		 90	/* <LINELEVEL> */
#define	CACHE_ERARGDUP		 91	/* <DUPLICATEARG> */
#define	CACHE_ERGLODEF		 92	/* <GLODEFCHANGE> */
#define	CACHE_ERCOLCHG		 93	/* <COLLATECHANGE> */
#define	CACHE_ERRESTART		 94	/* <NORESTART> */
#define	CACHE_ERROUDEF		 95	/* <ROUDEFCHANGE> */
#define	CACHE_ERMNESPACE	 96	/* <MNEMONICSPACE> */
#define	CACHE_ERTRANSLOCK	 97	/* <INVTRANSLOCK> */
#define	CACHE_ERTRANSQUIT	 98	/* <INVTRANSQUIT> */
#define	CACHE_ERECODETRAP	 99	/* <ECODETRAP> */
#define	CACHE_LOCKLOST		100	/* <LOCKLOST> */
#define	CACHE_DATALOST		101	/* <DATALOST> */
#define	CACHE_TRANSLOST		102	/* <TRANSLOST> */
#define	CACHE_ERZOLE		103	/* <ZOLE> */
#define	CACHE_ERLICENSE		104	/* <LICENSE LIMIT EXCEEDED> */
#define	CACHE_ERNOLICSERV	105 	/* <LICENSE SERVER UNAVAILABLE> */
#define	CACHE_ERCOLNSUP		106	/* <COLLATION NOT SUPPORTED> */
#define	CACHE_ERNLSTAB		107	/* <NLS TABLE> */
#define	CACHE_ERXLATE		108	/* <TRANSLATE> */
#define	CACHE_ERNULLVAL		109	/* <NULL VALUE> */
#define	CACHE_ERNOVAL		110	/* <MISSING VALUE> */
#define	CACHE_ERUNKTYPE		111	/* <UNKNOWN TYPE> */
#define	CACHE_ERVALRANGE	112	/* <VALUE OUT OF RANGE> */
#define	CACHE_ERRANGE		113	/* <RANGE> */
#define	CACHE_ERDOMAINSPACERETRY 114	/* <DOMAINSPACERETRY> */
#define CACHE_ERCSMISMATCH	115	/* <CLIENT-SERVER MISMATCH> */
#define CACHE_ERSTRMISMATCH	116	/* <STRMISMATCH> */
#define CACHE_ERBADCLASS	117	/* <INVALID CLASS> */
#define CACHE_ERIOBJECT		118	/* <INTERNAL OBJECT ERROR> */
#define CACHE_ERBADOREF		119	/* <INVALID OREF> */
#define CACHE_ERNOMETHOD	120	/* <METHOD DOES NOT EXIST> */
#define CACHE_ERNOPROPERTY	121	/* <PROPERTY DOES NOT EXIST> */
#define CACHE_ERNOCLASS		122	/* <CLASS DOES NOT EXIST> */
#define CACHE_ERCLASSDESC	123	/* <CLASS DESCRIPTOR> */
#define CACHE_ERCLASSCOMP	124	/* <CLASS COMPILING> */
#define CACHE_ERCLASSRECOMP	125	/* <CLASS RECOMPILED> */
#define	CACHE_ERCLASSMEMORY	126	/* <INSUFFICIENT CLASS MEMORY> */
#define	CACHE_ERNUMCLASS	127	/* <TOO MANY USERS OF CLASS> */
#define	CACHE_ERNUMOREF		128	/* <TOO MANY OREFS> */
#define	CACHE_ERGETINVALID	129	/* <CANNOT GET THIS PROPERTY> */
#define	CACHE_ERSETINVALID	130	/* <CANNOT SET THIS PROPERTY> */
#define CACHE_ERREMCLASSRECOMP	131	/* <REMOTE CLASS RECOMPILED> */
#define CACHE_ERREMCLASSEDITED	132	/* <REMOTE CLASS EDITED> */
#define CACHE_ERCLASSEDITED	133	/* <CLASS EDITED> */
#define CACHE_ERCLASSLOAD	134	/* <CLASS TOO BIG TO LOAD> */
#define CACHE_ERCLASSSIZE	135	/* <CLASS TOO BIG TO SAVE> */
#define CACHE_ERCORRUPTOBJECT	136	/* <CORRUPT OBJECT> */
#define CACHE_ERDISCONNECT	137	/* <DISCONNECT> */
#define CACHE_ERNOCURRENTOBJECT	138	/* <NO CURRENT OBJECT> */
#define CACHE_ERZFHEAPSPACE	139	/* <OUT OF $ZF HEAP SPACE> */
#define CACHE_ERTOOMANYCLASSES	140	/* <TOO MANY CLASSES> */
#define CACHE_ERNETSTRMISMATCH  141 /* <NETWORK DATA UPDATE FAILED - STRMISMATCH> */
#define CACHE_ERNETSSTACK       142 /* <NETWORK DATA UPDATE FAILED - STRINGSTACK> */
#define CACHE_ERNETDIR          143 /* <NETWORK DATA UPDATE FAILED - DIRECTORY> */
#define	CACHE_ERNETDKHER        144 /* <NETWORK DATA UPDATE FAILED - DISKHARD> */
#define CACHE_ERNETDBDGD        145 /* <NETWORK DATA UPDATE FAILED - DATABASE> */
#define	CACHE_ERNETCLUSTER      146 /* <NETWORK DATA UPDATE FAILED - CLUSTERFAILED> */
#define	CACHE_ERNETCSMISMATCH   147 /* <NETWORK DATA .... - CLIENT-SERVER MISMATCH> */
#define CACHE_ERNETPROT         148 /* <NETWORK DATA UPDATE FAILED - PROTECT> */
#define	CACHE_ERNETDKSER        149 /* <NETWORK DATA UPDATE FAILED - BLOCKNUMBER> */
#define	CACHE_ERNETDKFUL        150 /* <NETWORK DATA UPDATE FAILED - DISKFULL> */
#define	CACHE_ERNETMXSTR        151 /* <NETWORK DATA UPDATE FAILED - MAXSTRING> */
#define	CACHE_ERNETSBSCR        152 /* <NETWORK DATA UPDATE FAILED - SUBSCRIPT> */
#define	CACHE_ERNETSYSTM        153 /* <NETWORK DATA UPDATE FAILED - SYSTEM> */
#define	CACHE_ERNETWIDECHAR     154 /* <NETWORK DATA UPDATE FAILED - WIDECHAR> */
#define CACHE_ERNETNVERSION	155 /* <NETWORK DATA UPDATE FAILED - NETVERSION> */
#define CACHE_ERNETNFORMAT	156 /* <NETWORK DATA UPDATE FAILED - NETFORMAT> */
#define	CACHE_ERNETNGLOREF	157 /* <NETWORK DATA UPDATE FAILED - NETGLOREF>  */
#define CACHE_ERBADBITSTR	158	/* <INVALID BIT STRING> */
#define CACHE_ERROUTINESIZE	159	/* <ROUTINE TOO BIG TO SAVE> */
#define CACHE_ERNOSOURCE	160	/* <NO SOURCE> */
#define CACHE_ERMALLOC		161	/* <STORE> - malloc failed */
#define CACHE_EROREFNO		162	/* <INVALID TYPE> */
#define CACHE_ERNOOREF		163	/* <INVALID OREF> */
#define	CACHE_ERINVARG		164	/* <INVALID ARGUMENT> */
#define	CACHE_ERDYNLOAD		165	/* <DYNAMIC LIBRARY LOAD> */
#define CACHE_NOJVM		166	/* <Java VM not loaded> */
#define CACHE_ERPRIVPROP	167	/* <PRIVATE PROPERTY> */
#define CACHE_ERPRIVMETH	168	/* <PRIVATE METHOD> */
#define CACHE_ERJAVAEXCEPTION	169	/* <Java Exception> */
#define	CACHE_ERNETLICENSE	170	/* <NETWORK UNLICENSED> */
#define CACHE_ERDDPJOBOVFL	171	/* <DDP JOB OVERFLOW> */
#define	CACHE_ERLICENSEALLOC	172	/* <LICENSE ALLOCATION EXCEEDED> */
#define	CACHE_ERTRANLEVEL	173	/* <TRANSACTION LEVEL> */
#define CACHE_ERRESJOB		174	/* <RESJOB> */		/* CDS585 */
#define CACHE_ERALARM		175	/* <ALARM> */		/* SAP223 */
#define	CACHE_ERLOSTTABOVER	176	/* <TOO MANY LONG STRINGS> */	/* KMK179 */
#define	CACHE_EREXTINT		177	/* <EXTERNAL INTERRUPT> *//* LRS876 */
#define	CACHE_ERNOFLOAT		178	/* <UNIMPLEMENTED FLOAT> *//* CDS644 */
#define	CACHE_ERMVWRAPUP	179	/* <MV WRAPUP> */	/* JPL003 */
#define CACHE_ERUNLICENSED	180	/* <UNLICENSED> */	/* RJW1198 */
#define	CACHE_ERPROTDB		181	/* <PROTECT> */		/* CDS707 */
#define	CACHE_ERARRDIM		182	/* <ARRAY DIMENSION> */
#define	CACHE_ERTHROW		183	/* <THROW> */		/* CDS2284 */
#define	CACHE_ERNOTFILVAR	184	/* <INVALID FILE VARIABLE> */
#define	CACHE_ERNOTSELLST	185	/* <INVALID SELECT LIST> */
#define	CACHE_ERCMPLXPAT	186	/* <COMPLEX PATTERN> */
#define	CACHE_ERCLASSPROP	187	/* <CLASS PROPERTY> */
#define	CACHE_ERMAXLOCKS	188	/* <MAX LOCKS> */	/* SML1059+ */
#define	CACHE_ERLOCKFULL	189	/* <LOCK TABLE FULL> */
#define	CACHE_ERSVRLOCKFULL	190	/* <SERVER LOCK TABLE FULL> */	/* SML1059- */
#define CACHE_ERMAXROUTINES	191	/* <MAX ROUTINES> */	/* GK686 */
#define CACHE_EROUTOFHEAP	192	/* <SHARED MEM HEAP> */	/* GK753,GK686 */
#define	CACHE_ERMAXINCR		193	/* <MAXINCREMENT> */	/* CDS1578 */
#define CACHE_ERWRONGNS		194	/* <WRONG NAMESPACE> */ /* GK760 */
#define	CACHE_ERCALLBKSYN	195	/* <_CALLBACK SYNTAX> */ /* CDS1613 */
#define CACHE_EROBJDISP		196	/* <OBJECT DISPATCH> */ /* GK791 */
#define	CACHE_ERCOMMATCH	197	/* <COMMON MISMATCH> */	/* CDS1725 */
#define	CACHE_ERRTNLIMIT	198	/* <ROUTINE LIMIT> */	/* CDS1980 */
#define	CACHE_ERZFMAXSTR	199	/* <MAX $ZF STRING> */	/* CDS2170 */
#define CACHE_ERTOOCOMPLEX	200	/* <ROUTINE TOO COMPLEX>   SOH369 */
#define CACHE_ERINVALGLOBALREF  201     /* <INVALID GLOBAL REFERENCE> JO2659 */
#define CACHE_ERTCPWRITE	202     /* <TCPWRITE> SML1761 */
#define CACHE_ERUNSUPPMETHOD	203     /* <METHOD NOT SUPPORTED>  CDS2779 */

#define CACHE_ERMAX		203	/* Maximum error number */
/* Note: do not use // style comments in this file! */


/*			Name mappings to internal entrypoints
*/

/* Popping a double works the same way regardless of the format */
#define	CachePopIEEEDbl	CachePopDbl
#define	CACHEPOPIEEEDBL	CACHEPOPDBL

#if	defined(_WIN32)					/* DMC181+,LRS1013+ */
#define	CFCALL			__cdecl
#else
#define	CFCALL
#endif

#if	defined(__STDC__) || defined(__cplusplus)	/* LRS755+,LRS756+ */
#define	CFPROTOD(func,args)	CFCALL func args
#define	CFPROTOI(func,args)	(CFCALL *func) args
#else
#define	CFPROTOD(func,args)	CFCALL func ()
#define	CFPROTOI(func,args)	(CFCALL *func) ()
#endif

typedef const struct  CacheExportFuncTab {
        int  CFPROTOI(CacheExecuteA,(CACHE_ASTRP volatile));
	int  CFPROTOI(CacheExecuteW,(CACHEWSTRP volatile));
	int  CFPROTOI(CacheEvalA,(CACHE_ASTRP volatile));
	int  CFPROTOI(CacheEvalW,(CACHEWSTRP volatile));
	int  CFPROTOI(CachePromptA,(CACHE_ASTRP));
	int  CFPROTOI(CachePromptW,(CACHEWSTRP));
	int  CFPROTOI(CacheErrxlateA,(int,CACHE_ASTRP));
	int  CFPROTOI(CacheErrxlateW,(int,CACHEWSTRP));
	int  CFPROTOI(CacheErrorA,(CACHE_ASTRP,CACHE_ASTRP,int *));
	int  CFPROTOI(CacheErrorW,(CACHEWSTRP,CACHEWSTRP,int *));
	int  CFPROTOI(CacheConvert,(unsigned long,void *));
	int  CFPROTOI(CacheType,(void));
	int  CFPROTOI(CacheAbort,(unsigned long));
#ifndef	__VMS
	int  CFPROTOI(CacheSignal,(int));
#endif
	int  CFPROTOI(CachePushInt,(int));
	int  CFPROTOI(CachePushDbl,(double));
	int  CFPROTOI(CachePushStr,(int,const Callin_char_t *));
	int  CFPROTOI(CachePushStrW,(int,const unsigned short *));
	int  CFPROTOI(CachePushRtn,(unsigned int *,int,const Callin_char_t *,int,const Callin_char_t *));
	int  CFPROTOI(CachePushRtnX,(unsigned int *,int,const Callin_char_t *,int,int,const Callin_char_t*,int,const Callin_char_t *));
	int  CFPROTOI(CachePushRtnW,(unsigned int *,int,const unsigned short *,int,const unsigned short *));
	int  CFPROTOI(CachePushRtnXW,(unsigned int *,int,const unsigned short *,int,int,const unsigned short *,int,const unsigned short *));
	int  CFPROTOI(CachePushFunc,(unsigned int *,int,const Callin_char_t *,int,const Callin_char_t *));
	int  CFPROTOI(CachePushFuncX,(unsigned int *,int,const Callin_char_t *,int,int,const Callin_char_t*,int,const Callin_char_t *));
	int  CFPROTOI(CachePushFuncW,(unsigned int *,int,const unsigned short *,int,const unsigned short *));
	int  CFPROTOI(CachePushFuncXW,(unsigned int *,int,const unsigned short *,int,int,const unsigned short*,int,const unsigned short *));
	int  CFPROTOI(CachePushGlobal,(int,const Callin_char_t *));
	int  CFPROTOI(CachePushGlobalX,(int,const Callin_char_t *,int,const Callin_char_t *));
	int  CFPROTOI(CachePushGlobalW,(int,const unsigned short *));
	int  CFPROTOI(CachePushGlobalXW,(int,const unsigned short *,int,const unsigned short *));
	int  CFPROTOI(CacheUnPop,(void));
	int  CFPROTOI(CachePop,(void **));
	int  CFPROTOI(CachePopInt,(int *));
	int  CFPROTOI(CachePopDbl,(double *));
	int  CFPROTOI(CachePopStr,(int *,Callin_char_t **));
	int  CFPROTOI(CachePopStrW,(int *,unsigned short **));
  	int  CFPROTOI(CacheSetVar,(int,const Callin_char_t *));
 	int  CFPROTOI(CacheGetVar,(int,const Callin_char_t *));
 	int  CFPROTOI(CacheDoRtn,(unsigned int,int));
	int  CFPROTOI(CacheExtFun,(unsigned int,int));
	int  CFPROTOI(CacheGlobalGet,(int,int));
	int  CFPROTOI(CacheGlobalSet,(int));
	int  CFPROTOI(CacheDoFun,(unsigned int,int));
	int  CFPROTOI(CachePushClassMethod,(int,const Callin_char_t *,int,const Callin_char_t *,int));
	int  CFPROTOI(CachePushClassMethodW,(int,const unsigned short *,int,const unsigned short *,int));
	int  CFPROTOI(CachePushMethod,(unsigned int,int,const Callin_char_t *,int));
	int  CFPROTOI(CachePushMethodW,(unsigned int,int,const unsigned short *,int));
	int  CFPROTOI(CacheInvokeClassMethod,(int));
	int  CFPROTOI(CacheInvokeMethod,(int));
	int  CFPROTOI(CachePushProperty,(unsigned int,int,const Callin_char_t *));
	int  CFPROTOI(CachePushPropertyW,(unsigned int,int,const unsigned short *));
	int  CFPROTOI(CacheGetProperty,(void));
	int  CFPROTOI(CacheSetProperty,(void));
	int  CFPROTOI(CachePushOref,(unsigned int));
	int  CFPROTOI(CachePopOref,(unsigned int *));
	int  CFPROTOI(CacheIncrementCountOref,(unsigned int));
	int  CFPROTOI(CacheCloseOref,(unsigned int));
	int  CFPROTOI(CachePushCvtW,(int,const unsigned short *));
	int  CFPROTOI(CachePopCvtW,(int *,unsigned short **));
	int  CFPROTOI(CachePushList,(int,const Callin_char_t *));
	int  CFPROTOI(CachePopList,(int *,Callin_char_t **));
	int  CFPROTOI(CachePushPtr,(void *));
	int  CFPROTOI(CachePopPtr,(void **));
	int  CFPROTOI(CacheGlobalIncrement,(int));
	int  CFPROTOI(CacheContext,(void));
	int  CFPROTOI(CacheCtrl,(unsigned long));
	int  CFPROTOI(CacheCvtInA,(CACHE_ASTRP,CACHE_ASTRP,CACHE_ASTRP));
	int  CFPROTOI(CacheCvtInW,(CACHE_ASTRP,CACHEWSTRP,CACHEWSTRP));
	int  CFPROTOI(CacheCvtOutA,(CACHE_ASTRP,CACHE_ASTRP,CACHE_ASTRP));
	int  CFPROTOI(CacheCvtOutW,(CACHEWSTRP,CACHEWSTRP,CACHE_ASTRP));
	int  CFPROTOI(CacheGlobalQuery,(int,int,int));	/* JO1874+ */
	int  CFPROTOI(CacheGlobalOrder,(int,int,int));
	int  CFPROTOI(CacheGlobalData,(int,int));
	int  CFPROTOI(CacheGlobalKill,(int,int));
	int  CFPROTOI(CacheGlobalRelease,(void));		/* JO1874- */
	int  CFPROTOI(CachePushLock,(int,const Callin_char_t *));/* JO1924+ */
	int  CFPROTOI(CachePushLockX,(int,const Callin_char_t *,int,const Callin_char_t *));
	int  CFPROTOI(CachePushLockW,(int,const unsigned short *));
	int  CFPROTOI(CachePushLockXW,(int,const unsigned short *,int,const unsigned short *));
	int  CFPROTOI(CacheAcquireLock,(int,int,int,int *));
	int  CFPROTOI(CacheReleaseLock,(int,int));
	int  CFPROTOI(CacheReleaseAllLocks,(void));
	int  CFPROTOI(CacheTStart,(void));
	int  CFPROTOI(CacheTCommit,(void));
	int  CFPROTOI(CacheTRollback,(int));
	int  CFPROTOI(CacheTLevel,(void));			/* JO1924- */
	int  CFPROTOI(CacheCvtExStrInA,(CACHE_EXSTRP, CACHE_ASTRP, CACHE_EXSTRP));
	int  CFPROTOI(CacheCvtExStrInW,(CACHE_EXSTRP, CACHEWSTRP, CACHE_EXSTRP));
	int  CFPROTOI(CacheCvtExStrOutA,(CACHE_EXSTRP, CACHE_ASTRP, CACHE_EXSTRP));
	int  CFPROTOI(CacheCvtExStrOutW,(CACHE_EXSTRP, CACHEWSTRP, CACHE_EXSTRP));
	int  CFPROTOI(CachePushIEEEDbl,(double));
#if	CACHE_HSTR						/* LRS1034+ */
	int  CFPROTOI(CacheExecuteH,(CACHEHSTRP volatile));
	int  CFPROTOI(CacheEvalH,(CACHEHSTRP volatile));
	int  CFPROTOI(CachePromptH,(CACHEHSTRP));
	int  CFPROTOI(CacheErrxlateH,(int,CACHEHSTRP));
	int  CFPROTOI(CacheErrorH,(CACHEHSTRP,CACHEHSTRP,int *));
	int  CFPROTOI(CacheCvtInH,(CACHE_ASTRP,CACHEHSTRP,CACHEHSTRP));
	int  CFPROTOI(CacheCvtOutH,(CACHEHSTRP,CACHEHSTRP,CACHE_ASTRP));
	int  CFPROTOI(CachePushStrH,(int,const wchar_t *));
	int  CFPROTOI(CachePushRtnH,(unsigned int *,int,const wchar_t *,int,const wchar_t *));
	int  CFPROTOI(CachePushRtnXH,(unsigned int *,int,const wchar_t *,int,int,const wchar_t *,int,const wchar_t *));
	int  CFPROTOI(CachePushFuncH,(unsigned int *,int,const wchar_t *,int,const wchar_t *));
	int  CFPROTOI(CachePushFuncXH,(unsigned int *,int,const wchar_t *,int,int,const wchar_t*,int,const wchar_t *));
	int  CFPROTOI(CachePushGlobalH,(int,const wchar_t *));
	int  CFPROTOI(CachePushGlobalXH,(int,const wchar_t *,int,const wchar_t *));
	int  CFPROTOI(CachePushClassMethodH,(int,const wchar_t *,int,const wchar_t *,int));
	int  CFPROTOI(CachePushMethodH,(unsigned int,int,const wchar_t *,int));
	int  CFPROTOI(CachePushPropertyH,(unsigned int,int,const wchar_t *));
	int  CFPROTOI(CachePopStrH,(int *,wchar_t **));
	int  CFPROTOI(CachePushCvtH,(int,const wchar_t *));
	int  CFPROTOI(CachePopCvtH,(int *,wchar_t **));
	int  CFPROTOI(CachePushLockH,(int,const wchar_t *));
	int  CFPROTOI(CachePushLockXH,(int,const wchar_t *,int,const wchar_t *));
	int  CFPROTOI(CacheCvtExStrInH,(CACHE_EXSTRP,CACHEHSTRP,CACHE_EXSTRP));
	int  CFPROTOI(CacheCvtExStrOutH,(CACHE_EXSTRP,CACHEHSTRP,CACHE_EXSTRP));
#endif	/* CACHE_HSTR */					/* LRS1034- */
        int  CFPROTOI(CacheSPCSend,(int,const Callin_char_t *));   /* JMM072+ */
        int  CFPROTOI(CacheSPCReceive,(int *,Callin_char_t *));    /* JMM072- */
	int  CFPROTOI(CacheExStrKill,(CACHE_EXSTRP));		/* KMK409+ KMK498 */
	unsigned char *CFPROTOI(CacheExStrNew,(CACHE_EXSTRP,int));
	unsigned short *CFPROTOI(CacheExStrNewW,(CACHE_EXSTRP,int));
#if CACHE_ZHARR
	wchar_t *CFPROTOI(CacheExStrNewH,(CACHE_EXSTRP,int));
#else
	unsigned short *CFPROTOI(CacheExStrNewH,(CACHE_EXSTRP,int));
#endif								/* KMK409- */
	int  CFPROTOI(CachePopExStr,(CACHE_EXSTRP));
	int  CFPROTOI(CachePopExStrW,(CACHE_EXSTRP));
	int  CFPROTOI(CachePushExStr,(CACHE_EXSTRP));
	int  CFPROTOI(CachePushExStrW,(CACHE_EXSTRP));
#if CACHE_HSTR
	int  CFPROTOI(CachePushExStrH,(CACHE_EXSTRP));
	int  CFPROTOI(CachePopExStrH,(CACHE_EXSTRP));
	int  CFPROTOI(CachePushExStrCvtH,(CACHE_EXSTRP));
	int  CFPROTOI(CachePopExStrCvtH,(CACHE_EXSTRP));
#endif
	int  CFPROTOI(CachePushExList,(CACHE_EXSTRP));		/* KMK523+ */
	int  CFPROTOI(CachePopExList,(CACHE_EXSTRP));		/* KMK523- */
	int  CFPROTOI(CachePushExStrCvtW,(CACHE_EXSTRP));
	int  CFPROTOI(CachePopExStrCvtW,(CACHE_EXSTRP));
	int  CFPROTOI(CachePushUndef,(void));			/* CDS1472 */
	int  CFPROTOI(CachePushInt64,(CACHE_INT64));      /* SAP1134, SOH106 */
	int  CFPROTOI(CachePopInt64,(CACHE_INT64 *));     /* SAP1134, SOH106 */
								/* JO2405+ */
	int  CFPROTOI(CachePushSSVN,(int,const Callin_char_t *));
	int  CFPROTOI(CachePushSSVNX,(int,const Callin_char_t *,int,const Callin_char_t *));
	int  CFPROTOI(CachePushSSVNW,(int,const unsigned short *));
	int  CFPROTOI(CachePushSSVNXW,(int,const unsigned short *,int,const unsigned short *));
	int  CFPROTOI(CacheAddSSVN,(int,const Callin_char_t *));
	int  CFPROTOI(CacheAddSSVNX,(int,const Callin_char_t *,int,const Callin_char_t *));
	int  CFPROTOI(CacheAddSSVNW,(int,const unsigned short *));
	int  CFPROTOI(CacheAddSSVNXW,(int,const unsigned short *,int,const unsigned short *));
#if CACHE_HSTR
	int  CFPROTOI(CachePushSSVNH,(int,const wchar_t *));
	int  CFPROTOI(CachePushSSVNXH,(int,const wchar_t *,int,const wchar_t *));
	int  CFPROTOI(CacheAddSSVNH,(int,const wchar_t *));
	int  CFPROTOI(CacheAddSSVNXH,(int,const wchar_t *,int,const wchar_t *));
	int  CFPROTOI(CacheAddGlobalH,(int,const wchar_t *));
	int  CFPROTOI(CacheAddGlobalXH,(int,const wchar_t *,int,const wchar_t *));
#endif
	int  CFPROTOI(CacheAddGlobal,(int,const Callin_char_t *));
	int  CFPROTOI(CacheAddGlobalX,(int,const Callin_char_t *,int,const Callin_char_t *));
	int  CFPROTOI(CacheAddGlobalW,(int,const unsigned short *));
	int  CFPROTOI(CacheAddGlobalXW,(int,const unsigned short *,int,const unsigned short *));
	int  CFPROTOI(CacheAddSSVNDescriptor,(int));
	int  CFPROTOI(CacheAddGlobalDescriptor,(int));
	int  CFPROTOI(CacheAddLocalDescriptor,(int));
	int  CFPROTOI(CacheSSVNGet,(int));
	int  CFPROTOI(CacheSSVNKill,(int,int));
	int  CFPROTOI(CacheSSVNOrder,(int,int));
	int  CFPROTOI(CacheSSVNQuery,(int,int));
	int  CFPROTOI(CacheSSVNData,(int));
	int  CFPROTOI(CacheMerge,(void));			/* JO2405- */
	int  CFPROTOI(CacheEnableMultiThread,(void));		/* JO2410 */
	int  CFPROTOI(CacheiKnowFuncs,(CACHE_IKNOWFUNCSP));
} CACHEEFT;							/* KMK409 */

#ifndef	ZF_DLL		/*==============================*/

#ifdef	__VMS							/* SOH392+ */
#ifndef VMS_NAMES_UPPERCASE
/* #define definitions to support VMS compiles with the CC/NAMES=AS_IS
   qualifier.  Mixed case external names are replaced with names containg
   all upper case.

   This header file supports both CC/NAMES=AS_IS and CC/NAMES=UPPERCASE 
   on VMS */

/* Some programs define additional macros that assume CacheXXxxXX is different
   from CACHEXXXXXX.  Also C++ programs will sometimes overload symbols of
   the form CacheXXxxXX.  In that case, the program may be incompatible with
   the following macros and the program should do a #define VMS_NAMES_UPPERCASE
   before include callin.h in order to disable support for CC/NAMES=AS_IS */

#define CacheStartA		CACHESTARTA
#define CacheStartW		CACHESTARTW
#define CacheSecureStartA	CACHESECURESTARTA
#define CacheSecureStartW	CACHESECURESTARTW
#define CacheChangePasswordA	CACHECHANGEPASSWORDA
#define CacheChangePasswordW	CACHECHANGEPASSWORDW
#define CacheExecuteA		CACHEEXECUTEA
#define CacheExecuteW		CACHEEXECUTEW
#define CacheEvalA		CACHEEVALA
#define CacheEvalW		CACHEEVALW
#define CacheContext		CACHECONTEXT
#define CacheCvtInA		CACHECVTINA
#define CacheCvtInW		CACHECVTINW
#define CacheCvtOutA		CACHECVTOUTA
#define CacheCvtOutW		CACHECVTOUTW
#define CacheCvtExStrInA	CACHECVTEXSTRINA
#define CacheCvtExStrInW	CACHECVTEXSTRINW
#define CacheCvtExStrOutA	CACHECVTEXSTROUTA
#define CacheCvtExStrOutW	CACHECVTEXSTROUTW
#define CachePromptA		CACHEPROMPTA
#define CachePromptW		CACHEPROMPTW
#define CacheErrxlateA		CACHEERRXLATEA
#define CacheErrxlateW		CACHEERRXLATEW
#define CacheCtrl		CACHECTRL
#define CacheConvert		CACHECONVERT
#define CacheType		CACHETYPE
#define CacheAbort		CACHEABORT
#define CacheEnd		CACHEEND
#define CacheOflush		CACHEOFLUSH
#define CacheErrorA		CACHEERRORA
#define CacheErrorW		CACHEERRORW
#define CachePushInt		CACHEPUSHINT
#define CachePushDbl		CACHEPUSHDBL
#define CachePushUndef		CACHEPUSHUNDEF
#define CachePushIEEEDbl	CACHEPUSHIEEEDBL
#define CachePushInt64		CACHEPUSHINT64
#define CachePushOref		CACHEPUSHOREF
#define CachePushStr		CACHEPUSHSTR
#define CachePushStrW		CACHEPUSHSTRW
#define CachePushExStr		CACHEPUSHEXSTR
#define CachePushExStrW		CACHEPUSHEXSTRW
#define CachePushRtn		CACHEPUSHRTN
#define CachePushRtnX		CACHEPUSHRTNX
#define CachePushRtnW		CACHEPUSHRTNW
#define CachePushRtnXW		CACHEPUSHRTNXW
#define CachePushFunc		CACHEPUSHFUNC
#define CachePushFuncX		CACHEPUSHFUNCX
#define CachePushFuncW		CACHEPUSHFUNCW
#define CachePushFuncXW		CACHEPUSHFUNCXW
#define CachePushGlobal		CACHEPUSHGLOBAL
#define CachePushGlobalX	CACHEPUSHGLOBALX
#define CachePushGlobalW	CACHEPUSHGLOBALW
#define CachePushGlobalXW	CACHEPUSHGLOBALXW
#define CacheUnPop		CACHEUNPOP
#define CachePop		CACHEPOP
#define CachePopInt		CACHEPOPINT
#define CachePopInt64		CACHEPOPINT64
#define CachePopDbl		CACHEPOPDBL
#define CachePopOref		CACHEPOPOREF
#define CachePopStr		CACHEPOPSTR
#define CachePopStrW		CACHEPOPSTRW
#define CachePopExStr		CACHEPOPEXSTR
#define CachePopExStrW		CACHEPOPEXSTRW
#define CacheSetVar		CACHESETVAR
#define CacheGetVar		CACHEGETVAR
#define CacheDoRtn		CACHEDORTN
#define CacheDoFun		CACHEDOFUN
#define CacheExtFun		CACHEEXTFUN
#define CacheGlobalGet		CACHEGLOBALGET
#define CacheGlobalGetBinary	CACHEGLOBALGETBINARY
#define CacheGlobalSet		CACHEGLOBALSET
#define CacheGlobalIncrement	CACHEGLOBALINCREMENT
#define CachePushClassMethod	CACHEPUSHCLASSMETHOD
#define CachePushClassMethodW	CACHEPUSHCLASSMETHODW
#define CachePushMethod		CACHEPUSHMETHOD
#define CachePushMethodW	CACHEPUSHMETHODW
#define CacheInvokeClassMethod	CACHEINVOKECLASSMETHOD
#define CacheInvokeMethod	CACHEINVOKEMETHOD
#define CachePushProperty	CACHEPUSHPROPERTY
#define CachePushPropertyW	CACHEPUSHPROPERTYW
#define CacheGetProperty	CACHEGETPROPERTY
#define CacheSetProperty	CACHESETPROPERTY
#define CacheIncrementCountOref	CACHEINCREMENTCOUNTOREF
#define CacheCloseOref		CACHECLOSEOREF
#define CachePushCvtW		CACHEPUSHCVTW
#define CachePopCvtW		CACHEPOPCVTW
#define CachePushExStrCvtW	CACHEPUSHEXSTRCVTW
#define CachePopExStrCvtW	CACHEPOPEXSTRCVTW
#define CachePushList		CACHEPUSHLIST
#define CachePopList		CACHEPOPLIST
#define CachePushPtr		CACHEPUSHPTR
#define CachePopPtr		CACHEPOPPTR
#define CacheGlobalQuery	CACHEGLOBALQUERY
#define CacheGlobalOrder	CACHEGLOBALORDER
#define CacheGlobalData		CACHEGLOBALDATA
#define CacheGlobalKill		CACHEGLOBALKILL
#define CacheGlobalRelease	CACHEGLOBALRELEASE
#define CachePushLock		CACHEPUSHLOCK
#define CachePushLockX		CACHEPUSHLOCKX
#define CachePushLockW		CACHEPUSHLOCKW
#define CachePushLockXW		CACHEPUSHLOCKXW
#define CacheAcquireLock	CACHEACQUIRELOCK
#define CacheReleaseLock	CACHERELEASELOCK
#define CacheReleaseAllLocks	CACHERELEASEALLLOCKS
#define CacheTStart		CACHETSTART
#define CacheTCommit		CACHETCOMMIT
#define CacheTRollback		CACHETROLLBACK
#define CacheTLevel		CACHETLEVEL
#define CacheSetDir		CACHESETDIR

#if	CACHE_HSTR
#define CacheStartH		CACHESTARTH
#define CacheSecureStartH	CACHESECURESTARTH
#define CacheChangePasswordH	CACHECHANGEPASSWORDH
#define CacheExecuteH		CACHEEXECUTEH
#define CacheEvalH		CACHEEVALH
#define CachePromptH		CACHEPROMPTH
#define CacheErrxlateH		CACHEERRXLATEH
#define CacheErrorH		CACHEERRORH
#define CacheCvtInH		CACHECVTINH
#define CacheCvtOutH		CACHECVTOUTH
#define CachePushStrH		CACHEPUSHSTRH
#define CachePushExStrH		CACHEPUSHEXSTRH
#define CachePushExStrCvtH	CACHEPUSHEXSTRCVTH
#define CachePushRtnH		CACHEPUSHRTNH
#define CachePushRtnXH		CACHEPUSHRTNXH
#define CachePushFuncH		CACHEPUSHFUNCH
#define CachePushFuncXH		CACHEPUSHFUNCXH
#define CachePushGlobalH	CACHEPUSHGLOBALH
#define CachePushGlobalXH	CACHEPUSHGLOBALXH
#define CachePushClassMethodH	CACHEPUSHCLASSMETHODH
#define CachePushMethodH	CACHEPUSHMETHODH
#define CachePushPropertyH	CACHEPUSHPROPERTYH
#define CachePopStrH		CACHEPOPSTRH
#define CachePopExStrH		CACHEPOPEXSTRH
#define CachePopExStrCvtH	CACHEPOPEXSTRCVTH
#define CachePushCvtH		CACHEPUSHCVTH
#define CachePopCvtH		CACHEPOPCVTH
#define CachePushLockH		CACHEPUSHLOCKH
#define CachePushLockXH		CACHEPUSHLOCKXH
#define CacheCvtExStrInH	CACHECVTEXSTRINH
#define CacheCvtExStrOutH	CACHECVTEXSTROUTH
#endif	/* CACHE_HSTR */

#define CacheSPCSend		CACHESPCSEND
#define CacheSPCReceive		CACHESPCRECEIVE
#define CacheExStrKill		CACHEEXSTRKILL
#define CacheExStrNew		CACHEEXSTRNEW
#define CacheExStrNewW		CACHEEXSTRNEWW
#define CacheExStrNewH		CACHEEXSTRNEWH
#define CachePushExList		CACHEPUSHEXLIST
#define CachePopExList		CACHEPOPEXLIST
#define CacheBitFind		CACHEBITFIND
#define CacheBitFindB		CACHEBITFINDB
#define CacheEndAll		CACHEENDALL
#define CachePushSSVN		CACHEPUSHSSVN
#define CachePushSSVNX		CACHEPUSHSSVNX
#define CachePushSSVNW		CACHEPUSHSSVNW
#define CachePushSSVNXW		CACHEPUSHSSVNXW
#define CacheAddSSVN		CACHEADDSSVN
#define CacheAddSSVNX		CACHEADDSSVNX
#define CacheAddSSVNW		CACHEADDSSVNW
#define CacheAddSSVNXW		CACHEADDSSVNXW

#if	CACHE_HSTR
#define CachePushSSVNH		CACHEPUSHSSVNH
#define CachePushSSVNXH		CACHEPUSHSSVNXH
#define CacheAddSSVNH		CACHEADDSSVNH
#define CacheAddSSVNXH		CACHEADDSSVNXH
#define CacheAddGlobalH		CACHEADDGLOBALH
#define CacheAddGlobalXH	CACHEADDGLOBALXH
#endif

#define CacheAddGlobal		CACHEADDGLOBAL
#define CacheAddGlobalX		CACHEADDGLOBALX
#define CacheAddGlobalW		CACHEADDGLOBALW
#define CacheAddGlobalXW	CACHEADDGLOBALXW
#define CacheAddSSVNDescriptor	CACHEADDSSVNDESCRIPTOR
#define CacheAddGlobalDescriptor CACHEADDGLOBALDESCRIPTOR
#define CacheAddLocalDescriptor	CACHEADDLOCALDESCRIPTOR
#define CacheSSVNGet		CACHESSVNGET
#define CacheSSVNKill		CACHESSVNKILL
#define CacheSSVNOrder		CACHESSVNORDER
#define CacheSSVNQuery		CACHESSVNQUERY
#define CacheSSVNData		CACHESSVNDATA
#define CacheMerge		CACHEMERGE
#define CacheEnableMultiThread	CACHEENABLEMULTITHREAD
#define CacheiKnowFuncs		CACHEIKNOWFUNCS
#endif	/* !defined(VMS_NAMES_UPPERCASE) */
#endif	/* __VMS */						/* SOH392- */

/*
			Function Prototypes

	These entry points are used when the user module is linked
	together with the Cache object module.
*/
extern	int  CFPROTOD(CacheStartA,(unsigned long,int,CACHE_ASTRP,CACHE_ASTRP));
extern	int  CFPROTOD(CacheStartW,(unsigned long,int,CACHEWSTRP,CACHEWSTRP));
								/* STC1114+ */
extern	int  CFPROTOD(CacheSecureStartA,(CACHE_ASTRP,CACHE_ASTRP,CACHE_ASTRP,unsigned long,int,CACHE_ASTRP,CACHE_ASTRP));
extern	int  CFPROTOD(CacheSecureStartW,(CACHEWSTRP,CACHEWSTRP,CACHEWSTRP,unsigned long,int,CACHEWSTRP,CACHEWSTRP));
extern	int  CFPROTOD(CacheChangePasswordA,(CACHE_ASTRP,CACHE_ASTRP,CACHE_ASTRP));
extern	int  CFPROTOD(CacheChangePasswordW,(CACHEWSTRP,CACHEWSTRP,CACHEWSTRP));
								/* STC1114- */
extern	int  CFPROTOD(CacheExecuteA,(CACHE_ASTRP volatile));
extern	int  CFPROTOD(CacheExecuteW,(CACHEWSTRP volatile));
extern	int  CFPROTOD(CacheEvalA,(CACHE_ASTRP volatile));
extern	int  CFPROTOD(CacheEvalW,(CACHEWSTRP volatile));
extern	int  CFPROTOD(CacheContext,(void));
extern	int  CFPROTOD(CacheCvtInA,(CACHE_ASTRP,CACHE_ASTRP,CACHE_ASTRP));
extern	int  CFPROTOD(CacheCvtInW,(CACHE_ASTRP,CACHEWSTRP,CACHEWSTRP));
extern	int  CFPROTOD(CacheCvtOutA,(CACHE_ASTRP,CACHE_ASTRP,CACHE_ASTRP));
extern	int  CFPROTOD(CacheCvtOutW,(CACHEWSTRP,CACHEWSTRP,CACHE_ASTRP));
extern	int  CFPROTOD(CacheCvtExStrInA,(CACHE_EXSTRP, CACHE_ASTRP, CACHE_EXSTRP));
extern	int  CFPROTOD(CacheCvtExStrInW,(CACHE_EXSTRP, CACHEWSTRP, CACHE_EXSTRP));
extern	int  CFPROTOD(CacheCvtExStrOutA,(CACHE_EXSTRP, CACHE_ASTRP, CACHE_EXSTRP));
extern	int  CFPROTOD(CacheCvtExStrOutW,(CACHE_EXSTRP, CACHEWSTRP, CACHE_EXSTRP));
extern	int  CFPROTOD(CachePromptA,(CACHE_ASTRP));
extern	int  CFPROTOD(CachePromptW,(CACHEWSTRP));
extern	int  CFPROTOD(CacheErrxlateA,(int,CACHE_ASTRP));
extern	int  CFPROTOD(CacheErrxlateW,(int,CACHEWSTRP));
extern	int  CFPROTOD(CacheCtrl,(unsigned long));
extern	int  CFPROTOD(CacheConvert,(unsigned long,void *));
extern	int  CFPROTOD(CacheType,(void));
extern	int  CFPROTOD(CacheAbort,(unsigned long));
extern	int  CFPROTOD(CacheEnd,(void));
extern	void CFPROTOD(CacheOflush,(void));
#ifndef	__VMS
extern	int  CFPROTOD(CacheSignal,(int));
#endif
extern	int  CFPROTOD(CacheErrorA,(CACHE_ASTRP,CACHE_ASTRP,int *));
extern	int  CFPROTOD(CacheErrorW,(CACHEWSTRP,CACHEWSTRP,int *));
extern	int  CFPROTOD(CachePushInt,(int));
extern	int  CFPROTOD(CachePushDbl,(double));
extern	int  CFPROTOD(CachePushUndef,(void));			/* CDS1472 */
extern	int  CFPROTOD(CachePushIEEEDbl,(double));
extern	int  CFPROTOD(CachePushInt64,(CACHE_INT64));      /* SAP1134, SOH106 */
extern	int  CFPROTOD(CachePushOref,(unsigned int));
extern	int  CFPROTOD(CachePushStr,(int,const Callin_char_t *));
extern	int  CFPROTOD(CachePushStrW,(int,const unsigned short *));
extern	int  CFPROTOD(CachePushExStr,(CACHE_EXSTRP));		/* KMK497+ */
extern	int  CFPROTOD(CachePushExStrW,(CACHE_EXSTRP));		/* KMK497- */
extern	int  CFPROTOD(CachePushRtn,(unsigned int *,int,const Callin_char_t *,int,const Callin_char_t *));
extern	int  CFPROTOD(CachePushRtnX,(unsigned int *,int,const Callin_char_t *,int,int,const Callin_char_t*,int,const Callin_char_t *));
extern	int  CFPROTOD(CachePushRtnW,(unsigned int *,int,const unsigned short *,int,const unsigned short *));
extern	int  CFPROTOD(CachePushRtnXW,(unsigned int *,int,const unsigned short *,int,int,const unsigned short *,int,const unsigned short *));
extern	int  CFPROTOD(CachePushFunc,(unsigned int *,int,const Callin_char_t *,int,const Callin_char_t *));
extern	int  CFPROTOD(CachePushFuncX,(unsigned int *,int,const Callin_char_t *,int,int,const Callin_char_t*,int,const Callin_char_t *));
extern	int  CFPROTOD(CachePushFuncW,(unsigned int *,int,const unsigned short *,int,const unsigned short *));
extern	int  CFPROTOD(CachePushFuncXW,(unsigned int *,int,const unsigned short *,int,int,const unsigned short*,int,const unsigned short *));
extern	int  CFPROTOD(CachePushGlobal,(int,const Callin_char_t *));
extern	int  CFPROTOD(CachePushGlobalX,(int,const Callin_char_t *,int,const Callin_char_t *));
extern	int  CFPROTOD(CachePushGlobalW,(int,const unsigned short *));
extern	int  CFPROTOD(CachePushGlobalXW,(int,const unsigned short *,int,const unsigned short *));
extern	int  CFPROTOD(CacheUnPop,(void));
extern	int  CFPROTOD(CachePop,(void **));
extern	int  CFPROTOD(CachePopInt,(int *));
extern	int  CFPROTOD(CachePopInt64,(CACHE_INT64 *));     /* SAP1134, SOH106 */
extern	int  CFPROTOD(CachePopDbl,(double *));
extern	int  CFPROTOD(CachePopOref,(unsigned int *));
extern	int  CFPROTOD(CachePopStr,(int *,Callin_char_t **));
extern	int  CFPROTOD(CachePopStrW,(int *,unsigned short **));
extern  int  CFPROTOD(CachePopExStr,(CACHE_EXSTRP));		/* KMK497+ */
extern  int  CFPROTOD(CachePopExStrW,(CACHE_EXSTRP));		/* KMK497- */
extern	int  CFPROTOD(CacheSetVar,(int,const Callin_char_t *));
extern	int  CFPROTOD(CacheGetVar,(int,const Callin_char_t *));
extern	int  CFPROTOD(CacheDoRtn,(unsigned int,int));
extern	int  CFPROTOD(CacheDoFun,(unsigned int,int));
extern	int  CFPROTOD(CacheExtFun,(unsigned int,int));
extern	int  CFPROTOD(CacheGlobalGet,(int,int));
extern	int  CFPROTOD(CacheGlobalGetBinary,(int,int,int *,Callin_char_t **));
extern	int  CFPROTOD(CacheGlobalSet,(int));
extern	int  CFPROTOD(CacheGlobalIncrement,(int));
extern	int  CFPROTOD(CachePushClassMethod,(int,const Callin_char_t *,int,const Callin_char_t *,int));
extern	int  CFPROTOD(CachePushClassMethodW,(int,const unsigned short *,int,const unsigned short *,int));
extern	int  CFPROTOD(CachePushMethod,(unsigned int,int,const Callin_char_t *,int));
extern	int  CFPROTOD(CachePushMethodW,(unsigned int,int,const unsigned short *,int));
extern	int  CFPROTOD(CacheInvokeClassMethod,(int));
extern	int  CFPROTOD(CacheInvokeMethod,(int));
extern	int  CFPROTOD(CachePushProperty,(unsigned int,int,const Callin_char_t *));
extern	int  CFPROTOD(CachePushPropertyW,(unsigned int,int,const unsigned short *));
extern	int  CFPROTOD(CacheGetProperty,(void));
extern	int  CFPROTOD(CacheSetProperty,(void));
extern	int  CFPROTOD(CacheIncrementCountOref,(unsigned int));
extern	int  CFPROTOD(CacheCloseOref,(unsigned int));
extern	int  CFPROTOD(CachePushCvtW,(int,const unsigned short *));
extern	int  CFPROTOD(CachePopCvtW,(int *,unsigned short **));
extern	int  CFPROTOD(CachePushExStrCvtW,(CACHE_EXSTRP));       /* SAP1121 */
extern	int  CFPROTOD(CachePopExStrCvtW,(CACHE_EXSTRP));        /* SAP1121 */
extern	int  CFPROTOD(CachePushList,(int,const Callin_char_t *));
extern	int  CFPROTOD(CachePopList,(int *,Callin_char_t **));
extern	int  CFPROTOD(CachePushPtr,(void *));
extern	int  CFPROTOD(CachePopPtr,(void **));
extern  int  CFPROTOD(CacheGlobalQuery,(int,int,int));		/* JO1874+ */
extern  int  CFPROTOD(CacheGlobalOrder,(int,int,int));
extern  int  CFPROTOD(CacheGlobalData,(int,int));
extern  int  CFPROTOD(CacheGlobalKill,(int,int));
extern  int  CFPROTOD(CacheGlobalRelease,(void));		/* JO1874- */
extern	int  CFPROTOD(CachePushLock,(int,const Callin_char_t *)); /* JO1924+ */
extern	int  CFPROTOD(CachePushLockX,(int,const Callin_char_t *,int,const Callin_char_t *));
extern	int  CFPROTOD(CachePushLockW,(int,const unsigned short *));
extern	int  CFPROTOD(CachePushLockXW,(int,const unsigned short *,int,const unsigned short *));
extern  int  CFPROTOD(CacheAcquireLock,(int,int,int,int *));
extern  int  CFPROTOD(CacheReleaseLock,(int,int));
extern  int  CFPROTOD(CacheReleaseAllLocks,(void));
extern  int  CFPROTOD(CacheTStart,(void));
extern  int  CFPROTOD(CacheTCommit,(void));
extern  int  CFPROTOD(CacheTRollback,(int));
extern  int  CFPROTOD(CacheTLevel,(void));		/* JO1924-,LRS1013- */
extern  int  CFPROTOD(CacheSetDir,(char *));                    /* SAP609 */
#if	CACHE_HSTR						/* LRS1034+ */
extern	int  CFPROTOD(CacheStartH,(unsigned long,int,CACHEHSTRP,CACHEHSTRP));
								/* STC1114+ */
extern	int  CFPROTOD(CacheSecureStartH,(CACHEHSTRP,CACHEHSTRP,CACHEHSTRP,unsigned long,int,CACHEHSTRP,CACHEHSTRP));
extern	int  CFPROTOD(CacheChangePasswordH,(CACHEHSTRP,CACHEHSTRP,CACHEHSTRP));
								/* STC1114- */
extern	int  CFPROTOD(CacheExecuteH,(CACHEHSTRP volatile));
extern	int  CFPROTOD(CacheEvalH,(CACHEHSTRP volatile));
extern	int  CFPROTOD(CachePromptH,(CACHEHSTRP));
extern	int  CFPROTOD(CacheErrxlateH,(int,CACHEHSTRP));
extern	int  CFPROTOD(CacheErrorH,(CACHEHSTRP,CACHEHSTRP,int *));
extern	int  CFPROTOD(CacheCvtInH,(CACHE_ASTRP,CACHEHSTRP,CACHEHSTRP));
extern	int  CFPROTOD(CacheCvtOutH,(CACHEHSTRP,CACHEHSTRP,CACHE_ASTRP));
extern	int  CFPROTOD(CachePushStrH,(int,const wchar_t *));
extern	int  CFPROTOD(CachePushExStrH,(CACHE_EXSTRP));		/* KMK497 */
extern	int  CFPROTOD(CachePushExStrCvtH,(CACHE_EXSTRP));		/* KMK497 */
extern	int  CFPROTOD(CachePushRtnH,(unsigned int *,int,const wchar_t *,int,const wchar_t *));
extern	int  CFPROTOD(CachePushRtnXH,(unsigned int *,int,const wchar_t *,int,int,const wchar_t *,int,const wchar_t *));
extern	int  CFPROTOD(CachePushFuncH,(unsigned int *,int,const wchar_t *,int,const wchar_t *));
extern	int  CFPROTOD(CachePushFuncXH,(unsigned int *,int,const wchar_t *,int,int,const wchar_t*,int,const wchar_t *));
extern	int  CFPROTOD(CachePushGlobalH,(int,const wchar_t *));
extern	int  CFPROTOD(CachePushGlobalXH,(int,const wchar_t *,int,const wchar_t *));
extern	int  CFPROTOD(CachePushClassMethodH,(int,const wchar_t *,int,const wchar_t *,int));
extern	int  CFPROTOD(CachePushMethodH,(unsigned int,int,const wchar_t *,int));
extern	int  CFPROTOD(CachePushPropertyH,(unsigned int,int,const wchar_t *));
extern	int  CFPROTOD(CachePopStrH,(int *,wchar_t **));
extern	int  CFPROTOD(CachePopExStrH,(CACHE_EXSTRP));		/* KMK497 */
extern	int  CFPROTOD(CachePopExStrCvtH,(CACHE_EXSTRP));		/* KMK497 */
extern	int  CFPROTOD(CachePushCvtH,(int,const wchar_t *));
extern	int  CFPROTOD(CachePopCvtH,(int *,wchar_t **));
extern	int  CFPROTOD(CachePushLockH,(int,const wchar_t *));
extern	int  CFPROTOD(CachePushLockXH,(int,const wchar_t *,int,const wchar_t *));
extern	int  CFPROTOD(CacheCvtExStrInH,(CACHE_EXSTRP,CACHEHSTRP,CACHE_EXSTRP));
extern	int  CFPROTOD(CacheCvtExStrOutH,(CACHE_EXSTRP,CACHEHSTRP,CACHE_EXSTRP));
#endif	/* CACHE_HSTR */					/* LRS1034- */
extern  int  CFPROTOD(CacheSPCSend,(int,const Callin_char_t *));  /* JMM072+ */
extern  int  CFPROTOD(CacheSPCReceive,(int *,Callin_char_t *));   /* JMM072+ */
extern	int CFPROTOD(CacheExStrKill,(CACHE_EXSTRP));		/* KMK409+ KMK498 */
extern	unsigned char *CFPROTOD(CacheExStrNew,(CACHE_EXSTRP,int));
extern	unsigned short *CFPROTOD(CacheExStrNewW,(CACHE_EXSTRP,int));
#if CACHE_ZHARR
extern	wchar_t *CFPROTOD(CacheExStrNewH,(CACHE_EXSTRP,int));
#else
extern	unsigned short *CFPROTOD(CacheExStrNewH,(CACHE_EXSTRP,int));
#endif								/* KMK409- */
extern	int  CFPROTOD(CachePushExList,(CACHE_EXSTRP));		/* KMK523+ */
extern	int  CFPROTOD(CachePopExList,(CACHE_EXSTRP));		/* KMK523- */
#ifdef	CACHE_UNICODE
extern	int  CFPROTOD(CacheBitFind,(int,unsigned short *,int,int,int));
#else
extern	int  CFPROTOD(CacheBitFind,(int,unsigned char *,int,int,int));
#endif
extern	int  CFPROTOD(CacheBitFindB,(int,unsigned char *,int,int,int));
extern	int  CFPROTOD(CacheEndAll,(void));
								/* JO2405+ */
extern	int  CFPROTOD(CachePushSSVN,(int,const Callin_char_t *));
extern	int  CFPROTOD(CachePushSSVNX,(int,const Callin_char_t *,int,const Callin_char_t *));
extern	int  CFPROTOD(CachePushSSVNW,(int,const unsigned short *));
extern	int  CFPROTOD(CachePushSSVNXW,(int,const unsigned short *,int,const unsigned short *));
extern	int  CFPROTOD(CacheAddSSVN,(int,const Callin_char_t *));
extern	int  CFPROTOD(CacheAddSSVNX,(int,const Callin_char_t *,int,const Callin_char_t *));
extern	int  CFPROTOD(CacheAddSSVNW,(int,const unsigned short *));
extern	int  CFPROTOD(CacheAddSSVNXW,(int,const unsigned short *,int,const unsigned short *));
#if	CACHE_HSTR
extern	int  CFPROTOD(CachePushSSVNH,(int,const wchar_t *));
extern	int  CFPROTOD(CachePushSSVNXH,(int,const wchar_t *,int,const wchar_t *));
extern	int  CFPROTOD(CacheAddSSVNH,(int,const wchar_t *));
extern	int  CFPROTOD(CacheAddSSVNXH,(int,const wchar_t *,int,const wchar_t *));
extern	int  CFPROTOD(CacheAddGlobalH,(int,const wchar_t *));
extern	int  CFPROTOD(CacheAddGlobalXH,(int,const wchar_t *,int,const wchar_t *));
#endif
extern	int  CFPROTOD(CacheAddGlobal,(int,const Callin_char_t *));
extern	int  CFPROTOD(CacheAddGlobalX,(int,const Callin_char_t *,int,const Callin_char_t *));
extern	int  CFPROTOD(CacheAddGlobalW,(int,const unsigned short *));
extern	int  CFPROTOD(CacheAddGlobalXW,(int,const unsigned short *,int,const unsigned short *));
extern	int  CFPROTOD(CacheAddSSVNDescriptor,(int));
extern	int  CFPROTOD(CacheAddGlobalDescriptor,(int));
extern	int  CFPROTOD(CacheAddLocalDescriptor,(int));
extern	int  CFPROTOD(CacheSSVNGet,(int));
extern	int  CFPROTOD(CacheSSVNKill,(int,int));
extern	int  CFPROTOD(CacheSSVNOrder,(int,int));
extern	int  CFPROTOD(CacheSSVNQuery,(int,int));
extern	int  CFPROTOD(CacheSSVNData,(int));
extern	int  CFPROTOD(CacheMerge,(void));			/* JO2405- */
extern  int  CFPROTOD(CacheEnableMultiThread,(void));		/* JO2410 */
extern	int  CFPROTOD(CacheiKnowFuncs,(CACHE_IKNOWFUNCSP));


#define	CACHESTARTA(flags,tout,prinp,prout) CacheStartA(flags,tout,prinp,prout)
#define	CACHESTARTW(flags,tout,prinp,prout) CacheStartW(flags,tout,prinp,prout)
								/* STC1114+ */
#define	CACHESECURESTARTA(username,password,exename,flags,tout,prinp,prout) CacheSecureStartA(username,password,exename,flags,tout,prinp,prout)
#define	CACHESECURESTARTW(username,password,exename,flags,tout,prinp,prout) CacheSecureStartW(username,password,exename,flags,tout,prinp,prout)
#define	CACHECHANGEPASSWORDA(username,oldpassword,newpassword) CacheChangePasswordA(username,oldpassword,newpassword)
#define	CACHECHANGEPASSWORDW(username,oldpassword,newpassword) CacheChangePasswordW(username,oldpassword,newpassword)
								/* STC1114- */
#define	CACHEEXECUTEA(cmd)	CacheExecuteA(cmd)
#define	CACHEEXECUTEW(cmd)	CacheExecuteW(cmd)
#define	CACHEEVALA(expr)	CacheEvalA(expr)
#define	CACHEEVALW(expr)	CacheEvalW(expr)
#define	CACHECONTEXT()		CacheContext()
#define	CACHEPROMPTA(rbuf)	CachePromptA(rbuf)
#define	CACHEPROMPTW(rbuf)	CachePromptW(rbuf)
#define	CACHEERRXLATEA(code,rbuf) CacheErrxlateA(code,rbuf)
#define	CACHEERRXLATEW(code,rbuf) CacheErrxlateW(code,rbuf)
#define CACHEERRORA(msg,src,offp) CacheErrorA(msg,src,offp)
#define CACHEERRORW(msg,src,offp) CacheErrorW(msg,src,offp)
#define	CACHECTRL(flags)	CacheCtrl(flags)
#define	CACHECONVERT(type,rbuf)	CacheConvert(type,rbuf)
#define	CACHETYPE()		CacheType()
#define	CACHEABORT(type)	CacheAbort(type)
#define	CACHEEND()		CacheEnd()
#define	CACHEENDALL()		CacheEndAll()
#define	CACHEOFLUSH()		CacheOflush()
#define	CACHECVTINA(src,tbl,res) CacheCvtInA(src,tbl,res)
#define	CACHECVTINW(src,tbl,res) CacheCvtInW(src,tbl,res)
#define	CACHECVTOUTA(src,tbl,res) CacheCvtOutA(src,tbl,res)
#define	CACHECVTOUTW(src,tbl,res) CacheCvtOutW(src,tbl,res)
#define	CACHECVTEXSTRINA(src,tbl,res) CacheCvtExStrInA(src,tbl,res)
#define	CACHECVTEXSTRINW(src,tbl,res) CacheCvtExStrInW(src,tbl,res)
#define	CACHECVTEXSTROUTA(src,tbl,res) CacheCvtExStrOutA(src,tbl,res)
#define	CACHECVTEXSTROUTW(src,tbl,res) CacheCvtExStrOutW(src,tbl,res)
#ifndef	__VMS
#define	CACHESIGNAL(signal)	CacheSignal(signal)
#endif
#define	CACHEPUSHINT(num)	CachePushInt(num)
#define	CACHEPUSHDBL(num)	CachePushDbl(num)
#define	CACHEPUSHIEEEDBL(num)	CachePushIEEEDbl(num)
#define	CACHEPUSHOREF(oref)	CachePushOref(oref)
#define	CACHEPUSHUNDEF()	CachePushUndef()		/* CDS1472 */
#define CACHEPUSHINT64(num)     CachePushInt64(num)             /* SAP1134 */
#define	CACHEPUSHSTRA(len,ptr)	(CachePushStr)(len,ptr)		/* SOH393 */
#define	CACHEPUSHSTRW(len,ptr)	CachePushStrW(len,ptr)
#define	CACHEPUSHEXSTRA(sptr)	(CachePushExStr)(sptr)		/*KMK497+,SOH393*/
#define	CACHEPUSHEXSTRW(sptr)	CachePushExStrW(sptr)		/* KMK497- */
#define	CACHEPUSHRTNA(rflag,tlen,tptr,nlen,nptr)			\
	(CachePushRtn)(rflag,tlen,tptr,nlen,nptr)		/* SOH393 */
#define	CACHEPUSHRTNXA(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
	(CachePushRtnX)(rflag,tlen,tptr,off,elen,eptr,nlen,nptr) /* SOH393 */
#define	CACHEPUSHRTNW(rflag,tlen,tptr,nlen,nptr)			\
	CachePushRtnW(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHRTNXW(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
	CachePushRtnXW(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHFUNCA(rflag,tlen,tptr,nlen,nptr)			\
	(CachePushFunc)(rflag,tlen,tptr,nlen,nptr)		/* SOH393 */
#define	CACHEPUSHFUNCXA(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)	\
	(CachePushFuncX)(rflag,tlen,tptr,off,elen,eptr,nlen,nptr) /* SOH393 */
#define	CACHEPUSHFUNCW(rflag,tlen,tptr,nlen,nptr)			\
	CachePushFuncW(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHFUNCXW(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)	\
	CachePushFuncXW(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHGLOBALA(nlen,nptr) (CachePushGlobal)(nlen,nptr) /* SOH393 */
#define	CACHEPUSHGLOBALXA(nlen,nptr,elen,eptr)				\
	      (CachePushGlobalX)(nlen,nptr,elen,eptr)		/* SOH393 */
#define	CACHEPUSHGLOBALW(nlen,nptr) CachePushGlobalW(nlen,nptr)
#define	CACHEPUSHGLOBALXW(nlen,nptr,elen,eptr)				\
	      CachePushGlobalXW(nlen,nptr,elen,eptr)
#define CACHEUNPOP()		CacheUnPop()
#define CACHEPOP(arg)		CachePop(arg)
#define CACHEPOPINT(nump)	CachePopInt(nump)
#define CACHEPOPDBL(nump)	CachePopDbl(nump)
#define CACHEPOPOREF(orefp)	CachePopOref(orefp)
#define CACHEPOPINT64(num)      CachePopInt64(num)              /* SAP1134 */
#define CACHEPOPSTRA(lenp,strp)	(CachePopStr)(lenp,strp)	/* SOH393 */
#define CACHEPOPSTRW(lenp,strp)	CachePopStrW(lenp,strp)
#define	CACHEPOPEXSTRA(sstrp)	(CachePopExStr)(sstrp)		/*KMK497+,SOH393*/
#define	CACHEPOPEXSTRW(sstrp)	CachePopExStrW(sstrp)		/* KMK497- */
#define CACHESETVAR(len,namep)	CacheSetVar(len,namep)		/* SOH392 */
#define CACHEGETVAR(len,namep)	CacheGetVar(len,namep)		/* SOH392 */
#define CACHEDORTN(flags,narg)	CacheDoRtn(flags,narg)
#define CACHEEXTFUN(flags,narg)	CacheExtFun(flags,narg)
#define CACHEGLOBALGET(narg,flag) CacheGlobalGet(narg,flag)
#define CACHEGLOBALSET(narg)	CacheGlobalSet(narg)
#define CACHEGLOBALINCREMENT(narg) CacheGlobalIncrement(narg)
								/* JO1874+ */
#define CACHEGLOBALQUERY(narg,dir,valueflag) \
		 CacheGlobalQuery(narg,dir,valueflag)
#define CACHEGLOBALORDER(narg,dir,valueflag) \
		 CacheGlobalOrder(narg,dir,valueflag)
#define CACHEGLOBALDATA(narg,valueflag) CacheGlobalData(narg,valueflag)
#define CACHEGLOBALKILL(narg,nodeonly) CacheGlobalKill(narg,nodeonly)
#define CACHEGLOBALRELEASE() CacheGlobalRelease()
								/* JO1874- */
								/* JO1924+ */
#define	CACHEPUSHLOCKA(nlen,nptr) (CachePushLock)(nlen,nptr)	/* SOH393 */
#define	CACHEPUSHLOCKXA(nlen,nptr,elen,eptr)				\
    (CachePushLockX)(nlen,nptr,elen,eptr)			/* SOH393 */
#define	CACHEPUSHLOCKW(nlen,nptr) CachePushLockW(nlen,nptr)
#define	CACHEPUSHLOCKXW(nlen,nptr,elen,eptr)				\
    CachePushLockXW(nlen,nptr,elen,eptr)
								/* JO1924- */
#define CACHEDOFUN(flags,narg)	CacheDoFun(flags,narg)
#define	CACHEPUSHCLASSMETHODA(clen,cptr,mlen,mptr,flg)			\
		(CachePushClassMethod)(clen,cptr,mlen,mptr,flg) /* SOH393 */
#define	CACHEPUSHCLASSMETHODW(clen,cptr,mlen,mptr,flg)			\
	       CachePushClassMethodW(clen,cptr,mlen,mptr,flg)
#define	CACHEPUSHMETHODA(oref,mlen,mptr,flg)				\
		(CachePushMethod)(oref,mlen,mptr,flg)		/* SOH393 */
#define	CACHEPUSHMETHODW(oref,mlen,mptr,flg)				\
		CachePushMethodW(oref,mlen,mptr,flg)
#define CACHEINVOKECLASSMETHOD(narg) CacheInvokeClassMethod(narg)
#define CACHEINVOKEMETHOD(narg)	 CacheInvokeMethod(narg)
#define	CACHEPUSHPROPERTYA(oref,plen,pptr)				\
		(CachePushProperty)(oref,plen,pptr)		/* SOH393 */
#define	CACHEPUSHPROPERTYW(oref,plen,pptr) CachePushPropertyW(oref,plen,pptr)
#define	CACHEGETPROPERTY()	CacheGetProperty()
#define CACHESETPROPERTY()	CacheSetProperty()
#define	CACHEINCREMENTCOUNTOREF(oref) CacheIncrementCountOref(oref)
#define	CACHECLOSEOREF(oref)	CacheCloseOref(oref)
#define	CACHEPUSHCVTW(len,ptr)	CachePushCvtW(len,ptr)
#define CACHEPOPCVTW(lenp,strp)	CachePopCvtW(lenp,strp)
#define	CACHEPUSHEXSTRCVTW(sstrp)	CachePushExStrCvtW(sstrp)
#define CACHEPOPEXSTRCVTW(sstrp)	CachePopExStrCvtW(sstrp)
#define	CACHEPUSHLIST(len,ptr)	CachePushList(len,ptr)
#define CACHEPOPLIST(lenp,strp)	CachePopList(lenp,strp)
#define CACHEPUSHPTR(ptr)	CachePushPtr(ptr)
#define CACHEPOPPTR(ptrp)	CachePopPtr(ptrp)
								/* JO1924+ */
#define CACHEACQUIRELOCK(nsub,flg,tout,rval) CacheAcquireLock(nsub,flg,tout,rval)
#define CACHERELEASELOCK(nsub,flg) CacheReleaseLock(nsub,flg)
#define CACHERELEASEALLLOCKS() CacheReleaseAllLocks()
#define CACHETSTART() CacheTStart()
#define CACHETCOMMIT() CacheTCommit()
#define CACHETROLLBACK(nlev) CacheTRollback(nlev)
#define CACHETLEVEL() CacheTLevel()
#define CACHESPCSEND(len,ptr) CacheSPCSend(len,ptr)
#define CACHESPCRECEIVE(lenp,ptr) CacheSPCReceive(lenp,ptr)
#define CACHESETDIR(dir) CacheSetDir(dir)
#if	CACHE_HSTR						/* LRS1034+ */
#define	CACHESTARTH(flags,tout,prinp,prout) CacheStartH(flags,tout,prinp,prout)
								/* STC1114+ */
#define	CACHESECURESTARTH(username,password,exename,flags,tout,prinp,prout) CacheSecureStartH(username,password,exename,flags,tout,prinp,prout)
#define	CACHECHANGEPASSWORDH(username,oldpassword,newpassword) CacheChangePasswordH(username,oldpassword,newpassword)
								/* STC1114- */
#define	CACHEEXECUTEH(cmd)	CacheExecuteH(cmd)
#define	CACHEEVALH(expr)	CacheEvalH(expr)
#define	CACHEPROMPTH(rbuf)	CachePromptH(rbuf)
#define	CACHEERRXLATEH(code,rbuf) CacheErrxlateH(code,rbuf)
#define CACHEERRORH(msg,src,offp) CacheErrorH(msg,src,offp)
#define	CACHECVTINH(src,tbl,res) CacheCvtInH(src,tbl,res)
#define	CACHECVTOUTH(src,tbl,res) CacheCvtOutH(src,tbl,res)
#define	CACHEPUSHSTRH(len,ptr)	CachePushStrH(len,ptr)
#define	CACHEPUSHEXSTRH(sptr)	CachePushExStrH(sptr)			/* KMK497 */
#define	CACHEPUSHEXSTRCVTH(sptr)	CachePushExStrCvtH(sptr)			/* KMK497 */
#define	CACHEPUSHRTNH(rflag,tlen,tptr,nlen,nptr)			\
	CachePushRtnH(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHRTNXH(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
	CachePushRtnXH(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHFUNCH(rflag,tlen,tptr,nlen,nptr)			\
	CachePushFuncH(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHFUNCXH(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)	\
	CachePushFuncXH(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHGLOBALH(nlen,nptr) CachePushGlobalH(nlen,nptr)
#define	CACHEPUSHGLOBALXH(nlen,nptr,elen,eptr)				\
	      CachePushGlobalXH(nlen,nptr,elen,eptr)
#define	CACHEPUSHCLASSMETHODH(clen,cptr,mlen,mptr,flg)			\
	       CachePushClassMethodH(clen,cptr,mlen,mptr,flg)
#define	CACHEPUSHMETHODH(oref,mlen,mptr,flg)				\
		CachePushMethodH(oref,mlen,mptr,flg)
#define	CACHEPUSHPROPERTYH(oref,plen,pptr) CachePushPropertyH(oref,plen,pptr)
#define CACHEPOPSTRH(lenp,strp)	CachePopStrH(lenp,strp)
#define	CACHEPOPEXSTRH(sstrp)	CachePopExStrH(sstrp)			/* KMK497 */
#define	CACHEPOPEXSTRCVTH(sstrp)	CachePopExStrCvtH(sstrp)			/* KMK497 */
#define	CACHEPUSHCVTH(len,ptr)	CachePushCvtH(len,ptr)
#define CACHEPOPCVTH(lenp,strp)	CachePopCvtH(lenp,strp)
#define	CACHEPUSHLOCKH(nlen,nptr) CachePushLockH(nlen,nptr)
#define	CACHEPUSHLOCKXH(nlen,nptr,elen,eptr) CachePushLockXH(nlen,nptr,elen,eptr)
#define	CACHECVTEXSTRINH(src,tbl,res) CacheCvtExStrInH(src,tbl,res)
#define	CACHECVTEXSTROUTH(src,tbl,res) CacheCvtExStrOutH(src,tbl,res)
#else	/* CACHE_HSTR */
#define	CACHESTARTH(flags,tout,prinp,prout) CacheStartW(flags,tout,prinp,prout)
									/* STC1114+ */
#define	CACHESECURESTARTH(username,password,exename,flags,tout,prinp,prout) CacheSecureStartW(username,password,exename,flags,tout,prinp,prout)
#define	CACHECHANGEPASSWORDH(username,oldpassword,newpassword) CacheChangePasswordW(username,oldpassword,newpassword)
									/* STC1114- */
#define	CACHEEXECUTEH(cmd)	CacheExecuteW(cmd)
#define	CACHEEVALH(expr)	CacheEvalW(expr)
#define	CACHEPROMPTH(rbuf)	CachePromptW(rbuf)
#define	CACHEERRXLATEH(code,rbuf) CacheErrxlateW(code,rbuf)
#define CACHEERRORH(msg,src,offp) CacheErrorW(msg,src,offp)
#define	CACHECVTINH(src,tbl,res) CacheCvtInW(src,tbl,res)
#define	CACHECVTOUTH(src,tbl,res) CacheCvtOutW(src,tbl,res)
#define	CACHEPUSHSTRH(len,ptr)	CachePushStrW(len,ptr)
#define	CACHEPUSHEXSTRH(sptr)	CachePushExStrW(sptr)			/* KMK497 */
#define	CACHEPUSHEXSTRCVTH(sptr)	CachePushExStrCvtW(sptr)	/* SAP1806 */
#define	CachePushRtnH(rflag,tlen,tptr,nlen,nptr)			\
	CachePushRtnW(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHRTNXH(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
	CachePushRtnXW(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHFUNCH(rflag,tlen,tptr,nlen,nptr)			\
	CachePushFuncW(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHFUNCXH(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)	\
	CachePushFuncXW(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHGLOBALH(nlen,nptr) CachePushGlobalW(nlen,nptr)
#define	CACHEPUSHGLOBALXH(nlen,nptr,elen,eptr)				\
	      CachePushGlobalXW(nlen,nptr,elen,eptr)
#define	CACHEPUSHCLASSMETHODH(clen,cptr,mlen,mptr,flg)			\
	       CachePushClassMethodW(clen,cptr,mlen,mptr,flg)
#define	CACHEPUSHMETHODH(oref,mlen,mptr,flg)				\
		CachePushMethodW(oref,mlen,mptr,flg)
#define	CACHEPUSHPROPERTYH(oref,plen,pptr) CachePushPropertyW(oref,plen,pptr)
#define CACHEPOPSTRH(lenp,strp)	CachePopStrW(lenp,strp)
#define	CACHEPOPEXSTRH(sstrp)	CachePopExStrW(sstrp)		/* KMK497 */
#define	CACHEPOPEXSTRCVTH(sstrp)	CachePopExStrW(sstrp)   /* SAP829 */
#define	CACHEPUSHCVTH(len,ptr)	CachePushCvtW(len,ptr)
#define CACHEPOPCVTH(lenp,strp)	CachePopCvtW(lenp,strp)
#define	CACHEPUSHLOCKH(nlen,nptr) CachePushLockW(nlen,nptr)
#define	CACHEPUSHLOCKXH(nlen,nptr,elen,eptr)				\
    CachePushLockXW(nlen,nptr,elen,eptr)
#define	CACHECVTEXSTRINH(src,tbl,res) CacheCvtExStrInW(src,tbl,res)
#define	CACHECVTEXSTROUTH(src,tbl,res) CacheCvtExStrOutW(src,tbl,res)
#endif	/* CACHE_HSTR */					/* LRS1034- */
#define	CACHEEXSTRKILL(obj) CacheExStrKill(obj)			/* KMK409+ */
#define	CACHEEXSTRNEWA(zstr,size) (CacheExStrNew)(zstr,size)	/*KMK495+,SOH393*/
#define	CACHEEXSTRNEWW(zstr,size) CacheExStrNewW(zstr,size)
#define	CACHEEXSTRNEWH(zstr,size) CacheExStrNewH(zstr,size)	/* KMK409- *//* KMK495- */
								/* JO1924- */
#define	CACHEPUSHEXLIST(sstr) CachePushExList(sstr)		/* KMK523+ */
#define CACHEPOPEXLIST(sstr) CachePopExList(sstr)			/* KMK523- */
								/* JO2405+ */
#define	CACHEPUSHSSVNA(nlen,nptr) (CachePushSSVN)(nlen,nptr)	/* SOH393 */
#define	CACHEPUSHSSVNXA(nlen,nptr,elen,eptr)				\
		(CachePushSSVNX)(nlen,nptr,elen,eptr)		/* SOH393 */
#define	CACHEPUSHSSVNW(nlen,nptr) CachePushSSVNW(nlen,nptr)
#define	CACHEPUSHSSVNXW(nlen,nptr,elen,eptr)				\
		CachePushSSVNXW(nlen,nptr,elen,eptr)
#define	CACHEADDSSVNA(nlen,nptr) (CacheAddSSVN)(nlen,nptr)	/* SOH393 */
#define	CACHEADDSSVNXA(nlen,nptr,elen,eptr)				\
	        (CacheAddSSVNX)(nlen,nptr,elen,eptr)		/* SOH393 */
#define	CACHEADDSSVNW(nlen,nptr) CacheAddSSVNW(nlen,nptr)
#define	CACHEADDSSVNXW(nlen,nptr,elen,eptr)				\
		CacheAddSSVNXW(nlen,nptr,elen,eptr)
#if	CACHE_HSTR
#define	CACHEPUSHSSVNH(nlen,nptr) CachePushSSVNH(nlen,nptr)
#define	CACHEPUSHSSVNXH(nlen,nptr,elen,eptr)				\
		CachePushSSVNXH(nlen,nptr,elen,eptr)
#define	CACHEADDSSVNH(nlen,nptr) CacheAddSSVNH(nlen,nptr)
#define	CACHEADDSSVNXH(nlen,nptr,elen,eptr)				\
	        CacheAddSSVNXH(nlen,nptr,elen,eptr)
#define	CACHEADDGLOBALH(nlen,nptr) CacheAddGlobalH(nlen,nptr)
#define	CACHEADDGLOBALXH(nlen,nptr,elen,eptr)				\
	        CacheAddGlobalXH(nlen,nptr,elen,eptr)
#else
#define	CACHEPUSHSSVNH(nlen,nptr) CachePushSSVNW(nlen,nptr)
#define	CACHEPUSHSSVNXH(nlen,nptr,elen,eptr)				\
		CachePushSSVNXW(nlen,nptr,elen,eptr)
#define	CACHEADDSSVNH(nlen,nptr) CacheAddSSVNW(nlen,nptr)
#define	CACHEADDSSVNXH(nlen,nptr,elen,eptr)				\
	        CacheAddSSVNXW(nlen,nptr,elen,eptr)
#define	CACHEADDGLOBALH(nlen,nptr) CacheAddGlobalW(nlen,nptr)
#define	CACHEADDGLOBALXH(nlen,nptr,elen,eptr)				\
		CacheAddGlobalXW(nlen,nptr,elen,eptr)
#endif
#define	CACHEADDGLOBALA(nlen,nptr) (CacheAddGlobal)(nlen,nptr)	/* SOH393 */
#define	CACHEADDGLOBALXA(nlen,nptr,elen,eptr)				\
		(CacheAddGlobalX)(nlen,nptr,elen,eptr)		/* SOH393 */
#define	CACHEADDGLOBALW(nlen,nptr) CacheAddGlobalW(nlen,nptr)
#define	CACHEADDGLOBALXW(nlen,nptr,elen,eptr)				\
		CacheAddGlobalXW(nlen,nptr,elen,eptr)
#define	CACHEADDSSVNDESCRIPTOR(num)	CacheAddSSVNDescriptor(num)
#define	CACHEADDGLOBALESCRIPTOR(num)	CacheAddGlobalDescriptor(num)
#define	CACHEADDLOCALDESCRIPTOR(num)	CacheAddLocalDescriptor(num)
#define CACHESSVNGET(num)		CacheSSVNGet(num)
#define CACHESSVNKILL(num,val)		CacheSSVNKill(num,val)
#define CACHESSVNORDER(num,val)		CacheSSVNOrder(num,val)
#define CACHESSVNQUERY(num,val)		CacheSSVNQuery(num,val)
#define CACHESSVNDATA(num)		CacheSSVNData(num)
#define CACHEMERGE()			CacheMerge()		/* JO2405- */
#define CACHEENABLEMULTITHREAD()	CacheEnableMultiThread() /* JO2410 */
#define CACHEIKNOWFUNCS(funcsp)				\
		CacheiKnowFuncs(funcsp)
#else	/* ZF_DLL */	/*==============================*/
/*
			Interface Function Definitions

	The export function table entries are used for calling back
	into Cache from a dynamically loaded user library module,
	i.e. one loaded using $ZF(-n).
*/
#define	CACHEEXECUTEA(cmd)	(*cachefunctabp->CacheExecuteA)(cmd)
#define	CACHEEXECUTEW(cmd)	(*cachefunctabp->CacheExecuteW)(cmd)
#define	CACHEEVALA(expr)	(*cachefunctabp->CacheEvalA)(expr)
#define	CACHEEVALW(expr)	(*cachefunctabp->CacheEvalW)(expr)
#define	CACHEPROMPTA(rbuf)	(*cachefunctabp->CachePromptA)(rbuf)
#define	CACHEPROMPTW(rbuf)	(*cachefunctabp->CachePromptW)(rbuf)
#define	CACHEERRXLATEA(code,rbuf) (*cachefunctabp->CacheErrxlateA)(code,rbuf)
#define	CACHEERRXLATEW(code,rbuf) (*cachefunctabp->CacheErrxlateW)(code,rbuf)
#define CACHEERRORA(msg,src,offp) (*cachefunctabp->CacheErrorA)(msg,src,offp)
#define CACHEERRORW(msg,src,offp) (*cachefunctabp->CacheErrorW)(msg,src,offp)
#define	CACHECONVERT(type,rbuf)	(*cachefunctabp->CacheConvert)(type,rbuf)
#define	CACHETYPE()		(*cachefunctabp->CacheType)()
#define	CACHEABORT(type)	(*cachefunctabp->CacheAbort)(type)
#ifndef	__VMS
#define	CACHESIGNAL(signal)	(*cachefunctabp->CacheSignal)(signal)
#endif
#define	CACHEPUSHINT(num)	(*cachefunctabp->CachePushInt)(num)
#define	CACHEPUSHDBL(num)	(*cachefunctabp->CachePushDbl)(num)
#define	CACHEPUSHIEEEDBL(num)	(*cachefunctabp->CachePushIEEEDbl)(num)
#define	CACHEPUSHOREF(oref)	(*cachefunctabp->CachePushOref)(oref)
#define	CACHEPUSHUNDEF()	(*cachefunctabp->CachePushUndef)()	/* CDS1472 */
#define CACHEPUSHINT64(num)     (*cachefunctabp->CachePushInt64)(num)   /* SAP1134 */
#define	CACHEPUSHSTRA(len,ptr)	(*cachefunctabp->CachePushStr)(len,ptr)
#define	CACHEPUSHSTRW(len,ptr)	(*cachefunctabp->CachePushStrW)(len,ptr)
#define	CACHEPUSHEXSTRA(sptr)	(*cachefunctabp->CachePushExStr)(sptr)	/* KMK497+ */
#define	CACHEPUSHEXSTRW(sptr)	(*cachefunctabp->CachePushExStrW)(sptr)	/* KMK497- */
#define	CACHEPUSHRTNA(rflag,tlen,tptr,nlen,nptr)			\
	(*cachefunctabp->CachePushRtn)(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHRTNXA(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
	(*cachefunctabp->CachePushRtnX)(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHRTNW(rflag,tlen,tptr,nlen,nptr)			\
	(*cachefunctabp->CachePushRtnW)(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHRTNXW(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
	(*cachefunctabp->CachePushRtnXW)(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHFUNCA(rflag,tlen,tptr,nlen,nptr)			\
	(*cachefunctabp->CachePushFunc)(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHFUNCXA(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)	\
	(*cachefunctabp->CachePushFuncX)(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHFUNCW(rflag,tlen,tptr,nlen,nptr)			\
	(*cachefunctabp->CachePushFuncW)(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHFUNCXW(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)	\
	(*cachefunctabp->CachePushFuncXW)(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHGLOBALA(nlen,nptr) (*cachefunctabp->CachePushGlobal)(nlen,nptr)
#define	CACHEPUSHGLOBALXA(nlen,nptr,elen,eptr)				\
	      (*cachefunctabp->CachePushGlobalX)(nlen,nptr,elen,eptr)
#define	CACHEPUSHGLOBALW(nlen,nptr) (*cachefunctabp->CachePushGlobalW)(nlen,nptr)
#define	CACHEPUSHGLOBALXW(nlen,nptr,elen,eptr)				\
	      (*cachefunctabp->CachePushGlobalXW)(nlen,nptr,elen,eptr)
#define CACHEUNPOP()		(*cachefunctabp->CacheUnPop)()
#define CACHEPOP(arg)		(*cachefunctabp->CachePop)(arg)
#define CACHEPOPINT(nump)	(*cachefunctabp->CachePopInt)(nump)
#define CACHEPOPDBL(nump)	(*cachefunctabp->CachePopDbl)(nump)
#define CACHEPOPOREF(orefp)	(*cachefunctabp->CachePopOref)(orefp)
#define CACHEPOPINT64(num)      (*cachefunctabp->CachePopInt64)(num)   /* SAP1134 */
#define CACHEPOPSTRA(lenp,strp)	(*cachefunctabp->CachePopStr)(lenp,strp)
#define CACHEPOPSTRW(lenp,strp)	(*cachefunctabp->CachePopStrW)(lenp,strp)
#define CACHEPOPEXSTRA(sstrp)	(*cachefunctabp->CachePopExStr)(sstrp)	/* KMK497+ */
#define CACHEPOPEXSTRW(sstrp)	(*cachefunctabp->CachePopExStrW)(sstrp)	/* KMK497- */
								/* SOH392+ */
#define CACHESETVAR(len,namep) 	(*cachefunctabp->CacheSetVar)(len,namep)
#define CACHEGETVAR(len,namep) 	(*cachefunctabp->CacheGetVar)(len,namep)
								/* SOH392- */
#define CACHEDORTN(flags,narg)	(*cachefunctabp->CacheDoRtn)(flags,narg)
#define CACHEEXTFUN(flags,narg)	(*cachefunctabp->CacheExtFun)(flags,narg)
#define CACHEGLOBALGET(narg,flag) (*cachefunctabp->CacheGlobalGet)(narg,flag)
#define CACHEGLOBALSET(narg)	(*cachefunctabp->CacheGlobalSet)(narg)
#define CACHEGLOBALINCREMENT(narg) (*cachefunctabp->CacheGlobalIncrement)(narg)
								/* JO1874+ */
#define CACHEGLOBALQUERY(narg,dir,valueflag) \
		(*cachefunctabp->CacheGlobalQuery)(narg,dir,valueflag)
#define CACHEGLOBALORDER(narg,dir,valueflag) \
		(*cachefunctabp->CacheGlobalOrder)(narg,dir,valueflag)
#define CACHEGLOBALDATA(narg,valueflag) \
		(*cachefunctabp->CacheGlobalData)(narg,valueflag)
#define CACHEGLOBALKILL(narg,nodeonly) \
		(*cachefunctabp->CacheGlobalKill)(narg,nodeonly)
#define CACHEGLOBALRELEASE() (*cachefunctabp->CacheGlobalRelease)()
								/* JO1874- */
#define CACHEDOFUN(flags,narg)	(*cachefunctabp->CacheDoFun)(flags,narg)
#define	CACHEPUSHCLASSMETHODA(clen,cptr,mlen,mptr,flg)			\
		(*cachefunctabp->CachePushClassMethod)(clen,cptr,mlen,mptr,flg)
#define	CACHEPUSHCLASSMETHODW(clen,cptr,mlen,mptr,flg)			\
	       (*cachefunctabp->CachePushClassMethodW)(clen,cptr,mlen,mptr,flg)
#define	CACHEPUSHMETHODA(oref,mlen,mptr,flg)				\
		(*cachefunctabp->CachePushMethod)(oref,mlen,mptr,flg)
#define	CACHEPUSHMETHODW(oref,mlen,mptr,flg)				\
		(*cachefunctabp->CachePushMethodW)(oref,mlen,mptr,flg)
#define CACHEINVOKECLASSMETHOD(narg)					\
		(*cachefunctabp->CacheInvokeClassMethod)(narg)
#define CACHEINVOKEMETHOD(narg)	(*cachefunctabp->CacheInvokeMethod)(narg)
#define	CACHEPUSHPROPERTYA(oref,plen,pptr)				\
		(*cachefunctabp->CachePushProperty)(oref,plen,pptr)
#define	CACHEPUSHPROPERTYW(oref,plen,pptr)				\
		(*cachefunctabp->CachePushPropertyW)(oref,plen,pptr)
#define CACHEGETPROPERTY()	(*cachefunctabp->CacheGetProperty)()
#define CACHESETPROPERTY()	(*cachefunctabp->CacheSetProperty)()
#define	CACHEINCREMENTCOUNTOREF(oref)					\
		(*cachefunctabp->CacheIncrementCountOref)(oref)
#define	CACHECLOSEOREF(oref)	(*cachefunctabp->CacheCloseOref)(oref)
#define	CACHEPUSHCVTW(len,ptr)	(*cachefunctabp->CachePushCvtW)(len,ptr)
#define CACHEPOPCVTW(lenp,strp)	(*cachefunctabp->CachePopCvtW)(lenp,strp)
#define	CACHEPUSHLIST(len,ptr)	(*cachefunctabp->CachePushList)(len,ptr)
#define CACHEPOPLIST(lenp,strp)	(*cachefunctabp->CachePopList)(lenp,strp)
#define CACHEPUSHPTR(ptr)	(*cachefunctabp->CachePushPtr)(ptr)
#define CACHEPOPPTR(ptrp)	(*cachefunctabp->CachePopPtr)(ptrp)
#define	CACHECONTEXT()		(*cachefunctabp->CacheContext)()
#define	CACHECTRL(flags)	(*cachefunctabp->CacheCtrl)(flags)
#define	CACHECVTINA(src,tbl,res) (*cachefunctabp->CacheCvtInA)(src,tbl,res)
#define	CACHECVTINW(src,tbl,res) (*cachefunctabp->CacheCvtInW)(src,tbl,res)
#define	CACHECVTOUTA(src,tbl,res) (*cachefunctabp->CacheCvtOutA)(src,tbl,res)
#define	CACHECVTOUTW(src,tbl,res) (*cachefunctabp->CacheCvtOutW)(src,tbl,res)
#define	CACHECVTEXSTRINA(src,tbl,res) (*cachefunctabp->CacheCvtExStrInA)(src,tbl,res)
#define	CACHECVTEXSTRINW(src,tbl,res) (*cachefunctabp->CacheCvtExStrInW)(src,tbl,res)
#define	CACHECVTEXSTROUTA(src,tbl,res) (*cachefunctabp->CacheCvtExStrOutA)(src,tbl,res)
#define	CACHECVTEXSTROUTW(src,tbl,res) (*cachefunctabp->CacheCvtExStrOutW)(src,tbl,res)
								/* JO1924+ */
#define	CACHEPUSHLOCKA(nlen,nptr) (*cachefunctabp->CachePushLock)(nlen,nptr)
#define	CACHEPUSHLOCKXA(nlen,nptr,elen,eptr)				\
    (*cachefunctabp->CachePushLockX)(nlen,nptr,elen,eptr)
#define	CACHEPUSHLOCKW(nlen,nptr) (*cachefunctabp->CachePushLockW)(nlen,nptr)
#define	CACHEPUSHLOCKXW(nlen,nptr,elen,eptr)				\
    (*cachefunctabp->CachePushLockXW)(nlen,nptr,elen,eptr)
#define CACHEACQUIRELOCK(nsub,flg,tout,rval) \
    (*cachefunctabp->CacheAcquireLock)(nsub,flg,tout,rval)
#define CACHERELEASELOCK(nsub,flg) (*cachefunctabp->CacheReleaseLock)(nsub,flg)
#define CACHERELEASEALLLOCKS() (*cachefunctabp->CacheReleaseAllLocks)()
#define CACHETSTART() (*cachefunctabp->CacheTStart)()
#define CACHETCOMMIT() (*cachefunctabp->CacheTCommit)()
#define CACHETROLLBACK(nlev) (*cachefunctabp->CacheTRollback)(nlev)
#define CACHETLEVEL() (*cachefunctabp->CacheTLevel)()
#define CACHESPCSEND(len,ptr) (*cachefunctabp->CacheSPCSend(len,ptr)         /* JMM072+ */
#define CACHESPCRECEIVE(lenp,ptr) (*cachefunctabp->CacheSPCReceive(lenp,ptr) /* JMM072- */
#if	CACHE_HSTR						/* LRS1034+ */
#define	CACHEEXECUTEH(cmd)	(*cachefunctabp->CacheExecuteH)(cmd)
#define	CACHEEVALH(expr)	(*cachefunctabp->CacheEvalH)(expr)
#define	CACHEPROMPTH(rbuf)	(*cachefunctabp->CachePromptH)(rbuf)
#define	CACHEERRXLATEH(code,rbuf) (*cachefunctabp->CacheErrxlateH)(code,rbuf)
#define CACHEERRORH(msg,src,offp) (*cachefunctabp->CacheErrorH)(msg,src,offp)
#define	CACHECVTINH(src,tbl,res) (*cachefunctabp->CacheCvtInH)(src,tbl,res)
#define	CACHECVTOUTH(src,tbl,res) (*cachefunctabp->CacheCvtOutH)(src,tbl,res)
#define	CACHEPUSHSTRH(len,ptr)	(*cachefunctabp->CachePushStrH)(len,ptr)
#define	CACHEPUSHEXSTRH(sptr)	(*cachefunctabp->CachePushExStrH)(sptr)	/* KMK497 */
#define	CACHEPUSHEXSTRCVTH(sptr)	(*cachefunctabp->CachePushExStrCvtH)(sptr) /* KMK497 */
#define	CACHEPUSHRTNH(rflag,tlen,tptr,nlen,nptr)			\
	(*cachefunctabp->CachePushRtnH)(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHRTNXH(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
	(*cachefunctabp->CachePushRtnXH)(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHFUNCH(rflag,tlen,tptr,nlen,nptr)			\
	(*cachefunctabp->CachePushFuncH)(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHFUNCXH(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)	\
	(*cachefunctabp->CachePushFuncXH)(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHGLOBALH(nlen,nptr) (*cachefunctabp->CachePushGlobalH)(nlen,nptr)
#define	CACHEPUSHGLOBALXH(nlen,nptr,elen,eptr)				\
	      (*cachefunctabp->CachePushGlobalXH)(nlen,nptr,elen,eptr)
#define	CACHEPUSHCLASSMETHODH(clen,cptr,mlen,mptr,flg)			\
	       (*cachefunctabp->CachePushClassMethodH)(clen,cptr,mlen,mptr,flg)
#define	CACHEPUSHMETHODH(oref,mlen,mptr,flg)				\
		(*cachefunctabp->CachePushMethodH)(oref,mlen,mptr,flg)
#define	CACHEPUSHPROPERTYH(oref,plen,pptr)				\
		(*cachefunctabp->CachePushPropertyH)(oref,plen,pptr)
#define CACHEPOPSTRH(lenp,strp)	(*cachefunctabp->CachePopStrH)(lenp,strp)
#define	CACHEPOPEXSTRH(sstrp)(*cachefunctabp->CachePopExStrH)(sstrp)	/* KMK497 */
#define	CACHEPOPEXSTRCVTH(sstrp)(*cachefunctabp->CachePopExStrCvtH)(sstrp) /* KMK497 */
#define	CACHEPUSHCVTH(len,ptr)	(*cachefunctabp->CachePushCvtH)(len,ptr)
#define CACHEPOPCVTH(lenp,strp)	(*cachefunctabp->CachePopCvtH)(lenp,strp)
#define	CACHEPUSHLOCKH(nlen,nptr) (*cachefunctabp->CachePushLockH)(nlen,nptr)
#define	CACHEPUSHLOCKXH(nlen,nptr,elen,eptr)				\
    (*cachefunctabp->CachePushLockXH)(nlen,nptr,elen,eptr)
#define	CACHECVTEXSTRINH(src,tbl,res) (*cachefunctabp->CacheCvtExStrInH)(src,tbl,res)
#define	CACHECVTEXSTROUTH(src,tbl,res) (*cachefunctabp->CacheCvtExStrOutH)(src,tbl,res)
#else	/* CACHE_HSTR */
#define	CACHEEXECUTEH(cmd)	(*cachefunctabp->CacheExecuteW)(cmd)
#define	CACHEEVALH(expr)	(*cachefunctabp->CacheEvalW)(expr)
#define	CACHEPROMPTH(rbuf)	(*cachefunctabp->CachePromptW)(rbuf)
#define	CACHEERRXLATEH(code,rbuf) (*cachefunctabp->CacheErrxlateW)(code,rbuf)
#define CACHEERRORH(msg,src,offp) (*cachefunctabp->CacheErrorW)(msg,src,offp)
#define	CACHECVTINH(src,tbl,res) (*cachefunctabp->CacheCvtInW)(src,tbl,res)
#define	CACHECVTOUTH(src,tbl,res) (*cachefunctabp->CacheCvtOutW)(src,tbl,res)
#define	CACHEPUSHSTRH(len,ptr)	(*cachefunctabp->CachePushStrW)(len,ptr)
#define	CACHEPUSHEXSTRH(sptr)	(*cachefunctabp->CachePushExStrW)(sptr)	/* KMK497 */
#define	CACHEPUSHEXSTRCVTH(sptr)	(*cachefunctabp->CachePushExStrW)(sptr) /* SAP1026 */
#define	CACHEPUSHRTNH(rflag,tlen,tptr,nlen,nptr)			\
	(*cachefunctabp->CachePushRtnW)(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHRTNXH(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
	(*cachefunctabp->CachePushRtnXW)(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHFUNCH(rflag,tlen,tptr,nlen,nptr)			\
	(*cachefunctabp->CachePushFuncW)(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHFUNCXH(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)	\
	(*cachefunctabp->CachePushFuncXW)(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHGLOBALH(nlen,nptr) (*cachefunctabp->CachePushGlobalW)(nlen,nptr)
#define	CACHEPUSHGLOBALXH(nlen,nptr,elen,eptr)				\
	      (*cachefunctabp->CachePushGlobalXW)(nlen,nptr,elen,eptr)
#define	CACHEPUSHCLASSMETHODH(clen,cptr,mlen,mptr,flg)			\
	       (*cachefunctabp->CachePushClassMethodW)(clen,cptr,mlen,mptr,flg)
#define	CACHEPUSHMETHODH(oref,mlen,mptr,flg)				\
		(*cachefunctabp->CachePushMethodW)(oref,mlen,mptr,flg)
#define	CACHEPUSHPROPERTYH(oref,plen,pptr)				\
		(*cachefunctabp->CachePushPropertyW)(oref,plen,pptr)
#define CACHEPOPSTRH(lenp,strp)	(*cachefunctabp->CachePopStrW)(lenp,strp)
#define CACHEPOPEXSTRH(sstrp)	(*cachefunctabp->CachePopExStrW)(sstrp)	/* KMK497 */
#define CACHEPOPEXSTRCVTH(sstrp)	(*cachefunctabp->CachePopExStrW)(sstrp) /* SAP829 */
#define	CACHEPUSHCVTH(len,ptr)	(*cachefunctabp->CachePushCvtW)(len,ptr)
#define CACHEPOPCVTH(lenp,strp)	(*cachefunctabp->CachePopCvtW)(lenp,strp)
#define	CACHEPUSHLOCKH(nlen,nptr) (*cachefunctabp->CachePushLockW)(nlen,nptr)
#define	CACHEPUSHLOCKXH(nlen,nptr,elen,eptr)				\
    (*cachefunctabp->CachePushLockXW)(nlen,nptr,elen,eptr)
#define	CACHECVTEXSTRINH(src,tbl,res) (*cachefunctabp->CacheCvtExStrInW)(src,tbl,res)
#define	CACHECVTEXSTROUTH(src,tbl,res) (*cachefunctabp->CacheCvtExStrOutW)(src,tbl,res)
#endif	/* CACHE_HSTR */					/* LRS1034- */
#define	CACHEEXSTRKILL(obj) (*cachefunctabp->CacheExStrKill)(obj)	/* KMK409+ */
#define	CACHEEXSTRNEWA(zstr,size) (*cachefunctabp->CacheExStrNew)(zstr,size) /* KMK495+ */
#define	CACHEEXSTRNEWW(zstr,size) (*cachefunctabp->CacheExStrNewW)(zstr,size)
#define	CACHEEXSTRNEWH(zstr,size) (*cachefunctabp->CacheExStrNewH)(zstr,size)/* KMK409- *//* KMK495- */
#define	CACHEPUSHEXLIST(sstr) (*cachefunctabp->CachePushExList)(sstr) /* KMK523+ */
#define CACHEPOPEXLIST(sstr) (*cachefunctabp->CachePopExList)(sstr) /* KMK523- */
#define	CACHEPUSHEXSTRCVTW(sstrp)	(*cachefunctabp->CachePushExStrCvtW)(sstrp)
#define CACHEPOPEXSTRCVTW(sstrp)	(*cachefunctabp->CachePopExStrCvtW)(sstrp)
								/* JO2405+ */
#define	CACHEPUSHSSVNA(nlen,nptr) (*cachefunctabp->CachePushSSVN)(nlen,nptr)
#define	CACHEPUSHSSVNXA(nlen,nptr,elen,eptr)				\
		(*cachefunctabp->CachePushSSVNX)(nlen,nptr,elen,eptr)
#define	CACHEPUSHSSVNW(nlen,nptr) (*cachefunctabp->CachePushSSVNW)(nlen,nptr)
#define	CACHEPUSHSSVNXW(nlen,nptr,elen,eptr)				\
		(*cachefunctabp->CachePushSSVNXW)(nlen,nptr,elen,eptr)
#define	CACHEADDSSVNA(nlen,nptr) (*cachefunctabp->CacheAddSSVN)(nlen,nptr)
#define	CACHEADDSSVNXA(nlen,nptr,elen,eptr)				\
		(*cachefunctabp->CacheAddSSVNX)(nlen,nptr,elen,eptr)
#define	CACHEADDSSVNW(nlen,nptr) (*cachefunctabp->CacheAddSSVNW)(nlen,nptr)
#define	CACHEADDSSVNXW(nlen,nptr,elen,eptr)				\
		(*cachefunctabp->CacheAddSSVNXW)(nlen,nptr,elen,eptr)
#ifdef	CACHE_HSTR
#define	CACHEPUSHSSVNH(nlen,nptr) (*cachefunctabp->CachePushSSVNH)(nlen,nptr)
#define	CACHEPUSHSSVNXH(nlen,nptr,elen,eptr)				\
		(*cachefunctabp->CachePushSSVNXH)(nlen,nptr,elen,eptr)
#define	CACHEADDSSVNH(nlen,nptr) (*cachefunctabp->CacheAddSSVNH)(nlen,nptr)
#define	CACHEADDSSVNXH(nlen,nptr,elen,eptr)				\
		(*cachefunctabp->CacheAddSSVNXH)(nlen,nptr,elen,eptr)
#define	CACHEADDGLOBALH(nlen,nptr) (*cachefunctabp->CacheAddGlobalH)(nlen,nptr)
#define	CACHEADDGLOBALXH(nlen,nptr,elen,eptr)				\
		(*cachefunctabp->CacheAddGlobalXH)(nlen,nptr,elen,eptr)
#else
#define	CACHEPUSHSSVNH(nlen,nptr) (*cachefunctabp->CachePushSSVNW)(nlen,nptr)
#define	CACHEPUSHSSVNXH(nlen,nptr,elen,eptr)				\
		(*cachefunctabp->CachePushSSVNXW)(nlen,nptr,elen,eptr)
#define	CACHEADDSSVNH(nlen,nptr) (*cachefunctabp->CacheAddSSVNW)(nlen,nptr)
#define	CACHEADDSSVNXH(nlen,nptr,elen,eptr)				\
		(*cachefunctabp->CacheAddSSVNXW)(nlen,nptr,elen,eptr)
#define	CACHEADDGLOBALH(nlen,nptr) (*cachefunctabp->CacheAddGlobalW)(nlen,nptr)
#define	CACHEADDGLOBALXH(nlen,nptr,elen,eptr)				\
		(*cachefunctabp->CacheAddGlobalXW)(nlen,nptr,elen,eptr)
#endif
#define	CACHEADDGLOBALA(nlen,nptr) (*cachefunctabp->CacheAddGlobal)(nlen,nptr)
#define	CACHEADDGLOBALXA(nlen,nptr,elen,eptr)				\
		(*cachefunctabp->CacheAddGlobalX)(nlen,nptr,elen,eptr)
#define	CACHEADDGLOBALW(nlen,nptr) (*cachefunctabp->CacheAddGlobalW)(nlen,nptr)
#define	CACHEADDGLOBALXW(nlen,nptr,elen,eptr)				\
		(*cachefunctabp->CacheAddGlobalXW)(nlen,nptr,elen,eptr)
#define	CACHEADDSSVNDESCRIPTOR(num)					\
	        (*cachefunctabp->CacheAddSSVNDescriptor)(num)
#define	CACHEADDGLOBALDESCRIPTOR(num)					\
		(*cachefunctabp->CacheAddGlobalDescriptor)(num)
#define	CACHEADDLOCALDESCRIPTOR(num)					\
		(*cachefunctabp->CacheAddLocalDescriptor)(num)
#define CACHESSVNGET(num)	(*cachefunctabp->CacheSSVNGet)(num)
#define CACHESSVNKILL(num,val)	(*cachefunctabp->CacheSSVNKill)(num,val)
#define CACHESSVNORDER(num,val)	(*cachefunctabp->CacheSSVNOrder)(num,val)
#define CACHESSVNQUERY(num,val)	(*cachefunctabp->CacheSSVNQuery)(num,val)
#define CACHESSVNDATA(num)	(*cachefunctabp->CacheSSVNData)(num)
#define CACHEMERGE()		(*cachefunctabp->CacheMerge)()
								/* JO2405- */
								/* JO2410+ */
#define CACHEENABLEMULTITHREAD()				\
		(*cachefunctabp->CacheEnableMultiThread)()	/* JO2410- */
#define CACHEIKNOWFUNCS(funcsp)		     	\
		(*cachefunctabp->CacheiKnowFuncs)(funcsp)
#endif	/* ZF_DLL */	/*==============================*/

#ifdef	__VMS
/* CacheSignal is not implemented on Cache for VMS systems as VMS versions
   of Cache do not use signal handling. It is provided here as returning
   CACHE_SUCCESS so that programs compiled for VMS and for Unix do not need
   to be sensitive to this difference.
*/
#define CacheSignal(x)		CACHE_SUCCESS
#define	CACHESIGNAL(x)		CACHE_SUCCESS
#endif

/*	Default function definitions (ASCII/Unicode/wchar_t)
*/
#if	defined(CACHE_UNICODE)	/* Unicode character strings */
#define	CacheCvtIn		CacheCvtInW
#define	CacheCvtOut		CacheCvtOutW
#define	CacheCvtExStrIn		CacheCvtExStrInW
#define	CacheCvtExStrOut		CacheCvtExStrOutW
#define	CacheError		CacheErrorW
#define	CacheErrxlate		CacheErrxlateW
#define	CacheEval		CacheEvalW
#define	CacheExecute		CacheExecuteW
#define	CachePrompt		CachePromptW
#define	CacheStart		CacheStartW
#define	CacheSecureStart	CacheSecureStartW		/* STC1114+ */
#define	CacheChangePassword	CacheChangePasswordW		/* STC1114- */
#define	CACHECVTIN		CACHECVTINW
#define	CACHECVTOUT		CACHECVTOUTW
#define	CACHECVTEXSTRIN		CACHECVTEXSTRINW
#define	CACHECVTEXSTROUT		CACHECVTEXSTROUTW
#define	CACHEERROR		CACHEERRORW
#define	CACHEERRXLATE		CACHEERRXLATEW
#define	CACHEEVAL		CACHEEVALW
#define	CACHEEXECUTE		CACHEEXECUTEW
#define	CACHEPROMPT		CACHEPROMPTW
#define	CACHESECURESTART	CACHESECURESTARTW
#define	CACHESTART		CACHESTARTW
#define CACHEPOPCVT		CACHEPOPCVTW
#define	CACHEPUSHCVT		CACHEPUSHCVTW
#ifndef __VMS
#define CACHEPOPSTR		CACHEPOPSTRW
#define	CACHEPOPEXSTR		CACHEPOPEXSTRW			/* KMK497 */
#define	CACHEPUSHCLASSMETHOD	CACHEPUSHCLASSMETHODW
#define	CACHEPUSHFUNC		CACHEPUSHFUNCW
#define	CACHEPUSHFUNCX		CACHEPUSHFUNCXW
#define	CACHEPUSHGLOBAL		CACHEPUSHGLOBALW
#define	CACHEPUSHGLOBALX	CACHEPUSHGLOBALXW
#define	CACHEPUSHMETHOD		CACHEPUSHMETHODW
#define	CACHEPUSHPROPERTY	CACHEPUSHPROPERTYW
#define	CACHEPUSHRTN		CACHEPUSHRTNW
#define	CACHEPUSHRTNX		CACHEPUSHRTNXW
#define	CACHEPUSHSTR		CACHEPUSHSTRW
#define	CACHEPUSHEXSTR		CACHEPUSHEXSTRW			/* KMK497 */
#define	CACHEPUSHLOCK		CACHEPUSHLOCKW			/* JO1924+ */
#define	CACHEPUSHLOCKX		CACHEPUSHLOCKXW			/* JO1924- */
#define CACHEEXSTRNEW           CACHEEXSTRNEWW                   /* SAP799 */
#define	CACHEPUSHSSVN		CACHEPUSHSSVNW			/* JO2405+ */
#define	CACHEPUSHSSVNX		CACHEPUSHSSVNXW
#define	CACHEADDGLOBAL		CACHEADDGLOBALW
#define	CACHEADDGLOBALX		CACHEADDGLOBALXW
#define	CACHEADDSSVN		CACHEADDSSVNW
#define	CACHEADDSSVNX		CACHEADDSSVNXW			/* JO2405- */
#else	/* defined(__VMS) */					/* SOH393+ */
  /* In order to make C #define macro replacement work as desired,
     defines (0a) of the form #define XXX XXXW must be given macro arg lists so
     they look like (0b) #define XXX(a,b,c) XXXW(a,b,c).

     This must be done in those cases where:
     (1) there exists a define of the from #define XXXA(args) (Xxx)(args), and
     (2) there exists a define of the form #define Xxx XXX.

     Otherwise, the macro #define XXXA(args) (Xxx)(args) will not be expanded
     properly into (XXX)(args).  An attempt to use XXXA(args) will be expanded
     into (Xxx)(args) by macro (1); then macro (2) will change this into
     (XXX)(args); and then macro (0a) will turns this into (XXXW)(args); and
     this results in a call on (XXXW)(args) which is *NOT* the desired
     expansion of XXXA(args).  However, if macro (0a) is replace by (0b) then
     a call on XXXA(args) will stop expanding when (XXX)(args) results, which
     is the desired final expansion.
  */
#define CACHEPOPSTR(lenp,strp)	CACHEPOPSTRW(lenp,strp)
#define	CACHEPOPEXSTR(sstrp)	CACHEPOPEXSTRW(sstrp)
#define	CACHEPUSHCLASSMETHOD(clen,cptr,mlen,mptr,flg)			\
		CACHEPUSHCLASSMETHODW(clen,cptr,mlen,mptr,flg)
#define	CACHEPUSHFUNC(rflag,tlen,tptr,nlen,nptr)			\
		CACHEPUSHFUNCW(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHFUNCX(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
		CACHEPUSHFUNCXW(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHGLOBAL(nlen,nptr)	CACHEPUSHGLOBALW(nlen,nptr)
#define	CACHEPUSHGLOBALX(nlen,nptr,elen,eptr)				\
		CACHEPUSHGLOBALXW(nlen,nptr,elen,eptr)
#define	CACHEPUSHMETHOD(oref,mlen,mptr,flg)				\
		CACHEPUSHMETHODW(oref,mlen,mptr,flg)
#define	CACHEPUSHPROPERTY(oref,plen,pptr)				\
		CACHEPUSHPROPERTYW(oref,plen,pptr)
#define	CACHEPUSHRTN(rflag,tlen,tptr,nlen,nptr)				\
		CACHEPUSHRTNW(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHRTNX(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
		CACHEPUSHRTNXW(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHSTR(len,ptr)	CACHEPUSHSTRW(len,ptr)
#define	CACHEPUSHEXSTR(sptr)	CACHEPUSHEXSTRW(sptr)
#define	CACHEPUSHLOCK(nlen,nptr) CACHEPUSHLOCKW(nlen,nptr)
#define	CACHEPUSHLOCKX(nlen,nptr,elen,eptr)				\
		CACHEPUSHLOCKXW(nlen,nptr,elen,eptr)
#define	CACHEEXSTRNEW(zstr,size) CACHEEXSTRNEWW(zstr,size)
#define	CACHEPUSHSSVN(nlen,nptr) CACHEPUSHSSVNW(nlen,nptr)
#define	CACHEPUSHSSVNX(nlen,nptr,elen,eptr)				\
		CACHEPUSHSSVNXW(nlen,nptr,elen,eptr)
#define	CACHEADDGLOBAL(nlen,nptr) CACHEADDGLOBALW(nlen,nptr)
#define	CACHEADDGLOBALX(nlen,nptr,elen,eptr)				\
		CACHEADDGLOBALXW(nlen,nptr,elen,eptr)
#define	CACHEADDSSVN(nlen,nptr)	CACHEADDSSVNW(nlen,nptr)
#define	CACHEADDSSVNX(nlen,nptr,elen,eptr)				\
		CACHEADDSSVNXW(nlen,nptr,elen,eptr)
#endif								/* SOH393- */
#elif	defined(CACHE_WCHART)	/* wchar_t character strings */	/* LRS1034+ */
#define	CacheCvtIn		CacheCvtInH
#define	CacheCvtOut		CacheCvtOutH
#define	CacheCvtExStrIn		CacheCvtExStrInW
#define	CacheCvtExStrOut		CacheCvtExStrOutW
#define	CacheError		CacheErrorH
#define	CacheErrxlate		CacheErrxlateH
#define	CacheEval		CacheEvalH
#define	CacheExecute		CacheExecuteH
#define	CachePrompt		CachePromptH
#define	CacheStart		CacheStartH
#define	CacheSecureStart	CacheSecureStartH		/* STC1114+ */
#define	CacheChangePassword	CacheChangePasswordH		/* STC1114- */
#define	CACHECVTIN		CACHECVTINH
#define	CACHECVTOUT		CACHECVTOUTH
#define	CACHECVTEXSTRIN		CACHECVTEXSTRINH
#define	CACHECVTEXSTROUT		CACHECVTEXSTROUTH
#define	CACHEERROR		CACHEERRORH
#define	CACHEERRXLATE		CACHEERRXLATEH
#define	CACHEEVAL		CACHEEVALH
#define	CACHEEXECUTE		CACHEEXECUTEH
#define	CACHEPROMPT		CACHEPROMPTH
#define	CACHESTART		CACHESTARTH
#define	CACHESECURESTART	CACHESECURESTARTH		/* STC1114+ */
#define	CACHECHANGEPASSWORD	CACHECHANGEPASSWORDH		/* STC11141 */
#define CACHEPOPCVT		CACHEPOPCVTH
#define	CACHEPUSHCVT		CACHEPUSHCVTH
#ifndef __VMS
#define CACHEPOPSTR		CACHEPOPSTRH
#define	CACHEPOPEXSTR		CACHEPOPEXSTRH			/* KMK497 */
#define	CACHEPUSHCLASSMETHOD	CACHEPUSHCLASSMETHODH
#define	CACHEPUSHFUNC		CACHEPUSHFUNCH
#define	CACHEPUSHFUNCX		CACHEPUSHFUNCXH
#define	CACHEPUSHGLOBAL		CACHEPUSHGLOBALH
#define	CACHEPUSHGLOBALX	CACHEPUSHGLOBALXH
#define	CACHEPUSHMETHOD		CACHEPUSHMETHODH
#define	CACHEPUSHPROPERTY	CACHEPUSHPROPERTYH
#define	CACHEPUSHRTN		CACHEPUSHRTNH
#define	CACHEPUSHRTNX		CACHEPUSHRTNXH
#define	CACHEPUSHSTR		CACHEPUSHSTRH
#define	CACHEPUSHEXSTR		CACHEPUSHEXSTRH			/* KMK497 */
#define	CACHEPUSHLOCK		CACHEPUSHLOCKH
#define	CACHEPUSHLOCKX		CACHEPUSHLOCKXH			/* LRS1034- */
#define CACHEEXSTRNEW           CACHEEXSTRNEWH                  /* SAP799 */
#define	CACHEPUSHSSVN		CACHEPUSHSSVNH			/* JO2405+ */
#define	CACHEPUSHSSVNX		CACHEPUSHSSVNXH
#define	CACHEADDGLOBAL		CACHEADDGLOBALH
#define	CACHEADDGLOBALX		CACHEADDGLOBALXH
#define	CACHEADDSSVN		CACHEADDSSVNH
#define	CACHEADDSSVNX		CACHEADDSSVNXH			/* JO2405- */
#else	/* defined(__VMS) */					/* SOH393+ */
  /* In order to make C #define macro replacement work as desired on VMS,
     defines (0a) of the form #define XXX XXXH must be given macro arg lists so
     they look like (0b) #define XXX(a,b,c) XXXH(a,b,c).

     This must be done in those cases where:
     (1) there exists a define of the from #define XXXA(args) (Xxx)(args), and
     (2) there exists a define of the form #define Xxx XXX.

     Otherwise, the macro #define XXXA(args) (Xxx)(args) will not be expanded
     properly into (XXX)(args).  An attempt to use XXXA(args) will be expanded
     into (Xxx)(args) by macro (1); then macro (2) will change this into
     (XXX)(args); and then macro (0a) will turns this into (XXXH)(args); and
     this results in a call on (XXXH)(args) which is *NOT* the desired
     expansion of XXXA(args).  However, if macro (0a) is replace by (0b) then
     a call on XXXA(args) will stop expanding when (XXX)(args) results, which
     is the desired final expansion.
  */
#define CACHEPOPSTR(lenp,strp)	CACHEPOPSTRH(lenp,strp)
#define	CACHEPOPEXSTR(sstrp)	CACHEPOPEXSTRH(sstrp)
#define	CACHEPUSHCLASSMETHOD(clen,cptr,mlen,mptr,flg)			\
		CACHEPUSHCLASSMETHODH(clen,cptr,mlen,mptr,flg)
#define	CACHEPUSHFUNC(rflag,tlen,tptr,nlen,nptr)			\
		CACHEPUSHFUNCH(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHFUNCX(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
		CACHEPUSHFUNCXH(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHGLOBAL(nlen,nptr)	CACHEPUSHGLOBALH(nlen,nptr)
#define	CACHEPUSHGLOBALX(nlen,nptr,elen,eptr)				\
		CACHEPUSHGLOBALXH(nlen,nptr,elen,eptr)
#define	CACHEPUSHMETHOD(oref,mlen,mptr,flg)				\
		CACHEPUSHMETHODH(oref,mlen,mptr,flg)
#define	CACHEPUSHPROPERTY(oref,plen,pptr)				\
		CACHEPUSHPROPERTYH(oref,plen,pptr)
#define	CACHEPUSHRTN(rflag,tlen,tptr,nlen,nptr)				\
		CACHEPUSHRTNH(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHRTNX(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
		CACHEPUSHRTNXH(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHSTR(len,ptr)	CACHEPUSHSTRH(len,ptr)
#define	CACHEPUSHEXSTR(sptr)	CACHEPUSHEXSTRH(sptr)
#define	CACHEPUSHLOCK(nlen,nptr) CACHEPUSHLOCKH(nlen,nptr)
#define	CACHEPUSHLOCKX(nlen,nptr,elen,eptr)				\
		CACHEPUSHLOCKXH(nlen,nptr,elen,eptr)
#define	CACHEEXSTRNEW(zstr,size) CACHEEXSTRNEWH(zstr,size)
#define	CACHEPUSHSSVN(nlen,nptr) CACHEPUSHSSVNH(nlen,nptr)
#define	CACHEPUSHSSVNX(nlen,nptr,elen,eptr)				\
		CACHEPUSHSSVNXH(nlen,nptr,elen,eptr)
#define	CACHEADDGLOBAL(nlen,nptr) CACHEADDGLOBALH(nlen,nptr)
#define	CACHEADDGLOBALX(nlen,nptr,elen,eptr)				\
		CACHEADDGLOBALXH(nlen,nptr,elen,eptr)
#define	CACHEADDSSVN(nlen,nptr)	CACHEADDSSVNH(nlen,nptr)
#define	CACHEADDSSVNX(nlen,nptr,elen,eptr)				\
		CACHEADDSSVNXH(nlen,nptr,elen,eptr)
#endif								/* SOH393- */
#else				/* ASCII character strings */
#define	CacheCvtIn		CacheCvtInA
#define	CacheCvtOut		CacheCvtOutA
#define	CacheCvtExStrIn		CacheCvtExStrInA
#define	CacheCvtExStrOut	CacheCvtExStrOutA
#define	CacheError		CacheErrorA
#define	CacheErrxlate		CacheErrxlateA
#define	CacheEval		CacheEvalA
#define	CacheExecute		CacheExecuteA
#define	CachePrompt		CachePromptA
#define	CacheStart		CacheStartA
#define	CacheSecureStart	CacheSecureStartA		/* STC1114+ */
#define	CacheChangePassword	CacheChangePasswordA		/* STC1114- */
#define	CACHECVTIN		CACHECVTINA
#define	CACHECVTOUT		CACHECVTOUTA
#define	CACHECVTEXSTRIN		CACHECVTEXSTRINA
#define	CACHECVTEXSTROUT	CACHECVTEXSTROUTA
#define	CACHEERROR		CACHEERRORA
#define	CACHEERRXLATE		CACHEERRXLATEA
#define	CACHEEVAL		CACHEEVALA
#define	CACHEEXECUTE		CACHEEXECUTEA
#define	CACHEPROMPT		CACHEPROMPTA
#define	CACHESTART		CACHESTARTA
#define	CACHESECURESTART	CACHESECURESTARTA		/* STC1114+ */
#define	CACHECHANGEPASSWORD	CACHECHANGEPASSWORDA		/* STC1114- */
#ifndef __VMS
#define CACHEPOPSTR		CACHEPOPSTRA
#define	CACHEPOPEXSTR		CACHEPOPEXSTRA			/* KMK497 */
#define	CACHEPUSHCLASSMETHOD	CACHEPUSHCLASSMETHODA
#define	CACHEPUSHFUNC		CACHEPUSHFUNCA
#define	CACHEPUSHFUNCX		CACHEPUSHFUNCXA
#define	CACHEPUSHGLOBAL		CACHEPUSHGLOBALA
#define	CACHEPUSHGLOBALX	CACHEPUSHGLOBALXA
#define	CACHEPUSHMETHOD		CACHEPUSHMETHODA
#define	CACHEPUSHPROPERTY	CACHEPUSHPROPERTYA
#define	CACHEPUSHRTN		CACHEPUSHRTNA
#define	CACHEPUSHRTNX		CACHEPUSHRTNXA
#define	CACHEPUSHSTR		CACHEPUSHSTRA
#define	CACHEPUSHEXSTR		CACHEPUSHEXSTRA			/* KMK497 */
#define	CACHEPUSHLOCK		CACHEPUSHLOCKA			/* JO1924+ */
#define	CACHEPUSHLOCKX		CACHEPUSHLOCKXA			/* JO1924- */
#define	CACHEEXSTRNEW		CACHEEXSTRNEWA			/* SAP799 */
#define	CACHEPUSHSSVN		CACHEPUSHSSVNA			/* JO2405+ */
#define	CACHEPUSHSSVNX		CACHEPUSHSSVNXA
#define	CACHEADDGLOBAL		CACHEADDGLOBALA
#define	CACHEADDGLOBALX		CACHEADDGLOBALXA
#define	CACHEADDSSVN		CACHEADDSSVNA
#define	CACHEADDSSVNX		CACHEADDSSVNXA			/* JO2405- */
#else	/* defined(__VMS) */					/* SOH393+ */
  /* In order to make C #define macro replacement work as desired on VMS,
     defines (0a) of the form #define XXX XXXA must be given macro arg lists so
     they look like (0b) #define XXX(a,b,c) XXXA(a,b,c).

     This must be done in those cases where:
     (1) there exists a define of the from #define XXXA(args) (Xxx)(args), and
     (2) there exists a define of the form #define Xxx XXX.

     Otherwise, the macro #define XXXA(args) (Xxx)(args) will not be expanded
     properly into (XXX)(args).  An attempt to use XXXA(args) will be expanded
     into (Xxx)(args) by macro (1); then macro (2) will change this into
     (XXX)(args); and then macro (0a) will turns this into (XXXA)(args); and
     this results in a call on (XXXA)(args) which is *NOT* desired because there
     is no routine named XXXA.  However, if macro (0a) is replaced by (0b) then
     a call on XXXA(args) will stop expanding when (XXX)(args) results, which
     is the desired final expansion because then a routine name XXX will be
     called.
  */
 
  /* Previously XXXA macros (which appear earlier in this header file) were
     defined to look like #define XXXA(args) Xxx(args).  This could lead to
     additional undesired macro expansion.  Now these definitions earlier
     in this header file look like #define XXXA(args) (Xxx)(args), which
     is the form of macro (1) discussed above.  This modification prevents
     undesired expansion of macros of the form (0b) during the expansion
     of macros of the form (1).
  */
#define CACHEPOPSTR(lenp,strp)	CACHEPOPSTRA(lenp,strp)
#define	CACHEPOPEXSTR(sstrp)	CACHEPOPEXSTRA(sstrp)
#define	CACHEPUSHCLASSMETHOD(clen,cptr,mlen,mptr,flg)			\
		CACHEPUSHCLASSMETHODA(clen,cptr,mlen,mptr,flg)
#define	CACHEPUSHFUNC(rflag,tlen,tptr,nlen,nptr)			\
		CACHEPUSHFUNCA(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHFUNCX(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
		CACHEPUSHFUNCXA(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHGLOBAL(nlen,nptr)	CACHEPUSHGLOBALA(nlen,nptr)
#define	CACHEPUSHGLOBALX(nlen,nptr,elen,eptr)				\
		CACHEPUSHGLOBALXA(nlen,nptr,elen,eptr)
#define	CACHEPUSHMETHOD(oref,mlen,mptr,flg)				\
		CACHEPUSHMETHODA(oref,mlen,mptr,flg)
#define	CACHEPUSHPROPERTY(oref,plen,pptr)				\
		CACHEPUSHPROPERTYA(oref,plen,pptr)
#define	CACHEPUSHRTN(rflag,tlen,tptr,nlen,nptr)				\
		CACHEPUSHRTNA(rflag,tlen,tptr,nlen,nptr)
#define	CACHEPUSHRTNX(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)		\
		CACHEPUSHRTNXA(rflag,tlen,tptr,off,elen,eptr,nlen,nptr)
#define	CACHEPUSHSTR(len,ptr)	CACHEPUSHSTRA(len,ptr)
#define	CACHEPUSHEXSTR(sptr)	CACHEPUSHEXSTRA(sptr)
#define	CACHEPUSHLOCK(nlen,nptr) CACHEPUSHLOCKA(nlen,nptr)
#define	CACHEPUSHLOCKX(nlen,nptr,elen,eptr)				\
		CACHEPUSHLOCKXA(nlen,nptr,elen,eptr)
#define	CACHEEXSTRNEW(zstr,size) CACHEEXSTRNEWA(zstr,size)
#define	CACHEPUSHSSVN(nlen,nptr) CACHEPUSHSSVNA(nlen,nptr)
#define	CACHEPUSHSSVNX(nlen,nptr,elen,eptr)				\
		CACHEPUSHSSVNXA(nlen,nptr,elen,eptr)
#define	CACHEADDGLOBAL(nlen,nptr) CACHEADDGLOBALA(nlen,nptr)
#define	CACHEADDGLOBALX(nlen,nptr,elen,eptr)				\
		CACHEADDGLOBALXA(nlen,nptr,elen,eptr)
#define	CACHEADDSSVN(nlen,nptr)	CACHEADDSSVNA(nlen,nptr)
#define	CACHEADDSSVNX(nlen,nptr,elen,eptr)				\
		CACHEADDSSVNXA(nlen,nptr,elen,eptr)
#endif								/* SOH393- */
#endif	/* CACHE_UNICODE */
							/* LRS755-,LRS756- */
#if	defined(__VMS) && !defined(__cplusplus)			/* GK525,JLC782 */
#pragma	__required_pointer_size	__restore
#endif

#ifdef __cplusplus
}
#endif

#endif	/* _ccallin_h_ */					/* LRS755 */
