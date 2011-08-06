/** Function  oyConversion_Correct
 *  @memberof oyConversion_s
 *  @brief    Check for correctly adhering to policies
 *
 *  Without any options the module selected with the registration argument shall
 *  perform graph analysis and correct the graph.
 *
 *  @par Typical Options:
 *  - "command"-"help" - a string option issuing a help text as message
 *  - "verbose" - reporting changes as message
 *
 *  TODO: display and selection of policy modules
 *
 *  @param   conversion                the to be checked configuration
 *  @param   registration              the to be used policy module
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED |
 *                                     OY_SELECT_FILTER |
 *                                     OY_SELECT_COMMON
 *  @param   options                   options to the policy module
 *  @return                            0 - indifferent, >= 1 - error
 *                                     + a message should be sent
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/24 (Oyranos: 0.1.10)
 *  @date    2009/07/24
 */
int                oyConversion_Correct (
                                       oyConversion_s    * conversion,
                                       const char        * registration,
                                       uint32_t            flags,
                                       oyOptions_s       * options )
{
  int error = 0;
  oyConversion_s_ * s = (oyConversion_s_*)conversion;
  const char * pattern = registration;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 1 )

  if(!s->input && !s->out_)
  {
    WARNc1_S( "%s",_("Found no node in conversion. give up") );
    return 1;
  }

  if(!error)
  {
    oyCMMapiFilters_s * apis;
    int apis_n = 0, i;
    oyCMMapi9_s_ * cmm_api9_ = 0;
    char * class, * api_reg;

    class = oyFilterRegistrationToText( pattern, oyFILTER_REG_TYPE, 0 );
    api_reg = oyStringCopy_("//", oyAllocateFunc_ );
    STRING_ADD( api_reg, class );
    oyFree_m_( class );

    apis = oyCMMsGetFilterApis_( 0,0, api_reg, oyOBJECT_CMM_API9_S,
                                 oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                 0,0 );
    apis_n = oyCMMapiFilters_Count( apis );
    for(i = 0; i < apis_n; ++i)
    {
      cmm_api9_ = (oyCMMapi9_s_*) oyCMMapiFilters_Get( apis, i );

      if(oyFilterRegistrationMatch( cmm_api9_->pattern, pattern, 0 ))
      {
        if(cmm_api9_->oyConversion_Correct)
          error = cmm_api9_->oyConversion_Correct( conversion, flags, options );
        if(error)
        {
          WARNc2_S( "%s %s",_("error in module:"), cmm_api9_->registration );
          return 1;
        }
      }

      if(cmm_api9_->release)
        cmm_api9_->release( (oyStruct_s**)&cmm_api9_ );
    }
    oyCMMapiFilters_Release( &apis );
  }

  return 0;
}

/** Function  oyConversion_CreateBasicPixels
 *  @memberof oyConversion_s
 *  @brief    Allocate initialise a basic oyConversion_s object
 *
 *  Provided options will be passed to oyFilterNode_NewWith(). There for no
 *  options defaults will be selected.
 *
 *  @param[in]     input               the input image data
 *  @param[in]     output              the output image data
 *  @param[in]     options             options to be passed to oyFilterCore_s
 *                                     Each option should be carefully named
 *                                     to be correctly selected by the intented
 *                                     filter (root, icc and output).
 *                                     The available options are particial
 *                                     defined by plugable policy modules,
 *                                     e.g. "oicc", and a filters own options.
 *  @param         object              the optional object
 *  @return                            the conversion context
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/08/01
 */
oyConversion_s   * oyConversion_CreateBasicPixels (
                                       oyImage_s         * input,
                                       oyImage_s         * output,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyConversion_s * s = 0;
  int error = !input || !output;
  oyFilterNode_s * in = 0, * out = 0;

  if(error <= 0)
  {
    s = oyConversion_New ( 0 );
    error = !s;

    if(error <= 0)
      in = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", options, 0 );
    if(error <= 0)
      error = oyConversion_Set( s, in, 0 );
    if(error <= 0)
      error = oyFilterNode_DataSet( in, (oyStruct_s*)input, 0, 0 );

    if(error <= 0)
      out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/icc", options, 0 );
    if(error <= 0)
      error = oyFilterNode_DataSet( out, (oyStruct_s*)output, 0, 0 );
    if(error <= 0)
      error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                    out, "//" OY_TYPE_STD "/data", 0 );

    in = out; out = 0;

    if(error <= 0)
      out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", options, 0 );
    if(error <= 0)
    {
      error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                    out, "//" OY_TYPE_STD "/data", 0 );
      if(error)
        WARNc1_S( "could not add  filter: %s\n", "//" OY_TYPE_STD "/output" );
    }
    if(error <= 0)
      error = oyConversion_Set( s, 0, out );
  }

  if(error)
    oyConversion_Release ( &s );

  return s;
}

/** Function  oyConversion_CreateBasicPixelsFromBuffers
 *  @memberof oyConversion_s
 *  @brief    One dimensional colour conversion context
 *
 *  The options are passed to oyConversion_CreateBasicPixels();
 *  The function does the lookups for the profiles and the modules contexts
 *  in the Oyranos cache on the fly.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/22 (Oyranos: 0.3.0)
 *  @date    2011/02/22
 */
oyConversion_s *   oyConversion_CreateBasicPixelsFromBuffers (
                                       oyProfile_s       * p_in,
                                       oyPointer           buf_in,
                                       oyDATATYPE_e        buf_type_in,
                                       oyProfile_s       * p_out,
                                       oyPointer           buf_out,
                                       oyDATATYPE_e        buf_type_out,
                                       oyOptions_s       * options,
                                       int                 count )
{
  oyImage_s * in  = NULL,
            * out = NULL;
  oyConversion_s * conv = NULL;

  if(count <= 0)
  {
    WARNc1_S("buffer requested with size of pixels: %d", count);
    return NULL;
  }

  if(!buf_in || !buf_out)
  {
    WARNc1_S("buffer%s missed", (!buf_in && !buf_out) ? "s" : "");
    return NULL;
  }

  in    = oyImage_Create( count, 1,
                         buf_in ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                         oyDataType_m(buf_type_in),
                         p_in,
                         0 );
  out   = oyImage_Create( count, 1,
                         buf_out ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                         oyDataType_m(buf_type_out),
                         p_out,
                         0 );

  conv   = oyConversion_CreateBasicPixels( in,out, options, 0 );

  oyImage_Release( &in );
  oyImage_Release( &out );

  return conv;
}

/** Function  oyConversion_GetGraph
 *  @memberof oyConversion_s
 *  @brief    Get the filter graph from a conversion context
 *
 *  @see oyFilterGraph_GetNode() to obtain a node.
 *
 *  @param[in,out] conversion          conversion object
 *  @return                            the filter graph containing all nodes
 *
 *  @version Oyranos: 0.2.1
 *  @since   2011/01/02 (Oyranos: 0.2.1)
 *  @date    2011/01/02
 */
oyFilterGraph_s  * oyConversion_GetGraph (
                                       oyConversion_s    * conversion )
{
  return oyFilterGraph_FromNode( ((oyConversion_s_*)conversion)->input, 0 );
}

/** Function  oyConversion_GetImage
 *  @memberof oyConversion_s
 *  @brief    Get a image copy at the desired position
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     flags               OY_INPUT or OY_OUTPUT
 *  @return                            the image
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/19 (Oyranos: 0.1.10)
 *  @date    2009/02/19
 */
oyImage_s        * oyConversion_GetImage (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags )
{
  oyImage_s * image = 0;
  oyFilterPlug_s * plug = 0;
  oyFilterSocket_s * sock = 0;
  int error = 0;
  oyConversion_s_ * s = (oyConversion_s_*)conversion;
  oyPixelAccess_s * pixel_access = 0;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 0 )

  if(!error)
  {
    if(oyToInput_m(flags))
    {
      sock = oyFilterNode_GetSocket( s->input, 0 );
      if(sock)
      {
        image = oyImage_Copy( (oyImage_s*) sock->data, 0 );

        if(!sock->data)
        {
          /* TODO: remove the following hack; the socket->plug cast is ugly */
          s->input->api7_->oyCMMFilterPlug_Run( (oyFilterPlug_s*) sock, 0 );
          image = oyImage_Copy( (oyImage_s*) sock->data, 0 );
        }
      }

    } else
    if(oyToOutput_m(flags))
    {
      plug = oyFilterNode_GetPlug( s->out_, 0 );
      if(plug && plug->remote_socket_)
      {
        image = oyImage_Copy( (oyImage_s*) plug->remote_socket_->data, 0);

        if(!image)
        {
          /* Run the graph to set up processing image data. */
          plug = oyFilterNode_GetPlug( conversion->out_, 0 );
          pixel_access = oyPixelAccess_Create( 0,0, plug,
                                               oyPIXEL_ACCESS_IMAGE, 0 );
          conversion->out_->api7_->oyCMMFilterPlug_Run( plug, pixel_access );

          /* Link the tickets image. It should be real copied in a plug-in. */
          /* error = oyFilterNode_DataSet( conversion->out_,
                                        (oyStruct_s*)pixel_access->output_image,
                                        0, 0 ); */
          oyPixelAccess_Release( &pixel_access );

          image = oyImage_Copy( (oyImage_s*) plug->remote_socket_->data, 0 );
        }
      }
    }
  }

  return image;
}

/** Function  oyConversion_GetNode
 *  @memberof oyConversion_s
 *  @brief    Get the filter node copy
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     flags               OY_INPUT or OY_OUTPUT
 *  @return                            the filter node as defined by flags
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/03/27 (Oyranos: 0.3.0)
 *  @date    2011/03/29
 */
oyFilterNode_s   * oyConversion_GetNode (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags )
{
  oyFilterNode_s * node = 0;
  int error = 0;
  oyConversion_s_ * s = (oyConversion_s_*)conversion;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 0 )

  if(!error)
  {
    if(oyToInput_m(flags))
      node = oyFilterNode_Copy( s->input, 0 );

    else
    if(oyToOutput_m(flags))
      node = oyFilterNode_Copy( s->out_, 0 );
  }

  return node;
}

/** Function  oyConversion_GetOnePixel
 *  @memberof oyConversion_s
 *  @brief    Compute one pixel at the given position
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     x                   position x
 *  @param[in]     y                   position y
 *  @param[in,out] pixel_access        pixel iterator configuration
 *  @return                            -1 end; 0 on success; error > 1
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/07/14 (Oyranos: 0.1.8)
 *  @date    2011/04/11
 */
int          oyConversion_GetOnePixel( oyConversion_s    * conversion,
                                       double              x,
                                       double              y,
                                       oyPixelAccess_s   * pixel_access )
{
  oyFilterPlug_s * plug = 0;
  oyFilterSocket_s * sock = 0;
  int error = 0;

  /* conversion->out_ has to be linear, so we access only the first socket */
  plug = oyFilterNode_GetPlug( oyConversionPriv_m(conversion)->out_, 0 );
  sock = oyFilterPlugPriv_m(plug)->remote_socket_;

  oyPixelAccessPriv_m(pixel_access)->start_xy[0] = x;
  oyPixelAccessPriv_m(pixel_access)->start_xy[1] = y;

  /* @todo */
  error = oyFilterSocketPriv_m(sock)->node->api7_->oyCMMFilterPlug_Run( plug, pixel_access );

  return error;
}

/** Function  oyConversion_RunPixels
 *  @memberof oyConversion_s
 *  @brief    Iterate over a conversion graph
 *
 *  @verbatim
    // use the output
    oyImage_s * image = oyConversion_GetImage( context, OY_OUTPUT );
    // get the data and draw the image
    for(i = 0; i < image->height; ++i)
    {
      image_data = image->getLine( image, i, &height, -1, &is_allocated );

      // ...

      if(is_allocated)
        free( image_data );
    }
@endverbatim
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in,out] pixel_access        optional pixel iterator configuration
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/06 (Oyranos: 0.1.8)
 *  @date    2009/03/06
 */
int                oyConversion_RunPixels (
                                       oyConversion_s    * conversion,
                                       oyPixelAccess_s   * pixel_access )
{
  oyConversion_s * s = conversion;
  oyFilterPlug_s * plug = 0;
  oyFilterNode_s * node_out = 0;
  oyImage_s * image_out = 0, * image_input = 0;
  int error = 0, result = 0, i,n, dirty = 0, tmp_ticket = 0;
  oyRectangle_s roi = {oyOBJECT_RECTANGLE_S, 0,0,0};
  double clck;

  oyConversion_s_ ** conversion_ = (oyConversion_s_**)&conversion;
  oyPixelAccess_s_ ** pixel_access_ = (oyPixelAccess_s_**)&pixel_access;
  oyImage_s_ ** image_out_ = (oyImage_s_**)&image_out;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 1 )

  /* should be the same as conversion->out_->filter */
  node_out = oyConversion_GetNode( conversion, OY_OUTPUT );
  plug = oyFilterNode_GetPlug( node_out, 0 );

  /* basic checks */
  if(!plug)
  {
    WARNc1_S("graph incomplete [%d]", s ? oyObject_GetId( s->oy_ ) : -1)
    return 1;
  }

  /* conversion->out_ has to be linear, so we access only the first plug */

  if(!pixel_access)
  {
    /* create a very simple pixel iterator as job ticket */
    if(plug)
    {
      clck = oyClock();
      pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, 0 );
      clck = oyClock() - clck;
      DBG_NUM1_S("oyPixelAccess_Create(): %g", clck/1000000.0 );
    }
    tmp_ticket = 1;
  }

  if(!pixel_access)
    error = 1;

  image_out = oyConversion_GetImage( conversion, OY_OUTPUT );

  if(error <= 0)
    oyRectangle_SetByRectangle( &roi, (*pixel_access_)->output_image_roi );

  if(error <= 0 && !(*pixel_access_)->array)
  {
    clck = oyClock();
    result = oyImage_FillArray( image_out, &roi, 0,
                                &(*pixel_access_)->array,
                                (*pixel_access_)->output_image_roi, 0 );
    clck = oyClock() - clck;
    DBG_NUM1_S("oyImage_FillArray(): %g", clck/1000000.0 );
    error = ( result != 0 );
  }

  /* run on the graph */
  if(error <= 0)
  {
    clck = oyClock();
    error = oyFilterNodePriv_m(node_out)->api7_->oyCMMFilterPlug_Run( plug, pixel_access );
    clck = oyClock() - clck;
    DBG_NUM1_S( "conversion->out_->api7_->oyCMMFilterPlug_Run(): %g",
                clck/1000000.0 );
  }

  if(error != 0 && pixel_access)
  {
    dirty = oyOptions_FindString( (*pixel_access_)->graph->options, "dirty", "true")
            ? 1 : 0;

    /* refresh the graph representation */
    clck = oyClock();
    oyFilterGraph_SetFromNode( (*pixel_access_)->graph, (*conversion_)->input, 0, 0 );
    clck = oyClock() - clck;
    DBG_NUM1_S("oyFilterGraph_SetFromNode(): %g", clck/1000000.0 );

    /* resolve missing data */
    clck = oyClock();
    image_input = oyFilterPlug_ResolveImage( plug, oyFilterPlugPriv_m(plug)->remote_socket_,
                                             pixel_access );
    clck = oyClock() - clck;
    DBG_NUM1_S("oyFilterPlug_ResolveImage(): %g", clck/1000000.0 );
    oyImage_Release( &image_input );

    n = oyFilterNodes_Count( (*pixel_access_)->graph->nodes );
    for(i = 0; i < n; ++i)
    {
#if 0
      clck = oyClock();
      l_error = oyArray2d_Release( &(*pixel_access_)->array ); OY_ERR
      l_error = oyImage_FillArray( image_out, &roi, 0,
                                   &(*pixel_access_)->array,
                                   (*pixel_access_)->output_image_roi, 0 ); OY_ERR
      clck = oyClock() - clck;
      DBG_NUM1_S("oyImage_FillArray(): %g", clck/1000000.0 );
#endif

      if(error != 0 &&
         dirty)
      {
        if((*pixel_access_)->start_xy[0] != (*pixel_access_)->start_xy_old[0] ||
           (*pixel_access_)->start_xy[1] != (*pixel_access_)->start_xy_old[1])
        {
          /* set back to previous values, at least for the simplest case */
          (*pixel_access_)->start_xy[0] = (*pixel_access_)->start_xy_old[0];
          (*pixel_access_)->start_xy[1] = (*pixel_access_)->start_xy_old[1];
        }

        clck = oyClock();
        oyFilterGraph_PrepareContexts( (*pixel_access_)->graph, 0 );
        clck = oyClock() - clck;
        DBG_NUM1_S("oyFilterGraph_PrepareContexts(): %g", clck/1000000.0 );
        clck = oyClock();
        error = (*conversion_)->out_->api7_->oyCMMFilterPlug_Run( plug,
                                                              pixel_access);
        clck = oyClock() - clck;
        DBG_NUM1_S("conversion->out_->api7_->oyCMMFilterPlug_Run(): %g", clck/1000000.0 );
      }

      if(error == 0)
        break;
    }
  }

  /* Write the data to the output image.
   *
   * The oyPixelAccess_s job ticket contains a oyArray2d_s object called array
   * holding the in memory data. After the job is done the output images
   * pixel_data pointer is compared with the job tickets array pointer. If
   * they are the same it is assumed that a observer of the output image will
   * see the same processed data, otherwise oyPixelAccess_s::array must be
   * copied to the output image.
   *
   * While the design of having whatever data storage in a oyImage_s is very
   * flexible, the oyPixelAccess_s::array's in memory buffer is not.
   * Users with very large data sets have to process the data in chunks and
   * the oyPixelAccess_s::array allocation can remain constant.
   */
  if(image_out && pixel_access &&
     ((oyPointer)(*image_out_)->pixel_data != (oyPointer)(*pixel_access_)->array ||
      image_out != (*pixel_access_)->output_image))
  {
    /* move the array to the top left place
     * same as : roi.x = roi.y = 0; */
    /*roi.x = (*pixel_access_)->start_xy[0];
    roi.y = (*pixel_access_)->start_xy[1];*/
    result = oyImage_ReadArray( image_out, &roi,
                                (*pixel_access_)->array, 0 );
  }

  if(tmp_ticket)
    oyPixelAccess_Release( &pixel_access );

  oyImage_Release( &image_out );

  return error;
}

/** Function oyConversion_Set
 *  @memberof oyConversion_s
 *  @brief   set input and output of a conversion graph
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     input               input node
 *  @param[in]     output              output node
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
int                oyConversion_Set  ( oyConversion_s    * conversion,
                                       oyFilterNode_s    * input,
                                       oyFilterNode_s    * output )
{
  oyConversion_s * s = conversion;
  int error = 0;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 1 )

  if(input)
    s->input = input;

  if(output)
    s->out_ = output;

  return error;
}

/** Function: oyConversion_ToText
 *  @memberof oyConversion_s
 *  @brief   text description of a conversion graph
 *
 *  @todo Should this function generate XFORMS compatible output? How?
 *
 *  @param[in]     conversion          conversion object
 *  @param[in]     head_line           text for inclusion
 *  @param[in]     reserved            future format selector (dot, xml ...)
 *  @param[in]     allocateFunc        allocation function
 *  @return                            the graph description
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/10/04 (Oyranos: 0.1.8)
 *  @date    2009/03/05
 */
char             * oyConversion_ToText (
                                       oyConversion_s    * conversion,
                                       const char        * head_line,
                                       int                 reserved,
                                       oyAlloc_f           allocateFunc )
{
  oyConversion_s * s = conversion;
  char * text = 0;
  oyFilterGraph_s * adjacency_list = 0;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 0 )

  adjacency_list = oyFilterGraph_New( 0 );
  oyFilterGraph_SetFromNode( adjacency_list, conversion->input, 0, 0 );

  text = oyFilterGraph_ToText( adjacency_list,
                               conversion->input, conversion->out_,
                               head_line, reserved, allocateFunc );

  oyFilterGraph_Release( &adjacency_list );

  return text;
}
