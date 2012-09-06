/** @file oyPointer_s.c

   [Template file inheritance graph]
   +-> oyPointer_s.template.c
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
 *  @date     2012/09/06
 */


  
#include "oyPointer_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyPointer_s_.h"

#include "oyHash_s.h"
  


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

#ifdef UNHIDE_CMM
/** Function  oyPointer_LookUpFromObject
 *  @brief    Get a module specific pointer from cache
 *  @memberof oyPointer_s
 *
 *  The returned oyPointer_s has to be released after using by the module with
 *  oyPointer_Release().
 *  In case the the oyPointer_s::ptr member is empty, it should be set by the
 *  requesting module.
 *
 *  @see oyPointer_LookUpFromText()
 *
 *  @param[in]     data                 object to look up
 *  @param[in]     data_type            four byte module type for this object
 *                                      type; The data_type shall enshure the
 *                                      returned oyPointer_s is specific to the
 *                                      calling module.
 *  @return                             the CMM specific oyPointer_s; It is owned
 *                                      by the CMM.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2009/11/05
 */
oyPointer_s  * oyPointer_LookUpFromObject (
                                       oyStruct_s        * data,
                                       const char        * data_type )
{
  oyStruct_s * s = data;
  int error = !s;
  oyPointer_s * cmm_ptr = 0;

  if(error <= 0 && !data_type)
    error = !data_type;

  if(error <= 0)
  {
    const char * tmp = 0;
    tmp = oyObject_GetName( s->oy_, oyNAME_NICK );
    cmm_ptr = oyPointer_LookUpFromText( tmp, data_type );
  }

  return cmm_ptr;
}

/** Function  oyPointer_LookUpFromText
 *  @brief    Get a module specific pointer from cache
 *  @memberof oyPointer_s
 *
 *  The returned oyPointer_s has to be released after using by the module with
 *  oyPointer_Release().
 *  In case the the oyPointer_s::ptr member is empty, it should be set by the
 *  requesting module.
 *
 *  @see e.g. lcmsCMMData_Open()
 *
 *  @param[in]     text                 hash text to look up
 *  @param[in]     data_type            four byte module type for this object
 *                                      type; The data_type shall enshure the
 *                                      returned oyPointer_s is specific to the
 *                                      calling module.
 *  @return                             the CMM specific oyPointer_s; It is owned
 *                                      by the CMM.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/05 (Oyranos: 0.1.10)
 *  @date    2009/11/05
 */
oyPointer_s * oyPointer_LookUpFromText( const char        * text,
                                       const char        * data_type )
{
  int error = !text;
  oyPointer_s * cmm_ptr = 0;

  if(error <= 0 && !data_type)
    error = !data_type;

  if(error <= 0)
  {
    /*oyPointer_s *cmm_ptr = 0;*/
    const char * tmp = 0;

    oyHash_s * entry = 0;
    oyChar * hash_text = 0;

    /** Cache Search \n
     *  1.     hash from input \n
     *  2.     query for hash in cache \n
     *  3.     check \n
     *  3a.       eighter take cache entry or \n
     *  3b.       update cache entry
     */

    /* 1. create hash text */
    STRING_ADD( hash_text, data_type );
    STRING_ADD( hash_text, ":" );
    tmp = text;
    STRING_ADD( hash_text, tmp );

    /* 2. query in cache */
    entry = oyCMMCacheListGetEntry_( hash_text );

    if(error <= 0)
    {
      /* 3. check and 3.a take*/
      cmm_ptr = (oyPointer_s*) oyHash_GetPointer( entry,
                                                  oyOBJECT_POINTER_S);

      if(!cmm_ptr)
      {
        cmm_ptr = oyPointer_New( 0 );
        error = !cmm_ptr;

        if(error <= 0)
          error = oyPointer_Set( cmm_ptr, 0,
                                 data_type, 0, 0, 0 );

        error = !cmm_ptr;

        if(error <= 0 && cmm_ptr)
          /* 3b.1. update cache entry */
          error = oyHash_SetPointer( entry,
                                     (oyStruct_s*) cmm_ptr );
      }
    }

    oyHash_Release( &entry );
  }

  return cmm_ptr;
}
#endif

/* } Include "Pointer.public_methods_definitions.c" */

