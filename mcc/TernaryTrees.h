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

#ifndef TERNARYTREES_H
#define TERNARYTREES_H

struct TNode
{
	TNode (STRPTR str, APTR data);
	virtual ~TNode ();
	struct TNode *TInsert (STRPTR str, APTR data);

	struct TNode *Left, *Right;
	union
	{
		struct TNode *Middle;
		APTR Data;
	};

	UBYTE SplitChar, _pad[3];
};

APTR TFind (struct TNode *root, CONST_STRPTR str, UBYTE *table);

template <class T>
VOID BinaryInsert (struct TNode *&tree, T *elements, ULONG from, ULONG to)
{
	ULONG diff = (to-from) / 2;

	tree = tree->TInsert(elements[from+diff].Name, elements[from+diff].GetData());
	if(from < to)
	{
		if(diff)
			BinaryInsert(tree, elements, from, from+diff-1);
		BinaryInsert(tree, elements, from+diff+1, to);
	}
}

#endif
