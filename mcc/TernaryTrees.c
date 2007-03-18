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

#include <stdio.h>
#include <exec/types.h>

#include "TernaryTrees.h"

#define QToUpper(c) ((c >= 'a' && c <= 'z') ? c-('a'-'A') : c)

TNode::TNode(CONST_STRPTR str, CONST_APTR data)
{
	if((SplitChar = *str))
			Middle = new TNode(str+1, data);
	else	Data = data;
}

TNode::~TNode ()
{
	delete Left;
	if(SplitChar)
		delete Middle;
	delete Right;
}

struct TNode *TNode::TInsert(CONST_STRPTR str, CONST_APTR data)
{
	struct TNode *res;

	if(!(res = this))
		res = new TNode(str, data);
	else if(*str < SplitChar)
		Left = Left->TInsert(str, data);
	else if(*str > SplitChar)
		Right = Right->TInsert(str, data);
	else if(SplitChar)
		Middle = Middle->TInsert(str+1, data);

	return res;
}

APTR TFind (struct TNode *node, CONST_STRPTR str, UBYTE *table)
{
	UBYTE chr, src = *str++;
	src = QToUpper(src);

	while(node)
	{
		chr = node->SplitChar;
		if(src < chr)
			node = node->Left;
		else if(src > chr)
			node = node->Right;
		else if((node = node->Middle), src)
//			src = (table[src = *str] ? 0 : (str++, QToUpper(src)));
			if(table[src = *str])
					src = 0;
			else	str++, (src = QToUpper(src));
		else break;
	}

	return node;
}
