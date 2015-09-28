#/***************************************************************************
#
# HTMLview.mcc - HTMLview MUI Custom Class
# Copyright (C) 1997-2000 Allan Odgaard
# Copyright (C) 2005-2007 by HTMLview.mcc Open Source Team
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# HTMLview class Support Site:  http://www.sf.net/projects/htmlview-mcc/
#
# $Id: rev.h,v 1.4 2005/04/11 03:04:38 tactica Exp $
#
#***************************************************************************/

###########################################################################
# This makefile is a very generic one. It tries to identify both, the host
# and the target operating system for which HTMLview should be compiled.
# However, this auto-detection can be easily overridden by directly
# specifying an option on the commandline while calling 'make'.
#
# Example:
#
# # to explicitly compile for AmigaOS3
# > make OS=os3
#
# # to compile for AmigaOS4 but with debugging
# > make OS=os4 DEBUG=
#

#############################################
# find out the HOST operating system
# on which this makefile is run
ifndef (HOST)
  HOST = $(shell uname)
  ifeq ($(HOST), AmigaOS)
    ifeq ($(shell uname -m), powerpc)
      HOST = AmigaOS4
    endif
    ifeq ($(shell uname -m), ppc)
      HOST = AmigaOS4
    endif
  endif
endif

#############################################
# now we find out the target OS for
# which we are going to compile HTMLview in case
# the caller didn't yet define OS himself
ifndef (OS)
  ifeq ($(HOST), AmigaOS4)
    OS = os4
  else
  ifeq ($(HOST), AmigaOS)
    OS = os3
  else
  ifeq ($(HOST), MorphOS)
    OS = mos
  else
  ifeq ($(HOST), Aros)
    OS = aros
  else
    OS = os4
  endif
  endif
  endif
  endif
endif

#############################################
# define common commands we use in this
# makefile. Please note that each of them
# might be overridden on the commandline.

# common commands
FLEX  = flex
FC    = flexcat
EXPR  = expr
CHMOD = chmod
SED   = sed
DATE  = date
RM    = rm -f
RMDIR = rm -rf
MKDIR = mkdir

# override commands for native builds
ifeq ($(HOST), AmigaOS4)
  # AmigaOS4 host
  RM    = delete force
  RMDIR = delete force all
  MKDIR = makedir
  DATE  = gdate
else
ifeq ($(HOST), AmigaOS)
  # AmigaOS3 host
  RM    = delete force
  RMDIR = delete force all
  MKDIR = makedir
else
ifeq ($(HOST), MorphOS)
  # MorphOS host
  RM    = delete force
  RMDIR = delete force all
  MKDIR = makedir
endif
endif
endif

###########################################################################
# CPU and DEBUG can be defined outside, defaults to above
# using e.g. "make DEBUG= CPU=-mcpu=603e" produces optimized non-debug
# PPC-603e version
#
# OPTFLAGS are disabled by DEBUG normally!
#
# ignored warnings are:
# none - because we want to compile with -Wall all the time

# Common Directories
OBJDIR = .obj_$(OS)
BINDIR = bin_$(OS)
LOCALE = locale

# target definition
MCPTARGET = $(BINDIR)/HTMLview.mcp
PREFTARGET= $(BINDIR)/HTMLview-Prefs

# Common compiler/linker flags
WARN     = -W -Wall -Wwrite-strings
OPTFLAGS = -O3 -fomit-frame-pointer -funroll-loops
DEBUG    = -DDEBUG -O0
DEBUGSYM = -gstabs
CFLAGS   = -I. -I../mcc -I../include $(CPU) $(WARN) $(OPTFLAGS) \
           $(DEBUG) $(DEBUGSYM) -c
LDFLAGS  = $(CPU) $(DEBUGSYM)
LDLIBS   = -lm

# different options per target OS
ifeq ($(OS), os4)

  ##############################
  # AmigaOS4

  # Compiler/link/strip commands
  CC      = ppc-amigaos-gcc
  STRIP   = ppc-amigaos-strip
  OBJDUMP = ppc-amigaos-objdump

  # Compiler/Linker flags
  CRT      = newlib
  CPU      = -mcpu=powerpc
  CFLAGS   += -mcrt=$(CRT) -D__USE_INLINE__ -D__NEW_TIMEVAL_DEFINITION_USED__ \
              -DNO_PPCINLINE_STDARG -DUSEHOTKEY -DUSEBETTERSTRING -Wa,-mregnames
  LDFLAGS  += -mcrt=$(CRT)

  # additional object files required
  M68KSTUBS = $(OBJDIR)/mccclass_68k.o

else
ifeq ($(OS), os3)

  ##############################
  # AmigaOS3

  # Compiler/link/strip commands
  CC      = m68k-amigaos-gcc
  STRIP   = m68k-amigaos-strip
  OBJDUMP = m68k-amigaos-objdump

  # Compiler/Linker flags
  CPU	    = -m68020-60 -msoft-float
  CFLAGS  += -noixemul -DNO_INLINE_STDARG -DUSEHOTKEY -DUSEBETTERSTRING
  LDFLAGS += -noixemul
  LDLIBS  += -ldebug -lmui

else
ifeq ($(OS), mos)

  ##############################
  # MorphOS

  # Compiler/link/strip commands
  CC      = ppc-morphos-gcc
  STRIP   = ppc-morphos-strip
  OBJDUMP = ppc-morphos-objdump

  # Compiler/Linker flags
  CPU     = -mcpu=powerpc
  CFLAGS  += -noixemul -DNO_PPCINLINE_STDARG
  LDFLAGS += -noixemul
  LDLIBS  += -ldebug

else
ifeq ($(OS), aros)

  ##############################
  # AROS

  # Compiler/link/strip commands
  CC      = i686-aros-gcc
  STRIP   = i686-aros-strip
  OBJDUMP = i686-aros-objdump

  # Compiler/Linker flags
  CPU     =

endif
endif
endif
endif

###########################################################################
# Here starts all stuff that is common for all target platforms and
# hosts.

MCPOBJS = $(OBJDIR)/library.o \
          $(OBJDIR)/locale.o \
          $(OBJDIR)/CreatePrefsGroup.o \
          $(OBJDIR)/Dispatcher.o \
          $(OBJDIR)/pic.o \
          $(OBJDIR)/Debug.o

PREFOBJS= $(OBJDIR)/locale.o \
          $(OBJDIR)/CreatePrefsGroup.o \
          $(OBJDIR)/Dispatcher.o \
          $(OBJDIR)/Preference.o \
          $(OBJDIR)/pic.o \
          $(OBJDIR)/Debug.o

ifeq ($(OS), os3)
MCPOBJS  += $(OBJDIR)/vastubs.o
PREFOBJS += $(OBJDIR)/vastubs.o
endif

# available catalog translations
CATALOGS = $(LOCALE)/czech.catalog \
           $(LOCALE)/italian.catalog \
           $(LOCALE)/french.catalog \
           $(LOCALE)/polish.catalog \
           $(LOCALE)/swedish.catalog

#

all: $(BINDIR) $(OBJDIR) locale.c $(MCPTARGET) $(PREFTARGET)

# make the object directories
$(OBJDIR):
	@echo "  MKDIR $@"
	@$(MKDIR) $(OBJDIR)

# make the binary directories
$(BINDIR):
	@echo "  MKDIR $@"
	@$(MKDIR) $(BINDIR)

#

$(OBJDIR)/%.o: %.c
	@echo "  CC    $<"
	@$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/mccclass_68k.o: ../include/mccclass_68k.c
	@echo "  CC    $<"
	@$(CC) $(CFLAGS) $< -o $@

#

$(MCPTARGET): $(M68KSTUBS) $(MCPOBJS)
	@echo "  LD    $@"
	@$(CC) -nostartfiles $(LDFLAGS) -o $@.debug $(MCPOBJS) $(M68KSTUBS) $(LDLIBS) -Wl,-Map,$@.map
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

$(PREFTARGET): $(PREFOBJS) $(OBJDIR)/Preference.o
	@echo "  LD    $@"
	@$(CC) $(LDFLAGS) -o $@.debug $(PREFOBJS) $(LDLIBS) -Wl,-Map,$@.map
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

$(OBJDIR)/library.o: library.c ../include/mccinit.c \
  ../mcc/HTMLview_mcc.h private.h icon.bh HTMLview_mcp.h rev.h

locale.h: locale.c
locale.c: locale/HTMLview_mcp.cd locale_c.sd locale_h.sd
	@echo "  FC    $@"
	@$(FC) locale/HTMLview_mcp.cd locale.h=locale_h.sd locale.c=locale_c.sd

## CATALOG GENERATION #################

$(LOCALE)/%.catalog: $(LOCALE)/%.ct
	@echo "  FC    $@"
	@$(FC) $(LOCALE)/HTMLview_mcp.cd $< CATALOG $@

.IGNORE: $(CATALOGS)

catalogs: $(CATALOGS)


#

.PHONY: clean
clean:
	-$(RM) $(MCPTARGET) $(MCPTARGET).debug $(MCPTARGET).map
	-$(RM) $(PREFTARGET) $(PREFTARGET).debug $(PREFTARGET).map
	-$(RM) $(MCPOBJS) $(PREFOBJS) $(M68KSTUBS)

.PHONY: distclean
distclean: clean
	-$(RM) locale.?
	-$(RMDIR) $(OBJDIR)
	-$(RMDIR) $(BINDIR)
