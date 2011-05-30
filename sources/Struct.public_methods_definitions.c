#include "oyranos_cmm.h" /* oyObjectInfoStatic_s */

const char * (*oyStruct_GetTextFromModule_p) (
                                       oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags ) = NULL;

/** Function oyStruct_GetText
 *  @memberof oyStruct_s
 *  @brief   get a text dump
 *
 *  Ask the object type module for text informations.
 *
 *  @param         obj                 the object
 *  @param         name_type           the text type
 *  @param         flags
 *                                     - 0: get object infos
 *                                     - 1: get object type infos
 *  @return                            the text
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/14 (Oyranos: 0.1.10)
 *  @date    2009/09/15
 */
const char * oyStruct_GetText        ( oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags )
{
  int error = !obj;
  const char * text = 0;
  oyOBJECT_e type = oyOBJECT_NONE;

  if(!error)
    text = oyObject_GetName( obj->oy_, oyNAME_NICK );

  if(!error && !text)
  {
    type = obj->type_;

    if(oyStruct_GetTextFromModule_p)
      text = oyStruct_GetTextFromModule_p(obj, name_type, flags);
#ifdef USE_MODULES /* FIXME move to oyStruct_GetTextFromModule_p */
    if(type)
    {
      oyCMMapiFilters_s * apis;
      int apis_n = 0, i,j;
      oyCMMapi9_s * cmm_api9 = 0;
      char * api_reg = 0;

      apis = oyCMMsGetFilterApis_( 0,0, api_reg, oyOBJECT_CMM_API9_S,
                                   oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                   0, 0);
      apis_n = oyCMMapiFilters_Count( apis );
      for(i = 0; i < apis_n; ++i)
      {
        cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );

        j = 0;
        while( cmm_api9->object_types && cmm_api9->object_types[j] &&
               cmm_api9->object_types[j]->type == oyOBJECT_CMM_DATA_TYPES_S &&
               cmm_api9->object_types[j]->oyCMMobjectGetText &&
               cmm_api9->object_types[j]->id == obj->type_ )
        {
          text = cmm_api9->object_types[j]->oyCMMobjectGetText( flags ? 0 : obj,
                                                   name_type, 0 );
          if(text)
            break;
          ++j;
        }
        if(cmm_api9->release)
          cmm_api9->release( (oyStruct_s**)&cmm_api9 );

        if(text)
          break;
      }
      oyCMMapiFilters_Release( &apis );
    }
#endif
  }

  if(!error && !text)
    text = oyStructTypeToText( obj->type_ );

  return text;
}

/** @internal
 *  Function oyStruct_TypeToText
 *  @brief   Objects type to small string
 *  @deprecated
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/24
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 */
const char * oyStruct_TypeToText     ( const oyStruct_s  * st )
{ return oyStructTypeToText( st->type_ ); }

/** Function oyStruct_GetAllocator
 *  @brief   get the memory allocator of a object
 *
 *  @version Oyranos: 0.3.0
 *  @date    2011/01/30
 *  @since   2011/01/30 (Oyranos: 0.3.0)
 */
oyAlloc_f    oyStruct_GetAllocator   ( oyStruct_s        * obj )
{
  if(obj && obj->type_ != oyOBJECT_NONE && obj->oy_)
    return obj->oy_->allocateFunc_;
  else
    return 0;
}
/** Function oyStruct_GetDeAllocator
 *  @brief   get the memory allocator of a object
 *
 *  @version Oyranos: 0.3.0
 *  @date    2011/01/30
 *  @since   2011/01/30 (Oyranos: 0.3.0)
 */
oyDeAlloc_f  oyStruct_GetDeAllocator ( oyStruct_s        * obj )
{
  if(obj && obj->type_ != oyOBJECT_NONE && obj->oy_)
    return obj->oy_->deallocateFunc_;
  else
    return 0;
}

/* Locking function definitions { */
oyPointer  oyStruct_LockCreateDummy_   ( oyStruct_s      * obj )    {return 0;}
void       oyLockReleaseDummy_         ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )   {;}
void       oyLockDummy_                ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )   {;}
void       oyUnLockDummy_              ( oyPointer         look,
                                         const char      * marker,
                                         int               line ) {;}

oyStruct_LockCreate_f   oyStruct_LockCreateFunc_ = oyStruct_LockCreateDummy_;
oyLockRelease_f         oyLockReleaseFunc_ = oyLockReleaseDummy_;
oyLock_f                oyLockFunc_        = oyLockDummy_;
oyUnLock_f              oyUnLockFunc_      = oyUnLockDummy_;

/** Function: oyThreadLockingSet
 *  @brief set locking functions for threaded applications
 *
 *  @since Oyranos: version 0.1.8      
 *  @date  14 january 2008 (API 0.1.8)
 */
void         oyThreadLockingSet        ( oyStruct_LockCreate_f  createLockFunc,
                                         oyLockRelease_f   releaseLockFunc,
                                         oyLock_f          lockFunc,
                                         oyUnLock_f        unlockFunc )
{
  oyStruct_LockCreateFunc_ = createLockFunc;
  oyLockReleaseFunc_ = releaseLockFunc;
  oyLockFunc_ = lockFunc;
  oyUnLockFunc_ = unlockFunc;

  if(!oyStruct_LockCreateFunc_ && !oyLockReleaseFunc_ &&
     !oyLockFunc_ && !oyUnLockFunc_)
  {
    oyStruct_LockCreateFunc_ = oyStruct_LockCreateDummy_;
    oyLockReleaseFunc_ = oyLockReleaseDummy_;
    oyLockFunc_ = oyLockDummy_;
    oyUnLockFunc_ = oyUnLockDummy_;
  }
}

/** Function oyStruct_CheckType
 *  @brief   check if the object is a class or is inherited of a class
 *
 *  @version Oyranos: 0.3.0
 *  @date    2011/02/15
 *  @since   2011/02/02 (Oyranos: 0.3.0)
 */
int          oyStruct_CheckType      ( oyStruct_s        * obj,
                                       oyOBJECT_e          type )
{
  int error = 1;
  if(obj && obj->type_ != oyOBJECT_NONE)
  {
    /* check dynamic objects */
    if(obj->type_ != oyOBJECT_OBJECT_S && obj->oy_)
    {
      int n, i;
      oyOBJECT_e * inheritance = 0;

      if(obj->oy_->type_ == oyOBJECT_OBJECT_S)
        inheritance = obj->oy_->parent_types_;
      else if(obj->oy_->type_ == oyOBJECT_INFO_STATIC_S)
      {
        oyObjectInfoStatic_s * static_object = (oyObjectInfoStatic_s*)obj->oy_;
        inheritance = static_object->inheritance;
      } else
        return 2;

      if((uint32_t)inheritance < (uint32_t)oyOBJECT_MAX)
      {
        WARNc1_S( "non plausible inheritance pointer: %s", 
                  oyStruct_GetInfo(obj,0) );
        return 2;
      }

      n = inheritance[0];
      for(i = 1; i <= n; ++i)
        if(inheritance[i] == type)
        {
          if(inheritance[0] == i)
            error = 0;
          else
            error = -1;
          break;
        }
    } else
    /* check static objects */
      if(obj->type_ == type)
        error = 0;
  } else
    error = 2;

  return error;
}
/* } Locking function definitions */
