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

#include <ctype.h>
#include <string.h>
#include <exec/types.h>
#include <proto/dos.h>

#include "General.h"
#include "TagInfo.h"
#include "TernaryTrees.h"

struct TagInfo TagTable[] =
{
  { "!--",        FLG_Inline,                           tag_COMMENT,      0 },
  { "A",          FLG_EndRequired,                      tag_A,            0 },
  { "ADDRESS",    FLG_EndRequired,                      tag_ADDRESS,      0 },
  { "AREA",       FLG_Inline,                           tag_AREA,         0 },
  { "B",          FLG_EndRequired,                      tag_B,            0 },
  { "BASE",       FLG_Inline,                           tag_BASE,         0 },
  { "BIG",        FLG_EndRequired,                      tag_BIG,          0 },
  { "BLOCKQUOTE", FLG_EndRequired | FLG_Blocklevel,     tag_BLOCKQUOTE,   0 },
  { "BODY",       0L,                                   tag_BODY,         0 },
  { "BR",         FLG_Inline,                           tag_BR,           0 },
  { "CENTER",     FLG_EndRequired | FLG_Blocklevel,     tag_CENTER,       0 },
  { "CITE",       FLG_EndRequired,                      tag_CITE,         0 },
  { "CODE",       FLG_EndRequired,                      tag_CODE,         0 },
  { "DD",         FLG_NoGroupNesting | FLG_Blocklevel,  tag_DD,           group_Definition },
  { "DFN",        FLG_EndRequired,                      tag_DFN,          0 },
  { "DIR",        FLG_EndRequired | FLG_Blocklevel,     tag_DIR,          0 },
  { "DIV",        FLG_EndRequired | FLG_Blocklevel,     tag_DIV,          0 },
  { "DL",         FLG_EndRequired | FLG_Blocklevel,     tag_DL,           0 },
  { "DT",         FLG_NoGroupNesting,                   tag_DT,           group_Definition },
  { "EM",         FLG_EndRequired,                      tag_EM,           0 },
  { "FONT",       FLG_EndRequired,                      tag_FONT,         0 },
  { "FORM",       FLG_EndRequired,                      tag_FORM,         0 },
  { "FRAME",      FLG_Inline,                           tag_FRAME,        0 },
  { "FRAMESET",   FLG_EndRequired,                      tag_FRAMESET,     0 },
  { "H1",         FLG_EndRequired | FLG_Blocklevel,     tag_H1,           0 },
  { "H2",         FLG_EndRequired | FLG_Blocklevel,     tag_H2,           0 },
  { "H3",         FLG_EndRequired | FLG_Blocklevel,     tag_H3,           0 },
  { "H4",         FLG_EndRequired | FLG_Blocklevel,     tag_H4,           0 },
  { "H5",         FLG_EndRequired | FLG_Blocklevel,     tag_H5,           0 },
  { "H6",         FLG_EndRequired | FLG_Blocklevel,     tag_H6,           0 },
  { "HEAD",       0L,                                   tag_HEAD,         0 },
  { "HR",         FLG_Inline,                           tag_HR,           0 },
  { "HTML",       0L,                                   tag_HTML,         0 },
  { "I",          FLG_EndRequired,                      tag_I,            0 },
  { "IMG",        FLG_Inline,                           tag_IMG,          0 },
  { "INPUT",      FLG_Inline,                           tag_INPUT,        0 },
  { "ISINDEX",    FLG_Inline,                           tag_ISINDEX,      0 },
  { "LI",         FLG_NoNesting,                        tag_LI,           0 },
  { "MAP",        FLG_EndRequired,                      tag_MAP,          0 },
  { "MENU",       FLG_EndRequired | FLG_Blocklevel,     tag_MENU,         0 },
  { "META",       FLG_Inline,                           tag_META,         0 },
  { "NOFRAMES",   FLG_EndRequired,                      tag_NOFRAMES,     0 },
  { "OL",         FLG_EndRequired | FLG_Blocklevel,     tag_OL,           0 },
  { "OPTION",     FLG_NoNesting,                        tag_OPTION,       0 },
  { "P",          FLG_Blocklevel,                       tag_P,            0 },
  { "PRE",        FLG_EndRequired | FLG_Blocklevel,     tag_PRE,          0 },
  { "Q",          FLG_EndRequired,                      tag_Q,            0 },
  { "S",          FLG_EndRequired,                      tag_S,            0 },
  { "SAMP",       FLG_EndRequired,                      tag_SAMP,         0 },
  { "SCRIPT",     FLG_EndRequired,                      tag_SCRIPT,       0 },
  { "SELECT",     FLG_EndRequired,                      tag_SELECT,       0 },
  { "SMALL",      FLG_EndRequired,                      tag_SMALL,        0 },
  { "STRIKE",     FLG_EndRequired,                      tag_STRIKE,       0 },
  { "STRONG",     FLG_EndRequired,                      tag_STRONG,       0 },
  { "SUB",        FLG_EndRequired,                      tag_SUB,          0 },
  { "SUP",        FLG_EndRequired,                      tag_SUP,          0 },
  { "TABLE",      FLG_EndRequired,                      tag_TABLE,        0 },
  { "TD",         FLG_NoGroupNesting | FLG_Blocklevel,  tag_TD,           group_Tablecell },
  { "TEXTAREA",   FLG_EndRequired,                      tag_TEXTAREA,     0 },
  { "TH",         FLG_NoGroupNesting | FLG_Blocklevel,  tag_TH,           group_Tablecell },
  { "TITLE",      FLG_EndRequired,                      tag_TITLE,        0 },
  { "TR",         FLG_NoNesting | FLG_Blocklevel,       tag_TR,           0 },
  { "TT",         FLG_EndRequired,                      tag_TT,           0 },
  { "U",          FLG_EndRequired,                      tag_U,            0 },
  { "UL",         FLG_EndRequired | FLG_Blocklevel,     tag_UL,           0 },
  { NULL,         0,                                    0,                0 }
};

UBYTE TagEndTable [256];
UBYTE WhitespaceTable [256];

typedef unsigned char uint8;

extern "C" VOID _INIT_6_CharTables ();
VOID _INIT_6_CharTables ()
{
   memset(TagEndTable, FALSE, 256);
   STRPTR tagenders = (STRPTR)"\f\r\n\t >";
   do {
      TagEndTable[(uint8)*tagenders] = TRUE;
   } while(*tagenders++);

   memset(WhitespaceTable, FALSE, 256);
   STRPTR whitespaces = (STRPTR)"\f\r\n\t ";
   while(*whitespaces)
      WhitespaceTable[(unsigned char)*whitespaces++] = TRUE;
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

struct TNode *TagTree = NULL;

extern "C" ULONG _INIT_7_BuildTagTree ();
ULONG _INIT_7_BuildTagTree ()
{
	return BinaryInsert(TagTree, TagTable, (ULONG)0, (ULONG)sizeof(TagTable) / sizeof(TagInfo) - 2);
}

extern "C" VOID _EXIT_7_DisposeTagTree ();
VOID _EXIT_7_DisposeTagTree ()
{
   if (TagTree) delete TagTree;
   TagTree = NULL;
}

struct TagInfo DummyTag = { "Unknown", 0, tag_Unknown, 0 };

struct TagInfo *GetTagInfo (STRPTR tag)
{
  struct TagInfo *res;
  if(!(res = (struct TagInfo *)TFind(TagTree, tag, TagEndTable)))
  {
    if(!strncmp(tag, "!--", 3))
        res = &TagTable[tag_COMMENT];
    else  res = &DummyTag;
  }
  return res;
}
