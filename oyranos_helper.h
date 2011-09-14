/** @file oyranos_helper.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    helpers
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/09/02
 *
 *  Dont use in non Oyranos projects.
 */


#ifndef OYRANOS_HELPER_H
#define OYRANOS_HELPER_H

#include "config.h"
#include "oyranos_debug.h"
#include "oyStruct_s.h"
#if defined(OY_CONFIG_H)
#include "oyranos_core.h"
#else
#include "oyranos/oyranos.h"
#endif

#include "oyranos_helper_macros.h"

#include <ctype.h>  /* toupper */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* --- memory handling --- */

void* oyAllocateFunc_           (size_t        size);
void* oyAllocateWrapFunc_       (size_t        size,
                                 oyAlloc_f     allocate_func);
void  oyDeAllocateFunc_         (void *        data);


extern intptr_t oy_observe_pointer_;
extern int oy_debug_memory;
extern int oy_debug_signals;
extern const char * oy_backtrace;



/* --- string helpers --- */


/* --- file i/o helpers --- */

/* complete an name from file including oyResolveDirFileName_ */
char*   oyMakeFullFileDirName_     (const char* name);
/* find an file/dir and do corrections on  ~ ; ../  */
char*   oyResolveDirFileName_      (const char* name);
char*   oyExtractPathFromFileName_ (const char* name);
char*   oyGetHomeDir_              ();
char*   oyGetParent_               (const char* name);
struct oyFileList_s {
  oyOBJECT_e type;                  /* oyOBJECT_FILE_LIST_S_ */
  int hopp;
  const char* coloursig;
  int mem_count;
  int count_files;
  char** names;
};
typedef struct oyFileList_s oyFileList_s;
int     oyRecursivePaths_      (int (*doInPath) (oyFileList_s *,
                                const char*,const char*),
                                struct oyFileList_s        * data,
                                const char ** path_names,
                                int           path_count);

int oyIsDir_      (const char* path);
int oyIsFile_     (const char* fileName);
int oyIsFileFull_ (const char* fullFileName, const char * read_mode);
int oyMakeDir_    (const char* path);
int  oyRemoveFile_                   ( const char        * full_file_name );

int   oyWriteMemToFile_ (const char* name, const void* mem, size_t size);

#define OY_FILE_APPEND 0x01
#define OY_FILE_NAME_SEARCH 0x02
#define OY_FILE_TEMP_DIR 0x04
int  oyWriteMemToFile2_              ( const char        * name,
                                       void              * mem,
                                       size_t              size,
                                       uint32_t            flags,
                                       char             ** result,
                                       oyAlloc_f           allocateFunc );
char* oyReadFileToMem_  (const char* fullFileName, size_t *size,
                         oyAlloc_f     allocate_func);
char * oyReadStdinToMem_             ( size_t            * size,
                                       oyAlloc_f           allocate_func );
char * oyReadFilepToMem_             ( FILE              * file,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func );


/* --- mathematical helpers --- */

/* --- miscellaneous helpers --- */

int                oyMiscBlobGetMD5_ ( void              * buffer,
                                       size_t              size,
                                       unsigned char     * md5_return );
uint32_t           oyMiscBlobGetL3_  ( void              * buffer,
                                       size_t              size );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_HELPER_H */
