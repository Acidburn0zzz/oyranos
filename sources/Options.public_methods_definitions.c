#include "oyranos_definitions.h"

/** Function oyOptions_FromBoolean
 *  @memberof oyOptions_s
 *  @brief   boolean operations on two sets of option
 *
 *  @see oyOptions_Add
 *
 *  @param[in]     set_a               options set A
 *  @param[in]     set_b               options set B
 *  @param[in]     type                the operation to perform
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/28 (Oyranos: 0.1.8)
 *  @date    2008/06/28
 */
oyOptions_s *  oyOptions_FromBoolean ( oyOptions_s       * set_a,
                                       oyOptions_s       * set_b,
                                       oyBOOLEAN_e         type,
                                       oyObject_s          object )
{
  int error = !set_a && !set_b;
  oyOptions_s * options = 0;
  oyOption_s * option_a = 0, * option_b = 0;
  int set_an = oyOptions_Count( set_a ),
      set_bn = oyOptions_Count( set_b );
  int i, j,
      found = 0;
  char * txt_1, * txt_2;

  if(error <= 0)
  {
    for(i = 0; i < set_an; ++i)
    {
      option_a = oyOptions_Get( set_a, i );
      txt_1 = oyFilterRegistrationToText( ((oyOption_s_*)option_a)->registration,
                                          oyFILTER_REG_OPTION, 0);

      for(j = 0; j < set_bn; ++j)
      {
        option_b = oyOptions_Get( set_b, j );
        txt_2 = oyFilterRegistrationToText( ((oyOption_s_*)option_b)->registration,
                                            oyFILTER_REG_OPTION, 0);

        found = oyTextboolean_( txt_1, txt_2, type );

        /* add to the list */
        if(found > 0)
        {
          if(!options)
            options = oyOptions_New(0);

          if(((oyOption_s_*)option_a)->value_type
              !=
             ((oyOption_s_*)option_b)->value_type)
            found = 0;

          if(found)
          {
            oyOptions_Add( options, option_a, -1, object );
            if(found == 2)
              oyOptions_Add( options, option_b, -1, object );
          }
        }

        oyFree_m_( txt_2 );
        oyOption_Release( &option_b );
      }

      oyFree_m_( txt_1 );
      oyOption_Release( &option_a );
    }

    if(!set_an && set_bn &&
       (type == oyBOOLEAN_UNION || type == oyBOOLEAN_DIFFERENZ))
      options = oyOptions_Copy( set_b, object );
    if(set_an && !set_bn &&
       (type == oyBOOLEAN_UNION))
      options = oyOptions_Copy( set_a, object );
  }

  return options;
}

#ifdef OY_FILTER_CORE_S_H
/** Function oyOptions_ForFilter
 *  @memberof oyOptions_s
 *  @brief   provide Oyranos behaviour settings
 *
 *  The returned options are read in from the Elektra settings and if thats not
 *  available from the inbuild defaults. The later can explicitely selected with
 *  oyOPTIONSOURCE_FILTER passed as flags argument.
 *  The key names map to the registration and XML syntax.
 *
 *  To obtain all advanced front end options from a meta module use:@verbatim
 *  flags = oyOPTIONATTRIBUTE_ADVANCED |
 *          oyOPTIONATTRIBUTE_FRONT |
 *          OY_SELECT_COMMON @endverbatim
 *
 *  @see OY_SELECT_FILTER OY_SELECT_COMMON oyOPTIONATTRIBUTE_e
 *
 *  @param[in]     registration        the filter registration to search for
 *  @param[in]     cmm                 a CMM to match
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED |
 *                                     OY_SELECT_FILTER |
 *                                     OY_SELECT_COMMON
 *  @param         object              the optional object
 *  @return                            the options
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/10/08 (Oyranos: 0.1.8)
 *  @date    2011/01/29
 */
oyOptions_s *  oyOptions_ForFilter   ( const char        * registration,
                                       const char        * cmm,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0;
  oyFilterCore_s * filter = 0;
  oyCMMapi4_s * cmm_api4 = 0;
  char * lib_name = 0;
  int error = 0;

  /*  1. get filter */
  filter = oyFilterCore_New_( object );

  error = !filter;

  if(error <= 0)
    cmm_api4 = (oyCMMapi4_s*) oyCMMsGetFilterApi_( cmm, registration,
                                                   oyOBJECT_CMM_API4_S );

  if(cmm_api4)
    lib_name = cmm_api4->id_;

  error = !(cmm_api4 && lib_name);

  if(error <= 0)
    error = oyFilterCore_SetCMMapi4_( filter, cmm_api4 );

  s = oyOptions_ForFilter_( filter, flags, object);

  oyFilterCore_Release( &filter );

  return s;
}
#endif /* OY_FILTER_CORE_S_H */

/** Function oyOptions_FromText
 *  @memberof oyOptions_s
 *  @brief   deserialise a text file to oyOptions_s data
 *
 *  This function is based on libxml2.
 *
 *  @param[in]     text                the text to process
 *  @param[in]     flags               for future use
 *  @param         object              the optional object
 *  @return                            the data
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/17 (Oyranos: 0.1.9)
 *  @date    2008/11/17
 */
oyOptions_s *  oyOptions_FromText    ( const char        * text,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0;
  int error = !text;
  xmlDocPtr doc = 0;
  xmlNodePtr cur = 0;
  char ** texts = 0;
  int texts_n = 0;
  const char * root_start = "<top>", * root_end = "</top>";
  char * tmp = 0;

  if(error <= 0)
  {
    /* add a root level node - <top> */
    tmp = oyStringAppend_( root_start, text, oyAllocateFunc_ );
    STRING_ADD( tmp, root_end );

    doc = xmlParseMemory( tmp, oyStrlen_( tmp ) );
    error = !doc;

    oyFree_m_( tmp );

    if(doc)
    {
      cur = xmlDocGetRootElement(doc);
      /* skip now the artifically added <top> level */
      cur = cur->xmlChildrenNode;
    }
    error = !cur;
  }

  if(error <= 0)
  {
    s = oyOptions_New(0);

    oyOptions_ParseXML_( (oyOptions_s_*)s, &texts, &texts_n, doc, cur );

    oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
    xmlFreeDoc(doc);
  }

  return s;
}

/** Function oyOptions_Add
 *  @memberof oyOptions_s
 *  @brief   add a element to a Options list
 *
 *  We must not add any already listed option. 
 *  (oyFILTER_REG_TOP)
 *
 *  Adding a new element without any checks is as simple as following code:
 *  @verbatim
 *     tmp = oyOption_Copy( option, object );
 *     oyOptions_MoveIn( options, &tmp, -1 ); @endverbatim
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/17 (Oyranos: 0.1.9)
 *  @date    2010/11/27
 */
int            oyOptions_Add         ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object )
{
  oyOption_s *tmp = 0;
  int error = !options || !option;
  int n, i, skip = 0;
  char * o_opt,
       * o_top,
       * l_opt,  /* l - list */
       * l_top,
       * t;

  if(error <= 0)
  {
    o_opt = oyFilterRegistrationToText( oyOptionPriv_m(option)->registration,
                                        oyFILTER_REG_MAX, 0 );
    if(strrchr(o_opt, '.' ))
    {
      t = strrchr(o_opt, '.' );
      *t = 0;
    }
    o_top = oyFilterRegistrationToText( oyOptionPriv_m(option)->registration,
                                        oyFILTER_REG_TOP, 0 );
    n = oyOptions_Count( options );

    for(i = 0; i < n; ++i)
    {
      tmp = oyOptions_Get( options, i );
      l_opt = oyFilterRegistrationToText( oyOptionPriv_m(tmp)->registration,
                                          oyFILTER_REG_MAX, 0 );
      if(strrchr(l_opt, '.' ))
      {
        t = strrchr(l_opt, '.' );
        *t = 0;
      }
      l_top = oyFilterRegistrationToText( oyOptionPriv_m(tmp)->registration,
                                          oyFILTER_REG_TOP, 0 );
      if(oyStrcmp_(l_opt, o_opt) == 0)
        skip = 2;

      oyFree_m_( l_opt );
      oyFree_m_( l_top );
      oyOption_Release( &tmp );
    }

    if(skip == 0)
    {
      tmp = oyOption_Copy( option, object );
      oyOptions_MoveIn( options, &tmp, -1 );
    }

    oyFree_m_( o_opt );
    oyFree_m_( o_top );
  }

  return error;
}

/** Function oyOptions_Set
 *  @memberof oyOptions_s
 *  @brief   set a element in a Options list
 *
 *  Already listed options are replaced by the new option. A option new to
 *  the existing set will be referenced if no object argument is given.
 *
 *  Adding a new element without any checks is as simple as following code:
 *  @verbatim
 *     tmp = oyOption_Copy( option, object );
 *     oyOptions_MoveIn( options, &tmp, -1 ); @endverbatim
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/25 (Oyranos: 0.1.10)
 *  @date    2009/10/25
 */
int            oyOptions_Set         ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object )
{
  oyOption_s *tmp = 0;
  int error = !options || !option;
  int n, i, replace = 0;

  if(error <= 0)
  {
    n = oyOptions_Count( options );

    for(i = 0; i < n && !replace; ++i)
    {
      tmp = oyOptions_Get( options, i );
      if(oyFilterRegistrationMatchKey( oyOptionPriv_m(tmp)->registration,
                                       oyOptionPriv_m(option)->registration, 0))
      {
        replace = 2;
        /* replace as we priorise the new value */
        oyOption_Copy__Members( oyOptionPriv_m(tmp), oyOptionPriv_m(option) );
      }
      oyOption_Release( &tmp );
    }

    if(replace == 0)
    {
      tmp = oyOption_Copy( option, object );
      oyOptions_MoveIn( options, &tmp, -1 );
    }
  }

  return error;
}

/** Function oyOptions_SetOpts
 *  @memberof oyOptions_s
 *  @brief   set options in a Options list
 *
 *  Already listed options are replaced by the new options.
 *
 *  @see oyOptions_Set It will use copying.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/25 (Oyranos: 0.1.10)
 *  @date    2009/10/25
 */
int            oyOptions_SetOpts     ( oyOptions_s       * list,
                                       oyOptions_s       * add )
{
  int error = !list;
  int i,n;
  oyOption_s * o = 0;

  if(error <= 0)
  {
    n = oyOptions_Count( add );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( add, i );
      oyOptions_Set( list, o, -1, list->oy_ );
      oyOption_Release( &o );
    }
  }

  return error;
}

/** Function oyOptions_AppendOpts
 *  @memberof oyOptions_s
 *  @brief   
 *
 *  Basic reference options from one list into an other.
 *
 *  @see oyOptions_MoveIn
 *
 *  @version Oyranos: 
 *  @since
 *  @date
 */
int            oyOptions_AppendOpts  ( oyOptions_s       * list,
                                       oyOptions_s       * append )
{
  int error = !list;
  int i,n;
  oyOption_s * o = 0;

  if(error <= 0)
  {
    n = oyOptions_Count( append );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( append, i );
      oyOptions_MoveIn( list, &o, -1 );
    }
  }

  return error;
}

/**
 *  Function oyOptions_CopyFrom
 *  @memberof oyOptions_s
 *  @brief   copy from one option set to an other option set
 *
 *  @param[out]    list                target
 *  @param[in]     from                source
 *  @param         type                boolean operation before copy
 *  @param         fields              registration fields of each option
 *  @param         object              the optional object
 *  @return                            0 - success; 1 - error; -1 issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/05 (Oyranos: 0.1.10)
 *  @date    2009/10/25
 */
int            oyOptions_CopyFrom    ( oyOptions_s      ** list,
                                       oyOptions_s       * from,
                                       oyBOOLEAN_e         type,
                                       oyFILTER_REG_e      fields,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0,
              * tmp = 0, * tmp2 = 0;
  int error = !list;

  if(!from || error)
    return error ? error : -1;

  if(!*list)
  {
    s = oyOptions_New( object );
    if(!*list)
      *list = s;
    error = !*list;
  }

  if(error <= 0)
  {
    s = *list;

    if(type == oyBOOLEAN_UNION)
    {
      if(oyOptionsPriv_m(s)->list_ || oyOptionsPriv_m(from)->list_)
      {
        if(!oyOptionsPriv_m(s)->list_)
          oyOptionsPriv_m(s)->list_ = oyStructList_New( 0 );
        if(!oyOptionsPriv_m(from)->list_)
          oyOptionsPriv_m(from)->list_ = oyStructList_New( 0 );
      }
      error = oyOptions_SetOpts( s, from );

    } else
    {
      int list_n = oyOptions_Count( *list ),
          from_n = oyOptions_Count( from ),
          i,j, found;
      oyOption_s * list_o = 0,
                 * from_o = 0;
      char * list_reg = 0, * from_reg = 0;
      
      tmp = oyOptions_New( 0 );

      for(i = 0; i < list_n && error <= 0; ++i)
      {
        found = 0;
        list_o = oyOptions_Get( *list, i );
        list_reg = oyFilterRegistrationToText( oyOptionPriv_m(list_o)->registration,
                                               fields, 0 );

        for(j = 0; j < from_n && error <= 0; ++j)
        {
          from_o = oyOptions_Get( from, j );
          from_reg = oyFilterRegistrationToText( oyOptionPriv_m(from_o)->registration,
                                                 fields, 0 );

          if(oyFilterRegistrationMatch( list_reg, from_reg, 0 ))
            found = 1;

          if(type == oyBOOLEAN_INTERSECTION && found)
          {
            oyOptions_MoveIn( tmp, &from_o, -1 );
            break;
          }

          oyFree_m_( from_reg );
          oyOption_Release( &from_o );
        }

        if((type == oyBOOLEAN_SUBSTRACTION ||
            type == oyBOOLEAN_DIFFERENZ) &&
           !found)
          oyOptions_MoveIn( tmp, &list_o, -1 );

        oyFree_m_( list_reg );
        oyOption_Release( &list_o );
      }

      if(type == oyBOOLEAN_SUBSTRACTION ||
         type == oyBOOLEAN_INTERSECTION)
      {
        oyStructList_Release( &(oyOptionsPriv_m(*list))->list_ );
        oyOptionsPriv_m(*list)->list_ = oyOptionsPriv_m(tmp)->list_;
        oyOptionsPriv_m(tmp)->list_ = 0;

      } else
      if(type == oyBOOLEAN_DIFFERENZ)
      {
        tmp2 = oyOptions_New( 0 );
        for(i = 0; i < from_n && error <= 0; ++i)
        {
          found = 0;
          from_o = oyOptions_Get( from, i );
          from_reg = oyFilterRegistrationToText( oyOptionPriv_m(from_o)->registration,
                                                 fields, 0 );

          for(j = 0; j < list_n && error <= 0; ++j)
          {
            list_o = oyOptions_Get( *list, j );
            list_reg = oyFilterRegistrationToText( oyOptionPriv_m(list_o)->registration,
                                                   fields, 0 );

            if(oyFilterRegistrationMatch( from_reg, list_reg, 0 ))
              found = 1;

            oyFree_m_( list_reg );
            oyOption_Release( &list_o );
          }

          if(!found)
            oyOptions_MoveIn( tmp2, &from_o, -1 );

          oyFree_m_( from_reg );
          oyOption_Release( &from_o );
        }

        oyStructList_Release( &oyOptionsPriv_m(*list)->list_ );
        oyOptions_CopyFrom( list, tmp, oyBOOLEAN_UNION, 0, 0 );
        oyOptions_CopyFrom( list, tmp2, oyBOOLEAN_UNION, 0, 0 );
      }

      oyOptions_Release( &tmp );
      oyOptions_Release( &tmp2 );
    }

  }

  if(error > 0)
    oyOptions_Release( &s );

  return error;
}

#ifdef OYRANOS_ELEKTRA_H_
/** Function oyOptions_DoFilter
 *  @memberof oyOptions_s
 *  @brief   filter the options
 *
 *  The returned options are read in from the Elektra settings and if thats not
 *  available from the inbuild defaults. The later can explicitely selected with
 *  oyOPTIONSOURCE_FILTER passed as flags argument. advanced options can be 
 *  filtered out by adding oyOPTIONATTRIBUTE_ADVANCED.
 *
 *  Modules should handle the advanced options as well but shall normally
 *  not act upon them. The convention to set them zero, keeps them inactive.
 *  
 *  On the front end side the CMM cache has to include them, as they will 
 *  influence the hash sum generation. The question arrises, whether to include
 *  these options marked as non visible along the path or require the CMM cache
 *  code to check each time for them on cache lookup. The oyOption_s::flags
 *  is already in place. So we use it and do inclusion. Front end options can be
 *  filtered as they do not affect the CMM cache.
 *
 *  @see oyOptions_Add
 *
 *  @param         opts                the options
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED;
 *                                     for front end options |
 *                                     oyOPTIONATTRIBUTE_FRONT
 *  @param         filter_type         the type level from a registration
 *  @return                            options
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2008/11/27
 */
int          oyOptions_DoFilter      ( oyOptions_s       * opts,
                                       uint32_t            flags,
                                       const char        * filter_type )
{
  oyOptions_s * opts_tmp = 0;
  oyOption_s * o = 0;
  int error = !opts;
  char * text;
  int i,n;

  oyExportStart_(EXPORT_SETTING);
  oyExportEnd_();

  if(error <= 0 && (flags || filter_type))
  {
    /*  6. get stored values */
    n = oyOptions_Count( opts );
    opts_tmp = oyOptions_New(0);
    for(i = 0; i < n; ++i)
    {
      int skip = 0;

      o = oyOptions_Get( opts, i );


      /* usage/type range filter */
      if(filter_type)
      {
        text = oyFilterRegistrationToText( oyOption_GetRegistration(o),
                                           oyFILTER_REG_TYPE, 0);
        if(oyStrcmp_( filter_type, text ) != 0)
          skip = 1;

        oyFree_m_( text );
      }

      /* front end options filter */
      if(!skip && !(flags & oyOPTIONATTRIBUTE_FRONT))
      {
        text = oyStrrchr_( oyOption_GetRegistration(o), '/' );

        if(text)
           text = oyStrchr_( text, '.' );
        if(text)
          if(oyStrstr_( text, "front" ))
            skip = 1;
      }

      /* advanced options mark and zero */
      if(!skip && !(flags & oyOPTIONATTRIBUTE_ADVANCED))
      {
        text = oyStrrchr_( oyOption_GetRegistration(o), '/' );
        if(text)
           text = oyStrchr_( text, '.' );
        if(text)
          if(oyStrstr_( text, "advanced" ))
          {
            oyOption_SetFromText( o, "0", 0 );
            oyOption_SetFlags( o,
                              oyOption_GetFlags(o) & (~oyOPTIONATTRIBUTE_EDIT));
          }
      } else
      /* Elektra settings, modify value */
      if(!skip && !(flags & oyOPTIONSOURCE_FILTER))
      {
        text = oyGetKeyString_( oyOption_GetText( o, oyNAME_DESCRIPTION),
                                oyAllocateFunc_ );
        if(text && text[0])
        {
          error = oyOption_SetFromText( o, text, 0 );
          oyOption_SetFlags(o, oyOption_GetFlags(o) & (~oyOPTIONATTRIBUTE_EDIT));
          oyOption_SetSource( o, oyOPTIONSOURCE_USER );
          oyFree_m_( text );
        }
      }

      if(!skip)
        oyOptions_Add( opts_tmp, o, -1, opts->oy_ );

      oyOption_Release( &o );
    }

    n = oyOptions_Count( opts_tmp );
    error = oyOptions_Clear(opts);
    for( i = 0; i < n && !error; ++i )
    {
      o = oyOptions_Get( opts_tmp, i );
      error = oyOptions_MoveIn( opts, &o, -1 );
    }
    oyOptions_Release( &opts_tmp );
  }

  return error;
}
#endif /* OYRANOS_ELEKTRA_H */

/** Function oyOptions_Filter
 *  @memberof oyOptions_s
 *  @brief   filter options
 *
 *  Each option added to the add_list is a cheaply linked one.
 *
 *  @see oyOptions_Add
 *
 *  @param[out]    add_list            the options list to add to
 *  @param[out]    count               the number of matching options
 *  @param[in]     flags               for inbuild defaults 
 *                                     | oyOPTIONSOURCE_FILTER
 *  @param[in]     type                support are oyBOOLEAN_INTERSECTION and
 *                                     oyBOOLEAN_DIFFERENZ
 *  @param[in]     registration        a registration which shall be matched
 *  @param[in]     src_list            the options to select from
 *  @return                            0 - success; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/05 (Oyranos: 0.1.10)
 *  @date    2009/05/05
 */
int            oyOptions_Filter      ( oyOptions_s      ** add_list,
                                       int32_t           * count,
                                       uint32_t            flags,
                                       oyBOOLEAN_e         type,
                                       const char        * registration,
                                       oyOptions_s       * src_list )
{
  int error = !src_list || !add_list || *add_list == src_list;
  oyOptions_s * s = src_list;
  oyOption_s * o = 0;
  int n, i;
  int32_t c = 0;
  uint32_t options_source = 0;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 )

  if(error <= 0)
  {
    n = oyOptions_Count( s );
    if(!*add_list)
      *add_list = oyOptions_New(0);

    if(!*add_list)
      error = 1;

    if(error <= 0)
    for(i = 0; i < n; ++i)
    {
      int found = 1;

      o = oyOptions_Get( s, i );

      if(found && registration &&
         !oyFilterRegistrationMatch( oyOptionPriv_m(o)->registration, registration, 0 ))
          found = 0;

      options_source = flags & oyOPTIONSOURCE_FILTER ? oyOPTIONSOURCE_FILTER :0;
      options_source |= flags & oyOPTIONSOURCE_DATA ? oyOPTIONSOURCE_DATA : 0;
      options_source |= flags & oyOPTIONSOURCE_USER ? oyOPTIONSOURCE_USER : 0;
      if(found && options_source && !(oyOptionPriv_m(o)->source & options_source))
        found = 0;

      if(type == oyBOOLEAN_UNION ||
         (type == oyBOOLEAN_INTERSECTION && found) ||
         (type == oyBOOLEAN_DIFFERENZ && !found)
        )
      {
        oyOptions_Add( *add_list, o, -1, 0 );
        ++c;
      }

      oyOption_Release( &o );
    }

    if(count)
      *count = c;
  }

  return error;
}

/** Function oyOptions_GetText
 *  @memberof oyOptions_s
 *  @brief   dump options to text
 *
 *  The type argument should select the following string in return: \n
 *  - oyNAME_NAME - a readable XFORMS data model
 *  - oyNAME_NICK - the hash ID
 *  - oyNAME_DESCRIPTION - option registration name with key and without value
 *
 *  @todo streamline output and group, avoid writing all levels for each key
 *
 *  @param[in,out] options             the option
 *  @param         type                oyNAME_NICK is equal to an ID
 *  @return                            the text
 *
 *  @version Oyranos: 0.3.3
 *  @since   2008/11/25 (Oyranos: 0.1.9)
 *  @date    2011/10/31
 */
const char *   oyOptions_GetText     ( oyOptions_s       * options,
                                       oyNAME_e            type )
{
  int error = !options;
  const char * erg = 0;
  char * text = 0;
  oyOption_s * o = 0, *o2 = 0;
  int i, n, ti, c, j,j_n,k, min_level;
  int indent = 6; /* base indentation for better looking XFORMS documents */
  int * sort, changed;
  char ** old_levels = 0;
  int old_levels_n = 0;
  int close_oy_struct = 0;

  if(error <= 0)
  {
    n = oyOptions_Count( options );
    sort = oyAllocateFunc_( n * sizeof(int) );
    for( i = 0; i < n; ++i )
      sort[i]=i;

    /* sort the options alphabetical */
    if(type == oyNAME_NAME)
    do
    {
      changed = 0;
      for( i = 0; i < n-1; ++i )
      {
        o = oyOptions_Get( options, sort[i] );
        o2 = oyOptions_Get( options, sort[i + 1] );
        c = oyStrcmp_(oyOptionPriv_m(o)->registration, oyOptionPriv_m(o2)->registration);
        if(c > 0)
        {
          ti = sort[i];
          sort[i] = sort[i + 1];
          sort[i+1] = ti;
          changed = 1;
        }
        oyOption_Release( &o );
        oyOption_Release( &o2 );
      }
      for( i = n-2; i >= 0; --i )
      {
        o = oyOptions_Get( options, sort[i] );
        o2 = oyOptions_Get( options, sort[i + 1] );
        c = oyStrcmp_(oyOptionPriv_m(o)->registration, oyOptionPriv_m(o2)->registration);
        if(c > 0)
        {
          ti = sort[i];
          sort[i] = sort[i + 1];
          sort[i+1] = ti;
          changed = 1;
        }
        oyOption_Release( &o );
        oyOption_Release( &o2 );
      }
    } while(changed);

    for( i = 0; i < n; ++i )
    {
      oyOption_s_ * o_ = 0;
      o = oyOptions_Get( options, sort[i] );
      o_ = (oyOption_s_*)o;

      /* Omit redundant XML level closes and opens based on alphabetical input.
       */
      if(type == oyNAME_NAME)
      {
        char * tmp = 0, **list = 0;

        j_n = 1;

        if(!oyObject_GetName( o->oy_, oyNAME_DESCRIPTION ))
          oyOption_GetText(o, oyNAME_DESCRIPTION);


        list = oyStringSplit_( oyObject_GetName( o->oy_, oyNAME_DESCRIPTION ),
                               '/', &j_n, oyAllocateFunc_);
        min_level = 1000;
        /* find the minimum different level */
        for( j = 0; j < j_n; ++j )
        {
          if(!old_levels ||
             (old_levels_n > j && oyStrcmp_(old_levels[j],list[j]) != 0))
            if(min_level > j)
              min_level = j;
        }

        /* close old levels */
        if(old_levels)
        for( j = j_n-1; j >= min_level; --j )
        {
          if(j+1 < j_n)
            for(k = 0; k < indent+j; ++k)
              STRING_ADD ( text, " " );
          if(old_levels_n > j)
          {
            if( close_oy_struct )
            {
              for(k = 0; k < indent+j_n-1; ++k)
                STRING_ADD ( text, " " );
              close_oy_struct = 0;
            }
            STRING_ADD ( text, "</" );
            STRING_ADD ( text, old_levels[j] );
            if(j)
              STRING_ADD ( text, ">\n" );
            else
              STRING_ADD ( text, ">" );
          }
        }

        /* open new levels */
        for( j = min_level; j < j_n; ++j )
        {
          for(k = 0; k < indent+j; ++k)
            STRING_ADD ( text, " " );
          STRING_ADD ( text, "<" );
          STRING_ADD ( text, list[j] );
          if(j+1==j_n)
            STRING_ADD ( text, ">" );
          else
            STRING_ADD ( text, ">\n" );
        }

        if( o_->value_type == oyVAL_STRUCT )
        {
          STRING_ADD ( text, "\n" );
          for(k = 0; k < indent+j_n-1; ++k)
            STRING_ADD ( text, " " );
          STRING_ADD ( text, oyOption_GetText( o, oyNAME_XML_VALUE ) );
          close_oy_struct = 1;
        }
        else
        {
          tmp = oyOption_GetValueText( o, oyAllocateFunc_ );
          STRING_ADD ( text, tmp );
          oyFree_m_( tmp );
        }

        if(old_levels)
          oyStringListRelease_( &old_levels, old_levels_n, oyDeAllocateFunc_ );
        old_levels = list;
        old_levels_n = j_n;
      }
      else
      {
        STRING_ADD ( text, oyOption_GetText( o, type) );
        STRING_ADD ( text, "\n" );
      }

      oyOption_Release( &o );
    }

    /* close all old levels */
    if(old_levels)
      for( j = j_n-1; j >= 0; --j )
      {
        if(j+1 < j_n)
          for(k = 0; k < indent+j; ++k)
            STRING_ADD ( text, " " );
        if(old_levels_n > j)
        {
          STRING_ADD ( text, "</" );
          STRING_ADD ( text, old_levels[j] );
          if(j)
            STRING_ADD ( text, ">\n" );
          else
            STRING_ADD ( text, ">" );
        }
      }

    if(old_levels)
      oyStringListRelease_( &old_levels, old_levels_n, oyDeAllocateFunc_ );

    error = oyObject_SetName( options->oy_, text, type );

    if(text)
      oyFree_m_( text );
  }

  if(error <= 0)
    erg = oyObject_GetName( options->oy_, type );

  return erg;
}

/** Function oyOptions_CountType
 *  @memberof oyOptions_s
 *  @brief   search for options with special attributes
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/04 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
int            oyOptions_CountType   ( oyOptions_s       * options,
                                       const char        * registration,
                                       oyOBJECT_e          type )
{
  int error = !options;
  int i, n, m = 0, found;
  oyOption_s_ * o = 0;
  oyOptions_s * s = options;


  if(!error)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 );

    n = oyOptions_Count( options );
    for(i = 0; i < n;  ++i)
    {
      o = (oyOption_s_*)oyOptions_Get( options, i );
      found = 1;

      if(found && registration &&
         !oyFilterRegistrationMatch( o->registration, registration, 0 ))
          found = 0;

      if(found && type && o->value_type == oyVAL_STRUCT &&
         o->value && o->value->oy_struct->type_ == type)
        ++m;

      oyOption_Release( (oyOption_s**)&o );
    }

  return m;
}

/** Function oyOptions_Find
 *  @memberof oyOptions_s
 *  @brief   search for a certain option key
 *
 *  This function returns the first found option for a given key.
 *  The key is represented by a registrations option level.
 *
 *  @param         options             set of options
 *  @param         registration        registration or key
 *  @return                            a matching options
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/05 (Oyranos: 0.1.9)
 *  @date    2009/04/13
 */
oyOption_s *   oyOptions_Find        ( oyOptions_s       * options,
                                       const char        * registration )
{
  int error = !options || !registration;
  oyOption_s * o = 0,
             * option = 0;
  int found;

  if(error <= 0 && options && options->type_ == oyOBJECT_OPTIONS_S)
  {
    oyOptions_s * set_a = options;
    int set_an = oyOptions_Count( set_a ), i;

    for(i = 0; i < set_an; ++i)
    {
      o = oyOptions_Get( options, i );
      found = 1;

      if(found && registration &&
         !oyFilterRegistrationMatch( oyOptionPriv_m(o)->registration, registration, 0 ))
          found = 0;

      if(found)
      {
        option = o;
        break;
      }

      oyOption_Release( &o );
    }
  }

  return option;
}

/** Function oyOptions_FindString
 *  @memberof oyOptions_s
 *  @brief   search for a certain option key and possibly value
 *
 *  This function returns the first found value for a given key.
 *  The key is represented by the oyOption_s::name::nick
 *  The returned string is valid along the livetime of the particular 
 *  option value.
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/10/07 (Oyranos: 0.1.8)
 *  @date    2009/04/13
 */
const char *   oyOptions_FindString  ( oyOptions_s       * options,
                                       const char        * registration,
                                       const char        * value )
{
  char * text = 0;
  int error = !options;
  oyOptions_s * s = options;
  oyOption_s_ * o = 0;
  int found = 0, j;

  if(!error)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 );

  if(error <= 0)
  {
    o = (oyOption_s_*)oyOptions_Find( options, registration );

    if(o && o->type_ == oyOBJECT_OPTION_S)
    {
      if(o->value_type == oyVAL_STRING)
      {
        text = o->value->string;

        if(text && text[0])
          if(!value ||
             (value && oyStrstr_(value, text)))
            found = 1;
      } else if(o->value_type == oyVAL_STRING_LIST)
      {
        j = 0;

        while(o->value->string_list[j])
        {
          text = o->value->string_list[j];

          if(text && text[0])
            if(!value ||
               (value && oyStrstr_(value, text)))
              found = 1;

          if(found) break;

          ++j;
        }
      }
    }

    oyOption_Release( (oyOption_s**)&o );

    error = !found;

    if(!found)
      text = 0;
  }

  return text;
}

/** Function oyOptions_SetFromText
 *  @memberof oyOptions_s
 *  @brief   change a value
 *
 *  @param         obj                 the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *  @param         value               the value to set
 *  @param         flags               can be OY_CREATE_NEW for a new option,
 *                                     OY_STRING_LIST or OY_ADD_ALWAYS
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2011/03/15
 */
int            oyOptions_SetFromText ( oyOptions_s      ** obj,
                                       const char        * registration,
                                       const char        * value,
                                       uint32_t            flags )
{
  int error = (obj && *obj && (*obj)->type_ != oyOBJECT_OPTIONS_S) ||
              !registration ||
              !value;
  oyOption_s * o = 0;

  if(error <= 0)
  {
    if(!*obj)
      *obj = oyOptions_New( 0 );

    o = oyOptions_Find( *obj, registration );

    /** Add a new option if the OY_CREATE_NEW flag is present.
     */
    if((!o && oyToCreateNew_m(flags)) ||
        oyToAddAlways_m(flags))
    {
      o = oyOption_FromRegistration( registration, (*obj)->oy_ );
      error = !o;

      if(error <= 0)
      {
        /** Flags are passed on to oyOption_SetFromText, e.g. OY_STRING_LIST. */
        error = oyOption_SetFromText( o, value, flags & 1 );
      } else
        WARNc3_S( "%s %s=%s",_("wrong argument to option:"),
                  oyNoEmptyString_m_(registration), oyNoEmptyString_m_(value) );

      oyOptions_MoveIn( (*obj), &o, -1 );

    } else
      oyOption_SetFromText( o, value, flags );

    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_FindInt
 *  @memberof oyOptions_s
 *  @brief   get a value
 *
 *  @param         options             the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *                                     or simple key, e.g. "my_opt"
 *  @param         pos                 the values position in a possible list
 *  @param         result              the integer
 *  @return                            0 -  option exists, is of correct type,
 *                                          holds a value at the position;
 *                                     -1 - not found;
 *                                     1 -  error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/04 (Oyranos: 0.1.10)
 *  @date    2009/05/04
 */
int            oyOptions_FindInt     ( oyOptions_s       * options,
                                       const char        * registration,
                                       int                 pos,
                                       int32_t           * result )
{
  int error = !options;
  oyOptions_s * s = options;
  oyOption_s * o = 0;

  if(!error)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return error );

  if(error <= 0)
  {
    o = oyOptions_Find( options, registration );

    if(o && o->type_ == oyOBJECT_OPTION_S &&
       (oyOptionPriv_m(o)->value_type == oyVAL_INT ||
        oyOptionPriv_m(o)->value_type == oyVAL_INT_LIST))
    {
      if(result)
        *result = oyOption_GetValueInt( o, pos );
      error = 0;

    } else
      error = -1;

    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_SetFromInt
 *  @memberof oyOptions_s
 *  @brief   change a value
 *
 *  @param         obj                 the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *  @param         value               the value to set
 *  @param         pos                 the position in a value list
 *  @param         flags               can be OY_CREATE_NEW for a new option,
 *                                     OY_STRING_LIST or OY_ADD_ALWAYS
 *  @return                            0 - success; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/04 (Oyranos: 0.1.10)
 *  @date    2009/05/04
 */
int            oyOptions_SetFromInt  ( oyOptions_s      ** obj,
                                       const char        * registration,
                                       int32_t             value,
                                       int                 pos,
                                       uint32_t            flags )
{
  int error = 0;
  oyOption_s * o = 0;
  oyOptions_s * s = *obj;

  if(s)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 )

  if(error <= 0)
  {
    if(!*obj)
      *obj = oyOptions_New( 0 );

    o = oyOptions_Find( *obj, registration );

    /** Add a new option if the OY_CREATE_NEW flag is present.
     */
    if((!o && oyToCreateNew_m(flags)) ||
        oyToAddAlways_m(flags))
    {
      o = oyOption_FromRegistration( registration, (*obj)->oy_ );
      error = !o;

      if(error <= 0)
        error = oyOption_SetFromInt( o, value, pos, 0 );

      oyOptions_MoveIn( (*obj), &o, -1 );

    } else
      oyOption_SetFromInt( o, value, pos, 0 );

    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_FindDouble
 *  @memberof oyOptions_s
 *  @brief   get a value
 *
 *  @param         options             the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *                                     or simple key, e.g. "my_opt"
 *  @param         result              the double
 *  @param         pos                 the value position
 *  @return                            0 -  option exists, is of correct type,
 *                                          holds a value at the position;
 *                                     -1 - not found;
 *                                     1 -  error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/04 (Oyranos: 0.1.10)
 *  @date    2009/09/10
 */
int            oyOptions_FindDouble  ( oyOptions_s       * options,
                                       const char        * registration,
                                       int                 pos,
                                       double            * result )
{
  int error = !options;
  oyOptions_s * s = options;
  oyOption_s_ * o = 0;

  if(!error)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return error );

  if(error <= 0)
  {
    o = (oyOption_s_*)oyOptions_Find( options, registration );

    if(o && o->type_ == oyOBJECT_OPTION_S &&
       (o->value_type == oyVAL_DOUBLE ||
        o->value_type == oyVAL_DOUBLE_LIST))
    {
      if(result)
        *result = oyOption_GetValueDouble( (oyOption_s*)o, pos );
      if(o->value_type == oyVAL_DOUBLE_LIST && o->value->dbl_list[0] <= pos)
        error = -1;
      else
        error = 0;

    } else
      error = -1;

    oyOption_Release( (oyOption_s**)&o );
  }

  return error;
}

/** Function oyOptions_SetFromDouble
 *  @memberof oyOptions_s
 *  @brief   change a value
 *
 *  @param         obj                 the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *  @param         value               the value to set
 *  @param         pos                 the position in a value list
 *  @param         flags               can be OY_CREATE_NEW for a new option,
 *                                     OY_STRING_LIST or OY_ADD_ALWAYS
 *  @return                            0 - success; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/04 (Oyranos: 0.1.10)
 *  @date    2009/09/10
 */
int            oyOptions_SetFromDouble(oyOptions_s      ** obj,
                                       const char        * registration,
                                       double              value,
                                       int                 pos,
                                       uint32_t            flags )
{
  int error = 0;
  oyOption_s * o = 0;
  oyOptions_s * s = *obj;

  if(s)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 )

  if(error <= 0)
  {
    if(!*obj)
      *obj = oyOptions_New( 0 );

    o = oyOptions_Find( *obj, registration );

    /** Add a new option if the OY_CREATE_NEW flag is present.
     */
    if((!o && oyToCreateNew_m(flags)) ||
        oyToAddAlways_m(flags))
    {
      o = oyOption_FromRegistration( registration, (*obj)->oy_ );
      error = !o;

      if(error <= 0)
        error = oyOption_SetFromDouble( o, value, pos, 0 );

      oyOptions_MoveIn( (*obj), &o, -1 );

    } else
      oyOption_SetFromDouble( o, value, pos, 0 );

    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_GetType
 *  @memberof oyOptions_s
 *  @brief   select from options with special attribute
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/04 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
oyStruct_s *   oyOptions_GetType     ( oyOptions_s       * options,
                                       int                 pos,
                                       const char        * registration,
                                       oyOBJECT_e          type )
{
  int error = !options;
  int i, n, m = -1, found;
  oyOption_s_ * o = 0;
  oyOptions_s * s = options;
  oyStruct_s * st = 0;

  if(!error)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 );

    n = oyOptions_Count( options );
    for(i = 0; i < n;  ++i)
    {
      o = (oyOption_s_*)oyOptions_Get( options, i );
      found = 1;

      if(found && registration &&
         !oyFilterRegistrationMatch( o->registration, registration, 0 ))
          found = 0;

      if(found && !(o->value && o->value->oy_struct))
        error = 1;

      if(found && type && !error &&
         (o->value_type != oyVAL_STRUCT || !o->value ||
          o->value->oy_struct->type_ != type))
        found = 0;

      if(found && !error)
      if(pos == -1 || ++m == pos)
      {
        if(o->value->oy_struct->copy)
          st = o->value->oy_struct->copy( o->value->oy_struct, 0 );
        else
          st = o->value->oy_struct;

        oyOption_Release( (oyOption_s**)&o );
        break;
      }

      oyOption_Release( (oyOption_s**)&o );
    }

  return st;
}

/** Function oyOptions_MoveInStruct
 *  @memberof oyOptions_s
 *  @brief   change a value
 *
 *  @param         obj                 the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *  @param         oy_struct           the Oyranos style object to move in
 *  @param         flags               can be OY_CREATE_NEW for a new option,
 *                                     or OY_ADD_ALWAYS
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
int            oyOptions_MoveInStruct( oyOptions_s      ** obj,
                                       const char        * registration,
                                       oyStruct_s       ** oy_struct,
                                       uint32_t            flags )
{
  int error = (obj && *obj && (*obj)->type_ != oyOBJECT_OPTIONS_S) ||
              !registration ||
              !oy_struct;
  oyOption_s * o = 0;

  if(error <= 0)
  {
    if(!*obj)
      *obj = oyOptions_New( 0 );

    o = oyOptions_Find( *obj, registration );

    /** Add a new option if the OY_CREATE_NEW flag is present.
     */
    if((!o && oyToCreateNew_m(flags)) ||
        oyToAddAlways_m(flags))
    {
      oyOption_Release( &o );

      o = oyOption_FromRegistration( registration, (*obj)->oy_ );
      error = !o;

      if(error <= 0)
        error = oyOption_StructMoveIn( o, oy_struct );

      if(error <= 0)
        error = oyOptions_MoveIn( (*obj), &o, -1 );
    }

    if(error <= 0 && o && *oy_struct)
      error = oyOption_StructMoveIn( o, oy_struct );
    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_SetFromData
 *  @memberof oyOptions_s
 *  @brief   set a data blob or plain pointer
 *
 *  @param         options             the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *  @param         ptr                 the pointer
 *  @param         size                the pointer size
 *  @param         flags               can be OY_CREATE_NEW for a new option,
 *                                     or OY_ADD_ALWAYS
 *  @return                            0 - success; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/07 (Oyranos: 0.1.10)
 *  @date    2009/11/07
 */
int            oyOptions_SetFromData ( oyOptions_s      ** options,
                                       const char        * registration,
                                       oyPointer           ptr,
                                       size_t              size,
                                       uint32_t            flags )
{
  int error = 0;
  oyOption_s * o = 0;
  oyOptions_s * s = options ? *options : 0;

  if(s)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 )

  if(error <= 0)
  {
    if(!*options)
      *options = oyOptions_New( 0 );

    o = oyOptions_Find( *options, registration );

    /** Add a new option if the OY_CREATE_NEW flag is present.
     */
    if((!o && oyToCreateNew_m(flags)) ||
        oyToAddAlways_m(flags))
    {
      o = oyOption_FromRegistration( registration, (*options)->oy_ );
      error = !o;

      if(error <= 0)
        error = oyOption_SetFromData( o, ptr, size );

      oyOptions_MoveIn( *options, &o, -1 );

    } else
      oyOption_SetFromData( o, ptr, size );

    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_FindData
 *  @memberof oyOptions_s
 *  @brief   get a value
 *
 *  @param         options             the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *                                     or simply a key, e.g. "my_opt"
 *  @param[out]    result              the data; With size == zero, the pointer
 *                                     is static and owned somewhere else.
 *                                     With size set, the pointer is owned my
 +                                     the caller.
 *  @param[out]    size                the data size
 *  @param[in]     allocateFunc        user allocator
 *  @return                            0 -  option exists, is of correct type,
 *                                          holds a value;
 *                                     -1 - not found;
 *                                     1 -  error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/07 (Oyranos: 0.1.10)
 *  @date    2009/11/07
 */
int            oyOptions_FindData    ( oyOptions_s       * options,
                                       const char        * registration,
                                       oyPointer         * result,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  int error = !options;
  oyOptions_s * s = options;
  oyOption_s * o = 0;

  if(!error)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return error );

  if(error <= 0)
  {
    o = oyOptions_Find( options, registration );

    if(o && o->type_ == oyOBJECT_OPTION_S &&
       oyOptionPriv_m(o)->value_type == oyVAL_STRUCT)
    {
      if(result)
        *result = oyOption_GetData( o, size, allocateFunc );
      error = 0;

    } else
      error = -1;

    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_SetSource
 *  @memberof oyOptions_s
 *  @brief   set source attribute
 *
 *  @param         options             the options list or set to manipulate
 *  @param         source              the options new source attribute
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.9)
 *  @date    2009/01/27
 */
int            oyOptions_SetSource   ( oyOptions_s       * options,
                                       oyOPTIONSOURCE_e    source )
{
  int error = !(options && options->type_ == oyOBJECT_OPTIONS_S);
  int i,n;
  oyOption_s * o = 0;

  if(error <= 0)
  {
    n = oyOptions_Count( options );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( options, i );
      oyOptionPriv_m(o)->source = source;
      oyOption_Release( &o );
    }
  }

  return error;
}


/** Function oyOptions_ObserverAdd
 *  @memberof oyOptions_s
 *  @brief   observe options and its elements
 *
 *  @param[in]     object              the options
 *  @param[in]     observer            the in observation intereressted object
 *  @param[in]     user_data           additional static informations
 *  @param[in]     signalFunc          the signal handler
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/28 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
OYAPI int  OYEXPORT
               oyOptions_ObserverAdd ( oyOptions_s       * object,
                                       oyStruct_s        * observer,
                                       oyStruct_s        * user_data,
                                       oyObserver_Signal_f signalFunc )
{
  int error = 0;

  error = oyStructList_ObserverAdd( oyOptionsPriv_m(object)->list_,
                                    (oyStruct_s*) object,
                                    user_data, 0 );
  error = oyStruct_ObserverAdd( (oyStruct_s*)object, observer,
                                user_data, signalFunc );
  return error;
}

/** Function oyOptions_ObserversDisable
 *  @memberof oyOptions_s
 *  @brief   disable signaling
 *
 *  @param[in]     object              the options
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/04/07 (Oyranos: 0.1.10)
 *  @date    2010/04/07
 */
OYAPI int  OYEXPORT
               oyOptions_ObserversDisable (
                                       oyOptions_s       * object )
{
  int error = 0;
    oyOption_s * o;
    int i,n = oyOptions_Count( object );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( object, i );
      oyStruct_DisableSignalSend( (oyStruct_s*)o );
      oyOption_Release( &o );
    }
  return error;
}

/** Function oyOptions_ObserversEnable
 *  @memberof oyOptions_s
 *  @brief   disable signaling
 *
 *  @param[in]     object              the options
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/04/07 (Oyranos: 0.1.10)
 *  @date    2010/04/07
 */
OYAPI int  OYEXPORT
               oyOptions_ObserversEnable (
                                       oyOptions_s       * object )
{
  int error = 0;
    oyOption_s * o;
    int i,n = oyOptions_Count( object );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( object, i );
      oyStruct_EnableSignalSend( (oyStruct_s*)o );
      oyOption_Release( &o );
    }
  return error;
}

#ifdef OY_CMM_API_FILTERS_S_H
/** Function oyOptions_Handle
 *  @brief   handle a request by a module
 *
 *  @param[in]     registration        the module selector
 *  @param[in]     options             options
 *  @param[in]     command             the command to handle
 *  @param[out]    result              options to the policy module
 *  @return                            0 - indifferent, >= 1 - error,
 *                                     <= -1 - issue,
 *                                     + a message should be sent
 *
 *  @version Oyranos: 0.3.0
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2011/02/22
 */
int             oyOptions_Handle     ( const char        * registration,
                                       oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  int error = 0;
  oyOptions_s * s = options;

  if(!options && !command)
    return error;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 )

  if(!error)
  {
    oyCMMapiFilters_s * apis;
    int apis_n = 0, i, found = 0;
    oyCMMapi10_s_ * cmm_api10 = 0;
    char * class_name, * api_reg;
    char * test = 0;
    uint32_t * rank_list = 0,
               api_n = 0;

    class_name = oyFilterRegistrationToText( registration, oyFILTER_REG_TYPE, 0 );
    api_reg = oyStringCopy_("//", oyAllocateFunc_ );
    STRING_ADD( api_reg, class_name );
    oyFree_m_( class_name );

    STRING_ADD( test, "can_handle." );
    if(command && command[0])
      STRING_ADD( test, command );

    apis = oyCMMsGetFilterApis_( 0,0, api_reg, oyOBJECT_CMM_API10_S,
                                 oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                 &rank_list, &api_n );
    if(rank_list) oyDeAllocateFunc_(rank_list); rank_list = 0;

    apis_n = oyCMMapiFilters_Count( apis );
    if(test)
      for(i = 0; i < apis_n; ++i)
      {
        cmm_api10 = (oyCMMapi10_s_*) oyCMMapiFilters_Get( apis, i );

        if(oyFilterRegistrationMatch( cmm_api10->registration, registration, 0))
        {
          if(cmm_api10->oyMOptions_Handle)
          {
            error = cmm_api10->oyMOptions_Handle( s, test, result );
            if(error == 0)
            {
              found = 1;
              error = cmm_api10->oyMOptions_Handle( s, command, result );
            }

          } else
            error = 1;

          if(error > 0)
          {
            WARNc2_S( "%s %s",_("error in module:"), cmm_api10->registration );
          }
        }

        if(cmm_api10->release)
          cmm_api10->release( (oyStruct_s**)&cmm_api10 );
      }
    else
      WARNc2_S( "%s %s",_("Could not allocate memory for:"),
                cmm_api10->registration );

    oyFree_m_( test );
    oyCMMapiFilters_Release( &apis );
    if(!found && error == 0)
      error = -1;
  }

  return error;
}
#endif /* OY_CMM_API_FILTERS_S_H */

/** Function oyOptions_SetDriverContext
 *  @memberof oyOptions_s
 *  @brief   set a device option from a given external context
 *
 *  The options will be created in case they do not exist. The
 *  driver_context_type accepts "xml". The data in driver_context will be
 *  converted to a options set following the Oyranos options XML schemes with
 *  the help oyOptions_FromText().
 *  Any other pointer will be converted to a oyBlob_s object. The name of that
 *  object will come from driver_context_type.
 *
 *  @param[in,out] options             options for the device
 *  @param[in]     driver_context      driver context
 *  @param[in]     driver_context_type "xml" or something related to the driver
 *  @param[in]     driver_context_size size of driver_context
 *  @param[in]     object              a optional object
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/18 (Oyranos: 0.1.10)
 *  @date    2009/05/18
 */
OYAPI int  OYEXPORT
           oyOptions_SetDriverContext( oyOptions_s      ** options,
                                       oyPointer           driver_context,
                                       const char        * driver_context_type,
                                       size_t              driver_context_size,
                                       oyObject_s          object )
{
  int error = !options || !driver_context_type,
      l_error = 0;
  oyOptions_s * s = 0,
              * opts_tmp = 0;
  oyOption_s * o = 0;
  char * key = 0;

  if(options && *options)
  {
    s = *options;
    oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 );
  }

  if(!error)
  {
    if(!s)
      s = oyOptions_New( 0 );
    error = !s;

    key = oyStringAppend_( "driver_context.", driver_context_type,
                           oyAllocateFunc_ );
  }

  if(!error)
  {
    o = oyOption_FromRegistration( key, object );

    if(oyFilterRegistrationMatch( driver_context_type, "xml", 0 ))
    {
      opts_tmp = oyOptions_FromText( (char*)driver_context, 0, object );
      error = oyOption_StructMoveIn ( o, (oyStruct_s**) &opts_tmp );
    }
    else
      error = oyOption_SetFromData( o, driver_context, driver_context_size );

    if(error <= 0)
      l_error = oyOptions_MoveIn( s, &o, -1 ); OY_ERR

    oyFree_m_( key );
  }

  if(!error)
    *options = s;
  else
    oyOptions_Release( &s );

  return error;
}
