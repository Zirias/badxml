define LIBRULES
$(T)_SOURCES_FULL := $(addprefix $(P)$$(PSEP),$$($(T)_SOURCES))
SOURCES += $$($(T)_SOURCES_FULL)
CLEAN += $$($(T)_SOURCES_FULL:.c=_s.o)


endef

include mk$(PSEP)platform$(PSEP)$(PLATFORM)$(PSEP)lib.mk

define LIBCOMMONRULES



$(P)$$(PSEP)%.d: $(P)$$(PSEP)%.c Makefile conf.mk
	$$(VDEP)
	$$(VR)$$(CCDEP) -MT"$@ $(@:.d=.o) $(@:.d=_s.o)" -MF$$@ \
	    $$(CFLAGS) $$(INCLUDES) $$<

ifneq ($$(MAKECMDGOALS),clean)
ifneq ($$(MAKECMDGOALS),distclean)
-include $$($(T)_SOURCES_FULL:.c=.d)
endif
endif

$(P)$$(PSEP)%.o: $(P)$$(PSEP)%.c Makefile conf.mk $(P)$$(PSEP)$(T).mk
	$$(VCC)
	$$(VR)$$(CC) -o$$@ -c $$(CFLAGS) $$($(T)_DEFINES) $$(INCLUDES) $$<

$(P)$$(PSEP)%_s.o: $(P)$$(PSEP)%.c Makefile conf.mk $(P)$$(PSEP)$(T).mk
	$$(VCC)
	$$(VR)$$(CC) -o$$@ -c $$(lib_CFLAGS) $$($(T)_DEFINES) $$(CFLAGS) \
		$$(INCLUDES) $$<
endef
LIBRULES += $(LIBPLATFORMRULES)$(LIBCOMMONRULES)
