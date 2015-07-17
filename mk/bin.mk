define BINRULES
BINARIES += $$(BINDIR)$$(PSEP)$(T)$$(EXE)

$$(BINDIR)$$(PSEP)$(T)$$(EXE): $$($(T)_SOURCES:.c=.o) $$($(T)_LIBS) | bindir
	$$(VLD)
	$$(VR)$$(CC) -o$$@ $$(LDFLAGS) $$^ $$($(T)_LIBS)

$(P)%.d: $(P)%.c Makefile conf.mk
	$$(VDEP)
	$$(VR)$$(CCDEP) -MT"$@ $(@:.d=.o)" -MF$$@ \
	    $$(CFLAGS) $$(INCLUDES) $$<

ifneq ($$(MAKECMDGOALS),clean)
ifneq ($$(MAKECMDGOALS),distclean)
-include $$($(T)_SOURCES:.c=.d)
endif
endif

$(P)%.o: $(P)%.c Makefile conf.mk $(P)$(T).mk
	$$(VCC)
	$$(VR)$$(CC) -o$$@ -c $$(CFLAGS) $$($(T)_DEFINES) \
	    $$(INCLUDES) $$($(T)_INCLUDES) $$<
endef
