/** @file oyConnectorImaging_s.h

   [Template file inheritance graph]
   +-> oyConnectorImaging_s.template.h
   |
   +-> oyConnector_s.template.h
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
 *  @date     2012/10/16
 */



#ifndef OY_CONNECTOR_IMAGING_S_H
#define OY_CONNECTOR_IMAGING_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyConnectorImaging_s oyConnectorImaging_s;




#include "oyStruct_s.h"


#include "oyranos_image.h"


/* Include "ConnectorImaging.public.h" { */

/* } Include "ConnectorImaging.public.h" */


/* Include "ConnectorImaging.dox" { */
/** @struct  oyConnectorImaging_s
 *  @ingroup objects_conversion
 *  @extends oyConnector_s
 *  @brief   Node connection descriptor
 *
 *  This structure holds informations about the connection capabilities.
 *  It holds common structure members of oyFilterPlug_s and oyFilterSocket_s.
 *
 *  To signal a value is not initialised or does not apply, set the according
 *  integer value to -1.
 *
 *  @todo generalise the connector properties
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/26 (Oyranos: 0.1.8)
 *  @date    2010/06/25
 */

/* } Include "ConnectorImaging.dox" */

struct oyConnectorImaging_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};


OYAPI oyConnectorImaging_s* OYEXPORT
  oyConnectorImaging_New( oyObject_s object );
OYAPI oyConnectorImaging_s* OYEXPORT
  oyConnectorImaging_Copy( oyConnectorImaging_s *connectorimaging, oyObject_s obj );
OYAPI int OYEXPORT
  oyConnectorImaging_Release( oyConnectorImaging_s **connectorimaging );



/* Include "ConnectorImaging.public_methods_declarations.h" { */
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetDataTypes (
                                       oyConnectorImaging_s  * c,
                                       const oyDATATYPE_e   ** data_types,
                                       int                   * data_types_n );
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetChannelTypes (
                                       oyConnectorImaging_s  * c,
                                       const oyCHANNELTYPE_e** channel_types,
                                       int                   * channel_types_n );

/** @enum    oyCONNECTOR_IMAGING_CAP_e
 *  @brief   Capabilities of oyConnectorImaging_s
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/06 (Oyranos: 0.5.0)
 *  @date    2012/09/06
 */
typedef enum {
  oyCONNECTOR_IMAGING_CAP_IS_PLUG,
  oyCONNECTOR_IMAGING_CAP_MAX_COLOUR_OFFSET,
  oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT,
  oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT,
  oyCONNECTOR_IMAGING_CAP_MIN_COLOUR_COUNT,
  oyCONNECTOR_IMAGING_CAP_MAX_COLOUR_COUNT,
  oyCONNECTOR_IMAGING_CAP_CAN_PLANAR,        /**< can read separated channels */
  oyCONNECTOR_IMAGING_CAP_CAN_INTERWOVEN,    /**< can read continuous channels */
  oyCONNECTOR_IMAGING_CAP_CAN_SWAP,          /**< can swap colour channels (BGR)*/
  oyCONNECTOR_IMAGING_CAP_CAN_SWAP_BYTES,    /**< non host byte order */
  oyCONNECTOR_IMAGING_CAP_CAN_REVERT,        /**< revert 1 -> 0 and 0 -> 1 */
  oyCONNECTOR_IMAGING_CAP_CAN_PREMULTIPLIED_ALPHA,
  oyCONNECTOR_IMAGING_CAP_CAN_NONPREMULTIPLIED_ALPHA,
  oyCONNECTOR_IMAGING_CAP_CAN_SUBPIXEL,      /**< understand subpixel order */
  oyCONNECTOR_IMAGING_CAP_ID,                /**< relative to oyFilterCore_s, e.g. 1*/
  /**< connector is mandatory or optional, important for modules */
  oyCONNECTOR_IMAGING_CAP_IS_MANDATORY
} oyCONNECTOR_IMAGING_CAP_e;
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetCapability (
                                       oyConnectorImaging_s    * c,
                                       oyCONNECTOR_IMAGING_CAP_e type );


/* } Include "ConnectorImaging.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CONNECTOR_IMAGING_S_H */