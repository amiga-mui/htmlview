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
#include <string.h>

/******************************************************************************/
/*                                                                            */
/* MCC/MCP name and version                                                   */
/*                                                                            */
/* ATTENTION:  The FIRST LETTER of NAME MUST be UPPERCASE                     */
/*                                                                            */
/******************************************************************************/

#include "HTMLview_mcc.h"
#include "ScrollGroup.h"
#include "rev.h"
#include "Debug.h"

#define VERSION       LIB_VERSION
#define REVISION      LIB_REVISION

#define CLASS         MUIC_HTMLview
#define SUPERCLASS    MUIC_Virtgroup

#define INSTDATASIZE  GetHTMLviewDataSize()

#define USERLIBID     CLASS " " LIB_REV_STRING CPU " (" LIB_DATE ") " LIB_COPYRIGHT
#define MASTERVERSION 19

#define CLASSINIT
#define CLASSEXPUNGE
#define MIN_STACKSIZE 8192

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

/******************************************************************************/
/* define the functions used by the startup code ahead of including mccinit.c */
/******************************************************************************/
static BOOL ClassInit(struct Library *base);
static VOID ClassExpunge(struct Library *base);
extern void _init(void);
extern void _fini(void);
extern ULONG GetHTMLviewDataSize(void);

CPPDISPATCHERGATE(_Dispatcher);
CPPDISPATCHERGATE(ScrollGroupDispatcher);
#ifdef USEMUISTRINGS
CPPDISPATCHERGATE(StringDispatcher);
#endif

/******************************************************************************/
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/******************************************************************************/
#include "mccinit.c"

struct SignalSemaphore ttt;
ULONG tttfail = 0;

/******************************************************************************/
/* define all implementations of our user functions                           */
/******************************************************************************/
static BOOL ClassInit(UNUSED struct Library *base)
{
  ENTER();

  memset(&ttt,0,sizeof(ttt));
  InitSemaphore(&ttt);
  tttfail=0;

  _init();

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
    if((ScrollGroupClass = MUI_CreateCustomClass(NULL, MUIC_Virtgroup, NULL, GetScrollGroupDataSize(), CPPDISPATCHERENTRY(ScrollGroupDispatcher))))
    {
  	  #ifdef USEMUISTRINGS
      if((StringClass = MUI_CreateCustomClass(NULL, MUIC_String, NULL, sizeof(struct StringData), CPPDISPATCHERENTRY(StringDispatcher))))
      #endif
  	  {
      	RETURN(TRUE);
        return TRUE;
  	  }
    }
  }

  ClassExpunge(base);

  RETURN(FALSE);
  return(FALSE);
}

static VOID ClassExpunge(UNUSED struct Library *base)
{
  ENTER();

  #ifdef USEMUISTRINGS
  if(StringClass)
  {
    MUI_DeleteCustomClass(StringClass);
    StringClass = NULL;
  }
  #endif

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

  _fini();

  LEAVE();
}
