#ifndef ZRS_BADXML_H
#define ZRS_BADXML_H

#include <stdio.h>

struct xmlDoc;
typedef struct xmlDoc XmlDoc;

struct xmlAttribute;
typedef struct xmlAttribute XmlAttribute;

struct xmlElement;
typedef struct xmlElement XmlElement;

XmlDoc *parseDoc(const char *xmlText);
void freeDoc(XmlDoc *doc);

const XmlElement *rootElement(const XmlDoc *doc);

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

