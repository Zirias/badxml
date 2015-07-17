define LIBRULES
$(T)_V := $$($(T)_VMAJOR).$$($(T)_VMINOR).$$($(T)_VREVISION)

LIBARCHIVES += $$(LIBDIR)$$(PSEP)lib$(T).a
LIBRARIES += $$(SODIR)$$(PSEP)lib$(T).so.$$($(T)_V)

$$(LIBDIR)$$(PSEP)lib$(T).a: $$($(T)_SOURCES:.c=.o) | libdir
	$$(VAR)
	$$(VR)$$(AR) rcs $$@ $$^

$$(SODIR)$$(PSEP)lib$(T).so.$$($(T)_V): \
    $$($(T)_SOURCES:.c=_s.o) | libdir
	$$(VLD)
	$$(VR)$$(CC) -shared \
	    -Wl,-soname,lib$(T).so.$$($(T)_VMAJOR) \
	    -o$$@ $$(LDFLAGS) $$^
endef

