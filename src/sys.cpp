/******************
Instructions
******************

Note that if you're replacing version 1.0, steps 1 through 5 should be done
already.

(1) Remove definition of MAX_STRING

(2) At the beginning of boot_db():

-	if ( ( string_space = calloc( 1, MAX_STRING ) ) == NULL )
-	{
-	    bug( "Boot_db: can't alloc %d string space.", MAX_STRING );
-	    exit( 1 );
-	}
+	strspace_alloc();


(3) In fread_string() and in fread_string_eol():

-    if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
+    if ( plast > &string_space[strspace_size*1024 - MAX_STRING_LENGTH] )
+		strspace_size_increment();
-   {
-	bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
-	exit( 1 );
-   }

(4) In db.h, add

#define DATPATH ""
extern unsigned strspace_size;
char* strspace_alloc();
void  strspace_size_increment();
extern int      nAllocString;
extern int      sAllocString;
//extern int      social_count;

(5) Remove old definition of do_memory()

(6) These three little functions I wrote do wonders for code readability, besides saving lots of typing. Put the declarations in your merc.h, use them
throughout your code and notice how they save typing and improve readability.

int		logfi( const char *format, ... );
// mkw: print to log formatted

int		chprintf( CHAR_DATA* ch, const char *format, ... );
// mkw: print to character formatted

int		strswitch( const char* arg, ... );
// mkw: "bundles" any number of string comparisons neatly
// into one return value that can be used in a switch statement.
// Last argument passed must be NULL to mark end of list.
// Will behave badly if that is forgotten or anything other than
// strings are passed.
// First variable argument is counted as one.
// Returns 0 if no strings match.

void	vinterpret( CHAR_DATA *ch, char *argument, ... );
// mkw: This is my subcommand interpreter.
// Pattern for variable args:
// { string_1, [do_fun_1|NULL], ... } NULL,[default_do_fun|NULL]
// NULL passed for a string marks end of list.
// NULL passed for a do_fun means no action on match.


(7) Remove the definitions of alloc_mem(), free_mem(), alloc_perm()
nAllocPerm, sAllocPerm, rgFreeList and rgSizeList from db.c

(8) Move the definition of MAX_PERM_BLOCK from db.c to db.h.

***************************************************************************/
#if defined (_WIN32)
#if defined (_DEBUG)
#pragma warning (disable : 4786)
#endif
#endif
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include "merc.h"
#include "db.h"

#if defined(__cplusplus)
extern "C" {
#endif



#define MAGIC_NUM 52571214

  /*********************************************************
*  settings
*/

#define STRSPACE_DEFAULT (1 * 1024)
  // unit is kB
  // purposely set too low, so that you'll see how it works

#define STRSPACE_INCREMENT 128
  // unit is kB

#define MPROGS_INSTALLED
  // comment out if you do not have Ivan's (or compatible)
  // mob programs

  //#define DOUBLE_LOGGING
  // I have stdout redirected to a log file and stderr writing
  // normally to the console. (That helps with debugging.) All
  // log strings are sent once to each stream if DOUBLE_LOGGING
  // is defined, otherwise only to stderr.

#define SHOW_ALL_RGSIZES
  // Comment out if you want memory report to show only used
  // block sizes

#define ALLOC_PERM_ALIGN sizeof(int)
  // use powers of two only
  // higher values may improve speed significantly on some
  // machines, at the expense of memory.
  // 32 should be the fastest value on most machines
  // I don't recommend values below machine word length, but
  // feel free to experiment setting it to 1 won't save much, // but slow things down considerably

#define MAX_MEM_LIST                                                           \
    24 // This used to be 12 and is also defined in const.h - Discordance

  const size_t rgSizeList[MAX_MEM_LIST] = {
    16,   32,   64,    128,        256,   1024,  2048, 4096, 8192, 16384, 32768 - 64, 65536, 131072}; // Added 131072 because 65536
  // wasn't big enough -
  // Discordance

  /*********************************************************
*  macros from mkw.h
*/
#ifndef __mkwMUD

#define MAGIC_CHECKING
  // comment out disable
  // internal assertion of
  // memory
  // allocation/recycling

#define chwrite(ch, str) send_to_char(str, ch)
  // find send_to_char()'s
  // syntax counter-intuitive

#define object_free obj_free
  // easy fix for name
  // inconsistency in merc

#define OK 0
#define ERR 1

#endif

  /*********************************************************
*  imports
*/

  extern char *string_space;
  extern int mobile_count;
  extern OBJ_DATA *obj_free;
  extern CharList *char_free;
  extern DESCRIPTOR_DATA *descriptor_free;
  extern PC_DATA *pcdata_free;
  extern AFFECT_DATA *affect_free;

  /*********************************************************
*  locals
*/

  int has_system_auth(CHAR_DATA *ch);
  int strspace_size_read();
  int strspace_size_set(int newsize);
  void strspace_size_default();

  // extern const size_t	rgSizeList	[MAX_MEM_LIST];
  extern void *rgFreeList[MAX_MEM_LIST];

  /****************************************************************
* Implementation section
*/

  const char ver_mkwSys[] = "mkwSys Version 2.01";

  unsigned strspace_size;

  void *rgFreeList[MAX_MEM_LIST];

  unsigned long nAllocGP[MAX_MEM_LIST] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  int nAllocPerm;
  size_t sAllocPerm = 0;
  int nPermBlocks = 0;

  void boot_fail(char *module_name) {
    logfi("*********************  Reboot by %-11s *********************", module_name);
    exit(EXIT_FAILURE);
  }

  char *strspace_alloc() {
    strspace_size_read();
    if ((string_space = (char *)calloc(1, strspace_size * 1024)) == NULL) {
      logfi("mkwSys.c: Unable to allocate %d kB string space from system.", strspace_size);
      if (strspace_size > STRSPACE_DEFAULT)
      strspace_size_default();
      else
      logfi("Increase your system's virtual memory size.", strspace_size);
      boot_fail("mkwSys.c");
    }
    return string_space;
  }

  void strspace_size_default() {
    strspace_size_set(STRSPACE_DEFAULT);
    logfi("mkwSys.c: String space size reset to program default of %d kB.", strspace_size);
  }

  void strspace_size_increment() {
    logfi("mkwSys.c: String space size of %d kB did not suffice.", strspace_size);
    strspace_size_set(strspace_size + STRSPACE_INCREMENT);
    logfi("mkwSys.c: Incrementing string space size to %d kB and rebooting.", strspace_size);
    boot_fail("mkwSys.c");
  }

  int strspace_size_set(int newsize) {
    if (newsize > 0) {
      FILE *memini = fopen(DATPATH "mkwSys.ini", "w");
      if (memini) {
        fprintf(memini, "stringspace = %u\n", newsize);
        fclose(memini);
        return OK;
      }
      else {
        logfi("Unable to open mkwSys.ini for writing");
      }
    }
    return ERR;
  }

  int strspace_size_read() {
    FILE *memini = fopen(DATPATH "mkwSys.ini", "r");
    if (memini) {
      strspace_size = 0;
      while (str_cmp(fread_word(memini), "stringspace"))
      ;
      fscanf(memini, " = %u", &strspace_size);
      fclose(memini);
      if (!strspace_size) {
        logfi("mkwSys.c: String space size not set in mkwSys.ini, using default.");
        strspace_size_default();
      }
      else
      return OK;
    }
    else {
      logfi("mkwSys.c: Unable to open memsize.ini for reading, using default string space size.");
      strspace_size_default();
    }
    return ERR;
  }

  int has_system_auth(CHAR_DATA *ch) {
    if (get_trust(ch) < LEVEL_CODER) {
      send_to_char("You are not authorized to issue system commands.\n\r", ch);
      return FALSE;
    }
    else
    return TRUE;
  }

#define IMP_COUNTER(type, start, px)                                           \
    int px##type() {                                                             \
      type *p = start;                                                           \
      int n = 0;                                                                 \
      while (p != NULL) {                                                        \
        p = p->next;                                                             \
        n++;                                                                     \
      }                                                                          \
      return n;                                                                  \
    }

#define IMP_LIST_COUNTERS(type, prefix)                                        \
    IMP_COUNTER(type, prefix##list, n) IMP_COUNTER(type, prefix##free, f)

  // these object types have no global list
  IMP_COUNTER(AFFECT_DATA, affect_free, f)

  // these do
  // IMP_LIST_COUNTERS ( OBJ_DATA, object_ );
  // IMP_LIST_COUNTERS ( CHAR_DATA, char_ );
  // IMP_LIST_COUNTERS ( DESCRIPTOR_DATA, descriptor_ );

#ifdef MPROGS_INSTALLED
  // IMP_COUNTER ( PROG_CODE, mprog_list, n );
  // IMP_LIST_COUNTER ( PROG_LIST, ??? );
  // not currently implemented
  // (There is no global list of PROG_LISTs to count from)
#endif

  /*
int nNOTE_DATA()
{
NOTE_DATA* p; int n = 0;
for( p=note_list;p!=NULL; p=p->next) n++;
return n;
}
*/

  // report on an object type
#define rpt(name, n, obj)                                                      \
    chprintf(ch, "%5d %-14s *%4d Bytes =%6d kB\n\r", n, name, sizeof(obj),       \
    (n) * sizeof(obj) / 1024)

  // report object with dynamic counting of list -
  // calls rpt() without evaluating n twice
#define rpt_dy(name, type)                                                     \
    {                                                                            \
      int n = n##type();                                                         \
      rpt(name, n, type);                                                        \
    }

  // report string and decimal
#define rptsd(st, sz) chprintf(ch, "%-34s %5d kB\n\r", st, sz)

  // report decimal, string, decimal
#define rptdsd(n, st, sz) chprintf(ch, "%5d %-28s %5d kB\n\r", n, st, sz)

  const char separator[] = "-------------------------------------------\n\r";

  _DOFUN(do_memory_perms) {
    int i;
    chwrite(ch, separator);
    chwrite(ch, "Permanent memory objects in use\n\r");
    chwrite(ch, separator);
    // rpt_dy( "Characters", CHAR_DATA );
    //	rpt( "PCs", nCHAR_DATA() - mobile_count, PC_DATA );
    //	rpt_dy( "Descriptors", DESCRIPTOR_DATA );
    rpt("Mob Prototypes", top_mob_index, MOB_INDEX_DATA);
    //	rpt_dy( "Objects", OBJ_DATA);
    rpt("Obj Prototypes", top_obj_index, OBJ_INDEX_DATA);
    rpt("Extra Descs", top_ed, EXTRA_DESCR_DATA);
    rpt("Affects", top_affect, AFFECT_DATA);
    rpt("Rooms", top_room, ROOM_INDEX_DATA);
    rpt("Exits", top_exit, EXIT_DATA);
    rpt("Shops", top_shop, SHOP_DATA);
    rpt("Areas", top_area, AREA_DATA);
    rpt("Resets", top_reset, RESET_DATA);
    rpt("Help entries", top_help, HELP_DATA);
    //	rpt_dy( "Notes", NOTE_DATA);
    //	rpt( "Socials", social_count, struct social_type );
#ifdef MPROGS_INSTALLED
    //	rpt_dy( "Mob progs", PROG_CODE )
#endif
    for (i = 0; i < MAX_MEM_LIST; i++)
#ifndef SHOW_ALL_RGSIZES
    if (nAllocGP[i])
#endif
    chprintf(ch, "%5d GP blocks of  %6d Bytes =%6d kB\n\r", nAllocGP[i], rgSizeList[i], nAllocGP[i] * rgSizeList[i] / 1024);
    chwrite(ch, separator);
  }

#define rptf(name, type)                                                       \
    {                                                                            \
      int n = f##type();                                                         \
      rpt(name, n, type);                                                        \
    }

  _DOFUN(do_memory_freelists) {
    int i;
    chwrite(ch, separator);
    chwrite(ch, "Memory objects waiting to be recycled\n\r");
    chwrite(ch, separator);
    //	rptf( "Characters", CHAR_DATA );
    //	rptf( "PCs", PC_DATA );
    //	rptf( "Descriptors", DESCRIPTOR_DATA );
    //	rptf( "Objects", OBJ_DATA);
    rptf("Affects", AFFECT_DATA);

    for (i = 0; i < MAX_MEM_LIST; i++)
#ifndef SHOW_ALL_RGSIZES
    if (rgFreeList[i])
#endif
    {
      void *p;
      int n = 0;
      for (p = rgFreeList[i]; p != NULL; p = *((void **)p))
      n++;
      chprintf(ch, "%5d GP blocks of  %6d Bytes =%6d kB\n\r", n, rgSizeList[i], n * rgSizeList[i] / 1024);
    }
    chwrite(ch, separator);
  }

  _DOFUN(do_memory_heap) {
    chwrite(ch, separator);
    chprintf(ch, "Perm Block size is set to %d kB\n\r", MAX_PERM_BLOCK / 1024);
    chprintf(ch, "Object alignment is set to %d Bytes\n\r", ALLOC_PERM_ALIGN);

    rptdsd(nPermBlocks, "Blocks allocated from heap", nPermBlocks * MAX_PERM_BLOCK / 1024);
    rptdsd(nAllocPerm, "Permanent objects occupy", sAllocPerm / 1024);
    rptsd("Current alloc_perm() overhead", (nPermBlocks * MAX_PERM_BLOCK - sAllocPerm) / 1024);
    chwrite(ch, separator);

    rptsd("String Space allocated at DB boot", strspace_size);
    rptdsd(nAllocString, "Strings in string space", sAllocString / 1024);
    rptsd("Excess string space", strspace_size - sAllocString / 1024);

    chwrite(ch, separator);
    rptsd("Total managed heap memory", nPermBlocks * MAX_PERM_BLOCK / 1024 + strspace_size);
    chwrite(ch, separator);
  }

  _DOFUN(do_memory_formats) {
    chwrite(ch, separator);
    chprintf(ch, "Old format mobs: %d\n\rOld format objects: %d\n\r", top_mob_index - newmobs, top_obj_index - newobjs);
    chwrite(ch, separator);
  }

#undef rpt
#undef rpt_dy
#undef rptsd
#undef rptdsd
  /* Those macros are not for general use */

  _DOFUN(do_memory_stringspace) {
    if (has_system_auth(ch)) {
      char arg[MAX_INPUT_LENGTH];
      int sz = 0;
      argument = one_argument(argument, arg);
      if (is_number(arg))
      sz = atoi(arg);
      if (sz <= 0 || sz > 32000) {
        chwrite(ch, "Invalid parameter.\n\r");
        return;
      }
      if (strspace_size_set(sz) == OK)
      chprintf(ch, "String space size for next DB boot set to %d kB.\n\r", sz);
      else
      chwrite(ch, "Error writing new setting to file.\n\r");
    }
  }

  _DOFUN(do_memory_help) {
    chwrite(ch, ver_mkwSys);
    chwrite(ch, " copyright (c) 1998 Michael K. Weise\n\rSee home.att.net/~mkw/mudframe.html for more information\n\n\rusage: memory <command>\n\r\ravailable commands:\n\rheap                Report heap memory allocated by the memory manager\n\rperms               Report managed memory objects in use by other modules\n\rfreelists           Report memory objects waiting to be recycled\n\rstringspace <size>  Set string space size in kB for next reboot\n\rformats             Report old format prototypes\n\r?                   This message\n\r");
  }

  _DOFUN(do_memory) {
    vinterpret(ch, argument, "perms", do_memory_perms, "heap", do_memory_heap, "freelists", do_memory_freelists, "stringspace", do_memory_stringspace, "formats", do_memory_formats, NULL, do_memory_help);
  }

  int strswitch(const char *arg, ...) {
    int i = 0;
    char *p;
    va_list caselist;
    if (arg[0]) {
      va_start(caselist, arg);
      while ((p = va_arg(caselist, char *)) != NULL) {
        i++;
        if (!str_prefix(arg, p))
        return i;
      }
    }
    return 0;
  }

  void vinterpret(CHAR_DATA *ch, char *argument, ...) {
    char arg[MAX_INPUT_LENGTH];
    char *iStr;
    DO_FUN *iFun;
    va_list caselist;
    va_start(caselist, argument);
    argument = one_argument(argument, arg);
    do {
      iStr = va_arg(caselist, char *);
      iFun = va_arg(caselist, DO_FUN *);
    } while (iStr != NULL && (!arg[0] || str_prefix(arg, iStr)));
    if (iFun != NULL)
    (*iFun)(ch, argument);
  }

  size_t rgSize_lookup(size_t size) {
    int iList;
    size += sizeof(int); // room needed for integrity marker

    for (iList = 0; iList < MAX_MEM_LIST; iList++) {
      if (size <= rgSizeList[iList])
      return rgSizeList[iList] - sizeof(int);
    }
    return 0; // size won't fit into any alloc_mem() block
  }

  /*
* Request a GP memory block.
* GP blocks must be returned using free_mem() when no longer needed.
* Checks freelist for block of appropriate size, * allocates heap memory if none available
*/
  void *alloc_mem(size_t sMem) {
    void *pMem;
    int iList;

    // logfi("Size Requested: %d", sMem);
#ifdef MAGIC_CHECKING
    int *magic;
    sMem += sizeof(*magic);
#endif

    for (iList = 0; iList < MAX_MEM_LIST; iList++) {
      if (sMem <= rgSizeList[iList])
      break;
    }

    if (iList == MAX_MEM_LIST) {
      bug("Alloc_mem: size %d too large.", sMem);
      exit(EXIT_FAILURE);
    }

    if (rgFreeList[iList] == NULL) {
      pMem = alloc_perm(rgSizeList[iList]);
      nAllocGP[iList]++;
    }
    else {
      pMem = rgFreeList[iList];
      rgFreeList[iList] = *((void **)rgFreeList[iList]);
    }

#ifdef MAGIC_CHECKING
    magic = (int *)pMem;
#ifndef MAGIC_NUM
#define MAGIC_NUM 52571214
#endif
    *magic = 52571214;
    pMem = (void *)(((char *)pMem) + sizeof(*magic));
#endif
    // logfi("Alloc_mem: %d %X", *magic, pMem);
    return pMem;
  }

  /*
* This is the memory manager's interface to the system heap.
* All memory allocated here is either permanently typecast
* and managed by the object recycler, or managed by the GP
* block manager.
*/
  void *alloc_perm(size_t sMem) {
    static int iMemPerm;
    static char *pMemPerm;
    void *pMem;

    while (sMem % ALLOC_PERM_ALIGN != 0)
    sMem++;

    if (sMem > MAX_PERM_BLOCK) {
      bug("Alloc_perm: %d too large.", sMem);
      exit(EXIT_FAILURE);
    }

    if (pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK) {
      iMemPerm = 0;
      nPermBlocks++;
      if ((pMemPerm = (char *)calloc(1, MAX_PERM_BLOCK)) == NULL) {
        perror("Alloc_perm");
        //	    exit( EXIT_FAILURE );
      }
    }

    pMem = pMemPerm + iMemPerm;
    iMemPerm += sMem;
    nAllocPerm += 1;
    sAllocPerm += sMem;
    return pMem;
  }

  /*
* Give a GP block back to the memory manager for recycling.
*/
  void free_mem(void *pMem, size_t sMem) {
    int iList;

#ifdef MAGIC_CHECKING
    int *magic;

    pMem = (void *)(((char *)pMem) - sizeof(*magic));
    magic = (int *)pMem;

#ifndef MAGIC_NUM
#define MAGIC_NUM 52571214
#endif
    if (*magic != 52571214) {
      bug("Attempt to recyle invalid memory of size %d.", sMem);
      bug((char *)pMem + sizeof(*magic), 0);
      return;
    }

    *magic = 0;
    sMem += sizeof(*magic);
#endif

    for (iList = 0; iList < MAX_MEM_LIST; iList++) {
      if (sMem <= rgSizeList[iList])
      break;
    }

    if (iList == MAX_MEM_LIST) {
      bug("Free_mem: size %d too large.", sMem);
      exit(EXIT_FAILURE);
    }

    *((void **)pMem) = rgFreeList[iList];
    rgFreeList[iList] = pMem;

    return;
  }

  int chprintf(CHAR_DATA *ch, const char *format, ...) {
    static char buf[MAX_STRING_LENGTH + 1];
    va_list arglist;
    int nChars = 0;
    va_start(arglist, format);
    if (format != NULL && format[0] && ch->desc != NULL) {
      nChars = vsprintf(buf, format, arglist);
      assert(nChars <= MAX_STRING_LENGTH);
      write_to_buffer(ch->desc, buf, nChars);
    }
    va_end(arglist);
    return nChars;
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif
