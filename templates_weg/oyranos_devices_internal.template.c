{% include "source_file_header.txt" %}

#include "oyranos_i18n.h"
#include "oyranos_string.h"
#include "oyranos_devices_internal.h"


/** \addtogroup devices_handling Device API
 *  @{
 */

/** @internal
 *  Function oyDeviceRegistrationCreate_
 *  @brief   combine a device registration
 *
 *  @param[in]     device_type         the device type ::oyFILTER_REG_TYPE,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        the device class, e.g. "monitor",
 *                                     ::oyFILTER_REG_APPLICATION
 *  @param[in]     key                 key_name to add at ::oyFILTER_REG_OPTION
 *  @param[in]     old_text            string to reuse
 *  @return                            the new registration
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
char * oyDeviceRegistrationCreate_   ( const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       char              * old_text )
{
  char * text = old_text;
  const char * device_type_ = device_type;

  if(!device_type_)
    device_type_ = OY_TYPE_STD;

  STRING_ADD( text, "//" );
  STRING_ADD( text, device_type_ );
  STRING_ADD( text, "/config" );
  if(device_class)
  {
    STRING_ADD( text, "." );
    STRING_ADD( text, device_class );
  }
  if(key)
  {
    STRING_ADD( text, "/" );
    STRING_ADD( text, key );
  }

  return text;
}

/** @internal
 *  Function oyOptions_SetDeviceTextKey_
 *  @brief   set a device option
 *
 *  @param[in,out] options             options for the device
 *  @param[in]     device_type         the device type ::oyFILTER_REG_TYPE,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        the device class, e.g. "monitor",
 *                                     ::oyFILTER_REG_APPLICATION
 *  @param[in]     key                 key_name to add at ::oyFILTER_REG_OPTION
 *  @param[in]     value               value of type ::oyVAL_STRING
 *  @return                            the new registration
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
int    oyOptions_SetDeviceTextKey_   ( oyOptions_s_      * options,
                                       const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       const char        * value )
{
  char * text = 0;
  int error = 0;

  text = oyDeviceRegistrationCreate_( device_type, device_class,
                                          key, text );
  error = oyOptions_SetFromText( (oyOptions_s**)&options, text, value, OY_CREATE_NEW );

  oyFree_m_( text );

  return error;
}

/** @internal
 *  Function oyOptions_SetRegistrationTextKey_
 *  @brief   set a device option
 *
 *  @param[in,out] options             options for the device
 *  @param[in]     registration        the registration string
 *  @param[in]     key                 key_name to add at ::oyFILTER_REG_OPTION
 *  @param[in]     value               value of type ::oyVAL_STRING
 *  @return                            the new registration
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/09 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int    oyOptions_SetRegistrationTextKey_(
                                       oyOptions_s_      * options,
                                       const char        * registration,
                                       const char        * key,
                                       const char        * value )
{
  char * text = 0;
  int error = 0;

  STRING_ADD( text, registration );
  STRING_ADD( text, "/" );
  STRING_ADD( text, key );

  error = oyOptions_SetFromText( (oyOptions_s**)&options, text, value, OY_CREATE_NEW );

  oyFree_m_( text );

  return error;
}
/**
 *  @} *//* devices_handling
 */
