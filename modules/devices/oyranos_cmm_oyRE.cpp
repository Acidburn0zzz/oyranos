/** @file oyranos_cmm_oyRE.cc
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Yiannis Belias
 *
 *  @brief    Oyranos "RAW image & EXIF data" device backend for Oyranos
 *  @internal
 *  @author   Yiannis Belias <orion@linux.gr>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2009/07/05
 */

#include <oyranos_cmm.h>
#include <oyranos_string.h>

#include <string.h>
#include <stdarg.h>
#include <stddef.h>           /* size_t ptrdiff_t */
#include <stdio.h>
#include <stdlib.h>

#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>

#include <libraw.h>

#include <string>
#include <sstream>

#include "oyRE_help.c"
#include "helper.c"
/* --- internal definitions --- */

#define PRFX "raw-image.oyRE: "
#define PRFX_EXIF "EXIF_"
#define PRFX_LRAW "LRAW_"
/* select a own four byte identifier string instead of "dDev" and replace the
 * dDev in the below macros.
 */
#define CMM_NICK "oyRE"
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.device.icc_profile.raw-image." CMM_NICK
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

#define catCMMfunc(nick,func) nick ## func

#define CMMInit                 catCMMfunc( oyRE , CMMInit )
#define CMMallocateFunc         catCMMfunc( oyRE , CMMallocateFunc )
#define CMMdeallocateFunc       catCMMfunc( oyRE , CMMdeallocateFunc )
#define CMMMessageFuncSet       catCMMfunc( oyRE , CMMMessageFuncSet )
#define ConfigsFromPatternUsage catCMMfunc( oyRE , ConfigsFromPatternUsage )
#define DeviceFromName_         catCMMfunc( oyRE , DeviceFromName_ )
#define GetDevices              catCMMfunc( oyRE , GetDevices )
#define _api8                   catCMMfunc( oyRE , _api8 )
#define _rank_map               catCMMfunc( oyRE , _rank_map )
#define Configs_FromPattern     catCMMfunc( oyRE , Configs_FromPattern )
#define Config_Rank             catCMMfunc( oyRE , Config_Rank )
#define GetText                 catCMMfunc( oyRE , GetText )
#define _texts                  catCMMfunc( oyRE , _texts )
#define _cmm_module             catCMMfunc( oyRE , _cmm_module )
#define _api8_ui                catCMMfunc( oyRE, _api8_ui )
#define Api8UiGetText           catCMMfunc( oyRE, Api8UiGetText )
#define _api8_ui_texts          catCMMfunc( oyRE, _api8_ui_texts )
#define _api8_icon              catCMMfunc( oyRE, _api8_icon )

#define _DBG_FORMAT_ "%s:%d %s()"
#define _DBG_ARGS_ __FILE__,__LINE__,__func__
#define _(x) x

const char * GetText                 ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
const char * Api8UiGetText           ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );

/** @instance _rank_map
 *  @brief    oyRankPad map for mapping device to configuration informations
 *
 *  @version Oyranos: 0.3.0
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2011/04/07
 *
 *  \todo { Pick better rank fields }
 */
oyRankPad _rank_map[] = {
   {const_cast < char *>("device_name"), 0, 0, 0},                   /**< Unused?*/
   {const_cast < char *>(PRFX_LRAW "driver_version"), 2, -1, 0},               /**< is good */
   {const_cast < char *>("profile_name"), 0, 0, 0},                  /**< non relevant for device properties*/
       /* EXIF Fields */
   {const_cast < char *>(PRFX_EXIF "manufacturer"), 1, -1, 0},              /**< is nice */
   {const_cast < char *>(PRFX_EXIF "model"), 5, -5, 0},             /**< important, should not fail */
   {const_cast < char *>(PRFX_EXIF "serial"), 1, 0, 0},                    /**< is nice */
   {const_cast < char *>(PRFX_EXIF "Photo_ISOSpeedRatings"), 1, 0, 0},    /**< is nice */
   {const_cast < char *>(PRFX_EXIF "Photo_ExposureProgram"), 1, 0, 0},    /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Photo_Flash"), 1, 0, 0},              /**< nice to match */

      /*Makernote Fields - no 1-1 mapping with exif tags */
      /* Makernote Tags: Serial Number */
   {const_cast < char *>(PRFX_EXIF "Canon_SerialNumber"), 1, 0, 0},        /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Fujifilm_SerialNumber"), 1, 0, 0},     /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Nikon3_SerialNumber"), 1, 0, 0},       /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Nikon3_SerialNO"), 1, 0, 0},           /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Olympus_SerialNumber"), 1, 0, 0},      /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Olympus_SerialNumber2"), 1, 0, 0},     /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "OlympusEq_SerialNumber"), 1, 0, 0},    /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "OlympusEq_InternalSerialNumber"), 1, 0, 0},/**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Sigma_SerialNumber"), 1, 0, 0},        /**< nice to match */

      /* Makernote Tags: Lens */
   {const_cast < char *>(PRFX_EXIF "CanonCs_LensType"), 1, 0, 0},          /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "CanonCs_Lens"), 1, 0, 0},              /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Minolta_LensID"), 1, 0, 0},            /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Nikon1_AuxiliaryLens"), 1, 0, 0},      /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Nikon2_AuxiliaryLens"), 1, 0, 0},      /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Nikon3_AuxiliaryLens"), 1, 0, 0},      /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Nikon3_LensType"), 1, 0, 0},           /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Nikon3_Lens"), 1, 0, 0},               /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "OlympusEq_LensType"), 1, 0, 0},        /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "OlympusEq_LensSerialNumber"), 1, 0, 0},/**< nice to match */
   {const_cast < char *>(PRFX_EXIF "OlympusEq_LensFirmwareVersion"), 1, 0, 0},/**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Pentax_LensType"), 1, 0, 0},           /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Pentax_LensInfo"), 1, 0, 0},           /**< nice to match */
   {const_cast < char *>(PRFX_EXIF "Sigma_LensRange"), 1, 0, 0},           /**< nice to match */

       /* Possibly not relevant options are marked with: O->Output R->Repair */
       /* LibRaw Options affecting open_file() */                     
       /* LibRaw Options affecting unpack() */                        
   {const_cast < char *>(PRFX_LRAW "use_camera_wb"), 1, -1, 0},                /**< is nice */
   {const_cast < char *>(PRFX_LRAW "use_camera_matrix"), 1, -1, 0},            /**< is nice */
   {const_cast < char *>(PRFX_LRAW "half_size"), 1, -1, 0},                    /**< is nice */
   {const_cast<char*>(PRFX_LRAW "filtering_mode"), 1, -1, 0},                   /**< is nice */
//This is a bit-field. Out of all the possible flags, only LIBRAW_FILTERING_NORAWCURVE
//seems to be relevant to color [From LibRaw API docs]:               
//This bit turns off tone curve processing (for tone curves read from file metadata or
//calculated from constants). This setting is supported only for bayer-pattern cameras
//with tone curve;                                                    
   {const_cast < char *>(PRFX_LRAW "threshold"), 1, -1, 0},                    /**< is nice */ /*R*/
   {const_cast < char *>(PRFX_LRAW "aber"), 1, -1, 0},                         /**< is nice */ /*R*/
       /* LibRaw Options affecting dcraw_process() */                 
   {const_cast < char *>(PRFX_LRAW "greybox"), 1, -1, 0},                      /**< is nice */
   {const_cast < char *>(PRFX_LRAW "gamm"), 1, -1, 0},                         /**< is nice */
   {const_cast < char *>(PRFX_LRAW "user_mul"), 1, -1, 0},                     /**< is nice */
   {const_cast < char *>(PRFX_LRAW "bright"), 1, -1, 0},                       /**< is nice */
   {const_cast < char *>(PRFX_LRAW "four_color_rgb"), 1, -1, 0},               /**< is nice */
   {const_cast < char *>(PRFX_LRAW "highlight"), 1, -1, 0},                    /**< is nice */
   {const_cast < char *>(PRFX_LRAW "use_auto_wb"), 1, -1, 0},                  /**< is nice */
   {const_cast < char *>(PRFX_LRAW "output_color"), 1, -1, 0},                 /**< is nice */
   {const_cast < char *>(PRFX_LRAW "camera_profile"), 1, -1, 0},               /**< is nice */
   {const_cast < char *>(PRFX_LRAW "output_bps"), 1, -1, 0},                   /**< is nice */
   {const_cast < char *>(PRFX_LRAW "user_qual"), 1, -1, 0},                    /**< is nice */
   {const_cast < char *>(PRFX_LRAW "user_black"), 1, -1, 0},                   /**< is nice */
   {const_cast < char *>(PRFX_LRAW "user_sat"), 1, -1, 0},                     /**< is nice */
   {const_cast < char *>(PRFX_LRAW "med_passes"), 1, -1, 0},                   /**< is nice */
   {const_cast < char *>(PRFX_LRAW "auto_bright_thr"), 1, -1, 0},              /**< is nice */
   {const_cast < char *>(PRFX_LRAW "no_auto_bright"), 1, -1, 0},               /**< is nice */
       /* Extra options (user supplied) */                            
   {const_cast < char *>(PRFX_LRAW "illumination_source"), 1, -1, 0},          /**< is nice */
   {0, 0, 0, 0}                                                      /**< end of list */
};


      oyMessage_f message = 0;

      extern oyCMMapi8_s _api8;


bool is_raw( int id );
int DeviceFromContext(oyConfig_s **config, libraw_output_params_t *params);
int DeviceFromHandle_opt(oyConfig_s *device, oyOption_s *option);

/* --- implementations --- */

int CMMInit( oyStruct_s * filter )
{
   int error = 0;
   return error;
}

oyPointer CMMallocateFunc(size_t size)
{
   oyPointer p = 0;
   if (size)
      p = malloc(size);
   return p;
}

void CMMdeallocateFunc(oyPointer mem)
{
   if (mem)
      free(mem);
}

/** @func  CMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int CMMMessageFuncSet(oyMessage_f message_func)
{
   message = message_func;
   return 0;
}

void ConfigsFromPatternUsage(oyStruct_s * options)
{
    /** oyMSG_WARN should make shure our message is visible. */
   message(oyMSG_WARN, options, _DBG_FORMAT_ "\n %s",
           _DBG_ARGS_, "The following help text informs about the communication protocol.");
   message(oyMSG_WARN, options, "%s()\n%s", __func__, help_message);

   return;
}

class exif2options {
   public:
      exif2options(Exiv2::ExifData *exif_data, oyOptions_s **options)
         :_exif_data_(exif_data), _options_(options) {}

      //FIXME We assume that any exif tag has exactly 2 '.' (dots)
      int add(const char *name)
      {
         std::string n(name), exif(name);
         n.replace(n.find("."),1,"_");
         if(n == PRFX_EXIF "Image_Make")
           n = PRFX_EXIF "manufacturer";
         else if(n == PRFX_EXIF "Image_Model")
           n = PRFX_EXIF "model";
         else if(n.find("SerialNumber") != -1)
           n = PRFX_EXIF "serial";

         exif.replace(0,5,"Exif.");

         std::ostringstream registration;
         registration << CMM_BASE_REG OY_SLASH << n.c_str();
         Exiv2::ExifKey key( exif );
         Exiv2::ExifData::iterator pos = _exif_data_->findKey(key);
         if (pos != _exif_data_->end()) {
            return oyOptions_SetFromText( _options_,
                                          registration.str().c_str(),
                                          pos->toString().c_str(), OY_CREATE_NEW );
         } else {
            return 0;
         }
      }
   private:
      Exiv2::ExifData *_exif_data_;
      oyOptions_s **_options_;
};

/** @internal
 *  @brief Get the options from EXIF Tags, and put them in a configuration object
 *
 *  @param[in]image              The Exiv2 raw image
 *  @param[out]options           The configuration object to hold the option-value pairs
 *
 * \todo { Untested }
 */
int DeviceFromHandle(oyOptions_s **options, Exiv2::Image::AutoPtr image)
{
   int error = 0;

   image->readMetadata();
   Exiv2::ExifData &exif_data = image->exifData();
   if (exif_data.empty()) //FIXME Add error message
      return 1;

   exif2options e2o(&image->exifData(),options);

   // Standard EXIF Tags
   error += e2o.add(PRFX_EXIF "Image.Model");
   error += e2o.add(PRFX_EXIF "Image.Make");
   error += e2o.add(PRFX_EXIF "Photo.ISOSpeedRatings");
   error += e2o.add(PRFX_EXIF "Photo.ExposureProgram");
   error += e2o.add(PRFX_EXIF "Photo.Flash");
 
   // Makernote Tags: Serial Number
   error += e2o.add(PRFX_EXIF "Canon.SerialNumber");
   error += e2o.add(PRFX_EXIF "Fujifilm.SerialNumber");
   //e2o.add("Minolta"); //Non existant?
   error += e2o.add(PRFX_EXIF "Nikon3.SerialNumber");
   error += e2o.add(PRFX_EXIF "Nikon3.SerialNO");
   error += e2o.add(PRFX_EXIF "Olympus.SerialNumber");
   error += e2o.add(PRFX_EXIF "Olympus.SerialNumber2");
   error += e2o.add(PRFX_EXIF "OlympusEq.SerialNumber");
   error += e2o.add(PRFX_EXIF "OlympusEq.InternalSerialNumber");
   //e2o.add(PRFX_EXIF "Panasonic.InternalSerialNumber"); //!in libexiv2?
   //e2o.add("Pentax"); //Non existant?
   error += e2o.add(PRFX_EXIF "Sigma.SerialNumber");
   //e2o.add("Sony"); //Non existant?

   // Makernote Tags: Lens
   error += e2o.add(PRFX_EXIF "CanonCs.LensType");
   error += e2o.add(PRFX_EXIF "CanonCs.Lens");
   //e2o.add("Fujifilm"); //Non existant?
   error += e2o.add(PRFX_EXIF "Minolta.LensID");
   error += e2o.add(PRFX_EXIF "Nikon1.AuxiliaryLens");
   error += e2o.add(PRFX_EXIF "Nikon2.AuxiliaryLens");
   error += e2o.add(PRFX_EXIF "Nikon3.AuxiliaryLens");
   error += e2o.add(PRFX_EXIF "Nikon3.LensType");
   error += e2o.add(PRFX_EXIF "Nikon3.Lens");
   error += e2o.add(PRFX_EXIF "OlympusEq.LensType");
   error += e2o.add(PRFX_EXIF "OlympusEq.LensSerialNumber");
   error += e2o.add(PRFX_EXIF "OlympusEq.LensFirmwareVersion");
   //e2o.add(PRFX_EXIF "Panasonic.ConversionLens"); //!in libexiv2?
   //e2o.add(PRFX_EXIF "Panasonic.LensType"); //!in libexiv2?
   //e2o.add(PRFX_EXIF "Panasonic.LensSerialNumber"); //!in libexiv2?
   error += e2o.add(PRFX_EXIF "Pentax.LensType");
   error += e2o.add(PRFX_EXIF "Pentax.LensInfo");
   error += e2o.add(PRFX_EXIF "Sigma.LensRange");

   return error;
}

/** Function Configs_FromPattern
 *  @brief   CMM_NICK oyCMMapi8_s raw images
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 *
 *  todo { Error Handling }
 */
int Configs_FromPattern(const char *registration, oyOptions_s * options, oyConfigs_s ** s)
{
   oyOption_s *context_opt = NULL, *handle_opt = NULL;

   int error = 0;
   const char *command_list = 0, *command_properties = 0;

   int rank = oyFilterRegistrationMatch(_api8.registration, registration,
                                        oyOBJECT_CMM_API8_S);
   oyAlloc_f allocateFunc = malloc;

   printf(PRFX "Entering %s(). Options:\n%s", __func__, oyOptions_GetText(options, oyNAME_NICK));

   /* "error handling" section */
   if (rank == 0) {
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "Registration match Failed. Options:\n%s", _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK)
          );
      return 1;
   }
   if (s == NULL) {
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "oyConfigs_s is NULL! Options:\n%s", _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK)
          );
      return 1;
   }
   if (*s != NULL) {
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "Devices struct already present! Options:\n%s", _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK)
          );
      return 1;
   }

   /* "help" call section */
   if (oyOptions_FindString(options, "command", "help") || !options || !oyOptions_Count(options)) {
      ConfigsFromPatternUsage((oyStruct_s *) options);
      return 0;
   }

   /*Prepare for "driver_version" option*/
   int driver_version_number = LibRaw::versionNumber();
   const char *driver_version_string = LibRaw::version();
   oyOption_s *version_opt = NULL;

   command_list = oyOptions_FindString(options, "command", "list");
   command_properties = oyOptions_FindString(options, "command", "properties");

   context_opt = oyOptions_Find(options, "device_context");
   handle_opt = oyOptions_Find(options, "device_handle");
   version_opt = oyOptions_Find(options, "driver_version");

   oyConfig_s *device = NULL;
   device = oyConfig_New(CMM_BASE_REG, 0);
   /*A device *must* have a device_name!*/
   oyOptions_SetFromText(&device->backend_core, CMM_BASE_REG OY_SLASH "device_name", "dummy", OY_CREATE_NEW);
   oyOptions_SetFromText(&device->backend_core, CMM_BASE_REG OY_SLASH "prefix",
                          PRFX_EXIF "," PRFX_LRAW, OY_CREATE_NEW);

   if (command_list) {
      /* "list" call section */

      printf(PRFX "Backend core:\n%s", oyOptions_GetText(device->backend_core, oyNAME_NICK));
      printf(PRFX "Data:\n%s", oyOptions_GetText(device->data, oyNAME_NICK));

      const char **device_list = LibRaw::cameraList();
      /*int num_devices = LibRaw::cameraCount();*/

      /*Handle "driver_version" option [IN] */
      if (version_opt) {
         error = oyOptions_SetFromText(&device->data,
                                       CMM_BASE_REG OY_SLASH "driver_version_string",
                                       driver_version_string,
                                       OY_CREATE_NEW);
         error = oyOptions_SetFromInt(&device->data,
                                      CMM_BASE_REG OY_SLASH "driver_version_number",
                                      driver_version_number,
                                      0,
                                      OY_CREATE_NEW);
      }
      

      /*Handle "device_handle" option [OUT:informative]*/
      if (!handle_opt)
         error = oyOptions_SetFromText(&device->data,
                                       CMM_BASE_REG OY_SLASH "device_handle",
                                       "filename\nblob",
                                       OY_CREATE_NEW);

      /*Handle "supported_devices_info" option [OUT:informative]*/
      if (!handle_opt) {
         int i = 0;
         while(device_list[i++]);
         printf("################### Found %d devices #######################\n",i-1);
         char *string_list = 0;
         const char ** cameras = device_list;
         i = 0;
         int mnft_n = -1;
         ptrdiff_t len;
         char manufacturer[128] = {0},
              manufacturer_old[128] = {0};

         if(cameras)
         while(cameras[i])
         {
           len = strchr(cameras[i], ' ') - cameras[i];
           memcpy( manufacturer, cameras[i], len );
           manufacturer[len] = '\000';
           if(strcmp(manufacturer,manufacturer_old) != 0)
           {
             mnft_n++;
             if(mnft_n)
               STRING_ADD(string_list,"\n");
             STRING_ADD(string_list,manufacturer);
             STRING_ADD(string_list,":");
             sprintf( manufacturer_old, "%s", manufacturer );
           } else
           if(cameras[i+1])
             STRING_ADD( string_list,";");

           STRING_ADD( string_list,
                       &cameras[i][len+1] );
           //cout << "  " << &cameras[i][len] << endl;
           ++i;
         }

         oyOptions_SetFromText ( &device->data,
                                 CMM_BASE_REG OY_SLASH "supported_devices_info",
                                 string_list,
                                 OY_CREATE_NEW | OY_STRING_LIST );
      }

      /*Copy the rank map*/
      device->rank_map = oyRankMapCopy(_rank_map, device->oy_->allocateFunc_);

   } else if (command_properties) {
      /* "properties" call section */

      const char * t = oyOptions_GetText(device->backend_core, oyNAME_NICK);
      printf(PRFX "Backend core:\n%s", t?t:"");
      t = oyOptions_GetText(device->data, oyNAME_NICK);
      printf(PRFX "Data:\n%s", t?t:"");

      /*Handle "device_handle" option [IN]*/
      if (handle_opt) {
         DeviceFromHandle_opt(device, handle_opt);
      } else { /*Bail out if no "device_handle" given*/
         printf("Missing \"device_handle\" option\n");
         return -1;
      }

      /*Handle "device_context" option [IN]*/
      if (context_opt) {
         libraw_output_params_t *device_context =
            *(libraw_output_params_t**)oyOption_GetData(context_opt, NULL, allocateFunc);
         DeviceFromContext(&device, device_context);
      }

      /* add device calibration to ICC device profiles meta tag */
      oyProfile_s * profile = (oyProfile_s*)oyOptions_GetType( options, -1,
                                        "icc_profile.add_meta",
                                        oyOBJECT_PROFILE_S );
      if(profile)
      {
        oyOptions_s * options = 0;
        size_t size = 0;
        oyPointer data = oyProfile_GetMem( profile, &size, 0, malloc );
        oyProfile_s * p = oyProfile_FromMem( size, data, 0, 0 );
        /* Filter the typical name spaces for embedding into the ICC profile.  */
        error = oyOptions_SetFromText( &options, "///key_prefix_required",
                                       PRFX_EXIF "." PRFX_LRAW ".prefix",
                                       OY_CREATE_NEW );
        oyProfile_DeviceAdd( p, device, options );
        oyProfile_Release( &profile );
        oyOptions_MoveInStruct( &device->data,
                                CMM_BASE_REG OY_SLASH "icc_profile.add_meta",
                                (oyStruct_s**)&p, OY_CREATE_NEW );
      }

      /*Copy the rank map*/
      device->rank_map = oyRankMapCopy(_rank_map, device->oy_->allocateFunc_);
   } else {
      /* not to be reached section, e.g. warning */
      oyConfig_Release(&device);

      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
           "This point should not be reached. Options:\n%s", _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK)
       );

      ConfigsFromPatternUsage((oyStruct_s *) options);
   } //End of Configuration

   /*Return the Configuration object*/
   oyConfigs_s *devices = *s;
   if(!devices)
     devices = oyConfigs_New(0);
   oyConfigs_MoveIn(devices, &device, -1);
   *s = devices;
 
   return error;
}

/** Function Configs_Modify
 *  @brief   oyCMMapi8_s SANE scanner manipulation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/08/21
 *
 *  \todo { Unimplemented }
 */
int Configs_Modify(oyConfigs_s * devices, oyOptions_s * options)
{
   oyAlloc_f allocateFunc = malloc;

   printf(PRFX "Entering %s(). Options:\n%s", __func__, oyOptions_GetText(options, oyNAME_NICK));

   /* "error handling" section */
   if (!devices || !oyConfigs_Count(devices)) {
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "No devices given! Options:\n%s", _DBG_ARGS_,
              oyOptions_GetText(options, oyNAME_NICK) );
      return 1;
   }

    /* "help" call section */
   if (oyOptions_FindString(options, "command", "help") || !options || !oyOptions_Count(options)) {
      ConfigsFromPatternUsage((oyStruct_s *) options);
      return 0;
   }

   /*Init for "driver_version" option*/
   int driver_version_number = LibRaw::versionNumber();
   const char *driver_version_string = LibRaw::version();

   const char *command_list = oyOptions_FindString(options, "command", "list");
   const char *command_properties = oyOptions_FindString(options, "command", "properties");

   oyOption_s *context_opt = oyOptions_Find(options, "device_context");
   oyOption_s *handle_opt = oyOptions_Find(options, "device_handle");
   oyOption_s *version_opt = oyOptions_Find(options, "driver_version");

   int num_devices = oyConfigs_Count(devices);
   if (command_list) {
      /* "list" call section */

      for (int i = 0; i < num_devices; ++i) {
         int error = 0;
         oyConfig_s *device = oyConfigs_Get(devices, i);

         printf(PRFX "Backend core:\n%s", oyOptions_GetText(device->backend_core, oyNAME_NICK));
         printf(PRFX "Data:\n%s", oyOptions_GetText(device->data, oyNAME_NICK));

         /*Handle "driver_version" option [IN/OUT] */
         oyOption_s *version_opt_dev = oyConfig_Find(device, "driver_version");
         if (!version_opt_dev && version_opt) {
            error = oyOptions_SetFromText(&device->backend_core,
                                          CMM_BASE_REG OY_SLASH "driver_version_string",
                                          driver_version_string,
                                          OY_CREATE_NEW);
            error = oyOptions_SetFromInt(&device->backend_core,
                                         CMM_BASE_REG OY_SLASH "driver_version_number",
                                         driver_version_number,
                                         0,
                                         OY_CREATE_NEW);
         }

         //FIXME: Should probably be removed, because command_list creates it anyway
         /*Handle "device_handle" option [OUT:informative]*/
         oyOption_s *handle_opt_dev = oyConfig_Find(device, "device_handle");
         if (!handle_opt_dev && handle_opt)
            error = oyOptions_SetFromText(&device->data,
                                          CMM_BASE_REG OY_SLASH "device_handle",
                                          "filename\nblob",
                                          OY_CREATE_NEW);

         /*Handle "supported_devices_info" option [OUT:informative]*/
         //FIXME: It is not here, because command_list creates it anyway
         //If used, it should become a function.

         /*Create static rank_map, if not already there*/
         if (!device->rank_map)
            device->rank_map = oyRankMapCopy(_rank_map, device->oy_->allocateFunc_);

         /*Cleanup*/
         oyConfig_Release(&device);
         oyOption_Release(&version_opt_dev);
         oyOption_Release(&handle_opt_dev);
      }
   } else if (command_properties) {
      /* "properties" call section */

      for (int i = 0; i < num_devices; ++i) {
         oyConfig_s *device = oyConfigs_Get(devices, i);
         oyConfig_s *device_new = oyConfig_New(CMM_BASE_REG, 0);

         printf(PRFX "Backend core:\n%s\n", oyOptions_GetText(device->backend_core, oyNAME_NICK));
         printf(PRFX "Data:\n%s\n", oyOptions_GetText(device->data, oyNAME_NICK));

         /* All previous device properties are considered obsolete
          * and a new device is created. Basic options are moved from
          * the old to new device */

         /*Get the "device_handle" from old device
          * and populate device_new with H/W options [OUT]*/
         oyOption_s *handle_opt_dev = oyConfig_Find(device, "device_handle");
         if (handle_opt_dev) {
            DeviceFromHandle_opt(device_new, handle_opt_dev);
            oyOption_s *tmp = oyOption_Copy(handle_opt_dev, 0);
            oyOptions_MoveIn(device_new->data, &tmp, -1);
            oyOption_Release(&handle_opt_dev);
         } else { /*Ignore device without a "device_handle"*/
           if(oyOptions_Count( device->backend_core ) < 2)
             message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ ": %s\n",
                     _DBG_ARGS_, "The \"device_handle\" is missing from config object!");
            oyConfig_Release(&device);
            oyConfig_Release(&device_new);
            continue;
         }

         /*Handle "driver_version" option [OUT] */
         oyOption_s *version_opt_dev = oyConfig_Find(device, "driver_version");
         if (version_opt_dev) {
            oyOption_s *tmp = oyOption_Copy(version_opt_dev, 0);
            oyOptions_MoveIn(device_new->backend_core, &tmp, -1);
            oyOption_Release(&version_opt_dev);
         }

         /*Handle "device_context" option [OUT]*/
         oyOption_s *context_opt_dev = oyConfig_Find(device, "device_context");
         if (context_opt_dev) {
            libraw_output_params_t *device_context =
               *(libraw_output_params_t**)oyOption_GetData(context_opt, NULL, allocateFunc);
            DeviceFromContext(&device_new, device_context);
            free(device_context);

            oyOption_s *tmp = oyOption_Copy(context_opt_dev, 0);
            oyOptions_MoveIn(device_new->data, &tmp, -1);
            oyOption_Release(&context_opt_dev);
         }

         /*Copy the rank map*/
         device_new->rank_map = oyRankMapCopy(_rank_map, device->oy_->allocateFunc_);

         /*Cleanup*/
         /* Remove old, add new device */
         oyConfig_Release(&device);
         oyConfigs_ReleaseAt(devices, i);
         oyConfigs_MoveIn(devices, &device_new, -1);
      }
   }

   return 0;
}

/** Function Config_Rank
 *  @brief   CMM_NICK oyCMMapi8_s device check
 *
 *  @param[in]     config              the monitor device configuration
 *  @return                            rank value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int Config_Rank(oyConfig_s * config)
{
   int error = !config, rank = 1;

   if (!config) {
      message(oyMSG_DBG, (oyStruct_s *) config, _DBG_FORMAT_ "\n " "No config argument provided.\n", _DBG_ARGS_);
      return 0;
   }

   if (error <= 0) {
      /* evaluate a driver specific part of the options */
   }

   return rank;
}

const char * Api8UiGetText           ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  static char * category = 0;
  if(strcmp(select,"name") == 0 ||
     strcmp(select,"help") == 0)
  {
    /* The "help" and "name" texts are identical, as the module contains only
     * one filter to provide help for. */
    return GetText(select,type,context);
  }
  else if(strcmp(select, "device_class") == 0)
    {
        if(type == oyNAME_NICK)
            return "raw-image";
        else if(type == oyNAME_NAME)
            return _("RawCamera");
        else
            return _("Raw camera data, which are in file containing raw sensor data from a camera still picture.");
    }
  else if(strcmp(select, "icc_profile_class")==0)
    {
      return "input";
    } 
  else if(strcmp(select,"category") == 0)
  {
    if(!category)
    {
      /* The following strings must match the categories for a menu entry. */
      const char * i18n[] = {_("Colour"),_("Device"),_("CameraRaw"),0};
      int len =  strlen(i18n[0]) + strlen(i18n[1]) + strlen(i18n[2]);
      category = (char*)malloc( len + 64 );
      if(category)
        sprintf( category,"%s/%s/%s", i18n[0], i18n[1], i18n[2] );
      else
        message(oyMSG_WARN, (oyStruct_s *) 0, _DBG_FORMAT_ "\n " "Could not allocate enough memory.", _DBG_ARGS_);
    }
         if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return category;
    else
      return category;
  }
  return 0;
}
const char * _api8_ui_texts[] = {"name", "help", "device_class", "icc_profile_class", "category", 0};

/** @instance _api8_ui
 *  @brief    oydi oyCMMapi4_s::ui implementation
 *
 *  The UI parts for oyRE devices.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/06 (Oyranos: 0.1.10)
 *  @date    2009/12/28
 */
oyCMMui_s _api8_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,3,0},                            /**< int32_t module_api[3] */

  0, /* oyCMMFilter_ValidateOptions_f */
  0, /* oyWidgetEvent_f */

  "Colour/Device/CameraRaw", /* category */
  0,   /* const char * options */

  0,    /* oyCMMuiGet_f oyCMMuiGet */

  Api8UiGetText,  /* oyCMMGetText_f getText */
  _api8_ui_texts  /* (const char**)texts */
};

oyIcon_s _api8_icon = {
  oyOBJECT_ICON_S, 0,0,0, 0,0,0, (char*)"oyranos_logo.png"
};

/** @instance _api8
 *  @brief    CMM_NICK oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/12/28
 */
oyCMMapi8_s _api8 = {
   oyOBJECT_CMM_API8_S,
   0, 0, 0,
   (oyCMMapi_s*) 0,                                                   /**< next */
   CMMInit,                                                           /**< oyCMMInit_f      oyCMMInit */
   CMMMessageFuncSet,                                                 /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
   const_cast < char *>(CMM_BASE_REG),                                /**< registration */
   {0, 2, 0},                                                         /**< int32_t version[3] */
   {0,3,0},                                                        /**< int32_t module_api[3] */
   0,                                                                 /**< char * id_ */
   0,                                                                 /**< oyCMMapi5_s * api5_ */
   Configs_FromPattern,                                               /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
   Configs_Modify,                                                    /**< oyConfigs_Modify_f oyConfigs_Modify */
   Config_Rank,                                                       /**< oyConfig_Rank_f oyConfig_Rank */

  &_api8_ui,                                                          /**< device class UI name and help */
  &_api8_icon,                                                        /**< device icon */

   _rank_map                                                          /**< oyRankPad ** rank_map */
};

/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 *
 *  \todo { Add usage info }
 */
const char *GetText(const char *select, oyNAME_e type,
                                       oyStruct_s        * context)
{
   if (strcmp(select, "name") == 0) {
      if (type == oyNAME_NICK)
         return _(CMM_NICK);
      else if (type == oyNAME_NAME)
         return _("Oyranos RAW Image");
      else
         return _("The raw image backend of Oyranos.");
   } else if (strcmp(select, "manufacturer") == 0) {
      if (type == oyNAME_NICK)
         return _("orionas");
      else if (type == oyNAME_NAME)
         return _("Yiannis Belias");
      else
         return
             _
             ("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
   } else if (strcmp(select, "copyright") == 0) {
      if (type == oyNAME_NICK)
         return _("MIT");
      else if (type == oyNAME_NAME)
         return _("Copyright (c) 2009 Kai-Uwe Behrmann; MIT");
      else
         return _("MIT license: http://www.opensource.org/licenses/mit-license.php");
   } else if (strcmp(select, "help") == 0) {
      if (type == oyNAME_NICK)
         return _("help");
      else if (type == oyNAME_NAME)
         return _("My filter introduction.");
      else
         return _("All the small details for using this module.");
   }
   return 0;
}
const char *_texts[5] = { "name", "copyright", "manufacturer", "help", 0 };

/** @instance _cmm_module
 *  @brief    CMM_NICK module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 *  @date    2009/06/23
 */
oyCMMInfo_s _cmm_module = {

   oyOBJECT_CMM_INFO_S,/**< ::type; the object type */
   0, 0, 0,            /**< static objects omit these fields */
   CMM_NICK,           /**< ::cmm; the four char filter id */
   (char *)"0.2",      /**< ::backend_version */
   GetText,            /**< ::getText; UI texts */
   (char **)_texts,    /**< ::texts; list of arguments to getText */
   OYRANOS_VERSION,    /**< ::oy_compatibility; last supported Oyranos CMM API*/

  /** ::api; The first filter api structure. */
   (oyCMMapi_s *) & _api8,

  /** ::icon; zero terminated list of a icon pyramid */
   {oyOBJECT_ICON_S, 0, 0, 0, 0, 0, 0, const_cast < char *>("oyranos_logo.png")},
};

/* Helper functions */

/** @internal
 * @brief Get the options from LibRaw, and put them in a configuration object
 *
 * @param[in]	params				The LibRaw parameter structure
 * @param[out]	config				The configuration object to hold the option-value pairs
 *
 * \todo { Untested }
 */
#define DFC_OPT_ADD_INT_ARR(name, i) if(!error) \
        error = oyOptions_SetFromInt( &((*config)->backend_core), \
                                      CMM_BASE_REG OY_SLASH PRFX_LRAW #name, \
                                      params->name[i], i, OY_CREATE_NEW );
#define DFC_OPT_ADD_INT(name) if(!error) \
        error = oyOptions_SetFromInt( &((*config)->backend_core), \
                                      CMM_BASE_REG OY_SLASH PRFX_LRAW #name, \
                                      params->name, 0, OY_CREATE_NEW );
#define DFC_OPT_ADD_FLOAT_ARR(name, i, n) if(!error) { \
if (i==n-1) { \
   oyOption_s *opt = oyOption_FromRegistration(CMM_BASE_REG OY_SLASH PRFX_LRAW #name, 0);\
   oyOption_SetFromDouble(opt, params->name[i], i, 0); \
   oyOptions_MoveIn((*config)->backend_core, &opt, -1); \
} else { \
   oyOption_s *opt = oyOptions_Find((*config)->backend_core, #name); \
   oyOption_SetFromDouble(opt, params->name[i], i, 0); \
} }
#define DFC_OPT_ADD_FLOAT(name) if(!error) { \
        oyOption_s *opt = oyOption_FromRegistration(CMM_BASE_REG OY_SLASH PRFX_LRAW #name, 0); \
        oyOption_SetFromDouble(opt, params->name, 0, 0); \
        oyOptions_MoveIn((*config)->backend_core, &opt, -1); \
}
int DeviceFromContext(oyConfig_s **config, libraw_output_params_t *params)
{
   int error = 0;

   DFC_OPT_ADD_FLOAT_ARR(aber,3,4) //4
   DFC_OPT_ADD_FLOAT_ARR(aber,2,4) //4
   DFC_OPT_ADD_FLOAT_ARR(aber,1,4) //4
   DFC_OPT_ADD_FLOAT_ARR(aber,0,4) //4
   DFC_OPT_ADD_FLOAT_ARR(gamm,4,5) //5
   DFC_OPT_ADD_FLOAT_ARR(gamm,3,5) //5
   DFC_OPT_ADD_FLOAT_ARR(gamm,2,5) //5
   DFC_OPT_ADD_FLOAT_ARR(gamm,1,5) //5
   DFC_OPT_ADD_FLOAT_ARR(gamm,0,5) //5
   DFC_OPT_ADD_FLOAT_ARR(user_mul,3,4) //4
   DFC_OPT_ADD_FLOAT_ARR(user_mul,2,4) //4
   DFC_OPT_ADD_FLOAT_ARR(user_mul,1,4) //4
   DFC_OPT_ADD_FLOAT_ARR(user_mul,0,4) //4

   DFC_OPT_ADD_FLOAT(auto_bright_thr)
   DFC_OPT_ADD_FLOAT(bright)
   DFC_OPT_ADD_FLOAT(threshold)

   DFC_OPT_ADD_INT(four_color_rgb)
   DFC_OPT_ADD_INT(half_size)
   DFC_OPT_ADD_INT(highlight)
   DFC_OPT_ADD_INT(med_passes)
   DFC_OPT_ADD_INT(no_auto_bright)
   DFC_OPT_ADD_INT(output_bps)
   DFC_OPT_ADD_INT(output_color)
   DFC_OPT_ADD_INT(use_auto_wb)
   DFC_OPT_ADD_INT(use_camera_matrix)
   DFC_OPT_ADD_INT(use_camera_wb)
   DFC_OPT_ADD_INT(user_black)
   DFC_OPT_ADD_INT(user_qual)
   DFC_OPT_ADD_INT(user_sat)

   DFC_OPT_ADD_INT_ARR(greybox,0) //4
   DFC_OPT_ADD_INT_ARR(greybox,1) //4
   DFC_OPT_ADD_INT_ARR(greybox,2) //4
   DFC_OPT_ADD_INT_ARR(greybox,3) //4
   DFC_OPT_ADD_INT(shot_select)

   return error;
}

int DeviceFromHandle_opt(oyConfig_s *device, oyOption_s *handle_opt)
{
   Exiv2::Image::AutoPtr device_handle;
   oyAlloc_f allocateFunc = malloc;
   if (handle_opt) {
      char * filename = NULL;
      size_t size = 0;
      const Exiv2::byte *raw_data = (Exiv2::byte*) 
                                           oyOption_GetData( handle_opt, &size,
                                                            allocateFunc );
      if(raw_data)
      {
        if (is_raw(Exiv2::ImageFactory::getType(raw_data, size)))
           device_handle = Exiv2::ImageFactory::open(raw_data, size);
      } else
      {
        filename = oyOption_GetValueText( handle_opt, malloc );
        if(filename)
        {
          if (is_raw(Exiv2::ImageFactory::getType(filename)))
            device_handle = Exiv2::ImageFactory::open(filename);
          printf("filename = %s\n", filename);
          free(filename); filename = 0;
        }
        else
          printf("Option \"device_handle\" is of a wrong type\n");
      }

      //The std::auto_ptr::get() method returns the pointer owned by the auto_ptr
      if (device_handle.get() && device_handle->good())
         DeviceFromHandle(&device->backend_core, device_handle);
      else {
         printf("Unable to open raw image.\n");
         return 1;
      }
   } else
      return 1;

   return 0;
}

bool is_raw( int id )
{
   //using namespace Exiv2::ImageType;
   switch (id) {
      case 7: //cr2:
      case 3: //crw:
      case 5: //mrw:
      case 9: //orf:
      case 8: //raf:
      case 16: //rw2:
         return true;
         break;
      default:
         return false;
   }
}
