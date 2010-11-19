/** @file oyranos_xml.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2006-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    xml handling
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @pat License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/02/10
 */


#ifndef OYRANOS_XML_H
#define OYRANOS_XML_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libxml/parser.h>


#include "config.h"
#include "oyranos.h"
#include "oyranos_alpha.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/* gives string bordered by a xml style keyword */
char*       oyXMLgetValue_     (const char       *xml,
                                const char       *key);

/* gives the position and length of a string bordered by a xml style keyword */
char*       oyXMLgetField_     (const char       *xml,
                                const char       *key,
                                int              *len);

/* gives all strings bordered by a xml style keyword from xml */
char**      oyXMLgetArray_     (const char       *xml,
                                const char       *key,
                                int              *count);

/* write option range to mem, allocating memory on demand */
void         oyWriteOptionToXML_     ( oyGROUP_e           group,
                                       oyWIDGET_e          start,
                                       oyWIDGET_e          end, 
                                       char             ** text,
                                       uint32_t            flags );

char*       oyPolicyToXML_     (oyGROUP_e           group,
                                int               add_header,
                                oyAlloc_f         allocate_func);

int         oyReadXMLPolicy_   (oyGROUP_e           group, 
                                const char       *xml);
char *       oyXMLgetElement_        ( const char        * xml,
                                       const char        * xpath,
                                       const char        * key );


void               oyParseXMLDoc_    ( xmlDocPtr           doc,
                                       xmlNodePtr          cur,
                                       oyUiHandler_s    ** ui_handlers,
                                       oyPointer           ui_handlers_context);
const char *       oyXFORMsModelGetXPathValue_
                                     ( xmlDocPtr           doc,
                                       const char        * reference );

const char *       oyXFORMsModelGetXPathValue
                                     ( xmlNodePtr          cur,
                                       const char        * attr_name,
                                       const char       ** path );
int                oyXMLNodeNameIs   ( xmlNodePtr          cur,
                                       const char        * node_name );
const char *       oyXML2NodeValue   ( xmlNodePtr          cur );


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_XML_H */

