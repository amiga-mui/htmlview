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

#ifndef MUI_HTMLVIEW_MCP_H
#define MUI_HTMLVIEW_MCP_H

#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif

#include <devices/inputevent.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
  #ifdef __PPC__
    #pragma pack(2)
  #endif
#elif defined(__VBCC__)
  #pragma amiga-align
#endif

#define MUIC_HTMLview_mcp "HTMLview.mcp"
#define HTMLviewMcpObject MUI_NewObject(MUIC_HTMLview_mcp

#define CFG_ID(x)                     (0xad003000 + x)

#define MUICFG_HTMLview_SmallFont			    CFG_ID(1)
#define MUICFG_HTMLview_NormalFont	      CFG_ID(2)
#define MUICFG_HTMLview_FixedFont			    CFG_ID(3)
#define MUICFG_HTMLview_LargeFont			    CFG_ID(4)
#define MUICFG_HTMLview_H1						    CFG_ID(5)
#define MUICFG_HTMLview_H2						    CFG_ID(6)
#define MUICFG_HTMLview_H3						    CFG_ID(7)
#define MUICFG_HTMLview_H4						    CFG_ID(8)
#define MUICFG_HTMLview_H5						    CFG_ID(9)
#define MUICFG_HTMLview_H6						    CFG_ID(10)

#define MUICFG_HTMLview_IgnoreDocCols		  CFG_ID(11)
#define MUICFG_HTMLview_Col_Background	  CFG_ID(12)
#define MUICFG_HTMLview_Col_Text				  CFG_ID(13)
#define MUICFG_HTMLview_Col_Link				  CFG_ID(14)
#define MUICFG_HTMLview_Col_VLink			    CFG_ID(15)
#define MUICFG_HTMLview_Col_ALink			    CFG_ID(16)

#define MUICFG_HTMLview_DitherType			  CFG_ID(17)
#define MUICFG_HTMLview_ImageCacheSize	  CFG_ID(18)

#define MUICFG_HTMLview_PageScrollSmooth	CFG_ID(19)
#define MUICFG_HTMLview_PageScrollKey		  CFG_ID(20)
#define MUICFG_HTMLview_PageScrollMove		CFG_ID(21)

#define MUICFG_HTMLview_ListItemFile		  CFG_ID(22)

#define MUICFG_HTMLview_GammaCorrection	  CFG_ID(23)

#ifdef __GNUC__
  #ifdef __PPC__
    #pragma pack()
  #endif
#elif defined(__VBCC__)
  #pragma default-align
#endif

#ifdef __cplusplus
}
#endif

#endif /* MUI_HTMLVIEW_MCP_H */
