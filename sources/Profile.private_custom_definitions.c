/** Function    oyProfile_Release__Members
 *  @memberof   oyProfile_s
 *  @brief      Custom Profile destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  profile  the Profile object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyProfile_Release__Members( oyProfile_s_ * profile )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &profile->member );
   */

  int i;
  if(profile->names_chan_)
    for(i = 0; i < profile->channels_n_; ++i)
      if(profile->names_chan_[i])
        oyObject_Release( &profile->names_chan_[i] );
  /*oyOptions_Release( profile->options );*/

  profile->sig_ = (icColorSpaceSignature)0;

  oyStructList_Release(&profile->tags_);

  if(profile->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = profile->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( profile->member );
     */
    if(profile->names_chan_)
      deallocateFunc( profile->names_chan_ ); profile->names_chan_ = 0;

    if(profile->block_)
      deallocateFunc( profile->block_ ); profile->block_ = 0; profile->size_ = 0;

    if(profile->file_name_)
      deallocateFunc( profile->file_name_ ); profile->file_name_ = 0;

  }
}

/** Function    oyProfile_Init__Members
 *  @memberof   oyProfile_s
 *  @brief      Custom Profile constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  profile  the Profile object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyProfile_Init__Members( oyProfile_s_ * profile )
{
  profile->tags_ = oyStructList_Create( profile->type_, 0, 0 );
  profile->tags_modified_ = 0;

  return 0;
}

/** Function    oyProfile_Copy__Members
 *  @memberof   oyProfile_s
 *  @brief      Custom Profile copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyProfile_s_ input object
 *  @param[out]  dst  the output oyProfile_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyProfile_Copy__Members( oyProfile_s_ * dst, oyProfile_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */
  if(src->block_ && src->size_)
  {
    dst->block_ = allocateFunc_( src->size_ );
    if(!dst->block_)
      error = 1;
    else
    {
      dst->size_ = src->size_;
      error = !memcpy( dst->block_, src->block_, src->size_ );
    }
  }

  if(error <= 0)
  {
    if(!oyProfile_Hashed_(dst))
      error = oyProfile_GetHash_( dst );
  }

  if(error <= 0)
  {
    if(src->sig_)
      dst->sig_ = src->sig_;
    else
      error = !oyProfile_GetSignature( (oyProfile_s*)dst, oySIGNATURE_COLOUR_SPACE );
  }

  if(error <= 0)
    dst->file_name_ = oyStringCopy_( src->file_name_, allocateFunc );

  if(error <= 0)
    dst->use_default_ = src->use_default_;

  if(error <= 0)
  {
    dst->channels_n_ = oyProfile_GetChannelsCount( (oyProfile_s*)dst );
    error = (dst->channels_n_ <= 0);
  }

  if(error <= 0)
    oyProfile_SetChannelNames( (oyProfile_s*)dst, src->names_chan_ );

  if(error)
  {
    WARNc_S("Could not create structure for profile.")
  }

  return error;
}
