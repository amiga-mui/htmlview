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
# and the target operating system for which YAM should be compiled.
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
# which we are going to compile YAM in case
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
MCCTARGET = $(BINDIR)/HTMLview.mcc
TESTTARGET= $(BINDIR)/HTMLview-Test

# Common compiler/linker flags
WARN     = -W -Wall -Wwrite-strings
OPTFLAGS = -O3 -fomit-frame-pointer -funroll-loops
DEBUG    = -DDEBUG -O0
CFLAGS   = -I. -I../mcp -I../include $(CPU) $(WARN) $(OPTFLAGS) \
           $(DEBUG) -g -c
LDFLAGS  = $(CPU)
LDLIBS   = -lm

# different options per target OS
ifeq ($(OS), os4)

  ##############################
  # AmigaOS4

  # Compiler/link/strip commands
  CC      = ppc-amigaos-gcc
  CPP     = ppc-amigaos-g++
  STRIP   = ppc-amigaos-strip
  OBJDUMP = ppc-amigaos-objdump

  # Compiler/Linker flags
  CRT      = newlib
  CPU      = -mcpu=604e -mmultiple -Wa,-mregnames
  CFLAGS   += -mcrt=$(CRT) -D__USE_INLINE__ -D__NEW_TIMEVAL_DEFINITION_USED__ -DNO_PPCINLINE_STDARG
  LDFLAGS  += -mcrt=$(CRT)

  # additional object files required
  M68KSTUBS = $(OBJDIR)/mccclass_68k.o

else
ifeq ($(OS), os3)

  ##############################
  # AmigaOS3

  # Compiler/link/strip commands
  CC      = m68k-amigaos-gcc
  CPP     = m68k-amigaos-g++
  STRIP   = m68k-amigaos-strip
  OBJDUMP = m68k-amigaos-objdump

  # Compiler/Linker flags
  CPU	    = -m68020-60 -msoft-float
  CFLAGS  += -noixemul -DNO_INLINE_STDARG
  LDFLAGS += -noixemul
  LDLIBS  += -ldebug -lmui

else
ifeq ($(OS), mos)

  ##############################
  # MorphOS

  # Compiler/link/strip commands
  CC      = ppc-morphos-gcc
  CPP     = ppc-morphos-g++
  STRIP   = ppc-morphos-strip
  OBJDUMP = ppc-morphos-objdump

  # Compiler/Linker flags
  CPU     = -mcpu=750 -mmultiple
  CFLAGS  += -noixemul -DNO_PPCINLINE_STDARG
  LDFLAGS += -noixemul
  LDLIBS  += -ldebug

else
ifeq ($(OS), aros)

  ##############################
  # AROS

  # Compiler/link/strip commands
  CC      = i686-aros-gcc
  CPP     = i686-aros-g++
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

MCCOBJS = \
          $(OBJDIR)/library.o \
          $(OBJDIR)/constructors.o \
          $(OBJDIR)/classes/AClass.o \
          $(OBJDIR)/classes/AreaClass.o \
          $(OBJDIR)/classes/AttrClass.o \
          $(OBJDIR)/classes/BackFillClass.o \
          $(OBJDIR)/classes/BaseClass.o \
          $(OBJDIR)/classes/BlockquoteClass.o \
          $(OBJDIR)/classes/BodyClass.o \
          $(OBJDIR)/classes/BrClass.o \
          $(OBJDIR)/classes/CommentClass.o \
          $(OBJDIR)/classes/DDClass.o \
          $(OBJDIR)/classes/DLClass.o \
          $(OBJDIR)/classes/DTClass.o \
          $(OBJDIR)/classes/DummyClass.o \
          $(OBJDIR)/classes/FontClass.o \
          $(OBJDIR)/classes/FontStyleClass.o \
          $(OBJDIR)/classes/FormClass.o \
          $(OBJDIR)/classes/FrameClass.o \
          $(OBJDIR)/classes/FramesetClass.o \
          $(OBJDIR)/classes/HRClass.o \
          $(OBJDIR)/classes/HostClass.o \
          $(OBJDIR)/classes/ImgClass.o \
          $(OBJDIR)/classes/InputClass.o \
          $(OBJDIR)/classes/IsIndexClass.o \
          $(OBJDIR)/classes/LIClass.o \
          $(OBJDIR)/classes/MapClass.o \
          $(OBJDIR)/classes/MetaClass.o \
          $(OBJDIR)/classes/NoFramesClass.o \
          $(OBJDIR)/classes/OLClass.o \
          $(OBJDIR)/classes/OptionClass.o \
          $(OBJDIR)/classes/PClass.o \
          $(OBJDIR)/classes/ScriptClass.o \
          $(OBJDIR)/classes/SelectClass.o \
          $(OBJDIR)/classes/SuperClass.o \
          $(OBJDIR)/classes/TDClass.o \
          $(OBJDIR)/classes/THClass.o \
          $(OBJDIR)/classes/TRClass.o \
          $(OBJDIR)/classes/TableClass.o \
          $(OBJDIR)/classes/TextAreaClass.o \
          $(OBJDIR)/classes/TextClass.o \
          $(OBJDIR)/classes/TitleClass.o \
          $(OBJDIR)/classes/TreeClass.o \
          $(OBJDIR)/classes/ULClass.o \
          $(OBJDIR)/AllocateColours.o \
          $(OBJDIR)/Animation.o \
          $(OBJDIR)/AppendGadget.o \
          $(OBJDIR)/Classes.o \
          $(OBJDIR)/Colours.o \
          $(OBJDIR)/Config.o \
          $(OBJDIR)/Dispatcher.o \
          $(OBJDIR)/Entities.o \
          $(OBJDIR)/Forms.o \
          $(OBJDIR)/GetSetAttrs.o \
          $(OBJDIR)/HandleMUIEvent.o \
          $(OBJDIR)/IM_ColourManager.o \
          $(OBJDIR)/IM_Dither.o \
          $(OBJDIR)/IM_Output.o \
          $(OBJDIR)/IM_Render.o \
          $(OBJDIR)/IM_Scale.o \
          $(OBJDIR)/ImageManager.o \
          $(OBJDIR)/InitCleanup.o \
          $(OBJDIR)/Layout.o \
          $(OBJDIR)/Map.o \
          $(OBJDIR)/Mark.o \
          $(OBJDIR)/MinMax.o \
          $(OBJDIR)/ParseMessage.o \
          $(OBJDIR)/ParseThread.o \
          $(OBJDIR)/Render.o \
          $(OBJDIR)/ScanArgs.o \
          $(OBJDIR)/ScrollGroup.o \
          $(OBJDIR)/SharedData.o \
          $(OBJDIR)/TagInfo.o \
          $(OBJDIR)/TernaryTrees.o \
          $(OBJDIR)/URLHandling.o \
          $(OBJDIR)/Debug.o

TESTOBJS= $(OBJDIR)/HTMLview-Test.o \
          $(OBJDIR)/classes/AClass.o \
          $(OBJDIR)/classes/AreaClass.o \
          $(OBJDIR)/classes/AttrClass.o \
          $(OBJDIR)/classes/BackFillClass.o \
          $(OBJDIR)/classes/BaseClass.o \
          $(OBJDIR)/classes/BlockquoteClass.o \
          $(OBJDIR)/classes/BodyClass.o \
          $(OBJDIR)/classes/BrClass.o \
          $(OBJDIR)/classes/CommentClass.o \
          $(OBJDIR)/classes/DDClass.o \
          $(OBJDIR)/classes/DLClass.o \
          $(OBJDIR)/classes/DTClass.o \
          $(OBJDIR)/classes/DummyClass.o \
          $(OBJDIR)/classes/FontClass.o \
          $(OBJDIR)/classes/FontStyleClass.o \
          $(OBJDIR)/classes/FormClass.o \
          $(OBJDIR)/classes/FrameClass.o \
          $(OBJDIR)/classes/FramesetClass.o \
          $(OBJDIR)/classes/HRClass.o \
          $(OBJDIR)/classes/HostClass.o \
          $(OBJDIR)/classes/ImgClass.o \
          $(OBJDIR)/classes/InputClass.o \
          $(OBJDIR)/classes/IsIndexClass.o \
          $(OBJDIR)/classes/LIClass.o \
          $(OBJDIR)/classes/MapClass.o \
          $(OBJDIR)/classes/MetaClass.o \
          $(OBJDIR)/classes/NoFramesClass.o \
          $(OBJDIR)/classes/OLClass.o \
          $(OBJDIR)/classes/OptionClass.o \
          $(OBJDIR)/classes/PClass.o \
          $(OBJDIR)/classes/ScriptClass.o \
          $(OBJDIR)/classes/SelectClass.o \
          $(OBJDIR)/classes/SuperClass.o \
          $(OBJDIR)/classes/TDClass.o \
          $(OBJDIR)/classes/THClass.o \
          $(OBJDIR)/classes/TRClass.o \
          $(OBJDIR)/classes/TableClass.o \
          $(OBJDIR)/classes/TextAreaClass.o \
          $(OBJDIR)/classes/TextClass.o \
          $(OBJDIR)/classes/TitleClass.o \
          $(OBJDIR)/classes/TreeClass.o \
          $(OBJDIR)/classes/ULClass.o \
          $(OBJDIR)/AllocateColours.o \
          $(OBJDIR)/Animation.o \
          $(OBJDIR)/AppendGadget.o \
          $(OBJDIR)/Classes.o \
          $(OBJDIR)/Colours.o \
          $(OBJDIR)/Config.o \
          $(OBJDIR)/Dispatcher.o \
          $(OBJDIR)/Entities.o \
          $(OBJDIR)/Forms.o \
          $(OBJDIR)/GetSetAttrs.o \
          $(OBJDIR)/HandleMUIEvent.o \
          $(OBJDIR)/IM_ColourManager.o \
          $(OBJDIR)/IM_Dither.o \
          $(OBJDIR)/IM_Output.o \
          $(OBJDIR)/IM_Render.o \
          $(OBJDIR)/IM_Scale.o \
          $(OBJDIR)/ImageManager.o \
          $(OBJDIR)/InitCleanup.o \
          $(OBJDIR)/Layout.o \
          $(OBJDIR)/Map.o \
          $(OBJDIR)/Mark.o \
          $(OBJDIR)/MinMax.o \
          $(OBJDIR)/ParseMessage.o \
          $(OBJDIR)/ParseThread.o \
          $(OBJDIR)/Render.o \
          $(OBJDIR)/ScanArgs.o \
          $(OBJDIR)/ScrollGroup.o \
          $(OBJDIR)/SharedData.o \
          $(OBJDIR)/TagInfo.o \
          $(OBJDIR)/TernaryTrees.o \
          $(OBJDIR)/URLHandling.o \
          $(OBJDIR)/Debug.o

# available catalog translations
CATALOGS = $(LOCALE)/deutsch.catalog

#

all: $(BINDIR) $(OBJDIR) $(MCCTARGET) $(TESTTARGET)

# make the object directories
$(OBJDIR):
	@printf '\033[33mGenerating $@ directory\033[0m\n'
	@$(MKDIR) $(OBJDIR)
	@$(MKDIR) $(OBJDIR)/classes

# make the binary directories
$(BINDIR):
	@printf '\033[33mGenerating $@ directory\033[0m\n'
	@$(MKDIR) $(BINDIR)

#

$(OBJDIR)/%.o: %.c
	@printf '\033[32mCompiling $<\033[0m\n'
	@$(CPP) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o: %.cpp
	@printf '\033[32mCompiling $<\033[0m\n'
	@$(CPP) $(CFLAGS) $< -o $@

$(OBJDIR)/mccclass_68k.o: ../include/mccclass_68k.c
	@printf '\033[32mCompiling $<\033[0m\n'
	@$(CC) $(CFLAGS) $< -o $@

$(LOCALE)/%.catalog: $(LOCALE)/%.ct
	@printf '\033[33mGenerating $@\033[0m\n'
	@$(FC) $(LOCALE)/HTMLview_mcc.cd $< CATALOG $@

#

$(MCCTARGET): $(M68KSTUBS) $(MCCOBJS)
	@printf '\033[32mLinking \033[1m$@\033[0m\n'
	@$(CPP) -nostartfiles $(LDFLAGS) -o $@.debug $(MCCOBJS) $(M68KSTUBS) $(LDLIBS)
	@$(STRIP) -R .comment -S -o $@ $@.debug

$(TESTTARGET): $(TESTOBJS) $(OBJDIR)/HTMLview-Test.o
	@printf '\033[32mLinking \033[1m$@\033[0m\n'
	@$(CPP) $(LDFLAGS) -o $@.debug $(TESTOBJS) $(LDLIBS)
	@$(STRIP) -R .comment -S -o $@ $@.debug

$(OBJDIR)/library.o: library.c ../include/mccheader.c \
  HTMLview_mcc.h private.h rev.h

locale.h: locale.c
locale.c: locale/HTMLview_mcc.cd locale_c.sd locale_h.sd
	@printf '\033[33mGenerating locale file $@...\033[0m\n'
	@$(FC) locale/HTMLview_mcc.cd locale.h=locale_h.sd locale.c=locale_c.sd

## CATALOG GENERATION #################

$(LOCALE)/%.catalog: $(LOCALE)/%.ct
	@printf '\033[33mGenerating $@\033[0m\n'
	@$(FC) $(LOCALE)/HTMLview_mcc.cd $< CATALOG $@

.IGNORE: $(CATALOGS)

catalogs: $(CATALOGS)


#

.PHONY: clean
clean:
	-$(RM) $(MCCTARGET) $(MCCTARGET).debug $(TESTTARGET) $(TESTTARGET).debug $(MCCOBJS) $(TESTOBJS) $(M68KSTUBS)

.PHONY: distclean
distclean: clean
	-$(RM) locale.?
	-$(RMDIR) $(OBJDIR)
	-$(RMDIR) $(BINDIR)
