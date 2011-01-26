/** @file oyranos_internal.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    misc internal API
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/24
 */


#ifndef OYRANOS_INTERNAL_H
#define OYRANOS_INTERNAL_H

#include "config.h"
#include "oyranos.h"
#include "oyranos_version.h"
#include "oyranos_i18n.h"
#include "oyranos_texts.h"
#include "oyStruct_s.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @internal
    @brief used by sort engine */
struct oyComp_s_ {
  oyOBJECT_e           type_;         /*!< internal struct type oyOBJECT_COMP_S_ */
  struct oyComp_s_ *next;   /* chain connection */
  struct oyComp_s_ *begin;  /* chain connection */
  char            *name;   /* key name */
  char            *val;    /* its value */
  int              hits;   /* weighting */
};

typedef struct oyComp_s_ oyComp_t_;

/**@internal A small search engine
 *
 * for one simple, single list, dont mix lists!!
 * name and val are not alloced or freed 
 */


oyComp_t_* oyInitComp_      (oyComp_t_ *compare, oyComp_t_ *top);
oyComp_t_* oyAppendComp_    (oyComp_t_ *list,    oyComp_t_ *new_);
void    oySetComp_         (oyComp_t_ *compare, const char* keyName,
                            const char* value, int hits );
void    oyDestroyCompList_ (oyComp_t_* list);
char*   printComp          (oyComp_t_* entry);


int     oySetProfile_Block                (const char      * name, 
                                           void            * mem,
                                           size_t            size,
                                           oyPROFILE_e       type,
                                           const char      * comnt);


/* separate from the external functions */
int   oyPathsCount_             (void);
char* oyPathName_               (int           number,
                                 oyAlloc_f     allocate_func);
int   oyPathAdd_                (const char* pathname);
void  oyPathRemove_             (const char* pathname);
/*void  oyPathSleep_              (const char* pathname);
void  oyPathActivate_           (const char* pathname);*/
char* oyGetPathFromProfileName_ (const char*   profilename,
                                 oyAlloc_f     allocate_func);

enum {
  oyUSER_SYS = 0,
  oyUSER,
  oySYS
};


/* device profiles */
/** \internal enum identifying device types for distinguishing in searches */
typedef enum  {
  /*oyNOTYPE,*/
  oyDISPLAY,          /**< dynamic viewing */
  oyPRINTER,          /**< static media (dye, ink, offset, imagesetters) */
  oySCANNER,          /**< contact digitiser */
  oyCAMERA            /**< virtual or contactless image capturing */
} oyDEVICETYP_e;

#define oyDEVICE_PROFILE oyDEFAULT_PROFILE_END

char* oyGetDeviceProfile                  (oyDEVICETYP_e typ,
                                           const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3,
                                           oyAlloc_f    );

int	oySetDeviceProfile                (oyDEVICETYP_e typ,
                                           const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3,
                                           const char* profilename,
                                           const void* mem,
                                           size_t size);
int oyEraseDeviceProfile                  (oyDEVICETYP_e typ,
                                           const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_INTERNAL_H */
