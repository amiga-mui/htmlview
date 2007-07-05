/*******************************************************************************

        Name:           mccinit.c
        Versionstring:  $VER: mccinit.c 1.4 (04.07.2007)
        Author:         Jens Langner <Jens.Langner@light-speed.de>
        Distribution:   PD (public domain)
        Description:    library init file for easy generation of a MUI
                        custom classes (MCC/MCP)
 History:

  1.0   09.06.2007 : created based on obsolete mccheader.c (damato)
  1.1   10.06.2007 : modified LibInit/LibOpen/LibExpunge to call the actual
                     ClassOpen() in LibOpen() rather than in LibInit(). This
                     should prevent stack issues common on e.g. OS3. (damato)
  1.2   01.07.2007 : adapted library interface initialization to what the
                     very latest idltool 52.7 produces as well.
  1.3   04.07.2007 : added MIN_STACKSIZE and all required StackSwap()
                     mechanisms to enforce a large enough stack.
  1.4   04.07.2007 : put the StackSwapStruct structure on the stack to avoid
                     crashes on OS3/MOS.

 About:

  The purpose of this source file is to provide a template for the library init
  code for a creation of an own MUI custom class (mcc/mcp) for AmigaOS4,
  AmigaOS3 and MorphOS. By directly including this file (#include "mccinit.c")
  and defining certain preprocessor values, a MUI developer doesn't have to
  care about library init stuff which is normally highly system dependent and
  various between different Amiga operating systems.

 Usage:

  This file should be included by another source file (e.g. 'library.c') in
  your main development branch while certain preprocessor macros right before
  the include statement can be defined to change the behaviour of mccinit.c.
  These possible macros are:

    USERLIBID     (char*)   - version string for the mcc/mcp (exluding $VER:)
    VERSION       (int)     - version number (must match USERLIBID)
    REVISION      (int)     - revision number (must match USERLIBID)
    CLASS         (char*)   - class name (including .mcc/.mcp)
    MASTERVERSION (int)     - the minimun required muimaster version
                              (default: MUIMASTER_VMIN)
    MIN_STACKSIZE (int)     - if defined, the specified minimum stack size
                              will be enforced when calling all user functions
                              like ClassXXXX() and PreClassXXX().

    MCC only:
    --------
    SUPERCLASS  (char*)   - superclass ID of MCC (e.g. MUIC_Area)
    INSTDATA              - name of instance data structure of MCC (e.g. InstData)
    USEDCLASSES           - name of NULL terminated string array which contains
                            names of other required custom classes for MCC.
    _Dispatcher           - name of Dispatcher function for MCC

    MCP only:
    -------
    SUPERCLASSP (char*)   - superclass ID of MCP (e.g. MUIC_Mccprefs)
    INSTDATAP             - name of instance data structure of MCP
    USEDCLASSESP          - name of NULL terminated string array which contains
                            names of other required custom classes for MCC.
    SHORTHELP   (char*)   - alternative help text for prefs program's listview
    PREFSIMAGEOBJECT      - pointer to the image object for the MCP
    _DispatcherP          - name of Dispatcher function for MCP

  In addition, the following defines and functions can be defined or are
  required:

    CLASSINIT     - if defined, a "BOOL ClassInit(struct Library *base)"
                    function can be defined in your own code and will be called
                    right after the general library initialization are
                    finished. This function should then open own libraries
                    or do own library initialization tasks as it is only called
                    once at the very first library/class open.

    CLASSEXPUNGE  - if defined a "VOID ClassExpunge(struct Library *base)"
                    function can be defined in your own code and will be called
                    as soon as the library will be freed/expunged by the
                    operating system. In this function you should close/free
                    stuff you normally opened/allocated in CLASSINIT.

    CLASSOPEN     - if defined, a "BOOL ClassOpen(struct Library *base)"
                    function can be defined in your own code and will be called
                    right after each single application opens the custom class.
                    In this function you can then set flags or do library open
                    specific tasks.

    CLASSCLOSE    - if defined a "VOID ClassClose(struct Library *base)"
                    function can be defined in your own code and will be called
                    right after the library was successfully flagged as closed
                    by the CloseLibrary() call of an application.


    PRECLASSINIT      - if defined a "BOOL PreClassInit(struct Library *base)"
                        function can be defined and will be called right _before_
                        and library initialization takes place.

    POSTCLASSEXPUNGE  - if defined a "BOOL PostClassExpunge(struct Library *base)"
                        function can be defined and will be called right _after_
                        the custom class was free via MUI_DeleteCustomClass() in
                        the library expunge phase.

   Warning:
   -------
   The above class functions are normally called by the operating system
   in a Forbid()/Permit() state. That means you are supposed to make sure that
   your operations doesn't break the Forbid() state or otherwise you may run
   into a race condition. However, we have added semaphore locking to partly
   protect you from that case - but you should still consider doing processor
   intensitive tasks in a library's own function instead of using those
   class initialization functions.

*******************************************************************************/

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/

#ifdef __MORPHOS__
#include <emul/emulinterface.h>
#include <emul/emulregs.h>
#endif

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <exec/resident.h>
#include <exec/execbase.h>
#include <dos/dos.h>

#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#if defined(__amigaos4__)
struct Library *MUIMasterBase = NULL;
struct Library *SysBase       = NULL;
struct Library *UtilityBase   = NULL;
struct Library *DOSBase       = NULL;
struct Library *GfxBase       = NULL;
struct Library *IntuitionBase = NULL;
struct ExecIFace *IExec       = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct UtilityIFace *IUtility     = NULL;
struct DOSIFace *IDOS             = NULL;
struct GraphicsIFace *IGraphics   = NULL;
struct IntuitionIFace *IIntuition = NULL;
#if defined(__NEWLIB__)
struct Library *NewlibBase = NULL;
struct Interface *INewlib = NULL;
#endif
#else
struct Library        *MUIMasterBase = NULL;
struct ExecBase       *SysBase       = NULL;
struct Library        *UtilityBase   = NULL;
struct DosLibrary     *DOSBase       = NULL;
struct GfxBase        *GfxBase       = NULL;
struct IntuitionBase  *IntuitionBase = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

// we place a stack cookie in the binary so that
// newer OS version can directly take the specified
// number for the ramlib process
#if defined(MIN_STACKSIZE)

// transforms a define into a string
#define STR(x)  STR2(x)
#define STR2(x) #x

static const char USED_VAR stack_size[] = "$STACK:" STR(MIN_STACKSIZE) "\n";
#endif

/* The name of the class will also become the name of the library. */
/* We need a pointer to this string in our ROMTag (see below). */
static const char UserLibName[] = CLASS;
static const char UserLibID[]   = "$VER: " USERLIBID;

#ifdef SUPERCLASS
static struct MUI_CustomClass *ThisClass = NULL;
DISPATCHERPROTO(_Dispatcher);
#endif

#ifdef SUPERCLASSP
static struct MUI_CustomClass *ThisClassP = NULL;
DISPATCHERPROTO(_DispatcherP);
#endif

#ifdef __GNUC__

  #if !defined(__NEWLIB__)
    #if defined(__amigaos4__)
    extern struct Library *__UtilityBase;   // clib2
    extern struct UtilityIFace* __IUtility; // clib2
    #else
    struct Library *__UtilityBase = NULL; // required by libnix & clib2
    #endif
  #endif

  /* these one are needed copies for libnix.a */
  #ifdef __libnix__
    #ifdef USE_MATHIEEEDOUBBASBASE
    struct Library *__MathIeeeDoubBasBase = NULL;
    #endif
    #ifdef USE_MATHIEEEDOUBTRANSBASE
    struct Library *__MathIeeeDoubTransBase = NULL;
    #endif
  #endif

#endif /* __GNUC__ */


// in case the user didn't specify an own minimum
// muimaster version we increase it here.
#ifndef MASTERVERSION
#define MASTERVERSION MUIMASTER_VMIN
#endif

/* Our library structure, consisting of a struct Library, a segment pointer */
/* and a semaphore. We need the semaphore to protect init/exit stuff in our */
/* open/close functions */
struct LibraryHeader
{
  struct Library         lh_Library;
  UWORD                  lh_Pad1;
  BPTR                   lh_Segment;
  struct SignalSemaphore lh_Semaphore;
  UWORD                  lh_Pad2;
};

/******************************************************************************/
/* Local Structures & Prototypes                                              */
/******************************************************************************/

#if defined(__amigaos4__)

static struct LibraryHeader * LIBFUNC LibInit    (struct LibraryHeader *base, BPTR librarySegment, struct ExecIFace *pIExec);
static BPTR                   LIBFUNC LibExpunge (struct LibraryManagerInterface *Self);
static struct LibraryHeader * LIBFUNC LibOpen    (struct LibraryManagerInterface *Self, ULONG version);
static BPTR                   LIBFUNC LibClose   (struct LibraryManagerInterface *Self);
static ULONG                  LIBFUNC MCC_Query  (UNUSED struct Interface *self, REG(d0, LONG which));

#elif defined(__MORPHOS__)

static struct LibraryHeader * LIBFUNC LibInit    (struct LibraryHeader *base, BPTR librarySegment, struct ExecBase *sb);
static BPTR                   LIBFUNC LibExpunge (void);
static struct LibraryHeader * LIBFUNC LibOpen    (void);
static BPTR                   LIBFUNC LibClose   (void);
static LONG                   LIBFUNC LibNull    (void);
static ULONG                  LIBFUNC MCC_Query  (void);

#else

static struct LibraryHeader * LIBFUNC LibInit    (REG(d0, struct LibraryHeader *base), REG(a0, BPTR librarySegment), REG(a6, struct ExecBase *sb));
static BPTR                   LIBFUNC LibExpunge (REG(a6, struct LibraryHeader *base));
static struct LibraryHeader * LIBFUNC LibOpen    (REG(a6, struct LibraryHeader *base));
static BPTR                   LIBFUNC LibClose   (REG(a6, struct LibraryHeader *base));
static LONG                   LIBFUNC LibNull    (void);
static ULONG                  LIBFUNC MCC_Query  (REG(d0, LONG which));

#endif

/******************************************************************************/
/* Dummy entry point and LibNull() function all in one                        */
/******************************************************************************/

/*
 * The system (and compiler) rely on a symbol named _start which marks
 * the beginning of execution of an ELF file. To prevent others from
 * executing this library, and to keep the compiler/linker happy, we
 * define an empty _start symbol here.
 *
 * On the classic system (pre-AmigaOS4) this was usually done by
 * moveq #0,d0
 * rts
 *
 */

#if defined(__amigaos4__)
int32 _start(void)
#else
int Main(void)
#endif
{
  return RETURN_FAIL;
}

#if !defined(__amigaos4__)
static LONG LIBFUNC LibNull(VOID)
{
  return(0);
}
#endif

/******************************************************************************/
/* Local data structures                                                      */
/******************************************************************************/

#if defined(__amigaos4__)
/* ------------------- OS4 Manager Interface ------------------------ */
STATIC uint32 _manager_Obtain(struct LibraryManagerInterface *Self)
{
  uint32 res;
  __asm__ __volatile__(
  "1: lwarx %0,0,%1\n"
  "addic  %0,%0,1\n"
  "stwcx. %0,0,%1\n"
  "bne- 1b"
  : "=&r" (res)
  : "r" (&Self->Data.RefCount)
  : "cc", "memory");

  return res;
}

STATIC uint32 _manager_Release(struct LibraryManagerInterface *Self)
{
  uint32 res;
  __asm__ __volatile__(
  "1: lwarx %0,0,%1\n"
  "addic  %0,%0,-1\n"
  "stwcx. %0,0,%1\n"
  "bne- 1b"
  : "=&r" (res)
  : "r" (&Self->Data.RefCount)
  : "cc", "memory");

  return res;
}

STATIC CONST APTR lib_manager_vectors[] =
{
  (APTR)_manager_Obtain,
  (APTR)_manager_Release,
  (APTR)NULL,
  (APTR)NULL,
  (APTR)LibOpen,
  (APTR)LibClose,
  (APTR)LibExpunge,
  (APTR)NULL,
  (APTR)-1
};

STATIC CONST struct TagItem lib_managerTags[] =
{
  { MIT_Name,         (Tag)"__library" },
  { MIT_VectorTable,  (Tag)lib_manager_vectors },
  { MIT_Version,      1 },
  { TAG_DONE,         0 }
};

/* ------------------- Library Interface(s) ------------------------ */

ULONG _MCCClass_Obtain(UNUSED struct Interface *Self)
{
  return 0;
}

ULONG _MCCClass_Release(UNUSED struct Interface *Self)
{
  return 0;
}

STATIC CONST APTR main_vectors[] =
{
  (APTR)_MCCClass_Obtain,
  (APTR)_MCCClass_Release,
  (APTR)NULL,
  (APTR)NULL,
  (APTR)MCC_Query,
  (APTR)-1
};

STATIC CONST struct TagItem mainTags[] =
{
  { MIT_Name,         (Tag)"main" },
  { MIT_VectorTable,  (Tag)main_vectors },
  { MIT_Version,      1 },
  { TAG_DONE,         0 }
};

STATIC CONST CONST_APTR libInterfaces[] =
{
  lib_managerTags,
  mainTags,
  NULL
};

// Our libraries always have to carry a 68k jump table with it, so
// lets define it here as extern, as we are going to link it to
// our binary here.
#ifndef NO_VECTABLE68K
extern CONST APTR VecTable68K[];
#endif

STATIC CONST struct TagItem libCreateTags[] =
{
  { CLT_DataSize,   sizeof(struct LibraryHeader) },
  { CLT_InitFunc,   (Tag)LibInit },
  { CLT_Interfaces, (Tag)libInterfaces },
  #ifndef NO_VECTABLE68K
  { CLT_Vector68K,  (Tag)VecTable68K },
  #endif
  { TAG_DONE,       0 }
};

#else

static const APTR LibVectors[] =
{
  #ifdef __MORPHOS__
  (APTR)FUNCARRAY_32BIT_NATIVE,
  #endif
  (APTR)LibOpen,
  (APTR)LibClose,
  (APTR)LibExpunge,
  (APTR)LibNull,
  (APTR)MCC_Query,
  (APTR)-1
};

static const ULONG LibInitTab[] =
{
  sizeof(struct LibraryHeader),
  (ULONG)LibVectors,
  (ULONG)NULL,
  (ULONG)LibInit
};

#endif

/* ------------------- ROM Tag ------------------------ */
static const USED_VAR struct Resident ROMTag =
{
  RTC_MATCHWORD,
  (struct Resident *)&ROMTag,
  (struct Resident *)(&ROMTag + 1),
  #if defined(__amigaos4__)
  RTF_AUTOINIT|RTF_NATIVE,      // The Library should be set up according to the given table.
  #elif defined(__MORPHOS__)
  RTF_AUTOINIT|RTF_PPC,
  #else
  RTF_AUTOINIT,
  #endif
  VERSION,
  NT_LIBRARY,
  0,
  (char *)UserLibName,
  (char *)UserLibID+6,          // +6 to skip '$VER: '
  #if defined(__amigaos4__)
  (APTR)libCreateTags,          // This table is for initializing the Library.
  #else
  (APTR)LibInitTab,
  #endif
  #if defined(__MORPHOS__)
  REVISION,
  0
  #endif
};

#if defined(__MORPHOS__)
/*
 * To tell the loader that this is a new emulppc elf and not
 * one for the ppc.library.
 * ** IMPORTANT **
 */
const USED_VAR ULONG __amigappc__ = 1;
const USED_VAR ULONG __abox__ = 1;

#endif /* __MORPHOS__ */

/******************************************************************************/
/* Standard Library Functions, all of them are called in Forbid() state.      */
/******************************************************************************/

#if defined(__amigaos4__)
static struct LibraryHeader * LibInit(struct LibraryHeader *base, BPTR librarySegment, struct ExecIFace *pIExec)
{
  struct Library *sb = (struct Library *)pIExec->Data.LibBase;
  IExec = pIExec;
#elif defined(__MORPHOS__)
static struct LibraryHeader * LibInit(struct LibraryHeader *base, BPTR librarySegment, struct ExecBase *sb)
{
#else
static struct LibraryHeader * LIBFUNC LibInit(REG(d0, struct LibraryHeader *base), REG(a0, BPTR librarySegment), REG(a6, struct ExecBase *sb))
{
#endif

  SysBase = sb;

  // make sure that this is really a 68020+ machine if optimized for 020+
  #if _M68060 || _M68040 || _M68030 || _M68020 || __mc68020 || __mc68030 || __mc68040 || __mc68060
  if(!(SysBase->AttnFlags & AFF_68020))
    return(NULL);
  #endif

  #if defined(__amigaos4__) && defined(__NEWLIB__)
  if((NewlibBase = OpenLibrary("newlib.library", 3)) &&
     GETINTERFACE(INewlib, struct Interface*, NewlibBase))
  #endif
  {       
    #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
    struct StackSwapStruct stack;
    #endif

    D(DBF_STARTUP, "LibInit(%s)", CLASS);

    // cleanup the library header structure beginning with the
    // library base, even if that is done automcatically, we explicitly
    // do it here for consistency reasons.
    base->lh_Library.lib_Node.ln_Type = NT_LIBRARY;
    base->lh_Library.lib_Node.ln_Pri  = 0;
    base->lh_Library.lib_Node.ln_Name = (char *)UserLibName;
    base->lh_Library.lib_Flags        = LIBF_CHANGED | LIBF_SUMUSED;
    base->lh_Library.lib_IdString     = (char *)UserLibID; // here without +6 or otherwise MUI doesn't identify it.
    base->lh_Library.lib_Version      = VERSION;
    base->lh_Library.lib_Revision     = REVISION;

    // init our protecting semaphore and the
    // initialized flag variable
    InitSemaphore(&base->lh_Semaphore);

    // protect ClassInit()
    ObtainSemaphore(&base->lh_Semaphore);

    // If we are not running on AmigaOS4 (no stackswap required) we go and
    // do an explicit StackSwap() in case the user wants to make sure we
    // have enough stack for his user functions
    #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
    if((stack.stk_Lower = AllocVec(MIN_STACKSIZE, MEMF_PUBLIC)) != NULL)
    {
      // perform the StackSwap
      stack.stk_Upper = (ULONG)stack.stk_Lower + MIN_STACKSIZE;
      stack.stk_Pointer = (APTR)stack.stk_Upper;
      StackSwap(&stack);
    #endif

      // now that this library/class is going to be initialized for the first time
      // we go and open all necessary libraries on our own
      if((DOSBase = (struct Library *)OpenLibrary("dos.library", 36)) &&
         GETINTERFACE(IDOS, struct DOSIFace*, DOSBase))
      if((GfxBase = (struct Library *)OpenLibrary("graphics.library", 36)) &&
         GETINTERFACE(IGraphics, struct GraphicsIFace*, GfxBase))
      if((IntuitionBase = (struct Library *)OpenLibrary("intuition.library", 36)) &&
         GETINTERFACE(IIntuition, struct IntuitionIFace*, IntuitionBase))
      if((UtilityBase = (struct Library *)OpenLibrary("utility.library", 36)) &&
         GETINTERFACE(IUtility, struct UtilityIFace*, UtilityBase))
      {
        // we have to please the internal utilitybase
        // pointers of libnix and clib2
        #if !defined(__NEWLIB__)
          __UtilityBase = (APTR)UtilityBase;
          #if defined(__amigaos4__)
          __IUtility = IUtility;
          #endif
        #endif

        #if defined(DEBUG)
        SetupDebug();
        #endif

        if((MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MASTERVERSION)) &&
           GETINTERFACE(IMUIMaster, struct MUIMasterIFace*, MUIMasterBase))
        {
          #if defined(PRECLASSINIT)
          if(PreClassInit())
          #endif
          {
            #ifdef SUPERCLASS
            ThisClass = MUI_CreateCustomClass(&base->lh_Library, SUPERCLASS, NULL, sizeof(struct INSTDATA), ENTRY(_Dispatcher));
            if(ThisClass)
            #endif
            {
              #ifdef SUPERCLASSP
              if((ThisClassP = MUI_CreateCustomClass(&base->lh_Library, SUPERCLASSP, NULL, sizeof(struct INSTDATAP), ENTRY(_DispatcherP))))
              #endif
              {
                #ifdef SUPERCLASS
                #define THISCLASS ThisClass
                #else
                #define THISCLASS ThisClassP
                #endif

                // in case the user defined an own ClassInit()
                // function we call it protected by a semaphore as
                // this user may be stupid and break the Forbid() state
                // of LibInit()
                #if defined(CLASSINIT)
                if(ClassInit(&base->lh_Library))
                #endif
                {
                  // everything was successfully so lets
                  // set the initialized value and contiue
                  // with the class open phase
                  base->lh_Segment = librarySegment;

                  // make sure to swap our stack back before we
                  // exit
                  #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
                  StackSwap(&stack);
                  FreeVec(stack.stk_Lower);
                  #endif

                  // unprotect
                  ReleaseSemaphore(&base->lh_Semaphore);

                  // return the library base as success
                  return base;
                }

                // if we pass this point than an error
                // occurred and we have to cleanup
                #if defined(SUPERCLASSP)
                MUI_DeleteCustomClass(ThisClassP);
                ThisClassP = NULL;
                #endif
              }

              #if defined(SUPERCLASS)
              MUI_DeleteCustomClass(ThisClass);
              ThisClass = NULL;
              #endif
            }
          }

          DROPINTERFACE(IMUIMaster);
          CloseLibrary(MUIMasterBase);
          MUIMasterBase = NULL;
        }

        DROPINTERFACE(IUtility);
        CloseLibrary(UtilityBase);
        UtilityBase = NULL;
      }

      if(IntuitionBase)
      {
        DROPINTERFACE(IIntuition);
        CloseLibrary((struct Library *)IntuitionBase);
        IntuitionBase = NULL;
      }

      if(GfxBase)
      {
        DROPINTERFACE(IGraphics);
        CloseLibrary((struct Library *)GfxBase);
        GfxBase = NULL;
      }

      if(DOSBase)
      {
        DROPINTERFACE(IDOS);
        CloseLibrary((struct Library *)DOSBase);
        DOSBase = NULL;
      }

      E(DBF_STARTUP, "ClassInit(%s) failed", CLASS);

    #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
      // make sure to swap our stack back before we
      // exit
      StackSwap(&stack);
      FreeVec(stack.stk_Lower);
    }
    #endif

    // unprotect
    ReleaseSemaphore(&base->lh_Semaphore);

    #if defined(__amigaos4__) && defined(__NEWLIB__)
    if(NewlibBase)
    {
      DROPINTERFACE(INewlib);
      CloseLibrary(NewlibBase);
      NewlibBase = NULL;
    }
    #endif
  }

  return(NULL);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#ifndef __amigaos4__
#define DeleteLibrary(LIB) \
  FreeMem((STRPTR)(LIB)-(LIB)->lib_NegSize, (ULONG)((LIB)->lib_NegSize+(LIB)->lib_PosSize))
#endif

#if defined(__amigaos4__)
static BPTR LibExpunge(struct LibraryManagerInterface *Self)
{
  struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
  struct LibraryHeader *base = (struct LibraryHeader *)Self->Data.LibBase;
#elif defined(__MORPHOS__)
static BPTR LibExpunge(void)
{
  struct LibraryHeader *base = (struct LibraryHeader *)REG_A6;
#else
static BPTR LIBFUNC LibExpunge(REG(a6, struct LibraryHeader *base))
{
#endif
  BPTR rc;

  D(DBF_STARTUP, "LibExpunge(%s): %ld", CLASS, base->lh_Library.lib_OpenCnt);

  // in case our open counter is still > 0, we have
  // to set the late expunge flag and return immediately
  if(base->lh_Library.lib_OpenCnt > 0)
  {
    base->lh_Library.lib_Flags |= LIBF_DELEXP;
    rc = 0;
  }
  else
  {
    #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
    struct StackSwapStruct stack;
    #endif

    // remove the library base from exec's lib list in advance
    Remove((struct Node *)base);

    // protect
    ObtainSemaphore(&base->lh_Semaphore);

    // make sure we have enough stack here
    #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
    if((stack.stk_Lower = AllocVec(MIN_STACKSIZE, MEMF_PUBLIC)) != NULL)
    {
      // perform the StackSwap
      stack.stk_Upper = (ULONG)stack.stk_Lower + MIN_STACKSIZE;
      stack.stk_Pointer = (APTR)stack.stk_Upper;
      StackSwap(&stack);
    #endif

      // in case the user specified that he has an own class
      // expunge function we call it right here, not caring about
      // any return value.
      #if defined(CLASSEXPUNGE)
      ClassExpunge(&base->lh_Library);
      #endif

      // now we remove our own stuff here step-by-step
      #ifdef SUPERCLASSP
      if(ThisClassP)
      {
        MUI_DeleteCustomClass(ThisClassP);
        ThisClassP = NULL;
      }
      #endif

      #ifdef SUPERCLASS
      if(ThisClass)
      {
        MUI_DeleteCustomClass(ThisClass);
        ThisClass = NULL;
      }
      #endif

      // we inform the user that all main class expunge stuff
      // is finished, if he want's to get informed.
      #if defined(POSTCLASSEXPUNGE)
      PostClassExpunge();
      #endif

      // cleanup the various library bases and such
      if(MUIMasterBase)
      {
        DROPINTERFACE(IMUIMaster);
        CloseLibrary(MUIMasterBase);
        MUIMasterBase = NULL;
      }

      if(UtilityBase)
      {
        DROPINTERFACE(IUtility);
        CloseLibrary(UtilityBase);
        UtilityBase = NULL;
      }

      if(IntuitionBase)
      {
        DROPINTERFACE(IIntuition);
        CloseLibrary((struct Library *)IntuitionBase);
        IntuitionBase = NULL;
      }

      if(GfxBase)
      {
        DROPINTERFACE(IGraphics);
        CloseLibrary((struct Library *)GfxBase);
        GfxBase = NULL;
      }

      if(DOSBase)
      {
        DROPINTERFACE(IDOS);
        CloseLibrary((struct Library *)DOSBase);
        DOSBase = NULL;
      }

    #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
      // make sure to swap our stack back before we
      // exit
      StackSwap(&stack);
      FreeVec(stack.stk_Lower);
    }
    #endif

    // release access to lh_Initialized
    ReleaseSemaphore(&base->lh_Semaphore);

    #if defined(__amigaos4__) && defined(__NEWLIB__)
    if(NewlibBase)
    {
      DROPINTERFACE(INewlib);
      CloseLibrary(NewlibBase);
      NewlibBase = NULL;
    }
    #endif

    // make sure the system deletes the library as well.
    rc = base->lh_Segment;
    DeleteLibrary(&base->lh_Library);
  }

  return(rc);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#if defined(__amigaos4__)
static struct LibraryHeader *LibOpen(struct LibraryManagerInterface *Self, ULONG version UNUSED)
{
  struct LibraryHeader *base = (struct LibraryHeader *)Self->Data.LibBase;
#elif defined(__MORPHOS__)
static struct LibraryHeader *LibOpen(void)
{
  struct LibraryHeader *base = (struct LibraryHeader *)REG_A6;
#else
static struct LibraryHeader * LIBFUNC LibOpen(REG(a6, struct LibraryHeader *base))
{
#endif
  struct LibraryHeader *res = NULL;

  D(DBF_STARTUP, "LibOpen(%s): %ld", CLASS, base->lh_Library.lib_OpenCnt);

  // LibOpen(), LibClose() and LibExpunge() are called while the system is in
  // Forbid() state. That means that these functions should be quick and should
  // not break this Forbid()!! Therefore the open counter should be increased
  // as the very first instruction during LibOpen(), because a ClassOpen()
  // which breaks a Forbid() and another task calling LibExpunge() will cause
  // to expunge this library while it is not yet fully initialized. A crash
  // is unavoidable then. Even the semaphore does not guarantee 100% protection
  // against such a race condition, because waiting for the semaphore to be
  // obtained will effectively break the Forbid()!

  // increase the open counter ahead of anything else
  base->lh_Library.lib_OpenCnt++;

  // delete the late expunge flag
  base->lh_Library.lib_Flags &= ~LIBF_DELEXP;

  // check if the user defined a ClassOpen() function
  #if defined(CLASSOPEN)
  {
    struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;

    #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
    struct StackSwapStruct stack;
    #endif

    // protect
    ObtainSemaphore(&base->lh_Semaphore);

    // make sure we have enough stack here
    #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
    if((stack.stk_Lower = AllocVec(MIN_STACKSIZE, MEMF_PUBLIC)) != NULL)
    {
      // perform the StackSwap
      stack.stk_Upper = (ULONG)stack.stk_Lower + MIN_STACKSIZE;
      stack.stk_Pointer = (APTR)stack.stk_Upper;
      StackSwap(&stack);
    #endif

      // here we call the user-specific function for LibOpen() where
      // he can do whatever he wants because of the semaphore protection.
      if(ClassOpen(&base->lh_Library))
        res = base;
      else
      {
        E(DBF_STARTUP, "ClassOpen(%s) failed", CLASS);

        // decrease the open counter again
        base->lh_Library.lib_OpenCnt--;
      }

    #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
      // make sure to swap our stack back before we
      // exit
      StackSwap(&stack);
      FreeVec(stack.stk_Lower);
    }
    #endif

    // release the semaphore
    ReleaseSemaphore(&base->lh_Semaphore);
  }
  #else
    res = base;
  #endif

  return res;
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#if defined(__amigaos4__)
static BPTR LibClose(struct LibraryManagerInterface *Self)
{
  struct LibraryHeader *base = (struct LibraryHeader *)Self->Data.LibBase;
#elif defined(__MORPHOS__)
static BPTR LibClose(void)
{
  struct LibraryHeader *base = (struct LibraryHeader *)REG_A6;
#else
static BPTR LIBFUNC LibClose(REG(a6, struct LibraryHeader *base))
{
#endif
  BPTR rc = 0;

  D(DBF_STARTUP, "LibClose(%s): %ld", CLASS, base->lh_Library.lib_OpenCnt);

  // check if the user defined a ClassClose() function
  #if defined(CLASSCLOSE)
  {
    struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;

    #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
    struct StackSwapStruct stack;
    #endif

    // protect
    ObtainSemaphore(&base->lh_Semaphore);

    // make sure we have enough stack here
    #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
    if((stack.stk_Lower = AllocVec(MIN_STACKSIZE, MEMF_PUBLIC)) != NULL)
    {
      // perform the StackSwap
      stack.stk_Upper = (ULONG)stack.stk_Lower + MIN_STACKSIZE;
      stack.stk_Pointer = (APTR)stack.stk_Upper;
      StackSwap(&stack);
    #endif

      // call the users' ClassClose() function
      ClassClose(&base->lh_Library);

    #if defined(MIN_STACKSIZE) && !defined(__amigaos4__)
      // make sure to swap our stack back before we
      // exit
      StackSwap(&stack);
      FreeVec(stack.stk_Lower);
    }
    #endif

    // release the semaphore
    ReleaseSemaphore(&base->lh_Semaphore);
  }
  #endif

  // decrease the open counter
  base->lh_Library.lib_OpenCnt--;

  // in case the opern counter is <= 0 we can
  // make sure that we free everything
  if(base->lh_Library.lib_OpenCnt <= 0)
  {
    // in case the late expunge flag is set we go and
    // expunge the library base right now
    if(base->lh_Library.lib_Flags & LIBF_DELEXP)
    {
      #if defined(__amigaos4__)
      rc = LibExpunge(Self);
      #elif defined(__MORPHOS__)
      rc = LibExpunge();
      #else
      rc = LibExpunge(base);
      #endif

      return rc;
    }
  }

  return rc;
}

/*****************************************************************************************************/
/*****************************************************************************************************/

#if defined(__amigaos4__)
static ULONG LIBFUNC MCC_Query(UNUSED struct Interface *self, REG(d0, LONG which))
{
#elif defined(__MORPHOS__)
static ULONG MCC_Query(void)
{
  LONG which = (LONG)REG_D0;
#else
static ULONG LIBFUNC MCC_Query(REG(d0, LONG which))
{
#endif

  D(DBF_STARTUP, "MCC_Query(%s): %ld", CLASS, which);

  switch (which)
  {
    #ifdef SUPERCLASS
    case 0: return((ULONG)ThisClass);
    #endif

    #ifdef SUPERCLASSP
    case 1: return((ULONG)ThisClassP);
    #endif

    #ifdef PREFSIMAGEOBJECT
    case 2:
    {
      Object *obj = PREFSIMAGEOBJECT;
      return((ULONG)obj);
    }
    #endif

    #ifdef ONLYGLOBAL
    case 3:
    {
      return(TRUE);
    }
    #endif

    #ifdef INFOCLASS
    case 4:
    {
      return(TRUE);
    }
    #endif

    #ifdef USEDCLASSES
    case 5:
    {
      return((ULONG)USEDCLASSES);
    }
    #endif

    #ifdef USEDCLASSESP
    case 6:
    {
      return((ULONG)USEDCLASSESP);
    }
    #endif

    #ifdef SHORTHELP
    case 7:
    {
      return((ULONG)SHORTHELP);
    }
    #endif
  }

  return(0);
}

#ifdef __cplusplus
}
#endif