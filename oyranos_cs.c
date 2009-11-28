/** @file oyranos.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    public Oyranos API's
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 *
 *  ColorSync specific functions for Apple osX
 */



/* --- Helpers  --- */

/* --- static variables   --- */

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

/* --- function definitions --- */


#ifdef __APPLE__
#include <Carbon/Carbon.h>

typedef struct {
  char   *data;
  SInt32  size;
} refcon;

OSErr
oyFlattenProfileProc (
   SInt32 command, 
   SInt32 *size, 
   void *data, 
   void *refCon)
{
  /* Alle Bestandteile einsammeln */
  if(*size)
  {
    refcon *ref = (refcon*) refCon;

    char* block = NULL;

    oyAllocHelper_m_( block,char, ref->size + *size, oyAllocateFunc_, return 1);
    /* old data */
    if(ref->data && ref->size) {
      memcpy(block, ref->data, ref->size);
      oyDeAllocateFunc_(ref->data);
    }
    /* new data */
    memcpy( &block[ref->size], data, *size );

    ref->data = block;
    ref->size += *size;
  }
  DBG_PROG2_S("command:%d size:%d", (int)command, (int)*size)

  return 0;
}

OSErr
oyFlattenProfileProcSize (
   SInt32 command, 
   SInt32 *size, 
   void *data, 
   void *refCon)
{
  /* Alle Bestandteile einsammeln */
  if(*size)
  {
    refcon *ref = (refcon*) refCon;
    ref->size += *size;
  }
  DBG_PROG2_S("command:%d size:%d", (int)command, (int)*size)

  return 0;
}


int
oyGetProfileBlockOSX (CMProfileRef prof, char **block, size_t *size, oyAlloc_f allocate_func)
{
    CMProfileLocation loc;
    UInt32 locationSize = sizeof(CMProfileLocation);

  DBG_PROG_START

    NCMGetProfileLocation(prof, &loc, &locationSize);
    switch(loc.locType)
    {
      case cmNoProfileBase:
             DBG_PROG_S("The profile is a temporary profile.")
             break;
#if !__LP64__ && !TARGET_OS_WIN32 && 0
      case cmFileBasedProfile:
             DBG_PROG_S("The profile is a file based profile.")
             break;
      case cmHandleBasedProfile:
             DBG_PROG_S("The profile is a profile handle.")
             break;
      case cmPtrBasedProfile:
             DBG_PROG_S("The profile is a pinter based profile.")
             break;
      case cmProcedureBasedProfile:
             DBG_PROG_S("The profile is a prozedural profile.")
             break;
#endif
      case cmPathBasedProfile:
             DBG_PROG_S("The profile is a path profile.")
             break;
      case cmBufferBasedProfile:
             DBG_PROG_S("The profile is a memory block Profile.")
             break;
      default:
             DBG_PROG_S("no profile found?")
             break;
    }

#if 0
    /* only the size */
    if(*size == 0) {
      err = CMFlattenProfile ( prof, 0, oyFlattenProfileProcSize, &ref, &bol);
      *size = ref.size;
      return err;
    }
#endif

#if !__LP64__ && !TARGET_OS_WIN32 && 0
    refcon ref = {0,0};
    CMError err = 0;
    Boolean bol;
    err = CMFlattenProfile ( prof, 0, oyFlattenProfileProc, &ref, &bol);
    if(ref.size && ref.data)
    {
        *size = ref.size;
        oyAllocHelper_m_( *block, char, ref.size, allocate_func, return 1);
        memcpy(*block, ref.data, ref.size);
          DBG_PROG_V( *size )
    } else
#endif

    if(loc.locType == cmBufferBasedProfile)
    {
        *size = loc.u.bufferLoc.size;
        oyAllocHelper_m_( *block, char, *size, allocate_func, return 1);
        memcpy(*block, loc.u.bufferLoc.buffer, *size);
          DBG_PROG_V( *size )
    }
    else if(cmPathBasedProfile)
    {
      char * name = oyGetProfileNameOSX( prof, oyAllocateFunc_ );
      if(name)
      {
        *block = oyReadFileToMem_( name, size, allocate_func );
        oyFree_m_(name);
      }
    }

  DBG_PROG_ENDE
  return 0;
}

char*
oyGetProfileNameOSX (CMProfileRef prof, oyAlloc_f     allocate_func)
{
  char * name = NULL;
  CMProfileLocation loc;
  UInt32 locationSize = sizeof(CMProfileLocation);

  NCMGetProfileLocation(prof, &loc, &locationSize);

      switch(loc.locType)
      {
        case cmNoProfileBase:
               DBG_PROG_S("The profile is a temporary profile.")
               break;
#if !__LP64__ && !TARGET_OS_WIN32 && 0
        case cmFileBasedProfile:
               DBG_PROG_S("The profile is a file based profile.")
               {
                 CMFileLocation file_loc = loc.u.fileLoc;
                 FSSpec spec = file_loc.spec;
                 unsigned char *name_ = NULL;
                 FSRef ref;
                 CMError err = 0;

                 oyAllocHelper_m_( name_, unsigned char, 1024, 
                                   allocate_func, return 0 );
                 err = FSpMakeFSRef( &spec, &ref );
                 err = FSRefMakePath( &ref, name_, 1024 );
                 DBG_PROG1_S("file is at: %s\n", name_ );
                 if(err == noErr)
                   name = (char*) name_;
               }
               break;
        case cmHandleBasedProfile:
               DBG_PROG_S("The profile is a profile handle.")
               break;
        case cmPtrBasedProfile:
               DBG_PROG_S("The profile is a pinter based profile.")
               break;
        case cmProcedureBasedProfile:
               DBG_PROG_S("The profile is a prozedural profile.")
               break;
#endif
        case cmPathBasedProfile:
               DBG_PROG_S("The profile is a path profile.")
               {
                 CMPathLocation path_loc = loc.u.pathLoc;
                 name = oyStringCopy_(  path_loc.path, allocate_func );
                 DBG_PROG1_S( "file is to: %s\n", name );
               }
               break;
        case cmBufferBasedProfile:
               DBG_PROG_S("The profile is a memory block Profile.")
               break;
        default:
               DBG_PROG_S("no profile found?")
               break;
      }

  return name;
}

char * oyGetDefaultProfileNameOSX_   ( oyPROFILE_e         type )
{
  char* name = 0;
  
  DBG_PROG_START

  {
    OSType dataColorSpace = 0;
    CMError err;
    CMProfileRef prof=NULL;
    UInt32 version = 0;
    const oyOption_t_ * t = 0;

    err = CMGetColorSyncVersion(&version);
    if(err == noErr)
      DBG_PROG1_S( "ColorSync version: %d\n", (int)version );

    switch(type)
    {
      case oyEDITING_RGB:            /**< Rgb Editing (Workspace) Profile */
                dataColorSpace = cmRGBData; break;
      case oyEDITING_CMYK:           /**< Cmyk Editing (Workspace) Profile */
                dataColorSpace = cmCMYKData; break;
      case oyEDITING_XYZ:            /**< XYZ Editing (Workspace) Profile */
                dataColorSpace = cmXYZData; break;
      case oyEDITING_LAB:            /**< Lab Editing (Workspace) Profile */
                dataColorSpace = cmLabData; break;
      case oyEDITING_GRAY:           /**< Gray Editing (Workspace) Profile */
                dataColorSpace = cmGrayData; break;

      case oyASSUMED_RGB:            /**< standard RGB assumed source profile */
      case oyASSUMED_WEB:            /**< std internet assumed source static_profile*/
      case oyASSUMED_CMYK:           /**< standard Cmyk assumed source profile*/
      case oyASSUMED_XYZ:            /**< standard XYZ assumed source profile */
      case oyASSUMED_LAB:            /**< standard Lab assumed source profile */
      case oyASSUMED_GRAY:           /**< standard Gray assumed source profile*/
      case oyPROFILE_PROOF:          /**< standard proofing profile */
                t = oyOptionGet_((oyWIDGET_e)type);
                if(t && t->config_string)
                  name = oyGetKeyString_( t->config_string, oyAllocateFunc_ );
                break;
      case oyDEFAULT_PROFILE_START:
      case oyDEFAULT_PROFILE_END:
                break;
    }

    if(dataColorSpace)
      err = CMGetDefaultProfileBySpace ( dataColorSpace, &prof);
    if(!name && !err && prof)
    {
      CMProfileLocation loc;
      UInt32 locationSize = sizeof(CMProfileLocation);

      err = NCMGetProfileLocation( prof, &loc, &locationSize );
      
      switch(loc.locType)
      {
        case cmNoProfileBase:
               DBG_PROG_S("The profile is a temporary profile.")
               break;
#if !__LP64__ && !TARGET_OS_WIN32 && 0
        case cmFileBasedProfile:
               DBG_PROG_S("The profile is a file based profile.")
               {
                 CMFileLocation file_loc = loc.u.fileLoc;
                 FSSpec spec = file_loc.spec;
                 unsigned char *name_ = NULL;
                 FSRef ref;

                 oyAllocHelper_m_( name_, unsigned char, MAX_PATH, oyAllocateFunc_,
                                   return NULL );
                 err = FSpMakeFSRef( &spec, &ref );
                 err = FSRefMakePath( &ref, name_, 1024 );
                 DBG_PROG1_S( "file is at: %s\n", name_ );
                 if(err == noErr)
                   name = (char*) name_;
               }
               break;
        case cmHandleBasedProfile:
               DBG_PROG_S(("The profile is a profile handle."))
               break;
        case cmPtrBasedProfile:
               DBG_PROG_S(("The profile is a pinter based profile."))
               break;
        case cmProcedureBasedProfile:
               DBG_PROG_S(("The profile is a prozedural profile."))
               break;
#endif
        case cmPathBasedProfile:
               DBG_PROG_S("The profile is a path profile.")
               {
                 CMPathLocation path_loc = loc.u.pathLoc;
                 char* path = path_loc.path;
                 char *name_ = NULL;

                 STRING_ADD( name_, path );
                 DBG_PROG1_S( "file is to: %s\n", name_ );
                 name = name_;
               }
               break;
        case cmBufferBasedProfile:
               DBG_PROG_S("The profile is a memory block Profile.")
               break;
        default:
               DBG_PROG_S("no profile found?")
               break;
      }
    } else if(!name)
      DBG_PROG2_S( "could not find:%d Profile:%s\n", (int)err, prof ? "no" : "yes" );

    err = CMCloseProfile( prof );
  }

  DBG_PROG_ENDE
  return name;
}
#endif


