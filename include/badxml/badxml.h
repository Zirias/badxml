#ifndef ZRS_BADXML_H
#define ZRS_BADXML_H

/* LICENSE

Copyright (c) 2015, Felix Palmen All rights reserved. This applies to all
files originating from https://github.com/Zirias/badxml/

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdio.h>

typedef enum xmlError
{
    /* no error, no info */
    XML_SUCCESS,

    /* unexpected end of input, no info */
    XML_EOF,

    /* second root element found,
     * position in xmlDocLine() and xmlDocColumn() */
    XML_SECONDROOT,

    /* Tag without name found,
     * position in xmlDocLine() and xmlDocColumn() */
    XML_UNNAMEDTAG,

    /* Attribute without name found,
     * position in xmlDocLine() and xmlDocColumn() */
    XML_UNNAMEDATTR,

    /* A closing tag doesn't match the tag opened,
     * name of opened tag in xmlDocErrInfo(),
     * position of closing tag in xmlDocLine() and xmlDocColumn() */
    XML_UNMATCHEDCLOSE,

    /* A closing tag was found before any tag was opened,
     * name of closing tag in xmlDocErrInfo(),
     * position in xmlDocLine() and xmlDocColumn() */
    XML_CLOSEWOOPEN,

    /* Unexpected character found while parsing,
     * offending character in xmlDocErrChar(),
     * position in xmlDocLine() and xmlDocColumn() */
    XML_UNEXPECTED
} XmlError;

/* represents the whole XML document */
typedef struct XmlDoc XmlDoc;

/* represents an attribute of an XML element */
typedef struct XmlAttribute XmlAttribute;

/* represents an XML element (tag) */
typedef struct XmlElement XmlElement;


/* parse xmlText as XML, return as XML document */
XmlDoc *parseDoc(const char *xmlText);

/* get result of parsing (XML_SUCCESS or an error code */
XmlError xmlDocError(const XmlDoc *doc);

/* get string with info about the error, or 0 if no string info available */
const char *xmlDocErrInfo(const XmlDoc *doc);

/* get character triggering the error, or 0 */
char xmlDocErrChar(const XmlDoc *doc);

/* get line position of an error */
long xmlDocLine(const XmlDoc *doc);

/* get column position of an error */
long xmlDocColumn(const XmlDoc *doc);

/* get root element of the document, or 0 if there was an error */
XmlElement *rootElement(const XmlDoc *doc);

/* format error message and print to file
 * (use stderr for printing to console) */
void xmlDocPerror(const XmlDoc *doc, FILE *file, const char *fmt, ...);

/* free all resources hold by doc,
 * must be called when finished with the XML data. */
void freeDoc(XmlDoc *doc);

/* navigation in the document */
XmlElement *firstChild(const XmlElement *element);
XmlElement *lastChild(const XmlElement *element);
XmlElement *nextSibling(const XmlElement *element);
XmlElement *parentElement(const XmlElement *element);

/* find first element matching the arguments to this function,
 * starting at given element, doing a depth-first search.
 *
 * if no tagname is given, only attributes matter.
 * if no attname is given, only the tagname matters.
 * if attname is given without attval, the presence of the attribute is a match
 *
 * might return the element itself if it matches. */
XmlElement *findMatching(const XmlElement *element,
        const char *tagname, const char *attname, const char *attval);

/* navigate attribute list of an element */
XmlAttribute *firstAttribute(const XmlElement *element);
XmlAttribute *nextAttribute(const XmlAttribute *attribute);
XmlElement *attributeElement(const XmlAttribute *attribute);

/* getters. The elementContent() gets the whole text (including tags) between
 * opening and closing tag of the element. */
const char *tagName(const XmlElement *element);
const char *elementContent(const XmlElement *element);
const char *attributeName(const XmlAttribute *attribute);
const char *attributeValue(const XmlAttribute *attribute);

#ifdef BADXML_DEBUG
/* for debugging: dump document structure to file (typically stderr) */
void dumpDoc(const XmlDoc *doc, FILE *file);
#else
#define dumpDoc(doc, file)
#endif

#endif

