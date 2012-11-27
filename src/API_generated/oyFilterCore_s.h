/** @file oyFilterCore_s.h

   [Template file inheritance graph]
   +-> oyFilterCore_s.template.h
   |
   +-> Base_s.h
   |
   +-- oyStruct_s.template.h

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/10/08
 */



#ifndef OY_FILTER_CORE_S_H
#define OY_FILTER_CORE_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyFilterCore_s oyFilterCore_s;



#include "oyStruct_s.h"

#include "oyOptions_s.h"


/* Include "FilterCore.public.h" { */

/* } Include "FilterCore.public.h" */


/* Include "FilterCore.dox" { */
/** @struct  oyFilterCore_s
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *  @brief   A basic filter to manipulate data
 *  
 *  This is the Oyranos filter object. Filters are categorised into basic
 *  classes of filters described in the registration_ (//xxx) member.
 *  Filters implement a container for data and options.
 *  Filters can be manipulated by changing their options or data set.
 *
 *  Filters are chained into a oyConversion_s in order to get applied to data.
 *  The relation of filters in a graph is defined through the oyFilterNode_s
 *  struct.
 *
 *  It is possible to chain filters in different ways together. The aimed way
 *  here is to use members and queries to find possible connections. For 
 *  instance a one in one out filter can not be connected to two sources at 
 *  once.
 *
 *  The registration_ describes different basic types of filters (//xxx).
 *  See oyranos::oyCONNECTOR_e.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/08 (Oyranos: 0.1.8)
 *  @date    2009/11/17
 */

/* } Include "FilterCore.dox" */

struct oyFilterCore_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};


OYAPI oyFilterCore_s* OYEXPORT
  oyFilterCore_New( oyObject_s object );
OYAPI oyFilterCore_s* OYEXPORT
  oyFilterCore_Copy( oyFilterCore_s *filtercore, oyObject_s obj );
OYAPI int OYEXPORT
  oyFilterCore_Release( oyFilterCore_s **filtercore );



/* Include "FilterCore.public_methods_declarations.h" { */
OYAPI const char *  OYEXPORT
                oyFilterCore_GetCategory (
                                       oyFilterCore_s    * filter,
                                       int                 nontranslated );
OYAPI const char *  OYEXPORT
                oyFilterCore_GetRegistration (
                                       oyFilterCore_s    * filter );
OYAPI const char *  OYEXPORT
                oyFilterCore_GetName ( oyFilterCore_s    * filter,
                                       oyNAME_e            name_type );
OYAPI const char *  OYEXPORT
                oyFilterCore_GetText ( oyFilterCore_s    * filter,
                                       oyNAME_e            name_type );
OYAPI oyFilterCore_s *  OYEXPORT
                oyFilterCore_NewWith ( const char        * registration,
                                       oyOptions_s       * options,
                                       oyObject_s          object );

/* } Include "FilterCore.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_CORE_S_H */
