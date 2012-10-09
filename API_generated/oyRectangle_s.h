/** @file oyRectangle_s.h

   [Template file inheritance graph]
   +-> oyRectangle_s.template.h
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



#ifndef OY_RECTANGLE_S_H
#define OY_RECTANGLE_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyRectangle_s oyRectangle_s;



#include "oyStruct_s.h"

#include "oyranos_image.h"
#include "oyImage_s.h"


/* Include "Rectangle.public.h" { */

/* } Include "Rectangle.public.h" */


/* Include "Rectangle.dox" { */
/** @struct  oyRectangle_s 
 *  @ingroup objects_image
 *  @extends oyStruct_s
 *  @brief   Start with a simple rectangle
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */

/* } Include "Rectangle.dox" */

struct oyRectangle_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};


OYAPI oyRectangle_s* OYEXPORT
  oyRectangle_New( oyObject_s object );
OYAPI oyRectangle_s* OYEXPORT
  oyRectangle_Copy( oyRectangle_s *rectangle, oyObject_s obj );
OYAPI int OYEXPORT
  oyRectangle_Release( oyRectangle_s **rectangle );



/* Include "Rectangle.public_methods_declarations.h" { */
OYAPI double  OYEXPORT
                 oyRectangle_CountPoints(oyRectangle_s     * rectangle );
OYAPI int  OYEXPORT
                 oyRectangle_Index   ( oyRectangle_s     * rectangle,
                                       double              x,
                                       double              y );
OYAPI int  OYEXPORT
                 oyRectangle_IsEqual ( oyRectangle_s     * rectangle1,
                                       oyRectangle_s     * rectangle2 );
OYAPI int  OYEXPORT
                 oyRectangle_IsInside( oyRectangle_s     * test,
                                       oyRectangle_s     * ref );
OYAPI void  OYEXPORT
                 oyRectangle_MoveInside (
                                       oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref );
OYAPI oyRectangle_s *  OYEXPORT
                oyRectangle_NewFrom  ( oyRectangle_s     * ref,
                                       oyObject_s          object );
OYAPI oyRectangle_s *  OYEXPORT
                oyRectangle_NewWith  ( double              x,
                                       double              y,
                                       double              width,
                                       double              height,
                                       oyObject_s          object );
OYAPI void  OYEXPORT
                 oyRectangle_Normalise(oyRectangle_s     * edit_rectangle );
OYAPI int  OYEXPORT
                 oyRectangle_PointIsInside( oyRectangle_s  * rectangle,
                                       double              x,
                                       double              y );
OYAPI void  OYEXPORT
                 oyRectangle_Round   ( oyRectangle_s     * edit_rectangle );
OYAPI void  OYEXPORT
                 oyRectangle_Scale   ( oyRectangle_s     * edit_rectangle,
                                       double              factor );
OYAPI void  OYEXPORT
                 oyRectangle_SetByRectangle (
                                       oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref );
OYAPI void  OYEXPORT
                 oyRectangle_SetGeo  ( oyRectangle_s     * edit_rectangle,
                                       double              x,
                                       double              y,
                                       double              width,
                                       double              height );
OYAPI double *  OYEXPORT
                 oyRectangle_SetGeo1 ( oyRectangle_s     * rectangle,
                                       int                 x_y_w_h );
OYAPI void  OYEXPORT
                 oyRectangle_GetGeo  ( oyRectangle_s     * rectangle,
                                       double            * x,
                                       double            * y,
                                       double            * width,
                                       double            * height );
OYAPI double  OYEXPORT
                 oyRectangle_GetGeo1 ( oyRectangle_s     * rectangle,
                                       int                 x_y_w_h );
OYAPI const char *  OYEXPORT
                oyRectangle_Show     ( oyRectangle_s     * rectangle );
OYAPI void  OYEXPORT
                 oyRectangle_Trim    ( oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref );

/* } Include "Rectangle.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_RECTANGLE_S_H */
