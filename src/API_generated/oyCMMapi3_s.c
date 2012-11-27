/** @file oyCMMapi3_s.c

   [Template file inheritance graph]
   +-> oyCMMapi3_s.template.c
   |
   +-> oyCMMapi_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

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


  
#include "oyCMMapi3_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMapi3_s_.h"
  


/** Function oyCMMapi3_New
 *  @memberof oyCMMapi3_s
 *  @brief   allocate a new CMMapi3 object
 */
OYAPI oyCMMapi3_s * OYEXPORT
  oyCMMapi3_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapi3_s_ * cmmapi3 = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapi3 = oyCMMapi3_New_( s );

  return (oyCMMapi3_s*) cmmapi3;
}

/** Function oyCMMapi3_Copy
 *  @memberof oyCMMapi3_s
 *  @brief   copy or reference a CMMapi3 object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapi3                 CMMapi3 struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triffers a real copy
 */
OYAPI oyCMMapi3_s* OYEXPORT
  oyCMMapi3_Copy( oyCMMapi3_s *cmmapi3, oyObject_s object )
{
  oyCMMapi3_s_ * s = (oyCMMapi3_s_*) cmmapi3;

  if(s)
    oyCheckType__m( oyOBJECT_CMM_API3_S, return 0 )

  s = oyCMMapi3_Copy_( s, object );

  return (oyCMMapi3_s*) s;
}
 
/** Function oyCMMapi3_Release
 *  @memberof oyCMMapi3_s
 *  @brief   release and possibly deallocate a oyCMMapi3_s object
 *
 *  @param[in,out] cmmapi3                 CMMapi3 struct object
 */
OYAPI int OYEXPORT
  oyCMMapi3_Release( oyCMMapi3_s **cmmapi3 )
{
  oyCMMapi3_s_ * s = 0;

  if(!cmmapi3 || !*cmmapi3)
    return 0;

  s = (oyCMMapi3_s_*) *cmmapi3;

  oyCheckType__m( oyOBJECT_CMM_API3_S, return 1 )

  *cmmapi3 = 0;

  return oyCMMapi3_Release_( &s );
}



/* Include "CMMapi3.public_methods_definitions.c" { */
oyCMMCanHandle_f   oyCMMapi3_GetCanHandleF
                                     ( oyCMMapi3_s       * api )
{
  return ((oyCMMapi3_s_*) api)->oyCMMCanHandle;
}


/* } Include "CMMapi3.public_methods_definitions.c" */
