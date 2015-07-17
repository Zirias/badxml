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
    XML_SUCCESS,
    XML_EOF,
    XML_SECONDROOT,
    XML_UNNAMEDTAG,
    XML_UNNAMEDATTR,
    XML_UNMATCHEDCLOSE,
    XML_CLOSEWOOPEN,
    XML_UNEXPECTED
} XmlError;

struct xmlDoc;
typedef struct xmlDoc XmlDoc;

struct xmlAttribute;
typedef struct xmlAttribute XmlAttribute;

struct xmlElement;
typedef struct xmlElement XmlElement;

XmlDoc *parseDoc(const char *xmlText);
XmlError xmlDocError(const XmlDoc *doc);
const char *xmlDocErrInfo(const XmlDoc *doc);
char xmlDocErrChar(const XmlDoc *doc);
long xmlDocLine(const XmlDoc *doc);
long xmlDocColumn(const XmlDoc *doc);
const XmlElement *rootElement(const XmlDoc *doc);
void xmlDocPerror(const XmlDoc *doc, FILE *file, const char *fmt, ...);
void freeDoc(XmlDoc *doc);

const XmlElement *firstChild(const XmlElement *element);
const XmlElement *lastChild(const XmlElement *element);
const XmlElement *nextSibling(const XmlElement *element);
const XmlElement *parentElement(const XmlElement *element);
const XmlElement *findMatching(const XmlElement *element,
        const char *tagname, const char *attname, const char *attval);

const XmlAttribute *firstAttribute(const XmlElement *element);
const XmlAttribute *nextAttribute(const XmlAttribute *attribute);
const XmlElement *attributeElement(const XmlAttribute *attribute);

const char *tagName(const XmlElement *element);
const char *elementContent(const XmlElement *element);
const char *attributeName(const XmlAttribute *attribute);
const char *attributeValue(const XmlAttribute *attribute);

#ifdef BADXML_DEBUG
void dumpDoc(const XmlDoc *doc, FILE *file);
#else
#define dumpDoc(doc, file)
#endif

#endif

