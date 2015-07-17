include mk$(PSEP)platform$(PSEP)$(PLATFORM)$(PSEP)lib.mk

define LIBRULES +=


SOURCES += $$($(T)_SOURCES)
CLEAN += $$($(T)_SOURCES:.c=_s.o)

$(P)%.d: $(P)%.c Makefile conf.mk
	$$(VDEP)
	$$(VR)$$(CCDEP) -MT"$@ $(@:.d=.o) $(@:.d=_s.o)" -MF$$@ \
	    $$(CFLAGS) $$(INCLUDES) $$<

ifneq ($$(MAKECMDGOALS),clean)
ifneq ($$(MAKECMDGOALS),distclean)
-include $$($(T)_SOURCES:.c=.d)
endif
endif

$(P)%.o: $(P)%.c Makefile conf.mk $(P)$(T).mk
	$$(VCC)
	$$(VR)$$(CC) -o$$@ -c $$(CFLAGS) $$($(T)_DEFINES) $$(INCLUDES) $$<

$(P)%_s.o: $(P)%.c Makefile conf.mk $(P)$(T).mk
	$$(VCC)
	$$(VR)$$(CC) -o$$@ -c $$(lib_CFLAGS) $$($(T)_DEFINES) $$(CFLAGS) \
		$$(INCLUDES) $$<
endef

