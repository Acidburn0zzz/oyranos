/** Function  oyConnectorImaging_GetDataTypes
 *  @memberof oyConnectorImaging_s
 *  @brief    Get capabilities of oyConnectorImaging_s
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetDataTypes (
                                       oyConnectorImaging_s  * c,
                                       const oyDATATYPE_e   ** data_types,
                                       int                   * data_types_n )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)c;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_IMAGING_S, return 1 )

  if(data_types)
    *data_types = s->data_types;
  if(data_types_n)
    *data_types_n = s->data_types_n;

  return 0;
}
/** Function  oyConnectorImaging_GetChannelTypes
 *  @memberof oyConnectorImaging_s
 *  @brief    Get capabilities of oyConnectorImaging_s
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetChannelTypes (
                                       oyConnectorImaging_s  * c,
                                       const oyCHANNELTYPE_e** channel_types,
                                       int                   * channel_types_n )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)c;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_IMAGING_S, return 1 )

  if(channel_types)
    *channel_types = s->channel_types;
  if(channel_types_n)
    *channel_types_n = s->channel_types_n;

  return 0;
}

/** Function  oyConnectorImaging_GetDataTypes
 *  @memberof oyConnectorImaging_s
 *  @brief    Get capabilities of oyConnectorImaging_s
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetCapability (
                                       oyConnectorImaging_s    * c,
                                       oyCONNECTOR_IMAGING_CAP_e type )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)c;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_IMAGING_S, return 0 )

  switch(type)
  {
    case oyCONNECTOR_IMAGING_CAP_IS_PLUG: return s->is_plug;
    case oyCONNECTOR_IMAGING_CAP_MAX_COLOUR_OFFSET: return s->max_colour_offset;
    case oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT: return s->min_channels_count;
    case oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT: return s->max_channels_count;
    case oyCONNECTOR_IMAGING_CAP_MIN_COLOUR_COUNT: return s->min_colour_count;
    case oyCONNECTOR_IMAGING_CAP_MAX_COLOUR_COUNT: return s->max_colour_count;
    case oyCONNECTOR_IMAGING_CAP_CAN_PLANAR: return s->can_planar;
    case oyCONNECTOR_IMAGING_CAP_CAN_INTERWOVEN: return s->can_interwoven;
    case oyCONNECTOR_IMAGING_CAP_CAN_SWAP: return s->can_swap;
    case oyCONNECTOR_IMAGING_CAP_CAN_SWAP_BYTES: return s->can_swap_bytes;
    case oyCONNECTOR_IMAGING_CAP_CAN_REVERT: return s->can_revert;
    case oyCONNECTOR_IMAGING_CAP_CAN_PREMULTIPLIED_ALPHA: return s->can_premultiplied_alpha;
    case oyCONNECTOR_IMAGING_CAP_CAN_NONPREMULTIPLIED_ALPHA: return s->can_nonpremultiplied_alpha;
    case oyCONNECTOR_IMAGING_CAP_CAN_SUBPIXEL: return s->can_subpixel;
    case oyCONNECTOR_IMAGING_CAP_ID: return s->id;
    case oyCONNECTOR_IMAGING_CAP_IS_MANDATORY: return s->is_mandatory;
    default: return 0; 
  }
  return 0;
}
