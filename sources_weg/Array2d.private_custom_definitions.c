/** Function    oyArray2d_Release__Members
 *  @memberof   oyArray2d_s
 *  @brief      Custom Array2d destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  array2d  the Array2d object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyArray2d_Release__Members( oyArray2d_s_ * array2d )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &array2d->member );
   */

  if(array2d->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = array2d->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( array2d->member );
     */
  }
}

/** Function    oyArray2d_Init__Members
 *  @memberof   oyArray2d_s
 *  @brief      Custom Array2d constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  array2d  the Array2d object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyArray2d_Init__Members( oyArray2d_s_ * array2d )
{
  return 0;
}

/** Function    oyArray2d_Copy__Members
 *  @memberof   oyArray2d_s
 *  @brief      Custom Array2d copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyArray2d_s_ input object
 *  @param[out]  dst  the output oyArray2d_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyArray2d_Copy__Members( oyArray2d_s_ * dst, oyArray2d_s_ * src)
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
