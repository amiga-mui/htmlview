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

#ifndef TAGINFO_H
#define TAGINFO_H

/* Protos */

struct TagInfo *GetTagInfo (STRPTR tag);

/*
BOOL NotTagEnd (REG(d0) UBYTE c);
BOOL IsWhitespace (REG(d0) UBYTE c);
*/

extern UBYTE TagEndTable [];
extern UBYTE WhitespaceTable [];
#define IsWhitespace(c) WhitespaceTable[(ULONG)c]
#define TagEnd(c) TagEndTable[c]

extern struct TagInfo TagTable[];

/* Data */

struct TagInfo
{
	CONST_STRPTR	Name;
	CONST UBYTE		Flags;
	CONST UBYTE		ID;
  CONST UBYTE   Group;
	CONST_APTR		GetData () { return this; }
};

#define FLG_Inline			    (1<<0)
#define FLG_EndRequired     (1<<1)
#define FLG_NoNesting		    (1<<2)
#define FLG_NoChilds			  (1<<3)
#define FLG_Depreciated		  (1<<4)
#define FLG_Blocklevel		  (1<<5)
#define FLG_NoGroupNesting	(1<<6)

enum
{
	group_Normal = 0,
	group_Definition,
	group_Tablecell,
	group_NumberOf
};

enum
{
	tag_COMMENT = 0,
	tag_A,
	tag_ADDRESS,
	tag_AREA,
	tag_B,
	tag_BASE,
	tag_BIG,
	tag_BLOCKQUOTE,
	tag_BODY,
	tag_BR,
	tag_CENTER,
	tag_CITE,
	tag_CODE,
	tag_DD,
	tag_DFN,
	tag_DIR,
	tag_DIV,
	tag_DL,
	tag_DT,
	tag_EM,
	tag_FONT,
	tag_FORM,
	tag_FRAME,
	tag_FRAMESET,
	tag_H1,
	tag_H2,
	tag_H3,
	tag_H4,
	tag_H5,
	tag_H6,
	tag_HEAD,
	tag_HR,
	tag_HTML,
	tag_I,
	tag_IMG,
	tag_INPUT,
	tag_ISINDEX,
	tag_LI,
	tag_MAP,
	tag_MENU,
	tag_META,
	tag_NOFRAMES,
	tag_OL,
	tag_OPTION,
	tag_P,
	tag_PRE,
	tag_Q,
	tag_S,
	tag_SAMP,
	tag_SCRIPT,
	tag_SELECT,
	tag_SMALL,
	tag_STRIKE,
	tag_STRONG,
	tag_SUB,
	tag_SUP,
	tag_TABLE,
	tag_TD,
	tag_TEXTAREA,
	tag_TH,
	tag_TITLE,
	tag_TR,
	tag_TT,
	tag_U,
	tag_UL,
	tag_Text,
	tag_Unknown,
	tag_NumberOf
};

#endif
