/** @file oyStructList_s_.c

   [Template file inheritance graph]
   +-> StructList_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2011/02/02
 */



  
#include "oyStructList_s.h"
#include "oyStructList_s_.h"

#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_generic_internal.h"
  


/* Include "StructList.private_custom_definitions.c" { */
/** Function    oyStructList_Release__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  structlist  the StructList object
 */
void oyStructList_Release__Members( oyStructList_s_ * structlist )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &structlist->member );
   */

  oyStructList_Clear((oyStructList_s*)structlist);

  if(structlist->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = structlist->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( structlist->member );
     */
    if(structlist->ptr_)
      deallocateFunc(structlist->ptr_);

    if(structlist->list_name)
      deallocateFunc(structlist->list_name);

    structlist->type_ = 0;
  }
}

/** Function    oyStructList_Init__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  structlist  the StructList object
 */
int oyStructList_Init__Members( oyStructList_s_ * structlist )
{
  return 0;
}

/** Function    oyStructList_Copy__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyStructList_s_ input object
 *  @param[out]  dst  the output oyStructList_s_ object
 */
int oyStructList_Copy__Members( oyStructList_s_ * dst, oyStructList_s_ * src)
{
  int error = 0, i;
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */
  dst->parent_type_ = src->parent_type_;
  if(src->list_name)
    dst->list_name = oyStringAppend_(0, src->list_name, allocateFunc_);

  oyObject_Lock( dst->oy_, __FILE__, __LINE__ );

  dst->n_reserved_ = (src->n_ > 10) ? (int)(src->n_ * 1.5) : 10;
  dst->n_ = src->n_;
  dst->ptr_ = oyAllocateFunc_( sizeof(int*) * dst->n_reserved_ );
  memset( dst->ptr_, 0, sizeof(int*) * dst->n_reserved_ );

  for(i = 0; i < src->n_; ++i)
    if(src->ptr_[i]->copy)
      dst->ptr_[i] = src->ptr_[i]->copy( src->ptr_[i], 0 );

  if(oyStruct_IsObserved( (oyStruct_s*)dst, 0) )
    error = oyStructList_ObserverAdd( (oyStructList_s*)src, 0, 0, 0 );

  oyObject_UnLock( dst->oy_, __FILE__, __LINE__ );

  return error;
}

/* } Include "StructList.private_custom_definitions.c" */



/** @internal
 *  Function oyStructList_New_
 *  @memberof oyStructList_s_
 *  @brief   allocate a new oyStructList_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyStructList_s_ * oyStructList_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_STRUCT_LIST_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyStructList_s_ * s = 0;

  if(s_obj)
    s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyStructList_s_) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyStructList_Copy;
  s->release = (oyStruct_Release_f) oyStructList_Release;

  s->oy_ = s_obj;
  /* ---- end of common object constructor ------- */

  
  
  /* ---- start of custom StructList constructor ----- */
  error = !oyObject_SetParent( s_obj, oyOBJECT_STRUCT_LIST_S, s );
  error = oyStructList_Init__Members( s );
  /* ---- end of custom StructList constructor ------- */
  
  
  
  
  

  return s;
}

/** @internal
 *  Function oyStructList_Copy__
 *  @memberof oyStructList_s_
 *  @brief   real copy a StructList object
 *
 *  @param[in]     structlist                 StructList struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyStructList_s_ * oyStructList_Copy__ ( oyStructList_s_ *structlist, oyObject_s object )
{
  oyStructList_s_ *s = 0;
  int error = 0;

  if(!structlist || !object)
    return s;

  s = oyStructList_New_( object );
  error = !s;

  if(!error) {
    
    
    /* ---- start of custom StructList copy constructor ----- */
    error = oyStructList_Copy__Members( s, structlist );
    /* ---- end of custom StructList copy constructor ------- */
    
    
    
    
    
  }

  if(error)
    oyStructList_Release_( &s );

  return s;
}

/** @internal
 *  Function oyStructList_Copy_
 *  @memberof oyStructList_s_
 *  @brief   copy or reference a StructList object
 *
 *  @param[in]     structlist                 StructList struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyStructList_s_ * oyStructList_Copy_ ( oyStructList_s_ *structlist, oyObject_s object )
{
  oyStructList_s_ *s = structlist;

  if(!structlist)
    return 0;

  if(structlist && !object)
  {
    s = structlist;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyStructList_Copy__( structlist, object );

  return s;
}
 
/** @internal
 *  Function oyStructList_Release_
 *  @memberof oyStructList_s_
 *  @brief   release and possibly deallocate a StructList object
 *
 *  @param[in,out] structlist                 StructList struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyStructList_Release_( oyStructList_s_ **structlist )
{
  /* ---- start of common object destructor ----- */
  oyStructList_s_ *s = 0;

  if(!structlist || !*structlist)
    return 0;

  s = *structlist;

  *structlist = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  
  /* ---- start of custom StructList destructor ----- */
  oyStructList_Release__Members( s );
  /* ---- end of custom StructList destructor ------- */
  
  
  
  


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "StructList.private_methods_definitions.c" { */
/** @internal
 *  @brief oyStructList_s pointer access
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
/*oyHandle_s **    oyStructList_GetRaw_( oyStructList_s    * list )
{
  int error = 0;
  oyStructList_s_ * s = (oyStructList_s_*)list;
  oyHandle_s ** p = 0;

  if(error <= 0)
  if(s->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
    p = s->ptr_;

  return p;
}*/

/** @internal
 *  @brief oyStructList_s pointer access
 *
 *  non thread save
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 november 2007 (API 0.1.8)
 */
oyStruct_s *     oyStructList_Get_   ( oyStructList_s    * list,
                                       int                 pos )
{
  int n = 0;
  oyStructList_s_ * s = (oyStructList_s_*)list;
  int error = !s;
  oyStruct_s * obj = 0;

  if(error <= 0)
  if(s->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
    n = s->n_;

  if(error <= 0)
  if(pos >= 0 && n > pos && s->ptr_[pos])
    obj = s->ptr_[pos];

  return obj;
}

/** @internal
 *  @brief oyStructList_s pointer referencing
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
int              oyStructList_ReferenceAt_( oyStructList_s * list,
                                       int                 pos )
{
  int n = 0;
  int error = 0;
  oyStructList_s_ * s = (oyStructList_s_*)list;
  oyStruct_s * p = 0;

  if(s)
    n = s->n_;
  else
    error = 1;

  if(error <= 0)
  if(s->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
  if(pos >= 0 && n > pos && s->ptr_[pos])
  {
    p = s->ptr_[pos];
    error = !(p && p->copy);

    if(error <= 0)
      p = p->copy( p, 0 );
  }

  return !p;
}


/* } Include "StructList.private_methods_definitions.c" */

