#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <badxml/badxml.h>

char *readFile(const char *filename)
{
    char *buffer;
    FILE *file = 0;
    size_t bufsize = 1024;
    size_t readsize;
    size_t readtotal;

    if (filename)
    {
	file = fopen(filename, "r");
	if (!file)
	{
	    fprintf(stderr, "Cannot open `%s': %s\n",
		    filename, strerror(errno));
	    exit(1);
	}
    }
    else file = stdin;

    buffer = malloc(bufsize);
    if (!buffer)
    {
	perror("malloc");
	exit(1);
    }

    readtotal = 0;
    while ((readsize = fread(buffer + readtotal, 1,
		    bufsize - readtotal, file)) == bufsize - readtotal)
    {
        readtotal += readsize;
        bufsize *= 2;
        if (!(buffer = realloc(buffer, bufsize)))
	{
	    perror("realloc");
	    exit(1);
	}
    }

    if (file != stdin) fclose(file);

    readtotal += readsize;
    buffer[readtotal] = 0;

    return buffer;
}

int main(int argc, char **argv)
{
    XmlDoc *doc;
    const XmlElement *element;
    const char *val;
    char *xml;

    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s tagname attrname attrval [filename]",
		argv[0]);
        return 1;
    }
    xml = readFile(argv[4]);

    /* parse the text and get an object tree */
    doc = parseDoc(xml);

    free(xml);

    if (xmlDocError(doc) != XML_SUCCESS)
    {
	if (argv[4])
	{
	    /* unparsable XML was read from file */
	    xmlDocPerror(doc, stderr, "Error parsing `%s'", argv[4]);
	}
	else
	{
	    /* unparsable XML was read from stdin */
	    xmlDocPerror(doc, stderr, "Error parsing standard input");
	}
	freeDoc(doc);
        return 1;
    }

    /* this is a no-op unless compiled with -DBADXML_DEBUG */
    dumpDoc(doc, stderr);

    xml = xmlText(doc);
    puts(xml);
    free(xml);

    /* example: find an element matching tagname and attribute name and value */
    element = findMatching(rootElement(doc), argv[1], argv[2], argv[3]);
    if (element) val = elementContent(element);
    else val = "<not found>";

    printf("%s[%s='%s']: %s\n", argv[1], argv[2], argv[3], val);

    /* release all resources allocated for the document */
    freeDoc(doc);

    return 0;
}

