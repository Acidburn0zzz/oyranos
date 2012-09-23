/** Function    oyFilterNodes_Release__Members
 *  @memberof   oyFilterNodes_s
 *  @brief      Custom FilterNodes destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filternodes  the FilterNodes object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterNodes_Release__Members( oyFilterNodes_s_ * filternodes )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filternodes->member );
   */

  if(filternodes->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = filternodes->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filternodes->member );
     */
  }
}

/** Function    oyFilterNodes_Init__Members
 *  @memberof   oyFilterNodes_s
 *  @brief      Custom FilterNodes constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filternodes  the FilterNodes object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterNodes_Init__Members( oyFilterNodes_s_ * filternodes )
{
  return 0;
}

/** Function    oyFilterNodes_Copy__Members
 *  @memberof   oyFilterNodes_s
 *  @brief      Custom FilterNodes copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterNodes_s_ input object
 *  @param[out]  dst  the output oyFilterNodes_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterNodes_Copy__Members( oyFilterNodes_s_ * dst, oyFilterNodes_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */

  return error;
}
