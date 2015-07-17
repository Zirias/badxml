P := src$(PSEP)badxml$(PSEP)
T := badxml

badxml_VMAJOR := 0
badxml_VMINOR := 0
badxml_VREVISION := 1
badxml_SOURCES := $(P)badxml.c

$(eval $(LIBRULES))

