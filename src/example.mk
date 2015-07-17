P := src
T := example

example_SOURCES := example.c
example_LIBS := $(LIBDIR)$(PSEP)libbadxml.a

$(eval $(BINRULES))

