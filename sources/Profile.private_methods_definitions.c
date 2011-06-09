/* Static helper functions { */
static
const char * oyProfile_Message_      ( oyPointer           profile,
                                       int                 flags )
{
  oyStruct_s*s = profile;
  return oyProfile_GetText( (oyProfile_s*)s, oyNAME_DESCRIPTION );
}

/** @internal
 *  @memberof oyProfile_s
 *  @brief   check internal ICC profile ID
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/04/10 (Oyranos: 0.3.0)
 *  @date    2011/04/10
 */
static int oyProfile_HasID_          ( oyProfile_s       * s )
{
  int has_id = 0;

  if(s->block_ && s->size_ >= 132 )
  {
    char * data = s->block_;
    uint32_t id[4];
    memcpy( id, &data[84], 16 );

    if(id[0] || id[1] || id[2] || id[3])
      has_id = 1;
  }

  return has_id;
}

/** @internal
 *  @memberof oyProfile_s
 *  @brief   hash for oyProfile_s
 *
 *  Get ICC ID from profile or compute.
 +
 *  @version Oyranos: 0.3.0
 *  @since   2007/11/0 (Oyranos: 0.1.8)
 *  @date    2011/04/10
 */
static int oyProfile_GetHash_        ( oyProfile_s       * s,
                                       int                 flags )
{
  int error = 1;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(s->block_ && s->size_)
  {
    int has_id = oyProfile_HasID_( s );

    oyObject_HashSet( s->oy_, 0 );
    if(flags & OY_COMPUTE ||
       !has_id)
      error = oyProfileGetMD5( s->block_, s->size_, s->oy_->hash_ptr_ );
    else
    {
      char * data = s->block_;
      uint32_t id[4];
      int i;
      memcpy( id, &data[84], 16 );

      for(i = 0; i < 4; ++i)
        id[i] = oyValueUInt32( id[i] );
      memcpy(s->oy_->hash_ptr_, id, 16);
      error = 0;
    }

    if(error > 0)
      oyObject_HashSet( s->oy_, 0 );
  }
  return error;
}
/* } Static helper functions */


/** Function  oyProfile_FromMemMove_
 *  @memberof oyProfile_s
 *  @brief    Create from in memory blob
 *  @internal
 *
 *  @param[in]    size           buffer size
 *  @param[in]    block          pointer to memory containing a profile
 *  @param[in]    flags          for future use
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyProfile_s_* oyProfile_FromMemMove_  ( size_t              size,
                                       oyPointer         * block,
                                       int                 flags,
                                       oyObject_s          object)
{
  oyProfile_s_ * s = oyProfile_New_( object );
  int error = 0;

  if(block  && *block && size)
  {
    s->block_ = *block;
    *block = 0;
    if(!s->block_)
      error = 1;
    else
      s->size_ = size;
  }

  if (!s->block_)
  {
    WARNc1_S( "%s", "no data" )
    return 0;
  }

  /* Comparision strategies
      A
       - search for similiar arguments in the structure
       - include the affect of the arguments (resolve type -> filename)
       - exclude paralell structure elements (filename -> ignore blob)
      B
       - use always the ICC profiles md5
      C
       - always the flattened Oyranos profile md5

       - A higher level API can maintain its own cache depending on costs.
   */

  if(error <= 0)
    error = oyProfile_GetHash_( s );

  if(error)
  {
    WARNc1_S( "hash error %d", error )
    return 0;
  }

  if(error <= 0)
    error = !oyProfile_GetSignature ( (oyProfile_s*)s, oySIGNATURE_COLOUR_SPACE );

  if(error)
  {
    WARNc1_S( "signature error %d", error )
    return 0;
  }

  if(error <= 0)
  {
    s->names_chan_ = 0;
    s->channels_n_ = 0;
    s->channels_n_ = oyProfile_GetChannelsCount( (oyProfile_s*)s );
    error = (s->channels_n_ <= 0);
  }

  if(error)
  {
    icHeader *h = 0;
    icSignature sig = 0;

    h = (icHeader*) s->block_;

    sig = oyValueCSpaceSig( h->colorSpace );

    WARNc3_S("Channels <= 0 %d %s %s", s->channels_n_,
             oyICCColourSpaceGetName(sig),
             oyICCColourSpaceGetName(h->colorSpace))
  }

  return s;
}

static oyStructList_s_ * oy_profile_s_file_cache_ = 0;

/** Function  oyProfile_FromFile_
 *  @memberof oyProfile_s
 *  @brief    Create from file
 *  @internal
 *
 *  @param[in]    name           profile file name
 *  @param[in]    flags          for future extension
 *  @param[in]    object         the optional base
 *
 *  flags supports OY_NO_CACHE_READ and OY_NO_CACHE_WRITE to disable cache
 *  reading and writing. The cache flags are useful for one time profiles or
 *  scanning large numbers of profiles.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/0 (Oyranos: 0.1.9)
 *  @date    2010/05/18
 */
oyProfile_s_ *  oyProfile_FromFile_  ( const char        * name,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyProfile_s_ * s = 0;
  int error = 0;
  size_t size = 0;
  oyPointer block = 0;
  oyAlloc_f allocateFunc = 0;
  oyHash_s_ * entry = 0;
  char * file_name = 0;

  if(object)
    allocateFunc = object->allocateFunc_;

  if(!oyToNoCacheRead_m(flags) || !oyToNoCacheWrite_m(flags))
  {
    if(!oy_profile_s_file_cache_)
      oy_profile_s_file_cache_ = oyStructList_New_( 0 );

    if(!object)
    {
      entry = oyCacheListGetEntry_ ( oy_profile_s_file_cache_, 0, name );

      if(!oyToNoCacheRead_m(flags))
      {
        s = (oyProfile_s_*) oyHash_GetPointer_( entry, oyOBJECT_PROFILE_S);
        s = (oyProfile_s_*) oyProfile_Copy( (oyProfile_s*)s, 0 );
        if(s)
          return s;
      }
    }
  }

  if(error <= 0 && name && !s)
  {
    file_name = oyFindProfile_( name );
    block = oyGetProfileBlock( file_name, &size, allocateFunc );
    if(!block || !size)
      error = 1;
  }

  if(error <= 0)
  {
    s = oyProfile_FromMemMove_( size, &block, 0, object );

    if(!s)
      error = 1;

    /* We expect a incomplete filename attached to s and try to correct this. */
    if(error <= 0 && !file_name && s->file_name_)
    {
      file_name = oyFindProfile_( s->file_name_ );
      if(file_name && s->oy_->deallocateFunc_)
      {
        s->oy_->deallocateFunc_( s->file_name_ );
        s->file_name_ = 0;
      }
    }

    if(error <= 0 && file_name)
    {
      s->file_name_ = oyStringCopy_( file_name, s->oy_->allocateFunc_ );
      oyDeAllocateFunc_( file_name ); file_name = 0;
    }

    if(error <= 0 && !s->file_name_)
      error = 1;
  }

  if(error <= 0 && s && entry)
  {
    if(!oyToNoCacheWrite_m(flags))
    {
      /* 3b.1. update cache entry */
      error = oyHash_SetPointer_( entry, (oyStruct_s*)s );
#if 0
    } else {
      int i = 0, n = 0, pos = -1;

      n = oyStructList_Count( (oyStructList_s*)oy_profile_s_file_cache_ );
      for( i = 0; i < n; ++i )
        if((oyStruct_s*)entry == oyStructList_Get_( oy_profile_s_file_cache_,i))
          pos = i;
      if(pos >= 0)
        oyStructList_ReleaseAt( (oyStructList_s*)oy_profile_s_file_cache_, pos );
#endif
    }
  }

  oyHash_Release_( &entry );

  return s;
}

/** Function  oyProfile_TagsToMem_
 *  @memberof oyProfile_s
 *  @brief    Get the parsed ICC profile back into memory
 *  @internal
 *
 *  Non thread save
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_TagsToMem_    ( oyProfile_s_      * profile,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  oyPointer block = 0;
  int error = !(profile && profile->block_ &&
                profile->size_ > 132 && profile->tags_ && size);

  if(error <= 0)
  {
    size_t size_ = 0;

    oyPointer icc_header = 0;
    oyPointer icc_tagtable = 0;

    /* 1. header */
    icc_header = oyProfile_WriteHeader_( profile, &size_ );

    error = !icc_header;

    /* 2. tag table */
    if(error <= 0)
    {
      icc_tagtable = oyProfile_WriteTagTable_( profile, &size_ );
      error = !icc_tagtable;
    }

    /* 3. tags */
    if(error <= 0)
    {
      block = oyProfile_WriteTags_( profile, &size_, icc_header, icc_tagtable,
                                    allocateFunc );
      error = !block;
    }

    if(error <= 0)
    {
      oyDeAllocateFunc_(icc_header);
      oyDeAllocateFunc_(icc_tagtable);
      *size = size_;
    }
  }

  return block;
}

/** Function  oyProfile_ToFile_
 *  @memberof oyProfile_s
 *  @brief    Save from in memory profile to file
 *  @internal
 *
 *
 *  @since Oyranos: version 0.1.8
 *  @date  20 december 2007 (API 0.1.8)
 */
int          oyProfile_ToFile_       ( oyProfile_s_      * profile,
                                       const char        * file_name )
{
  oyProfile_s_ * s = profile;
  int error = !s || !file_name;
  oyPointer buf = 0;
  size_t size = 0;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(error <= 0)
  {
    buf = oyProfile_GetMem ( (oyProfile_s*)s, &size, 0, 0 );
    if(buf && size)
    error = oyWriteMemToFile_( file_name, buf, size );

    if(buf) oyDeAllocateFunc_(buf);
    size = 0;
  }

  return error;
}

/** Function  oyProfile_Match_
 *  @memberof oyProfile_s
 *  @brief    Check if a profiles matches by some properties
 *  @internal
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 */
int32_t      oyProfile_Match_        ( oyProfile_s_      * pattern,
                                       oyProfile_s_      * profile )
{
  int32_t match = 0;
  int i;
  icSignature pattern_sig, profile_sig;

  if(pattern && profile)
  {
    /*match = oyProfile_Equal_(pattern, profile);*/ /* too expensive */
    if(!match)
    {
      match = 1;
      for( i = 0; i < (int)oySIGNATURE_MAX; ++i)
      {
        pattern_sig = oyProfile_GetSignature( (oyProfile_s*)pattern, (oySIGNATURE_TYPE_e) i );
        profile_sig = oyProfile_GetSignature( (oyProfile_s*)profile, (oySIGNATURE_TYPE_e) i );

        if(pattern_sig && profile_sig && pattern_sig != profile_sig)
          match = 0;
      }
    }
  }

  return match;
}

/** Function  oyProfile_Hashed_
 *  @memberof oyProfile_s
 *  @brief    Check if a profile has a hash sum computed
 *  @internal
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 */
int32_t      oyProfile_Hashed_       ( oyProfile_s_      * s )
{
  if(s && s->type_ == oyOBJECT_PROFILE_S)
    return oyObject_Hashed_( s->oy_ );
  else
    return 0;
}

/** Function  oyProfile_GetFileName_r
 *  @memberof oyProfile_s
 *  @brief    get the ICC profile location in the filesystem
 *  @internal
 *
 *  This function tries to find a profile on disk matching a possibly memory
 *  only profile. In case the profile was previously opened from file or
 *  from a Oyranos default profile, the associated filename will simply be
 *  retuned.
 *
 *  @param         profile             the profile
 *  @param         allocateFunc        memory allocator
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 *  @date    2008/02/01
 */
char *       oyProfile_GetFileName_r ( oyProfile_s_      * profile,
                                       oyAlloc_f           allocateFunc )
{
  char * name = 0;
  oyProfile_s * tmp = 0;
  oyProfile_s_ * s = profile;
  int error = !s;
  char ** names = 0;
  uint32_t count = 0, i = 0;
  char *  hash = 0;
  char    tmp_hash[34];

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
  {
    if(s->file_name_ && !hash)
    {
      name = s->file_name_;
    } else {

      names = /*(const char**)*/ oyProfileListGet_ ( NULL, &count );

      for(i = 0; i < count; ++i)
      {
        if(names[i])
        {
          if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
            tmp = oyProfile_FromFile( names[i], OY_NO_CACHE_WRITE, 0 );

          if(hash && tmp)
          {
            uint32_t * h = (uint32_t*)s->oy_->hash_ptr_;
            if(h)
              oySprintf_(tmp_hash, "%x%x%x%x", h[0], h[1], h[2], h[3]);
            else
              oySprintf_(tmp_hash, "                " );
            if(memcmp( hash, tmp_hash, 2*OY_HASH_SIZE ) == 0 )
            {
              name = names[i];
              break;
            }
          } else
          if(oyProfile_Equal( (oyProfile_s*)s, tmp ))
          {
            name = names[i];
            break;
          }

          oyProfile_Release( &tmp );
        }
      }

      name = oyFindProfile_( name );
      if(name)
      {
        s->file_name_ = oyStringCopy_( name, s->oy_->allocateFunc_ );
        oyDeAllocateFunc_( name );
      }
      oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
    }
  }

  name = oyStringCopy_( s->file_name_, allocateFunc );

  return name;
}

/** Function  oyProfile_GetTagByPos_
 *  @memberof oyProfile_s
 *  @brief    Get a profile tag
 *  @internal
 *
 *  Non thread save
 *
 *  @param[in]     profile             the profile
 *  @param[in]     pos                 header + tag position
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/01
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 */
oyProfileTag_s * oyProfile_GetTagByPos_( oyProfile_s_    * profile,
                                       int                 pos )
{
  oyProfileTag_s * tag = 0;
  oyProfile_s_ * s = profile;
  int error = !profile;
  int n = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && profile->type_ != oyOBJECT_PROFILE_S)
    error = 1;

  if(error <= 0)
  {
    s = profile;
    n = oyStructList_Count( profile->tags_ );
  }

  if(error <= 0 && n)
  {
    tag = (oyProfileTag_s*) oyStructList_GetRef( profile->tags_, pos );
    return tag;
  }

  /* parse the ICC profile struct */
  if(error <= 0 && s->block_)
  {
    icSignature magic = oyProfile_GetSignature( (oyProfile_s*)s, oySIGNATURE_MAGIC );
    icSignature profile_cmmId = oyProfile_GetSignature( (oyProfile_s*)s, oySIGNATURE_CMM );
    char profile_cmm[5] = {0,0,0,0,0};
    icProfile * ic_profile = s->block_;
    int min_icc_size = 132 + sizeof(icTag);

    error = (magic != icMagicNumber);

    profile_cmmId = oyValueUInt32( profile_cmmId );
    error = !memcpy( profile_cmm, &profile_cmmId, 4 );
    profile_cmmId = 0;

    if(error <= 0 && s->size_ > min_icc_size)
    {
      uint32_t size = 0;
      uint32_t tag_count = 0;
      icTag *tag_list = 0;
      int i = 0;
      oyProfileTag_s_ * tag_ = oyProfileTag_New_( 0 );
      char h[5] = {"head"};
      uint32_t * hi = (uint32_t*)&h;
      char *tag_block = 0;

      oyAllocHelper_m_( tag_block, char, 132, 0, return 0 );
      error = !memcpy( tag_block, s->block_, 132 );
      error = oyProfileTag_Set( (oyProfileTag_s*)tag_,
                                (icTagSignature)*hi,
                                (icTagTypeSignature)*hi,
                                oyOK, 132, tag_block );
      if(error <= 0)
        error = !memcpy( tag_->profile_cmm_, profile_cmm, 4 );

      if(0 == pos)
        tag = oyProfileTag_Copy( (oyProfileTag_s*)tag_, 0 );
      error = oyProfile_TagMoveIn_( s, (oyProfileTag_s**)&tag_, -1 );


      size = oyProfile_GetSignature( (oyProfile_s*)s, oySIGNATURE_SIZE );
      tag_count = oyValueUInt32( ic_profile->count );

      tag_list = (icTag*)&((char*)s->block_)[132];

      /* parse the profile and add tags to the oyProfile_s::tags_ list */
      for(i = 0; i < tag_count; ++i)
      {
        icTag *ic_tag = &tag_list[i];
        size_t offset = oyValueUInt32( ic_tag->offset );
        size_t tag_size = oyValueUInt32( ic_tag->size );
        char *tmp = 0;
#ifdef DEBUG
        oyChar **texts = 0;
        int32_t texts_n = 0;
        int j;
#endif
        oySTATUS_e status = oyOK;
        icTagSignature sig = oyValueUInt32( ic_tag->sig );
        icTagTypeSignature tag_type = 0;

        oyProfileTag_s_ * tag_ = oyProfileTag_New_( 0 ); //FIXME Redeclaration

        tag_block = 0;

        if((offset+tag_size) > s->size_)
          status = oyCORRUPTED;
        else
        {
          icTagBase * tag_base = 0;

          oyAllocHelper_m_( tag_block, char, tag_size, 0, return 0 );
          tmp = &((char*)s->block_)[offset];
          error = !memcpy( tag_block, tmp, tag_size );

          tag_base = (icTagBase*) tag_block;
          tag_type = oyValueUInt32( tag_base->sig );
        }

        error = oyProfileTag_Set( (oyProfileTag_s*)tag_, sig, tag_type,
                                  status, tag_size, tag_block );
        tag_->offset_orig = offset;
        if(error <= 0)
          error = !memcpy( tag_->profile_cmm_, profile_cmm, 4 );

#ifdef DEBUG
        if(oy_debug > 3)
        {
          DBG_PROG5_S("%d[%d @ %d]: %s %s",
            i, (int)tag_->size_, (int)tag_->offset_orig,
            oyICCTagTypeName( tag_->tag_type_ ),
            oyICCTagDescription( tag_->use ) );
          texts = oyProfileTag_GetText((oyProfileTag_s*)tag_,&texts_n,0,0,0,0);
          for(j = 0; j < texts_n; ++j)
            DBG_PROG2_S("%s: %s", tag_->last_cmm_, texts[j]?texts[j]:"");
          if(texts_n && texts)
            oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
        }
#endif

        if(i == pos-1)
          tag = oyProfileTag_Copy( (oyProfileTag_s*)tag_, 0 );

        if(error <= 0)
          error = oyProfile_TagMoveIn_( s, (oyProfileTag_s**)&tag_, -1 );
      }
    }
  }

  return tag;
}

/** Function  oyProfile_GetTagCount_
 *  @memberof oyProfile_s
 *  @internal
 *
 *  Non thread save
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 *  @date    2009/12/29
 */
int                oyProfile_GetTagCount_ ( oyProfile_s_      * profile )
{
  int n = 0;
  oyProfile_s_ *s = profile;
  int error = !s;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && !s->tags_)
    error = 1;

  if(error <= 0)
    n = oyStructList_Count( s->tags_ );

  if(error <= 0 && !n)
  {
    oyProfileTag_s * tag = oyProfile_GetTagByPos_ ( s, 0 );
    oyProfileTag_Release( &tag );
    n = oyStructList_Count( s->tags_ );
  }

  return n;
}

/** Function  oyProfile_TagMoveIn_
 *  @memberof oyProfile_s
 *  @brief    Add a tag to a profile
 *  @internal
 *
 *  non thread save
 *
 *  The profile is needs probably be marked as modified after calling this
 *  function.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/06 (Oyranos: 0.1.10)
 *  @date    2009/12/29
 */
int          oyProfile_TagMoveIn_    ( oyProfile_s_      * profile,
                                       oyProfileTag_s   ** obj,
                                       int                 pos )
{
  oyProfile_s_ * s = profile;
  int error = !s;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(!(obj && *obj && (*obj)->type_ == oyOBJECT_PROFILE_TAG_S))
    error = 1;

  if(error <= 0)
    error = oyStructList_MoveIn ( s->tags_, (oyStruct_s**)obj, pos,
                                  OY_OBSERVE_AS_WELL );

  return error;
}

/** Function  oyProfile_TagReleaseAt_
 *  @memberof oyProfile_s
 *  @brief    Remove a tag from a profile
 *  @internal
 *
 *  Non thread save
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/03/11 (Oyranos: 0.1.8)
 *  @date    2009/12/29
 */
int          oyProfile_TagReleaseAt_ ( oyProfile_s_      * profile,
                                       int                 pos )
{
  oyProfile_s_ * s = profile;
  return oyStructList_ReleaseAt ( s->tags_, pos );
}

/** Function  oyProfile_DeviceAdd_
 *  @memberof oyProfile_s
 *  @brief    Add device and driver informations to a profile
 *  @internal
 *
 *  oyProfile_DeviceAdd_() is for storing device/driver informations in a
 *  ICC profile. So the profile can be sent over internet and Oyranos, or
 *  an other CMS, can better match to a device/driver on the new host.
 *
 *  @param         profile             the profile
 *  @param         device              device and driver informations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/18 (Oyranos: 0.1.10)
 *  @date    2009/05/18
 */
int                oyProfile_DeviceAdd_(oyProfile_s_      * profile,
                                       oyConfig_s        * device )
{
  int error = !profile;
  oyProfile_s_ * s = profile;
  oyProfileTag_s_ * pddt = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
  {
      pddt = (oyProfileTag_s_*)oyProfile_GetTagById( (oyProfile_s*)s, icSigProfileDetailDescriptionTag_ );

      /* icSigProfileDetailDescriptionTag_ */
      if(error <= 0 && !pddt)
      {
        oyStructList_s * list = 0;

        list = oyStructList_New(0);
        error = oyStructList_MoveIn( list, (oyStruct_s**) &device, 0,
                                     OY_OBSERVE_AS_WELL );

        if(error <= 0)
        {
          pddt = (oyProfileTag_s_*)oyProfileTag_Create( list, icSigProfileDetailDescriptionTag_,
                                                        0, OY_MODULE_NICK, 0);
          error = !pddt;
        }

        if(error <= 0)
          pddt->use = icSigProfileDetailDescriptionTag_;

        oyStructList_Release( &list );

        if(pddt)
        {
          error = oyProfile_TagMoveIn_( s, (oyProfileTag_s**)&pddt, -1 );
          ++s->tags_modified_;
        }
      }
  }

  return error;
}

/** Function  oyProfile_WriteHeader_
 *  @memberof oyProfile_s
 *  @brief    Get the parsed ICC profile back into memory
 *  @internal
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_WriteHeader_  ( oyProfile_s_      * profile,
                                       size_t            * size )
{
  oyPointer block = 0;
  char h[5] = {"head"};
  uint32_t * hi = (uint32_t*)&h;

  if(profile && profile->block_ && profile->size_ > 132 &&
     profile->tags_)
  {
    int n = oyProfile_GetTagCount_( profile );
    oyProfileTag_s_ * tag = (oyProfileTag_s_*)oyProfile_GetTagByPos_ ( profile, 0 );

    if(n && tag->use == *hi && tag->block_ && tag->size_ >= 128)
    {
      block = oyAllocateFunc_ (132);
      if(block)
      {
        memset( block, 0, 132 );
        memcpy( block, tag->block_, 128 );
      }
    }

    oyProfileTag_Release( (oyProfileTag_s**)&tag );
  }

  return block;
}

/** Function  oyProfile_WriteTags_
 *  @memberof oyProfile_s
 *  @brief    Get the parsed ICC profile back into memory
 *  @internal
 *
 *  Call in following order:
 *         -  oyProfile_WriteHeader_
 *         -  oyProfile_WriteTagTable_
 *         -  oyProfile_WriteTags_
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/01
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_WriteTags_    ( oyProfile_s_      * profile,
                                       size_t            * size,
                                       oyPointer           icc_header,
                                       oyPointer           icc_list,
                                       oyAlloc_f           allocateFunc )
{
  char * block = 0;
  int error = !(profile && profile->block_ &&
                profile->size_ > 132 && profile->tags_ && size &&
                icc_header && icc_list);

  if(error <= 0)
  {
    int n = 0, i;
    size_t len = 0;

    n = oyProfile_GetTagCount_( profile );
    block = (char*) oyAllocateFunc_(132 + n * sizeof(icTag));
    error = !block;

    if(error <= 0)
    {
      memset( block, 0, 132 + n * sizeof(icTag) );
      error = !memcpy( block, icc_header, 132 );
    }

    len = 132;

    if(error <= 0)
      error = !memcpy( &block[len], icc_list, (n-1) * sizeof(icTag) );

    len += sizeof(icTag) * (n-1);

    for(i = 0; i < n - 1; ++i)
    {
      char h[5] = {"head"};
      uint32_t * hi = (uint32_t*)&h;
      char * temp = 0;
      icTagList* list = (icTagList*) &block[128];
      oyProfileTag_s_ * tag = (oyProfileTag_s_*)oyProfile_GetTagByPos_ ( profile, i + 1 );
      size_t size = 0;

      if(error <= 0)
        error = !tag;

      if(error <= 0)
        size = tag->size_;

      if(error <= 0 && tag->use == *hi)
      {
        oyProfileTag_Release( (oyProfileTag_s**)&tag );
        continue;
      }

      if(error <= 0)
      {
        list->tags[i].sig = oyValueUInt32( (icTagSignature)tag->use );
        list->tags[i].offset = oyValueUInt32( (icUInt32Number)len );
        list->tags[i].size = oyValueUInt32( (icUInt32Number)size );
        temp = (char*) oyAllocateFunc_ ( len + size +
                                               (size%4 ? 4 - size%4 : 0));
        if(temp)
          memset( temp, 0, len + size + (size%4 ? 4 - size%4 : 0));
      }

      if(temp)
        error = !memcpy( temp, block, len );
      if(error <= 0)
      {
        error = !memcpy( &temp[len], tag->block_, tag->size_);
        len += size + (size%4 ? 4 - size%4 : 0);
      }

      if(error <= 0)
      {
        oyDeAllocateFunc_(block);
        block = temp;

        oyProfileTag_Release( (oyProfileTag_s**)&tag );
      }
      temp = 0;
    }

    if(error <= 0)
    {
      char h[5] = {OY_MODULE_NICK};
      uint32_t * hi = (uint32_t*)&h;
      icProfile* p = 0;
      icHeader* header = 0;
      oyPointer temp = oyAllocateWrapFunc_( len, allocateFunc );

      error = !temp;
      if(error <= 0)
        error = !memcpy( temp, block, len );

      oyDeAllocateFunc_( block );
      block = temp; temp = 0;

      p = (icProfile*) block;
      p->count = oyValueUInt32( (icUInt32Number) n - 1);

      header = (icHeader*) block;
      header->size = oyValueUInt32( (icUInt32Number) len);
#if 0 /* we dont override the CMM's id */
      header->creator = *hi;
#endif
#if defined(__APPLE__)
      oySprintf_( h, "APPL" );
#elif defined(WIN32)
      oySprintf_( h, "MSFT" );
#else
      oySprintf_( h, "*nix" );
#endif
      header->platform = *hi;
      *size = len;
    }
  }

  return block;
}

/** Function  oyProfile_WriteTagTable_
 *  @memberof oyProfile_s
 *  @brief    Get the parsed ICC profile back into memory
 *  @internal
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_WriteTagTable_( oyProfile_s_      * profile,
                                       size_t            * size )
{
  oyPointer block = 0;
  int error = !(profile && profile->block_ &&
                profile->size_ > 132 && profile->tags_ && size);

  if(error <= 0)
  {
    int n = oyProfile_GetTagCount_( profile );
    size_t size = 0;

    size = sizeof (icTag) * n;
    error = !size;

    if(error <= 0)
    {
      block = oyAllocateFunc_( size );
      error = !block;
    }

    if(error <= 0)
      error = !memset( block, 0, size );
  }

  return block;
}
