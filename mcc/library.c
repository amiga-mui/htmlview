/***************************************************************************

 HTMLview.mcc - HTMLview MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2007 by HTMLview.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 HTMLview class Support Site:  http://www.sf.net/projects/htmlview-mcc/

 $Id$

***************************************************************************/

#include <proto/exec.h>
#include <proto/muimaster.h>

/******************************************************************************/
/*                                                                            */
/* MCC/MCP name and version                                                   */
/*                                                                            */
/* ATTENTION:  The FIRST LETTER of NAME MUST be UPPERCASE                     */
/*                                                                            */
/******************************************************************************/

#include "constructors.h"
#include "ScrollGroup.h"

#include "private.h"
#include "rev.h"

#define VERSION       LIB_VERSION
#define REVISION      LIB_REVISION

#define CLASS         MUIC_HTMLview
#define SUPERCLASS    MUIC_Area

#define INSTDATA      HTMLviewData

#define UserLibID     "$VER: HTMLview.mcc " LIB_REV_STRING CPU " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION 19

#define ClassInit
#define ClassExit
#define ClassExpunge

struct Library *LayersBase = NULL;
struct Library *KeymapBase = NULL;
struct Library *CxBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *DiskfontBase = NULL;
struct Library *DataTypesBase = NULL;

#if defined(__amigaos4__)
struct LayersIFace*       ILayers = NULL;
struct KeymapIFace*       IKeymap = NULL;
struct CommoditiesIFace*  ICommodities = NULL;
struct CyberGfxIFace*     ICyberGfx = NULL;
struct DiskfontIFace*     IDiskfont = NULL;
struct DataTypesIFace*    IDataTypes = NULL;
#endif

static ULONG initCPP(void);
static VOID cleanupCPP(void);

BOOL ClassInitFunc(UNUSED struct Library *base)
{
  BOOL result = FALSE;

  ENTER();

  if((LayersBase = OpenLibrary("layers.library", 36)) &&
    GETINTERFACE(ILayers, struct LayersIFace*, LayersBase))
  if((KeymapBase = OpenLibrary("keymap.library", 36)) &&
    GETINTERFACE(IKeymap, struct KeymapIFace*, KeymapBase))
  if((CxBase = OpenLibrary("commodities.library", 36)) &&
    GETINTERFACE(ICommodities, struct CommoditiesIFace*, CxBase))
  if((DiskfontBase = OpenLibrary("diskfont.library", 36)) &&
    GETINTERFACE(IDiskfont, struct DiskfontIFace*, DiskfontBase))
  if((DataTypesBase = OpenLibrary("datatypes.library", 36)) &&
    GETINTERFACE(IDataTypes, struct DataTypesIFace*, DataTypesBase))
  if((CyberGfxBase = OpenLibrary("cybergraphics.library", 40)) &&
    GETINTERFACE(ICyberGfx, struct CyberGfxIFace*, CyberGfxBase))
  {
    // setup our sub custom classes
    if((ScrollGroupClass = MUI_CreateCustomClass(NULL, MUIC_Virtgroup, NULL, sizeof(ScrollGroupData), ENTRY(ScrollGroupDispatcher))))
    {
      // before we can use our class we have to
      // setup all C++ specfic tasks
      if(initCPP())
        result = TRUE;
    }
  }

  RETURN(result);
  return(result);
}

VOID ClassExitFunc(UNUSED struct Library *base)
{
  ENTER();

  if(ScrollGroupClass)
  {
    MUI_DeleteCustomClass(ScrollGroupClass);
    ScrollGroupClass = NULL;
  }

  if(CyberGfxBase)
  {
    DROPINTERFACE(ICyberGfx);
    CloseLibrary(CyberGfxBase);
    CyberGfxBase = NULL;
  }

  if(DataTypesBase)
  {
    DROPINTERFACE(IDataTypes);
    CloseLibrary(DataTypesBase);
    DataTypesBase = NULL;
  }

  if(DiskfontBase)
  {
    DROPINTERFACE(IDiskfont);
    CloseLibrary(DiskfontBase);
    DiskfontBase = NULL;
  }

  if(CxBase)
  {
    DROPINTERFACE(ICommodities);
    CloseLibrary(CxBase);
    CxBase = NULL;
  }

  if(KeymapBase)
  {
    DROPINTERFACE(IKeymap);
    CloseLibrary(KeymapBase);
    KeymapBase = NULL;
  }

  if(LayersBase)
  {
    DROPINTERFACE(ILayers);
    CloseLibrary(LayersBase);
    LayersBase = NULL;
  }

  LEAVE();
}


VOID ClassExpungeFunc(UNUSED struct Library *base)
{
  ENTER();

  cleanupCPP();

  LEAVE();
}

/////////////////////////////////////////////////////////////////////////////////
// In this section we setup specfic C++ related things. In fact, we initialize
// the vtables for various classes, including running certain constructors
// defined by the C-runtime libraries involved
// Here we


// prototypes for the C++ virtual tables setup/cleanup
// functions automatically defined by the C++ compiler of GCC
extern "C"
{
  // C++ virtual table init functions
  void _INIT_5_CMapMutex(void);
  void _INIT_6_CharTables(void);
  void _INIT_7_BuildTagTree(void);
  void _INIT_7_BuildColourTree(void);
  void _INIT_7_BuildEntityTree(void);
  void _INIT_7_PrepareDecoders(void);
  //void _GLOBAL__I__ZN14ImageCacheItemC2EPcP12PictureFrame(void);
  //void _Z41__static_initialization_and_destruction_0ii(uint32, uint32);

  // C++ virtual table exit/cleanup functions
  void _EXIT_7_FlushDecoders(void);
  void _EXIT_7_DisposeTagTree(void);
  void _EXIT_7_DisposeColourTree(void);
  void _EXIT_7_DisposeEntityTree(void);
}

// C-runtime specific constructor/destructor
// initialization routines.
#if defined(__MORPHOS__)
#include "libnix.c"
#endif

static ULONG initCPP(void)
{
  static int initialized = 0;

  #if defined(__MORPHOS__)
  if (!run_constructors())
		return 0;
  #endif

  if (initialized)
    return 1;

  initialized = 1;

  // call the virtual tables setup in the
  // correct priority
  _INIT_5_CMapMutex();
  _INIT_6_CharTables();
  _INIT_7_BuildTagTree();
  _INIT_7_BuildColourTree();
  _INIT_7_BuildEntityTree();
  _INIT_7_PrepareDecoders();

  return 1;
}


static VOID cleanupCPP(void)
{
  // cleanup the virtual tables of various
  // classes
  _EXIT_7_FlushDecoders();
  _EXIT_7_DisposeTagTree();
  _EXIT_7_DisposeColourTree();
  _EXIT_7_DisposeEntityTree();

  #if defined(__MORPHOS__)
  run_destructors();
  #endif
}

/******************************************************************************/
/*                                                                            */
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/*                                                                            */
/******************************************************************************/

#define USE_UTILITYBASE
#include "mccheader.c"
