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

/******************************************************************************/
/*                                                                            */
/* MCC/MCP name and version                                                   */
/*                                                                            */
/* ATTENTION:  The FIRST LETTER of NAME MUST be UPPERCASE                     */
/*                                                                            */
/******************************************************************************/

#include "constructors.h"

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

/*
struct Library *LocaleBase = NULL;
struct Library *RexxSysBase = NULL;
struct Library *WorkbenchBase = NULL;

struct LocaleIFace *ILocale = NULL;
struct RexxSysIFace *IRexxSys = NULL;
struct Interface *IWorkbench = NULL;
*/

extern "C" {
void _Z17_INIT_5_CMapMutexv(void);
void _Z18_INIT_6_CharTablesv(void);
void _Z20_INIT_7_BuildTagTreev(void);
void _Z23_INIT_7_BuildColourTreev(void);
void _Z23_INIT_7_BuildEntityTreev(void);
void _Z23_INIT_7_PrepareDecodersv(void);
//void _GLOBAL__I__ZN14ImageCacheItemC2EPcP12PictureFrame(void);
//void _Z41__static_initialization_and_destruction_0ii(uint32, uint32);
} // extern "C"

BOOL ClassInitFunc(UNUSED struct Library *base)
{
  ENTER();

_Z17_INIT_5_CMapMutexv();
_Z18_INIT_6_CharTablesv();
_Z20_INIT_7_BuildTagTreev();
_Z23_INIT_7_BuildColourTreev();
_Z23_INIT_7_BuildEntityTreev();
_Z23_INIT_7_PrepareDecodersv();
//_GLOBAL__I__ZN14ImageCacheItemC2EPcP12PictureFrame();
//_Z41__static_initialization_and_destruction_0ii(1, 65535);

  if((LayersBase = OpenLibrary("layers.library", 36)) &&
     GETINTERFACE(ILayers, struct LayersIFace*, LayersBase))
  {
    if((KeymapBase = OpenLibrary("keymap.library", 36)) &&
       GETINTERFACE(IKeymap, struct KeymapIFace*, KeymapBase))
    {
      if((CxBase = OpenLibrary("commodities.library", 36)) &&
         GETINTERFACE(ICommodities, struct CommoditiesIFace*, CxBase))
      {
        if((DiskfontBase = OpenLibrary("diskfont.library", 36)) &&
           GETINTERFACE(IDiskfont, struct DiskfontIFace*, DiskfontBase))
        {
          if((DataTypesBase = OpenLibrary("datatypes.library", 36)) &&
             GETINTERFACE(IDataTypes, struct DataTypesIFace*, DataTypesBase))
          {
            if((CyberGfxBase = OpenLibrary("cybergraphics.library", 40)) &&
               GETINTERFACE(ICyberGfx, struct CyberGfxIFace*, CyberGfxBase))
            {
              if(run_constructors())
              {
                RETURN(TRUE);
                return(TRUE);
              }
            }
          }
        }
      }
    }
  }

  RETURN(FALSE);
  return(FALSE);
}


extern "C" {
void _Z21_EXIT_7_FlushDecodersv(void);
void _Z22_EXIT_7_DisposeTagTreev(void);
void _Z25_EXIT_7_DisposeColourTreev(void);
void _Z25_EXIT_7_DisposeEntityTreev(void);
} // extern "C"
VOID ClassExitFunc(UNUSED struct Library *base)
{
  ENTER();

_Z21_EXIT_7_FlushDecodersv();
_Z22_EXIT_7_DisposeTagTreev();
_Z25_EXIT_7_DisposeColourTreev();
_Z25_EXIT_7_DisposeEntityTreev();

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

  run_destructors();

  LEAVE();
}

/******************************************************************************/
/*                                                                            */
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/*                                                                            */
/******************************************************************************/

#define USE_UTILITYBASE
#include "mccheader.c"
