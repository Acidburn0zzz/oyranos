/** @file oyCMMapi8_s_.c

   [Template file inheritance graph]
   +-> oyCMMapi8_s_.template.c
   |
   +-> oyCMMapiFilter_s_.template.c
   |
   +-> oyCMMapi_s_.template.c
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



  
#include "oyCMMapi8_s.h"
#include "oyCMMapi8_s_.h"



#include "oyCMMapi_s_.h"
#include "oyCMMapiFilter_s_.h"



#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"



#include "oyCMMapiFilter_s_.h"
  


/* Include "CMMapi8.private_custom_definitions.c" { */
/** Function    oyCMMapi8_Release__Members
 *  @memberof   oyCMMapi8_s
 *  @brief      Custom CMMapi8 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi8  the CMMapi8 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapi8_Release__Members( oyCMMapi8_s_ * cmmapi8 )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi8->member );
   */

  if(cmmapi8->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapi8->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi8->member );
     */
  }
}

/** Function    oyCMMapi8_Init__Members
 *  @memberof   oyCMMapi8_s
 *  @brief      Custom CMMapi8 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi8  the CMMapi8 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi8_Init__Members( oyCMMapi8_s_ * cmmapi8 )
{
  return 0;
}

/** Function    oyCMMapi8_Copy__Members
 *  @memberof   oyCMMapi8_s
 *  @brief      Custom CMMapi8 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi8_s_ input object
 *  @param[out]  dst  the output oyCMMapi8_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi8_Copy__Members( oyCMMapi8_s_ * dst, oyCMMapi8_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */

  return 0;
}

/* } Include "CMMapi8.private_custom_definitions.c" */



/** @internal
 *  Function oyCMMapi8_New_
 *  @memberof oyCMMapi8_s_
 *  @brief   allocate a new oyCMMapi8_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi8_s_ * oyCMMapi8_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_API8_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMapi8_s_ * s = 0;

  if(s_obj)
    s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyCMMapi8_s_) );
  if(error)
    WARNc_S( "memset failed" );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyCMMapi8_Copy;
  s->release = (oyStruct_Release_f) oyCMMapi8_Release;

  s->oy_ = s_obj;

  
  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_S, (oyPointer)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_FILTER_S, (oyPointer)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  /* ---- start of custom CMMapi8 constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API8_S, (oyPointer)s );
  /* ---- end of custom CMMapi8 constructor ------- */
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += oyCMMapi_Init__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += oyCMMapiFilter_Init__Members( (oyCMMapiFilter_s_*)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  /* ---- start of custom CMMapi8 constructor ----- */
  error += oyCMMapi8_Init__Members( s );
  /* ---- end of custom CMMapi8 constructor ------- */
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyCMMapi8_Copy__
 *  @memberof oyCMMapi8_s_
 *  @brief   real copy a CMMapi8 object
 *
 *  @param[in]     cmmapi8                 CMMapi8 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi8_s_ * oyCMMapi8_Copy__ ( oyCMMapi8_s_ *cmmapi8, oyObject_s object )
{
  oyCMMapi8_s_ *s = 0;
  int error = 0;

  if(!cmmapi8 || !object)
    return s;

  s = (oyCMMapi8_s_*) oyCMMapi8_New( object );
  error = !s;

  if(!error) {
    
    
    
    /* ---- start of custom CMMapi copy constructor ----- */
    error = oyCMMapi_Copy__Members( (oyCMMapi_s_*)s, (oyCMMapi_s_*)cmmapi8 );
    /* ---- end of custom CMMapi copy constructor ------- */
    /* ---- start of custom CMMapiFilter copy constructor ----- */
    error = oyCMMapiFilter_Copy__Members( (oyCMMapiFilter_s_*)s, (oyCMMapiFilter_s_*)cmmapi8 );
    /* ---- end of custom CMMapiFilter copy constructor ------- */
    /* ---- start of custom CMMapi8 copy constructor ----- */
    error = oyCMMapi8_Copy__Members( s, cmmapi8 );
    /* ---- end of custom CMMapi8 copy constructor ------- */
    
    
    
    
  }

  if(error)
    oyCMMapi8_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapi8_Copy_
 *  @memberof oyCMMapi8_s_
 *  @brief   copy or reference a CMMapi8 object
 *
 *  @param[in]     cmmapi8                 CMMapi8 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi8_s_ * oyCMMapi8_Copy_ ( oyCMMapi8_s_ *cmmapi8, oyObject_s object )
{
  oyCMMapi8_s_ *s = cmmapi8;

  if(!cmmapi8)
    return 0;

  if(cmmapi8 && !object)
  {
    s = cmmapi8;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMapi8_Copy__( cmmapi8, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapi8_Release_
 *  @memberof oyCMMapi8_s_
 *  @brief   release and possibly deallocate a CMMapi8 object
 *
 *  @param[in,out] cmmapi8                 CMMapi8 struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyCMMapi8_Release_( oyCMMapi8_s_ **cmmapi8 )
{
  /* ---- start of common object destructor ----- */
  oyCMMapi8_s_ *s = 0;

  if(!cmmapi8 || !*cmmapi8)
    return 0;

  s = *cmmapi8;

  *cmmapi8 = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  
  
  /* ---- start of custom CMMapi destructor ----- */
  oyCMMapi_Release__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi destructor ------- */
  /* ---- start of custom CMMapiFilter destructor ----- */
  oyCMMapiFilter_Release__Members( (oyCMMapiFilter_s_*)s );
  /* ---- end of custom CMMapiFilter destructor ------- */
  /* ---- start of custom CMMapi8 destructor ----- */
  oyCMMapi8_Release__Members( s );
  /* ---- end of custom CMMapi8 destructor ------- */
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMapi8.private_methods_definitions.c" { */

/* } Include "CMMapi8.private_methods_definitions.c" */
