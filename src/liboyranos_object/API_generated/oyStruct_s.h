/** @file oyStruct_s.h

   [Template file inheritance graph]
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



#ifndef OY_STRUCT_S_H
#define OY_STRUCT_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyStruct_s oyStruct_s;


/* Include "Struct.public.h" { */
#include <oyranos_core.h>

const char * oyStruct_TypeToText     ( const oyStruct_s  * st );

typedef oyStruct_s * (*oyStruct_Copy_f ) ( oyStruct_s *, oyPointer );
typedef int       (*oyStruct_Release_f ) ( oyStruct_s ** );
typedef oyPointer (*oyStruct_LockCreate_f)(oyStruct_s * obj );

/* Locking function declarations { */
extern oyStruct_LockCreate_f   oyStruct_LockCreateFunc_;
extern oyLockRelease_f         oyLockReleaseFunc_;
extern oyLock_f                oyLockFunc_;
extern oyUnLock_f              oyUnLockFunc_;
/* } Locking function declarations */

void         oyThreadLockingSet      ( oyStruct_LockCreate_f  createLockFunc,
                                       oyLockRelease_f     releaseLockFunc,
                                       oyLock_f            lockFunc,
                                       oyUnLock_f          unlockFunc );


/* } Message function declarations */

/* } Include "Struct.public.h" */


/* Include "Struct.dox" { */
/** @struct oyStruct_s
 *  @ingroup objects_generic
 *  @brief Oyranos base structure
 *
 *  All Oyranos structures should be castable to this basic one.
 *
 *  @note New templates will not be created automaticly [notemplates]
 *
 *  @version Oyranos: x.x.x
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */

/* } Include "Struct.dox" */

struct oyStruct_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};




/* Include "Struct.public_methods_declarations.h" { */
OYAPI const char *  OYEXPORT
                oyStruct_GetText        ( oyStruct_s        * obj,
                                          oyNAME_e            name_type,
                                          uint32_t            flags );

OYAPI oyAlloc_f  OYEXPORT
                 oyStruct_GetAllocator   ( oyStruct_s        * obj );
OYAPI oyDeAlloc_f  OYEXPORT
                 oyStruct_GetDeAllocator ( oyStruct_s        * obj );
OYAPI int  OYEXPORT
                 oyStruct_CheckType      ( oyStruct_s        * obj,
                                           oyOBJECT_e          type );
OYAPI int  OYEXPORT
                 oyStruct_GetId          ( oyStruct_s        * obj );
OYAPI oyPointer  OYEXPORT
                 oyStruct_Allocate       ( oyStruct_s        * st,
                                           size_t              size );

/* } Include "Struct.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_STRUCT_S_H */
