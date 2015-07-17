define LIBRULES
LIBARCHIVES += $$(LIBDIR)$$(PSEP)lib$(T).a
LIBRARIES += $$(SODIR)$$(PSEP)$(T)-$$($(T)_VMAJOR).dll

$$(LIBDIR)$$(PSEP)lib$(T).a $$(LIBDIR)$$(PSEP)$(T).def: \
    $$(SODIR)$$(PSEP)$(T)-$$($(T)_VMAJOR).dll

$$(SODIR)$$(PSEP)$(T)-$$($(T)_VMAJOR).dll: \
    $$($(T)_SOURCES:.c=_s.o) | bindir libdir
	$$(VLD)
	$$(VR)$$(CC) -shared \
	    -Wl,--out-implib,$$(LIBDIR)$$(PSEP)lib$(T).a \
	    -Wl,--output-def,$$(LIBDIR)$$(PSEP)$(T).def \
	    -o$$@ $$(LDFLAGS) $$^
endef

