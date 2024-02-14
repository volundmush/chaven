#ifndef _MERC_H_
#define _MERC_H_

#if defined(WIN32)
#include <winsock.h>
#include <time.h>
#else
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ext/hash_set>
#endif

#include <cstdio>
#include <fstream>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <string>

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(WIN32) && !defined(_WIN32)
#define _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#endif

#if	!defined(FALSE)
	#define FALSE	 0
#endif

#if	!defined(TRUE)
	#define TRUE	 1
#endif

#if defined(WIN32)
	#pragma once
	#define APPNAME "PatternsEnd.exe"
	#define crypt(s1, s2) (s1)
#endif

#if !defined(__FUNCTION__)
#define __FUNCTION__ "Undefined"
#endif

typedef short int sh_int;

/* system calls */
//int unlink(const char *);

#if defined(_AIX)
	#if	!defined(const)
		#define const
	#endif
	typedef int			sh_int;
	typedef int			bool;
	#define unix
#else
	typedef short   int		sh_int;
	#ifndef __cplusplus
		typedef unsigned char		bool;
	#endif
#endif

typedef void Sigfunc(int);

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(_WIN32)
#pragma once
   void gettimeofday (struct timeval *t, void *tz);
	//#if !defined(_export)
		#define _export __declspec( dllexport )
	//#endif
#else
	#if !defined(_export)
		#define _export extern
	#endif
#endif

#if defined(TRADITIONAL)
#define const
#define args( list )					( )
#define DECLARE_DO_FUN( fun )			_export void fun( )
#define DECLARE_SPEC_FUN( fun )			bool fun( )
#define DECLARE_SPELL_FUN( fun )		_export void fun( )
#define DECLARE_SPELL_HEAL_FUN( fun )   _export void fun( )
#define DECLARE_HERB_FUN( fun )			_export void fun( )
#define DECLARE_OBJ_FUN( fun )			_export void fun( )
#define DECLARE_ROOM_FUN( fun )			_export void fun( )
#define DECLARE_FORM_FUN( fun )			_export void fun( )
#else
#define args( list )					list
#define DECLARE_DO_FUN( fun )			_export DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )			SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )		SPELL_FUN fun
#define DECLARE_SPELL_HEAL_FUN( fun )	SPELL_HEAL_FUN fun
#define DECLARE_HERB_FUN( fun )			HERB_FUN fun
#define DECLARE_OBJ_FUN( fun )			OBJ_FUN fun
#define DECLARE_ROOM_FUN( fun )			ROOM_FUN fun
#define DECLARE_FORM_FUN( fun )			FORM_FUN fun
#endif

#define _WEAVE( fun )  void fun(int sn, int level, CHAR_DATA *ch, void *vo, int target)

#define _DOFUN( fun )	void fun(CHAR_DATA *ch, char *argument)
#define _HERBFUN( fun )	void fun(CHAR_DATA *ch, char *argument)
#define _OBJFUN( fun )	void fun(OBJ_DATA *obj, char *argument)
#define _ROOMFUN( fun )	void fun(ROOM_INDEX_DATA *room, char *argument)
#define _FORMFUN( fun )	void fun(CHAR_DATA *ch, CHAR_DATA *victim)

//MXP - Discordance
/* strings */

#define MXP_BEG "\x3c"    /* becomes < */
#define MXP_END "\x3e"    /* becomes > */
#define MXP_AMP "\x05"    /* becomes & */

/* characters */

#define MXP_BEGc '\x3c'    /* becomes < */
#define MXP_ENDc '\x3e'    /* becomes > */
#define MXP_AMPc '\x05'    /* becomes & */

/* constructs an MXP tag with < and > around it */
   
#define MXP_TAG(arg) MXP_BEG arg MXP_END
#define ESC "\x1B"  /* esc character */
#define MXP_MODE(arg) ESC "[" #arg "z"
   
/* flags for show_list_to_char */

enum {
  eItemNothing,   /* item is not readily accessible */
  eItemGet,     /* item on ground */
  eItemDrop,    /* item in inventory */
  eItemBid     /* auction item */
};

#include "const.h"
#include "Buffer.h"
#include "structs.h"
//#include "Ability.h"
#include "global.h"
#include "functions.h"

#if	defined(linux)
	char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined(macintosh)
	#define NOCRYPT
	#if	defined(unix)
		#undef	unix
	#endif
#endif

/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined(NOCRYPT)
	#define crypt(s1, s2)	(s1)
#endif

// This must be at the end of the file - Brad
#if defined(__cplusplus)
}
#endif

#endif
