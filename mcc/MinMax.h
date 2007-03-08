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

#ifndef MINMAX_MESSAGE_H
#define MINMAX_MESSAGE_H

struct CountCellsMessage
{
	ULONG Columns;
	ULONG Rows;
	ULONG RowCount;
	UWORD OpenRows;
	UWORD RowSpan[64];
};

struct CellWidth
{
	ULONG Min, Max, Percent, Relative;
	ULONG Width; // Result!
	BOOL Fixed;
};

struct MinMaxMessage
{
	MinMaxMessage (struct TextFont **fonts, struct LayoutMessage *lmsg, struct CellWidth *widths = NULL);
	VOID Reset ();
	VOID Newline ();

	LONG Min, Max;
	ULONG X, Columns;

	ULONG *RowOpenCounts;
	struct CellWidth *Widths;

	ULONG Indent;
	ULONG ImageLeftIndent, ImageRightIndent;

	struct TextFont *Font;
	struct TextFont **Fonts;

	LONG Spacing, Padding;
	BOOL TableBorder, Pass;

	/* Mainly for accessing the image cache... */
	struct LayoutMessage *LMsg;
};

#endif
