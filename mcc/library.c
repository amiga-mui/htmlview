/***************************************************************************

 HTMLview.mcc - HTMLview MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005 by HTMLview.mcc Open Source Team

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
BOOL ClassInitFunc(UNUSED struct Library *base)
{
  ENTER();

  if((LayersBase = OpenLibrary("layers.library", 36)) &&
     GETINTERFACE(ILayers, LayersIFace*, LayersBase))
  {
    if((KeymapBase = OpenLibrary("keymap.library", 36)) &&
       GETINTERFACE(IKeymap, KeymapIFace*, KeymapBase))
    {
      if((CxBase = OpenLibrary("commodities.library", 36)) &&
         GETINTERFACE(ICommodities, CommoditiesIFace*, CxBase))
      {
        if((DiskfontBase = OpenLibrary("diskfont.library", 36)) &&
           GETINTERFACE(IDiskfont, DiskfontIFace*, DiskfontBase))
        {
          if((DataTypesBase = OpenLibrary("datatypes.library", 36)) &&
             GETINTERFACE(IDataTypes, DataTypesIFace*, DataTypesBase))
          {
            if((CyberGfxBase = OpenLibrary("cybergraphics.library", 40)) &&
               GETINTERFACE(ICyberGfx, CyberGfxIFace*, CyberGfxBase))
            { }

            RETURN(TRUE);
            return(TRUE);
          }
        }
      }
    }
  }

  RETURN(FALSE);
  return(FALSE);
}


VOID ClassExitFunc(UNUSED struct Library *base)
{
  ENTER();

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

/******************************************************************************/
/*                                                                            */
/* include the lib startup code for the mcc/mcp  (and muimaster inlines)      */
/*                                                                            */
/******************************************************************************/

#define USE_UTILITYBASE
#include "mccheader.c"
