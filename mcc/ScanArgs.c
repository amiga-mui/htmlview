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

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <exec/types.h>
#include <proto/dos.h>

#include "General.h"
#include "Classes.h"
#include "ScanArgs.h"
#include "Colours.h"
#include "TagInfo.h"
#include "Entities.h"

VOID PrintTag (STRPTR);

STRPTR NextKeyword (STRPTR current)
{
	UBYTE quote = 0;
	while((!IsWhitespace(*current) || quote) && *current)
	{
		switch(*current)
		{
			case '"':
			case '\'':
			{
				if(!quote)
					quote = *current;
				else if(quote == *current)
					quote = 0;
			}
			break;
		}
		current++;
	}

	while(IsWhitespace(*current))
		current++;

	return(current);
}

VOID ScanArgs (STRPTR tag, struct ArgList *args)
{
	tag = NextKeyword(tag);
	while(*tag)
	{
		BOOL nomatch = TRUE;
		for(UWORD i = 0; args[i].Name && nomatch; i++)
		{
			ULONG len = strlen(args[i].Name);
			if(!strnicmp(tag, args[i].Name, len) && (tag[len] == '=' || IsWhitespace(tag[len]) || !tag[len]))
			{
				nomatch = FALSE;

				tag += len;
				while(IsWhitespace(*tag))
					tag++;

				STRPTR value = "";
				if(*tag == '=')
				{
					tag++;
					while(IsWhitespace(*tag))
						tag++;

					UBYTE quote = *tag;
					if(quote == '"' || quote == '\'')
							tag++;
					else	quote = FALSE;

					while(IsWhitespace(*tag))
						tag++;

					value = tag;
					if(quote)
					{
						while(*tag != quote && *tag)
							tag++;
					}
					else
					{
						while(!IsWhitespace(*tag) && *tag)
							tag++;
					}

					if(*tag)
					{
						*tag++ = '\0';
						while(IsWhitespace(*tag))
							tag++;
					}
				}

				switch(args[i].Type)
				{
					case ARG_SWITCH:
						*((BOOL *)args[i].Storage) = TRUE;
					break;

					case ARG_URL:
					case ARG_STRING:
					{
						UWORD len = strlen(value);
						while(len && IsWhitespace(value[len-1]))
							len--;

						STRPTR str = new char[len+1], dst = str;;
						*((STRPTR *)args[i].Storage) = str;

						BOOL space = FALSE;
						while(len--)
						{
							UBYTE character;
							switch(character = *value++)
							{
								case '&':
								{
									if(*value == '#')
									{
										WORD t_char = 0;
										BOOL scanning = TRUE;
										UWORD i = 1;
										do {

											t_char *= 10;
											t_char += value[i]-'0';
											i++;

										}	while(value[i] != ';' && i < 4);

										if(value[i++] == ';' && t_char < 256)
										{
											character = t_char;
											value += i;
											len -= i;
										}
									}
									else
									{
										struct EntityInfo *entity;
										if(entity = GetEntityInfo(value))
										{
											character = entity->ByteCode;
											value += strlen(entity->Name);
											len -= strlen(entity->Name);
											if(*value == ';')
											{
												value++;
												len--;
											}
										}
									}
								}
								/* `break' mangler, og det er med vijle ;-) */

								default:
								{
									if(IsWhitespace(character))
									{
										if(!space)
										{
											*dst++ = ' ';
											space = TRUE;
										}
									}
									else
									{
										*dst++ = character;
										space = FALSE;
									}
								}
								break;
							}
						}
						*dst = '\0';
					}
					break;

					case ARG_NUMBER:
					{
						if(isdigit(*value))
								sscanf(value, "%d", args[i].Storage);
						else	*((ULONG *)args[i].Storage) = TRUE;
					}
					break;

					case ARG_MULTIVALUE:
					{
						if(*value == '*')
						{
							*((struct ArgSize **)args[i].Storage) = new struct ArgSize(1, Size_Relative);
							break;
						}
					}
					case ARG_VALUE:
					{
						struct ArgSize *size = new struct ArgSize;
						LONG len = StrToLong(value, (LONG *)&size->Size);
						if(len > 0 && (LONG)size->Size >= 0)
						{
							switch(value[len])
							{
								case '%':
									size->Type = Size_Percent;
								break;

								case '*':
									size->Type = Size_Relative;
									if(args[i].Type == ARG_MULTIVALUE)
										break;
								default:
									size->Type = Size_Pixels;
								break;
							}
							*((struct ArgSize **)args[i].Storage) = size;
						}
						else
						{
							delete size;
						}
					}
					break;

					case ARG_KEYWORD:
					{
						ULONG result = 0;
						const char **keywords = args[i].KeywordList;

						while(keywords[result])
						{
							ULONG len = strlen(keywords[result]);
							if(!strnicmp(value, keywords[result], len))
							{
								*((ULONG *)args[i].Storage) = result;
								break;
							}
							result++;
						}
					}
					break;

					case ARG_COLOUR:
					{
						UBYTE *RGB;
						LONG res, cnt;
						if(RGB = GetColourInfo(value))
							res = (*(ULONG *)RGB) >> 8;
//							res = (RGB[0] << 16) | (RGB[1] << 8) | RGB[2];
						else if(sscanf(value, "%*[#]%x%n", &res, &cnt) < 3 || value[cnt])
							break;

						*((ULONG *)args[i].Storage) = res;
					}
					break;

					case ARG_BOOLEAN:
					{
						if(*value)
							*((ULONG *)args[i].Storage) = *value == '1' || !strnicmp(value, "YES", 3);
					}
					break;
				}
			}
		}
		if(nomatch)
			tag = NextKeyword(tag);
	}
	*tag = '<';
}
