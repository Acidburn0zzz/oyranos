/** Function  oyFilterCore_GetCategory
 *  @memberof oyFilterCore_s
 *  @brief    Get category string
 *
 *  @param[in,out] filter              filter object
 *  @param         nontranslated       switch for translation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
const char * oyFilterCore_GetCategory( oyFilterCore_s    * filter,
                                       int                 nontranslated )
{
  if(!filter)
    return 0;

  return ((oyFilterCore_s_*)filter)->category_;
}

/** Function  oyFilterCore_GetName
 *  @memberof oyFilterCore_s
 *  @brief    Get name
 *
 *  provides the original filter names
 *
 *  @param[in,out] filter              filter object
 *  @param         name_type           type of name
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
const char * oyFilterCore_GetName    ( oyFilterCore_s    * filter,
                                       oyNAME_e            name_type )
{
  oyFilterCore_s_ * s = (oyFilterCore_s_*)filter;

  if(!s)
    return 0;

  return oyNoEmptyName_m_( s->api4_->ui->getText( "name", name_type,
                           (oyStruct_s*)s->api4_->ui ) );
}

/** Function  oyFilterCore_GetText
 *  @memberof oyFilterCore_s
 *  @brief    Get text
 *
 *  oyNAME_NAME provides a XML element with child elements and attributes
 *
 *  @param[in,out] filter              filter object
 *  @param         name_type           type of name
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
const char * oyFilterCore_GetText    ( oyFilterCore_s    * filter,
                                       oyNAME_e            name_type )
{
  char * text = 0;
  oyFilterCore_s_ * s = (oyFilterCore_s_*)filter;
  int error = !s;

  if(error)
    return 0;

  if(name_type == oyNAME_NAME && !oyObject_GetName(s->oy_, name_type))
  {
    text = oyAllocateWrapFunc_( 512, s->oy_ ? s->oy_->allocateFunc_ : 0 );
    if(!text)
      error = 1;
    sprintf(text, "<oyFilterCore_s registration=\"%s\" category=\"%s\" version=\"%d.%d.%d\"/>\n",
                  s->registration_,
                  s->category_,
                  s->api4_->version[0],
                  s->api4_->version[1],
                  s->api4_->version[2]
           );

#if 0
    if(error <= 0 && s->profiles_)
    {
      int i = 0, n = oyProfiles_Count(s->profiles_);
      oyProfile_s * profile = 0;
      for(i = 0; i < n; ++i)
      {
        profile = oyProfiles_Get( s->profiles_, i );
        sprintf( &text[oyStrlen_(text)], "    %s\n",
        oyNoEmptyName_m_(oyProfile_GetText( profile, oyNAME_NAME)) );
      }
    }
#endif

    if(error <= 0)
      error = oyObject_SetName( s->oy_, text, name_type );

    if(error)
      WARNc_S(_("MEM Error."));
  }

  return oyObject_GetName(s->oy_, name_type);
}

/** Function  oyFilterCore_NewWith
 *  @memberof oyFilterCore_s
 *  @brief    Lookup and initialise a new filter object
 *
 *  back end selection: \n
 *  - the user knows, which kind of filter is requested -> registration, e.g. "//color"
 *  - the user probably knows, which special CMM to use (e.g. lcms, icc, shiva)
 *
 *  @param[in]     registration        the filter registration pattern
 *  @param[in]     options             the supplied filter options
 *  @param[in]     object              the optional object
 *  @return                            a filter core
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2009/07/27
 */
oyFilterCore_s * oyFilterCore_NewWith( const char        * registration,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyFilterCore_s * s = (oyFilterCore_s*)oyFilterCore_New_( object );
  int error = !s;
  oyCMMapi4_s_ * api4_ = 0;

  if(error <= 0)
  {
    api4_ = (oyCMMapi4_s_*) oyCMMsGetFilterApi_( 0,
                                            registration, oyOBJECT_CMM_API4_S );
    error = !api4_;
  }

  if(error <= 0)
    error = oyFilterCore_SetCMMapi4_( (oyFilterCore_s_*)s, api4_ );

  if(error <= 0 && !options)
  {
#if 0
    oyFilterCorePriv_m(s)->options_ = api4_->oyCMMFilter_ValidateOptions( s, options, 0, &ret );
#endif
  }

  if(error <= 0 && options)
    oyFilterCorePriv_m(s)->options_ = oyOptions_Copy( options, 0 );

  if(error && s)
  {
    oyFilterCore_Release( &s );
    WARNc1_S("could not create filter: \"%s\"", oyNoEmptyName_m_(registration));
  }

  return s;
}

/** Function  oyFilterCore_GetRegistration
 *  @memberof oyFilterCore_s
 *  @brief    Get registration text
 *
 *  @param[in]     filter              filter object
 *  @return                            registration string
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/04
 *  @since    2012/10/04 (Oyranos: 0.5.0)
 */
OYAPI const char *  OYEXPORT
                oyFilterCore_GetRegistration (
                                       oyFilterCore_s    * filter )
{
  oyFilterCore_s_ * s = (oyFilterCore_s_*)filter;

  if(!s)
    return 0;

  return s->registration_;
}

