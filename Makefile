include defs.mk

# default configuration
CC := gcc
DEBUG := 0
GCC32 := 0
USELTO := 1

# read local configuration
-include defaults.mk

CFLAGS += -fvisibility=hidden -std=c89 -fstrict-aliasing \
	  -Wall -Wextra -pedantic -Wformat=2 -Winit-self -Wshadow \
	  -Wbad-function-cast -Wwrite-strings -Wconversion

BINDIR := bin
LIBDIR := lib

VTAGS :=
V := 0

prefix := /usr/local
bindir := $(prefix)/bin
libbasedir := $(prefix)/lib
libdir := $(libbasedir)/badxml
includebasedir := $(prefix)/include
includedir := $(includebasedir)/badxml
docbasedir := $(prefix)/share/doc
docdir := $(docbasedir)/badxml

INSTALL := install

ifneq ($(GCC32),0)
CC := gcc -m32
CFLAGS += -DGCC32BIT
VTAGS += [32bit]
endif

ifeq ($(DEBUG), 0)
VTAGS += [release]
CFLAGS += -g0 -O3
ifneq ($(USELTO),0)
CFLAGS += -flto
LDFLAGS += -flto
AR := gcc-ar
else
AR := ar
endif
else
CFLAGS += -DBADXML_DEBUG -g3 -O0
VTAGS += [debug]
endif

ifeq ($(PLATFORM),win32)
SODIR := $(BINDIR)
else
SODIR := $(LIBDIR)
endif

CCDEP := $(CC) -MM

ifeq ($(V),1)
VCC :=
VDEP :=
VLD :=
VAR :=
VCCLD :=
VGEN :=
VGENT :=
VR :=
else
VCC = @echo $(EQT)   [CC]   $@$(EQT)
VDEP = @echo $(EQT)   [DEP]  $@$(EQT)
VLD = @echo $(EQT)   [LD]   $@$(EQT)
VAR = @echo $(EQT)   [AR]   $@$(EQT)
VCCLD = @echo $(EQT)   [CCLD] $@$(EQT)
VGEN = @echo $(EQT)   [GEN]  $@$(EQT)
VGENT = @echo $(EQT)   [GEN]  $@: $(VTAGS)$(EQT)
VR := @
endif

INCLUDES := -Iinclude

all: libs

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
conf.mk:
	$(VGENT)
	$(VR)echo $(EQT)C_CC := $(CC)$(EQT) >conf.mk
	$(VR)echo $(EQT)C_DEBUG := $(DEBUG)$(EQT) >>conf.mk
	$(VR)echo $(EQT)C_GCC32 := $(GCC32)$(EQT) >>conf.mk
	$(VR)echo $(EQT)C_USELTO := $(USELTO)$(EQT) >>conf.mk

-include conf.mk

ifneq ($(strip $(C_CC))_$(strip $(C_DEBUG))_$(strip $(C_GCC32))_$(strip $(C_USELTO)),$(strip $(CC))_$(strip $(DEBUG))_$(strip $(GCC32))_$(strip $(USELTO)))
.PHONY: conf.mk
endif
endif
endif

include mk$(PSEP)lib.mk
include mk$(PSEP)bin.mk

include src$(PSEP)src.mk

bins: $(BINARIES)

libs: $(LIBRARIES) $(LIBARCHIVES)

clean:
	$(RMF) $(SOURCES:.c=.o) $(CMDQUIET)
	$(RMF) $(SOURCES:.c=.d) $(CMDQUIET)
ifneq ($(strip $(CLEAN)),)
	$(RMF) $(CLEAN) $(CMDQUIET)
endif

distclean: clean
	$(RMF) conf.mk $(CMDQUIET)
	$(RMFR) $(LIBDIR) $(CMDQUIET)
	$(RMFR) $(BINDIR) $(CMDQUIET)

strip: all
ifneq ($(strip $(BINARIES)),)
	strip --strip-all $(BINARIES)
endif
ifneq ($(strip $(LIBRARIES)),)
	strip --strip-unneeded $(LIBRARIES)
endif

ifeq ($(PLATFORM),posix)
install: strip
ifneq ($(strip $(BINARIES)),)
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(BINARIES) $(DESTDIR)$(bindir)
endif
ifneq ($(strip $(LIBRARIES)$(LIBARCHIVES)),)
	$(INSTALL) -d $(DESTDIR)$(libdir)
endif
ifneq ($(strip $(LIBRARIES)),)
	$(INSTALL) $(LIBRARIES) $(DESTDIR)$(libdir)
endif
ifneq ($(strip $(LIBARCHIVES)),)
	$(INSTALL) $(LIBARCHIVES) $(DESTDIR)$(libdir)
endif
	$(INSTALL) -d $(DESTDIR)$(includedir)
	$(INSTALL) -d $(DESTDIR)$(docdir)
	$(INSTALL) -m644 include/cwo/*.h $(DESTDIR)$(includedir)
ifneq ($(strip $(LIBRARIES)),)
	@echo
	@echo $(EQT)*** Attention ***$(EQT)
	@echo $(EQT) Dynamic libraries were installed on your system.$(EQT)
	@echo $(EQT) You should run /sbin/ldconfig to make them available.$(EQT)
endif
endif

$(BINDIR):
	$(VR)$(MDP) $(BINDIR) $(CMDQUIET)

$(LIBDIR):
	$(VR)$(MDP) $(LIBDIR) $(CMDQUIET)

bindir: $(BINDIR)

libdir: $(LIBDIR)

.PHONY: all bins libs bindir libdir strip clean distclean install
.SUFFIXES:

# vim: noet:si:ts=8:sts=8:sw=8
