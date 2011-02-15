/** @file oyPointer_s.c

   [Template file inheritance graph]
   +-> Pointer_s.template.c
   |
   +-> Base_s.c
   |
   +-- Struct_s.template.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2011/02/15
 */


  
#include "oyPointer_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyPointer_s_.h"
  


/** Function oyPointer_New
 *  @memberof oyPointer_s
 *  @brief   allocate a new Pointer object
 */
OYAPI oyPointer_s * OYEXPORT
  oyPointer_New( oyObject_s object )
{
  oyObject_s s = object;
  oyPointer_s_ * pointer = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 );

  pointer = oyPointer_New_( s );

  return (oyPointer_s*) pointer;
}

/** Function oyPointer_Copy
 *  @memberof oyPointer_s
 *  @brief   copy or reference a Pointer object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     pointer                 Pointer struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triffers a real copy
 */
OYAPI oyPointer_s* OYEXPORT
  oyPointer_Copy( oyPointer_s *pointer, oyObject_s object )
{
  oyPointer_s_ * s = (oyPointer_s_*) pointer;

  if(s)
    oyCheckType__m( oyOBJECT_POINTER_S, return 0 );

  s = oyPointer_Copy_( s, object );

  return (oyPointer_s*) s;
}
 
/** Function oyPointer_Release
 *  @memberof oyPointer_s
 *  @brief   release and possibly deallocate a oyPointer_s object
 *
 *  @param[in,out] pointer                 Pointer struct object
 */
OYAPI int OYEXPORT
  oyPointer_Release( oyPointer_s **pointer )
{
  oyPointer_s_ * s = 0;

  if(!pointer || !*pointer)
    return 0;

  s = (oyPointer_s_*) *pointer;

  oyCheckType__m( oyOBJECT_POINTER_S, return 1 )

  *pointer = 0;

  return oyPointer_Release_( &s );
}



/* Include "Pointer.public_methods_definitions.c" { */
/** Function oyPointer_Set
 *  @brief   set a oyPointer_s
 *  @ingroup module_api
 *  @memberof oyPointer_s
 *
 *  Use for initialising.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/22 (Oyranos: 0.1.10)
 *  @date    2009/08/16
 */
int          oyPointer_Set           ( oyPointer_s       * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease )
{
  return oyPointer_Set_( (oyPointer_s_*)cmm_ptr, lib_name, resource, ptr, func_name, ptrRelease);
}

const char * oyPointer_GetFuncName    ( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->func_name;
  else
    return NULL;
}
const char * oyPointer_GetLibName     ( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->lib_name;
  else
    return NULL;
}
const char * oyPointer_GetResourceName( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->resource;
  else
    return NULL;
}
int          oyPointer_GetSize        ( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->size;
  else
    return 0;
}
int          oyPointer_SetSize        ( oyPointer_s        * cmm_ptr,
                                       int                 size )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    c->size = size;
  return 0;
}
oyPointer    oyPointer_GetPointer     ( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->ptr;
  else
    return NULL;
}


/* } Include "Pointer.public_methods_definitions.c" */

