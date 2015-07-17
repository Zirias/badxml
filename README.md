#   badxml
### A really BAD (but tiny) xml parser

This is a one-file library for parsing text to a very simple object model,
recognizing the most basic XML syntax. The only things recognized are:

  - Elements (tags with names)
  - Attributes as long as their value is enclosed in double quotes
  - The content of elements (anything between opening and closing tag as text)

It might be useful in cases where you don't need a real XML parser and pulling
in a big library seems not feasible.

### Things NOT supported
(This list is incomplete)

  - Character encodings
  - Doctypes, XSD and the like
  - XML entities
  - CDATA sections
  - namespaces

### Things maybe added later

  - A line counter so the line where a parsing error occured can be retreived

### Note on building / using

The build system here will build libraries (static and dynamic, for windows a
DLL and an import library). This is probably overkill for most cases, but why
not when I had a build system lying around doing this -- this might become a
separate project later.

Typical usage would probably be to just include the files `badxml.c` and
`badxml.h` in your own source tree and maybe adapt the `#include` in
`badxml.c` to your source tree layout.
