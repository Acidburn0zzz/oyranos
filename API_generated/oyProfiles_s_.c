/** @file oyProfiles_s_.c

   [Template file inheritance graph]
   +-> oyProfiles_s_.template.c
   |
   +-> BaseList_s_.c
   |
   +-- Base_s_.c

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



  
#include "oyProfiles_s.h"
#include "oyProfiles_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "Profiles.private_custom_definitions.c" { */
/** Function    oyProfiles_Release__Members
 *  @memberof   oyProfiles_s
 *  @brief      Custom Profiles destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  profiles  the Profiles object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyProfiles_Release__Members( oyProfiles_s_ * profiles )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &profiles->member );
   */

  if(profiles->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = profiles->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( profiles->member );
     */
  }
}

/** Function    oyProfiles_Init__Members
 *  @memberof   oyProfiles_s
 *  @brief      Custom Profiles constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  profiles  the Profiles object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyProfiles_Init__Members( oyProfiles_s_ * profiles )
{
  return 0;
}

/** Function    oyProfiles_Copy__Members
 *  @memberof   oyProfiles_s
 *  @brief      Custom Profiles copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyProfiles_s_ input object
 *  @param[out]  dst  the output oyProfiles_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyProfiles_Copy__Members( oyProfiles_s_ * dst, oyProfiles_s_ * src)
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

/* } Include "Profiles.private_custom_definitions.c" */



/** @internal
 *  Function oyProfiles_New_
 *  @memberof oyProfiles_s_
 *  @brief   allocate a new oyProfiles_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyProfiles_s_ * oyProfiles_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_PROFILES_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyProfiles_s_ * s = 0;

  if(s_obj)
    s = (oyProfiles_s_*)s_obj->allocateFunc_(sizeof(oyProfiles_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyProfiles_s_) );
  if(error)
    WARNc_S( "memset failed" );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyProfiles_Copy;
  s->release = (oyStruct_Release_f) oyProfiles_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Profiles constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_PROFILES_S, (oyPointer)s );
  /* ---- end of custom Profiles constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom Profiles constructor ----- */
  error += oyProfiles_Init__Members( s );
  /* ---- end of custom Profiles constructor ------- */
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyProfiles_Copy__
 *  @memberof oyProfiles_s_
 *  @brief   real copy a Profiles object
 *
 *  @param[in]     profiles                 Profiles struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyProfiles_s_ * oyProfiles_Copy__ ( oyProfiles_s_ *profiles, oyObject_s object )
{
  oyProfiles_s_ *s = 0;
  int error = 0;

  if(!profiles || !object)
    return s;

  s = (oyProfiles_s_*) oyProfiles_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Profiles copy constructor ----- */
    error = oyProfiles_Copy__Members( s, profiles );
    /* ---- end of custom Profiles copy constructor ------- */
    
    
    
    
    
    s->list_ = oyStructList_Copy( profiles->list_, s->oy_ );

  }

  if(error)
    oyProfiles_Release_( &s );

  return s;
}

/** @internal
 *  Function oyProfiles_Copy_
 *  @memberof oyProfiles_s_
 *  @brief   copy or reference a Profiles object
 *
 *  @param[in]     profiles                 Profiles struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyProfiles_s_ * oyProfiles_Copy_ ( oyProfiles_s_ *profiles, oyObject_s object )
{
  oyProfiles_s_ *s = profiles;

  if(!profiles)
    return 0;

  if(profiles && !object)
  {
    s = profiles;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyProfiles_Copy__( profiles, object );

  return s;
}
 
/** @internal
 *  Function oyProfiles_Release_
 *  @memberof oyProfiles_s_
 *  @brief   release and possibly deallocate a Profiles list
 *
 *  @param[in,out] profiles                 Profiles struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyProfiles_Release_( oyProfiles_s_ **profiles )
{
  /* ---- start of common object destructor ----- */
  oyProfiles_s_ *s = 0;

  if(!profiles || !*profiles)
    return 0;

  s = *profiles;

  *profiles = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom Profiles destructor ----- */
  oyProfiles_Release__Members( s );
  /* ---- end of custom Profiles destructor ------- */
  
  
  
  

  oyStructList_Release( &s->list_ );


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Profiles.private_methods_definitions.c" { */

/* } Include "Profiles.private_methods_definitions.c" */

