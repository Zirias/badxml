#include <badxml/badxml.h>

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

struct xmlDoc
{
    XmlElement *root;
    const char *currLine;
    union {
	char c;
	char *s;
    } errInfo;
    XmlError err;
    long line;
    long col;
};

struct xmlAttribute
{
    char *name;
    char *value;
    XmlElement *parent;
    XmlAttribute *prev;
    XmlAttribute *next;
};

struct xmlElement
{
    char *name;
    char *value;
    XmlElement *parent;
    XmlElement *prev;
    XmlElement *next;
    XmlAttribute *attributes;
    XmlElement *children;
};

static char *
cloneString(const char *s)
{
    char *cpy = malloc(strlen(s)+1);
    strcpy(cpy, s);
    return cpy;
}

static void
skipWs(XmlDoc *doc, const char **pos)
{
    while (isspace(**pos))
    {
	if (**pos == '\n')
	{
	    ++(doc->line);
	    doc->currLine = ++(*pos);
	}
	else ++(*pos);
    }
}

static void
skipUntil(XmlDoc *doc, const char **pos, char endmark)
{
    while (**pos && **pos != endmark)
    {
	if (**pos == '\n')
	{
	    ++(doc->line);
	    doc->currLine = ++(*pos);
	}
	else ++(*pos);
    }
}

#define FAIL(x) \
    do { doc->err = (x); goto fail; } while(0)
#define FAILS(x, es) \
    do { doc->err = (x); doc->errInfo.s = (es); goto fail; } while (0)
#define FAILC(x, ec) \
    do { doc->err = (x); doc->errInfo.c = (ec); goto fail; } while (0)

static char *
readBareWord(const char **pos, char endmark)
{
    const char *start;
    char *word;

    start = *pos;

    if (!*start) return 0;
    while (!isspace(**pos) && **pos && **pos != endmark) ++(*pos);
    if (*pos == start) return 0;

    word = calloc(1, (size_t)(*pos - start) + 1);
    memcpy(word, start, (size_t)(*pos - start));
    return word;
}

static void
freeAttributeListStep(XmlAttribute *first, XmlAttribute *attribute)
{
    if (attribute->next != first) freeAttributeListStep(first, attribute->next);
    free(attribute->value);
    free(attribute->name);
    free(attribute);
}

static void
freeAttributeList(XmlAttribute *first)
{
    if (first) freeAttributeListStep(first, first);
}

static void freeElementList(XmlElement *first);
static void
freeElementListStep(XmlElement *first, XmlElement *element)
{
    freeElementList(element->children);
    freeAttributeList(element->attributes);
    if (element->next != first) freeElementListStep(first, element->next);
    free(element->value);
    free(element->name);
    free(element);
}

static void
freeElementList(XmlElement *first)
{
    if (first) freeElementListStep(first, first);
}

void
freeDoc(XmlDoc *doc)
{
    if (doc)
    {
	freeElementList(doc->root);
	if (doc->err == XML_UNMATCHEDCLOSE || doc->err == XML_CLOSEWOOPEN)
	{
	    free(doc->errInfo.s);
	}
	free(doc);
    }
}

static XmlAttribute *
parseAttribute(XmlDoc *doc, const char **xmlText, XmlElement *element)
{
    const char *startval;
    XmlAttribute *attribute = calloc(1, sizeof(XmlAttribute));
    attribute->next = attribute->prev = attribute;
    attribute->parent = element;

    attribute->name = readBareWord(xmlText, '=');
    if (!attribute->name) FAIL(XML_UNNAMEDATTR);
    if (!**xmlText) FAIL(XML_EOF);
    skipWs(doc, xmlText);
    if (**xmlText != '=') FAILC(XML_UNEXPECTED, **xmlText);
    ++(*xmlText);
    skipWs(doc, xmlText);
    if (**xmlText != '"') FAILC(XML_UNEXPECTED, **xmlText);
    ++(*xmlText);

    startval = *xmlText;
    skipUntil(doc, xmlText, '"');
    if (!**xmlText) FAIL(XML_EOF);
    attribute->value = calloc(1, (size_t)(*xmlText - startval) + 1);
    memcpy(attribute->value, startval, (size_t)(*xmlText - startval));
    ++(*xmlText);
    return attribute;

fail:
    doc->col = *xmlText - doc->currLine + 1;
    freeAttributeList(attribute);
    return 0;
}

static XmlElement *
parseElement(XmlDoc *doc, const char **xmlText, XmlElement *parent)
{
    XmlElement *element = 0;
    XmlElement *childnode = 0;
    XmlAttribute *attribute = 0;
    const char *startval = 0;

    ++(*xmlText);
    if (!**xmlText) FAIL(XML_EOF);
    if (**xmlText == '/')
    {
	++(*xmlText);
	FAILS(XML_CLOSEWOOPEN, readBareWord(xmlText, '>'));
    }

    element = calloc(1, sizeof(XmlElement));
    element->prev = element->next = element;
    element->parent = parent;
    element->name = readBareWord(xmlText, '>');
    if (!element->name) FAIL(XML_UNNAMEDTAG);
    if (!**xmlText) FAIL(XML_EOF);

    while (1)
    {
	skipWs(doc, xmlText);
	if (!**xmlText) FAIL(XML_EOF);

	if (**xmlText == '>')
	{
	    ++(*xmlText);
	    break;
	}
	if (**xmlText == '/')
	{
	    ++(*xmlText);
	    skipWs(doc, xmlText);
	    if (!**xmlText) FAIL(XML_EOF);
	    if (**xmlText != '>') FAILC(XML_UNEXPECTED, **xmlText);
	    ++(*xmlText);
	    return element;
	}
	attribute = parseAttribute(doc, xmlText, element);
	if (!attribute) goto failp;
	if (element->attributes)
	{
	    attribute->prev = element->attributes->prev;
	    attribute->next = element->attributes;
	    element->attributes->prev->next = attribute;
	    element->attributes->prev = attribute;
	}
	else
	{
	    element->attributes = attribute;
	}
	attribute = 0;
	if (!**xmlText) FAIL(XML_EOF);
    }

    startval = *xmlText;
    while (**xmlText)
    {
	if (**xmlText == '<')
	{
	    if ((*xmlText)[1] == '/')
	    {
		if (*xmlText != startval)
		{
		    element->value = calloc(1,
			    (size_t)(*xmlText - startval) + 1);
		    memcpy(element->value, startval,
			    (size_t)(*xmlText - startval));
		}
		*xmlText += 2;
		skipWs(doc, xmlText);
		if (!**xmlText) FAIL(XML_EOF);
		if (strncmp(*xmlText, element->name, strlen(element->name)))
		{
		    FAILS(XML_UNMATCHEDCLOSE, cloneString(element->name));
		}
		*xmlText += strlen(element->name);
		skipWs(doc, xmlText);
		if (!**xmlText) FAIL(XML_EOF);
		if (**xmlText != '>') FAILC(XML_UNEXPECTED, **xmlText);
		++(*xmlText);
		return element;
	    }
	    else
	    {
		childnode = parseElement(doc, xmlText, element);
		if (!childnode) goto failp;
		if (element->children)
		{
		    childnode->prev = element->children->prev;
		    childnode->next = element->children;
		    element->children->prev->next = childnode;
		    element->children->prev = childnode;
		}
		else
		{
		    element->children = childnode;
		}
		childnode = 0;
	    }
	}
	else skipUntil(doc, xmlText, '<');
    }
    doc->err = XML_EOF;

fail:
    doc->col = *xmlText - doc->currLine + 1;
failp:
    freeElementList(element);
    return 0;
}

XmlDoc *
parseDoc(const char *xmlText)
{
    XmlDoc* doc = malloc(sizeof(XmlDoc));

    doc->root = 0;
    doc->err = XML_SUCCESS;
    doc->line = 1;

    while(*xmlText)
    {
	if (*xmlText == '<')
	{
	    if (xmlText[1] == '!' || xmlText[1] == '?')
	    {
		++xmlText;
		skipUntil(doc, &xmlText, '>');
		if (!*xmlText)
		{
		    doc->err = XML_EOF;
		    freeElementList(doc->root);
		    doc->root = 0;
		    return doc;
		}
		++xmlText;
	    }
	    else
	    {
		if (doc->root)
		{
		    doc->col = xmlText - doc->currLine + 1;
		    doc->err = XML_SECONDROOT;
		    freeElementList(doc->root);
		    doc->root = 0;
		    return doc;
		}
		else
		{
		    doc->root = parseElement(doc, &xmlText, 0);
		    if (!doc->root) return doc;
		}
	    }
	}
	else if (isspace(*xmlText))
	{
	    skipWs(doc, &xmlText);
	}
	else
	{
	    doc->col = xmlText - doc->currLine + 1;
	    doc->err = XML_UNEXPECTED;
	    doc->errInfo.c = *xmlText;
	    freeElementList(doc->root);
	    doc->root = 0;
	    return doc;
	}
    }

    return doc;
}

XmlError
xmlDocError(const XmlDoc *doc)
{
    return doc->err;
}

const char *
xmlDocErrInfo(const XmlDoc *doc)
{
    if (doc->err == XML_UNMATCHEDCLOSE || doc->err == XML_CLOSEWOOPEN)
    {
	return doc->errInfo.s;
    }
    return 0;
}

char
xmlDocErrChar(const XmlDoc *doc)
{
    if (doc->err == XML_UNEXPECTED) return doc->errInfo.c;
    return 0;
}

long
xmlDocLine(const XmlDoc *doc)
{
    return doc->line;
}

long
xmlDocColumn(const XmlDoc *doc)
{
    return doc->col;
}

const XmlElement *
rootElement(const XmlDoc *doc)
{
    return doc->root;
}

void
xmlDocPerror(const XmlDoc *doc, FILE *file, const char *fmt, ...)
{
    va_list ap;

    if (!file) file = stderr;
    if (fmt)
    {
	va_start(ap, fmt);
	vfprintf(file, fmt, ap);
	va_end(ap);
    }
    switch (doc->err)
    {
	case XML_SUCCESS:
	    fputs(": successfully parsed.\n", file);
	    break;

	case XML_EOF:
	    fputs(": unexpected end of file while parsing.\n", file);
	    break;

	case XML_SECONDROOT:
	    fprintf(file, ": second root element found "
		    "at line %ld, column %ld.\n", doc->line, doc->col);
	    break;

	case XML_UNNAMEDTAG:
	    fprintf(file, ": tag without name found "
		    "at line %ld, column %ld.\n", doc->line, doc->col);
	    break;

	case XML_UNNAMEDATTR:
	    fprintf(file, ": attribute without name found "
		    "at line %ld, column %ld.\n", doc->line, doc->col);
	    break;

	case XML_UNMATCHEDCLOSE:
	    fprintf(file, ": closing tag doesn't match <%s> at line %ld, "
		    "column %ld\n", doc->errInfo.s, doc->line, doc->col);
	    break;

	case XML_CLOSEWOOPEN:
	    fprintf(file, ": closing tag </%s> was never opened at line %ld, "
		    "column %ld\n", doc->errInfo.s, doc->line, doc->col);
	    break;

	case XML_UNEXPECTED:
	    fprintf(file, ": found unexpected character `%c' at line %ld, "
		    "column %ld\n", doc->errInfo.c, doc->line, doc->col);
	    break;

	default:
	    fputs(": unknown error (aka BUG).\n", file);
    }
}

const XmlElement *
firstChild(const XmlElement *element)
{
    return element->children;
}

const XmlElement *
lastChild(const XmlElement *element)
{
    return (element->children ? element->children->prev : 0);
}

const XmlElement *
nextSibling(const XmlElement *element)
{
    return (element->parent && element->next != element->parent->children ?
	    element->next : 0);
}

const XmlElement *
parentElement(const XmlElement *element)
{
    return element->parent;
}

const XmlElement *
findMatching(const XmlElement *element,
	const char *tagname, const char *attname, const char *attval)
{
    const XmlElement *found;
    XmlAttribute *att = element->attributes;
    XmlElement *elem = element->children;

    if (!tagname || !strcmp(tagname, element->name))
    {
	if (attname && att) do
	{
	    if (!strcmp(attname, att->name) &&
		   (!attval || !strcmp(attval, att->value)))
	    {
		return element;
	    }
	    att = att->next;
	} while (att != element->attributes);
	else return element;
    }

    if (elem) do
    {
	found = findMatching(elem, tagname, attname, attval);
	if (found) return found;
	elem = elem->next;
    } while (elem != element->children);

    return 0;
}

const XmlAttribute *
firstAttribute(const XmlElement *element)
{
    return element->attributes;
}

const XmlAttribute *
nextAttribute(const XmlAttribute *attribute)
{
    return (attribute->next != attribute->parent->attributes ?
	    attribute->next : 0);
}

const XmlElement *
attributeElement(const XmlAttribute *attribute)
{
    return attribute->parent;
}

const char *
tagName(const XmlElement *element)
{
    return element->name;
}

const char *
elementContent(const XmlElement *element)
{
    return element->value;
}

const char *
attributeName(const XmlAttribute *attribute)
{
    return attribute->name;
}

const char *
attributeValue(const XmlAttribute *attribute)
{
    return attribute->value;
}

#ifdef BADXML_DEBUG
static void
dumpXmlAttribute(const XmlAttribute *a, FILE *file, int shift)
{
    int i;

    if (!a) return;

    for (i=0; i<shift; ++i) fputs(" ", file);
    fprintf(file, "[XmlAttribute]: %s, value: %s\n", a->name, a->value);
    if (a->next != a->parent->attributes) dumpXmlAttribute(a->next, file, shift);
}

static void
dumpXmlElement(const XmlElement *e, FILE *file, int shift)
{
    int i;

    if (!e) return;

    for (i=0; i<shift; ++i) fputs(" ", file);
    fprintf(file, "[XmlElement]: %s\n", e->name);
    dumpXmlAttribute(e->attributes, file, shift+2);
    if (e->children)
    {
	dumpXmlElement(e->children, file, shift+2);
    }
    else if (e->value)
    {
	for (i=0; i<shift; ++i) fputs(" ", file);
	fprintf(file, "  value: %s\n", e->value);
    }
    if (e->parent && e->next != e->parent->children)
    {
	dumpXmlElement(e->next, file, shift);
    }
}

void
dumpDoc(const XmlDoc *doc, FILE *file)
{
    fputs("[XmlDoc]:\n", file);
    dumpXmlElement(doc->root, file, 2);
}
#endif

