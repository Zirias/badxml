#include <badxml/badxml.h>

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

struct xmlDoc
{
    XmlElement *root;
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


static void
skipWs(const char **pos)
{
    while (isspace(**pos)) ++(*pos);
}

static char *
readBareWord(const char **pos, char endmark)
{
    const char *start;
    char *word;

    start = *pos;

    if (!*start) return 0;
    while (!isspace(**pos) && **pos && **pos != endmark) ++(*pos);

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
        free(doc);
    }
}

static XmlAttribute *
parseAttribute(const char **xmlText, XmlElement *element)
{
    const char *startval;
    XmlAttribute *attribute = calloc(1, sizeof(XmlAttribute));
    attribute->next = attribute->prev = attribute;
    attribute->parent = element;

    attribute->name = readBareWord(xmlText, '=');
    if (!attribute->name || !**xmlText) goto parseAttribute_fail;
    skipWs(xmlText);
    if (**xmlText != '=') goto parseAttribute_fail;
    ++(*xmlText);
    skipWs(xmlText);
    if (**xmlText != '"') goto parseAttribute_fail;
    ++(*xmlText);

    startval = *xmlText;
    while (**xmlText && **xmlText != '"') ++(*xmlText);
    if (!**xmlText) goto parseAttribute_fail;
    attribute->value = calloc(1, (size_t)(*xmlText - startval) + 1);
    memcpy(attribute->value, startval, (size_t)(*xmlText - startval));
    ++(*xmlText);
    return attribute;

parseAttribute_fail:
    freeAttributeList(attribute);
    return 0;
}

static XmlElement *
parseElement(const char **xmlText, XmlElement *parent)
{
    XmlElement *element;
    XmlElement *childnode;
    XmlAttribute *attribute;
    const char *startval;

    ++(*xmlText);
    if (!**xmlText || **xmlText == '/') return 0;

    element = calloc(1, sizeof(XmlElement));
    element->prev = element->next = element;
    element->parent = parent;
    element->name = readBareWord(xmlText, '>');
    if (!element->name || !**xmlText) goto parseElement_fail;

    childnode = 0;
    attribute = 0;
    while (1)
    {
        skipWs(xmlText);
        if (!**xmlText) goto parseElement_fail;

        if (**xmlText == '>')
        {
            ++(*xmlText);
            break;
        }
        if (**xmlText == '/')
        {
            ++(*xmlText);
            skipWs(xmlText);
            if (!**xmlText) goto parseElement_fail;
            if (**xmlText != '>') goto parseElement_fail;
            ++(*xmlText);
            return element;
        }
        attribute = parseAttribute(xmlText, element);
        if (!attribute) goto parseElement_fail;
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
        if (!**xmlText) goto parseElement_fail;
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
                skipWs(xmlText);
                if (!**xmlText) goto parseElement_fail;
                if (strncmp(*xmlText, element->name, strlen(element->name)))
		{
		    goto parseElement_fail;
		}
                *xmlText += strlen(element->name);
                skipWs(xmlText);
                if (!**xmlText || **xmlText != '>') goto parseElement_fail;
                ++(*xmlText);
                return element;
            }
            else
            {
                childnode = parseElement(xmlText, element);
                if (!childnode) goto parseElement_fail;
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
        else ++(*xmlText);
    }

parseElement_fail:
    freeElementList(element);
    return 0;
}

XmlDoc *
parseDoc(const char *xmlText)
{
    XmlDoc* doc = malloc(sizeof(XmlDoc));
    const char *p = xmlText;

    doc->root = 0;
    while(*p)
    {
        if (*p == '<')
        {
            if (p[1] == '!' || p[1] == '?')
            {
                ++p;
                while (*p && *p != '>') ++p;
                if (!*p)
                {
                    freeElementList(doc->root);
                    free(doc);
                }
                ++p;
            }
            else
            {
                if (doc->root)
                {
                    freeElementList(doc->root);
                    free(doc);
                    return 0;
                }
                else
                {
                    doc->root = parseElement(&p, 0);
                    if (!doc->root)
                    {
                        free(doc);
                        return 0;
                    }
                }
            }
        }
        else if (isspace(*p))
        {
            skipWs(&p);
        }
        else
        {
            freeElementList(doc->root);
            free(doc);
            return 0;
        }
    }

    return doc;
}

const XmlElement *
rootElement(const XmlDoc *doc)
{
    return doc->root;
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

