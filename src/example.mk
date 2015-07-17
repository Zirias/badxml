P := src$(PSEP)
T := example

example_SOURCES := $(P)example.c
example_LIBS := $(LIBDIR)$(PSEP)libbadxml.a

$(eval $(BINRULES))

