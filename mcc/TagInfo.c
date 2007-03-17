/***************************************************************************

 HTMLview.mcc - HTMLview MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005 by TextEditor.mcc Open Source Team

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

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <exec/types.h>
#include <proto/dos.h>

#include "General.h"
#include "TagInfo.h"
#include "TernaryTrees.h"

struct TagInfo TagTable[] =
{
	{ "!--",			FLG_Inline,      tag_COMMENT },
	{ "A",			FLG_EndRequired, tag_A },
	{ "ADDRESS",	FLG_EndRequired, tag_ADDRESS },
	{ "AREA",		FLG_Inline,      tag_AREA },
	{ "B",			FLG_EndRequired, tag_B },
	{ "BASE",		FLG_Inline,      tag_BASE },
	{ "BIG",			FLG_EndRequired, tag_BIG },
	{ "BLOCKQUOTE",FLG_EndRequired | FLG_Blocklevel, tag_BLOCKQUOTE },
	{ "BODY",		0L,              tag_BODY },
	{ "BR",			FLG_Inline,      tag_BR },
	{ "CENTER",		FLG_EndRequired | FLG_Blocklevel, tag_CENTER },
	{ "CITE",		FLG_EndRequired, tag_CITE },
	{ "CODE",		FLG_EndRequired, tag_CODE },
	{ "DD",			FLG_NoGroupNesting | FLG_Blocklevel, tag_DD, group_Definition },
	{ "DFN",			FLG_EndRequired, tag_DFN },
	{ "DIR",			FLG_EndRequired | FLG_Blocklevel, tag_DIR },
	{ "DIV",			FLG_EndRequired | FLG_Blocklevel, tag_DIV },
	{ "DL",			FLG_EndRequired | FLG_Blocklevel, tag_DL },
	{ "DT",			FLG_NoGroupNesting, tag_DT, group_Definition },
	{ "EM",			FLG_EndRequired, tag_EM },
	{ "FONT",		FLG_EndRequired, tag_FONT },
	{ "FORM",		FLG_EndRequired, tag_FORM },
	{ "FRAME",		FLG_Inline,      tag_FRAME },
	{ "FRAMESET",	FLG_EndRequired, tag_FRAMESET },
	{ "H1",			FLG_EndRequired | FLG_Blocklevel, tag_H1 },
	{ "H2",			FLG_EndRequired | FLG_Blocklevel, tag_H2 },
	{ "H3",			FLG_EndRequired | FLG_Blocklevel, tag_H3 },
	{ "H4",			FLG_EndRequired | FLG_Blocklevel, tag_H4 },
	{ "H5",			FLG_EndRequired | FLG_Blocklevel, tag_H5 },
	{ "H6",			FLG_EndRequired | FLG_Blocklevel, tag_H6 },
	{ "HEAD",		0L,              tag_HEAD },
	{ "HR",			FLG_Inline,      tag_HR },
	{ "HTML",		0L,              tag_HTML },
	{ "I",			FLG_EndRequired, tag_I },
	{ "IMG",			FLG_Inline,      tag_IMG },
	{ "INPUT",		FLG_Inline,      tag_INPUT },
	{ "ISINDEX",	FLG_Inline,      tag_ISINDEX },
	{ "LI",			FLG_NoNesting,   tag_LI },
	{ "MAP",			FLG_EndRequired, tag_MAP },
	{ "MENU",		FLG_EndRequired | FLG_Blocklevel, tag_MENU },
	{ "META",		FLG_Inline,      tag_META },
	{ "NOFRAMES",	FLG_EndRequired, tag_NOFRAMES },
	{ "OL",			FLG_EndRequired | FLG_Blocklevel, tag_OL },
	{ "OPTION",		FLG_NoNesting,   tag_OPTION },
	{ "P",			FLG_Blocklevel,  tag_P },
	{ "PRE",			FLG_EndRequired | FLG_Blocklevel, tag_PRE },
	{ "Q",			FLG_EndRequired, tag_Q },
	{ "S",			FLG_EndRequired, tag_S },
	{ "SAMP",		FLG_EndRequired, tag_SAMP },
	{ "SCRIPT",		FLG_EndRequired, tag_SCRIPT },
	{ "SELECT",		FLG_EndRequired, tag_SELECT },
	{ "SMALL",		FLG_EndRequired, tag_SMALL },
	{ "STRIKE",		FLG_EndRequired, tag_STRIKE },
	{ "STRONG",		FLG_EndRequired, tag_STRONG },
	{ "SUB",			FLG_EndRequired, tag_SUB },
	{ "SUP",			FLG_EndRequired, tag_SUP },
	{ "TABLE",		FLG_EndRequired, tag_TABLE },
	{ "TD",			FLG_NoGroupNesting | FLG_Blocklevel, tag_TD, group_Tablecell },
	{ "TEXTAREA",	FLG_EndRequired, tag_TEXTAREA },
	{ "TH",			FLG_NoGroupNesting | FLG_Blocklevel, tag_TH, group_Tablecell },
	{ "TITLE",		FLG_EndRequired, tag_TITLE },
	{ "TR",			FLG_NoNesting | FLG_Blocklevel,   tag_TR },
	{ "TT",			FLG_EndRequired, tag_TT },
	{ "U",			FLG_EndRequired, tag_U },
	{ "UL",			FLG_EndRequired | FLG_Blocklevel, tag_UL },
	{ NULL }
};

UBYTE TagEndTable [256];
UBYTE WhitespaceTable [256];

extern "C" 
{
	VOID _INIT_6_CharTables ()
	{
		memset(TagEndTable, FALSE, 256);
		const UBYTE *tagenders = (const UBYTE *)"\f\r\n\t >";
		do {
			TagEndTable[*tagenders] = TRUE;
		} while(*tagenders++);

		memset(WhitespaceTable, FALSE, 256);
		const UBYTE *whitespaces = (const UBYTE *)"\f\r\n\t ";
		while(*whitespaces)
			WhitespaceTable[*whitespaces++] = TRUE;
	}
}

/*BOOL IsWhitespace (REG(d0) UBYTE c)
{
	return(WhitespaceTable[c]);
	switch(c)
	{
		case '\f':
		case '\n':
		case '\r':
		case '\t':
		case ' ':
			return(TRUE);
	}
	return(FALSE);
}

BOOL NotTagEnd (REG(d0) UBYTE c)
{
	return(NotTagEndTable[c]);
	switch(c)
	{
		case '\f':
		case '\r':
		case '\n':
		case '\t':
		case ' ':
		case '>':
			return(FALSE);
		break;

		default:
			return(TRUE);
		break;
	}
}
*/

struct TNode *TagTree;

extern "C" 
{
VOID _INIT_7_BuildTagTree ()
{
	BinaryInsert(TagTree, TagTable, (ULONG)0, (ULONG)sizeof(TagTable) / sizeof(TagInfo) - 2);
}

VOID _EXIT_7_DisposeTagTree ()
{
	delete TagTree;
}
} /* end of extern "C" */

struct TagInfo DummyTag = { "Unknown", 0, tag_Unknown };

struct TagInfo *GetTagInfo (STRPTR tag)
{
	struct TagInfo *res;
	if(!(res = (struct TagInfo *)TFind(TagTree, tag, TagEndTable)))
	{
		if(!strncmp(tag, "!--", 3))
				res = &TagTable[tag_COMMENT];
		else	res = &DummyTag;
	}
	return res;
}
