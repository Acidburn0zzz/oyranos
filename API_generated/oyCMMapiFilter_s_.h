/** @file oyCMMapiFilter_s_.h

   [Template file inheritance graph]
   +-> oyCMMapiFilter_s_.template.h
   |
   +-> oyCMMapi_s_.template.h
   |
   +-- Base_s_.h

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


#ifndef OY_CMM_API_FILTER_S__H
#define OY_CMM_API_FILTER_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMapiFilterPriv_m( var ) ((oyCMMapiFilter_s_*) (var))

typedef struct oyCMMapiFilter_s_ oyCMMapiFilter_s_;

  
#include <oyranos_object.h>



#include "oyranos_module.h"
#include "oyCMMinfo_s_.h"

#include "oyCMMapi5_s_.h"
  

#include "oyCMMapiFilter_s.h"

/* Include "CMMapiFilter.private.h" { */

/* } Include "CMMapiFilter.private.h" */

/** @internal
 *  @struct   oyCMMapiFilter_s_
 *  @brief      the module API 4,6,7 interface base
 *  @ingroup  module_api
 *  @extends  oyCMMapi_s
 */
struct oyCMMapiFilter_s_ {

/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */




/* Include "CMMapi.members.h" { */
  oyCMMapi_s     * next;               /**< the next CMM api */

  oyCMMInit_f      oyCMMInit;          /**< */
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet; /**< */

  /** e.g. "sw/oyranos.org/colour.tonemap.imaging/hydra.shiva.CPU.GPU" or "sw/oyranos.org/colour/icc.lcms.CPU",
      see as well @ref registration */
  const char     * registration;

  /** 0: major - should be stable for the live time of a filter, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  char           * id_;                /**< @private Oyranos id; keep to zero */

/* } Include "CMMapi.members.h" */

/* Include "CMMapiFilter.members.h" { */
  oyCMMapi5_s_   * api5_;            /**< @private meta module; keep to zero */

/* } Include "CMMapiFilter.members.h" */

};


oyCMMapiFilter_s_*
  oyCMMapiFilter_New_( oyObject_s object );
oyCMMapiFilter_s_*
  oyCMMapiFilter_Copy_( oyCMMapiFilter_s_ *cmmapifilter, oyObject_s object);
oyCMMapiFilter_s_*
  oyCMMapiFilter_Copy__( oyCMMapiFilter_s_ *cmmapifilter, oyObject_s object);
int
  oyCMMapiFilter_Release_( oyCMMapiFilter_s_ **cmmapifilter );



/* Include "CMMapiFilter.private_methods_declarations.h" { */

/* } Include "CMMapiFilter.private_methods_declarations.h" */



void oyCMMapiFilter_Release__Members( oyCMMapiFilter_s_ * cmmapifilter );
int oyCMMapiFilter_Init__Members( oyCMMapiFilter_s_ * cmmapifilter );
int oyCMMapiFilter_Copy__Members( oyCMMapiFilter_s_ * dst, oyCMMapiFilter_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API_FILTER_S__H */