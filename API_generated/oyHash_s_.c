/** @file oyHash_s_.c

   [Template file inheritance graph]
   +-> Hash_s_.template.c
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
 *  @date     2011/05/30
 */



  
#include "oyHash_s.h"
#include "oyHash_s_.h"

#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_generic_internal.h"
  


/* Include "Hash.private_custom_definitions.c" { */
/** Function    oyHash_Release__Members
 *  @memberof   oyHash_s
 *  @brief      Custom Hash destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  hash  the Hash object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyHash_Release__Members( oyHash_s_ * hash )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &hash->member );
   */

  /* should not happen */
  if(hash->entry && hash->entry->release)
    hash->entry->release( &hash->entry );

  if(hash->oy_->deallocateFunc_)
  {
    /*oyDeAlloc_f deallocateFunc = hash->oy_->deallocateFunc_;*/

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( hash->member );
     */
  }
}

/** Function    oyHash_Init__Members
 *  @memberof   oyHash_s
 *  @brief      Custom Hash constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  hash  the Hash object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyHash_Init__Members( oyHash_s_ * hash )
{
  oyAllocHelper_m_( hash->oy_->hash_ptr_,
                    unsigned char,
                    OY_HASH_SIZE*2,
                    hash->oy_->allocateFunc_,
                    oyHash_Release_( &hash ));

  return 0;
}

/** Function    oyHash_Copy__Members
 *  @memberof   oyHash_s
 *  @brief      Custom Hash copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyHash_s_ input object
 *  @param[out]  dst  the output oyHash_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyHash_Copy__Members( oyHash_s_ * dst, oyHash_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */

  return error;
}

/* } Include "Hash.private_custom_definitions.c" */



/** @internal
 *  Function oyHash_New_
 *  @memberof oyHash_s_
 *  @brief   allocate a new oyHash_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyHash_s_ * oyHash_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_HASH_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyHash_s_ * s = 0;

  if(s_obj)
    s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyHash_s_) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyHash_Copy;
  s->release = (oyStruct_Release_f) oyHash_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Hash constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_HASH_S, s );
  /* ---- end of custom Hash constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */


  
  

  
  /* ---- start of custom Hash constructor ----- */
  error += oyHash_Init__Members( s );
  /* ---- end of custom Hash constructor ------- */
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyHash_Copy__
 *  @memberof oyHash_s_
 *  @brief   real copy a Hash object
 *
 *  @param[in]     hash                 Hash struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyHash_s_ * oyHash_Copy__ ( oyHash_s_ *hash, oyObject_s object )
{
  oyHash_s_ *s = 0;
  int error = 0;

  if(!hash || !object)
    return s;

  s = oyHash_New_( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Hash copy constructor ----- */
    error = oyHash_Copy__Members( s, hash );
    /* ---- end of custom Hash copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyHash_Release_( &s );

  return s;
}

/** @internal
 *  Function oyHash_Copy_
 *  @memberof oyHash_s_
 *  @brief   copy or reference a Hash object
 *
 *  @param[in]     hash                 Hash struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyHash_s_ * oyHash_Copy_ ( oyHash_s_ *hash, oyObject_s object )
{
  oyHash_s_ *s = hash;

  if(!hash)
    return 0;

  if(hash && !object)
  {
    s = hash;
    
    if(s->entry && s->entry->copy)
      s->entry = s->entry->copy( s->entry, 0 );

    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyHash_Copy__( hash, object );

  return s;
}
 
/** @internal
 *  Function oyHash_Release_
 *  @memberof oyHash_s_
 *  @brief   release and possibly deallocate a Hash object
 *
 *  @param[in,out] hash                 Hash struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyHash_Release_( oyHash_s_ **hash )
{
  /* ---- start of common object destructor ----- */
  oyHash_s_ *s = 0;

  if(!hash || !*hash)
    return 0;

  s = *hash;

  *hash = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom Hash destructor ----- */
  oyHash_Release__Members( s );
  /* ---- end of custom Hash destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Hash.private_methods_definitions.c" { */
/** Function  oyHash_Get_
 *  @memberof oyHash_s
 *  @brief    Get a new Oyranos cache entry
 *  @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
oyHash_s_ *         oyHash_Get_       ( const char        * hash_text,
                                        oyObject_s          object )
{
  oyHash_s_ * s = 0;
  int error = !hash_text;
  uint32_t * val = 0;

  if(error <= 0)
  {
    s = oyHash_New_(object);
    error = !s;
  }

  if(error <= 0)
  {
    val = (uint32_t*) s->oy_->hash_ptr_;

    if(oyStrlen_(hash_text) < OY_HASH_SIZE*2-1)
      memcpy(s->oy_->hash_ptr_, hash_text, oyStrlen_(hash_text)+1);
    else
#if 0
      error = oyMiscBlobGetMD5_( (void*)hash_text, oyStrlen_(hash_text),
                                 s->oy_->hash_ );
#else
      (*val) = oyMiscBlobGetL3_( (void*)hash_text, oyStrlen_(hash_text) );
#endif
  }

  if(error <= 0)
    error = oyObject_SetName(s->oy_, hash_text, oyNAME_NAME);

  return s;
}

/** Function  oyHash_IsOf_
 *  @memberof oyHash_s
 *  @brief    Hash is of type
 *  @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  3 december 2007 (API 0.1.8)
 */
int                oyHash_IsOf_      ( oyHash_s_         * hash,
                                       oyOBJECT_e          type )
{
  return (hash && hash->entry && hash->entry->type_ == type);
}

/** Function  oyHash_SetPointer_
 *  @memberof oyHash_s
 *  @internal
 *
 *  @param[in,out] hash                the to be set hash
 *  @param[in,out] obj                 the to be referenced object
 *  @return                            0 - good; >= 1 - error; < 0 issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/03 (Oyranos: 0.1.8)
 *  @date    2009/11/05
 */
int                oyHash_SetPointer_( oyHash_s_         * hash,
                                       oyStruct_s        * obj )
{
  if(hash)
  {
    if(obj && obj->copy)
      hash->entry = obj->copy( obj, 0 );
    else
      hash->entry = obj;
    return 0;
  } else
    return 1;
}

/** Function  oyHash_GetPointer_
 *  @memberof oyHash_s
 *  @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  3 december 2007 (API 0.1.8)
 */
oyStruct_s *       oyHash_GetPointer_( oyHash_s_         * hash,
                                       oyOBJECT_e          type )
{
  if(oyHash_IsOf_( hash, type))
    return hash->entry;
  else
    return 0;
}

/* } Include "Hash.private_methods_definitions.c" */

