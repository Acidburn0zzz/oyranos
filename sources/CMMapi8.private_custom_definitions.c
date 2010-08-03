/** Function    oyCMMapi8_Release__Members
 *  @memberof   oyCMMapi8_s
 *  @brief      Custom CMMapi8 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi8  the CMMapi8 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapi8_Release__Members( oyCMMapi8_s_ * cmmapi8 )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi8->member );
   */

  if(cmmapi8->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = cmmapi8->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi8->member );
     */
  }
}

/** Function    oyCMMapi8_Init__Members
 *  @memberof   oyCMMapi8_s
 *  @brief      Custom CMMapi8 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi8  the CMMapi8 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi8_Init__Members( oyCMMapi8_s_ * cmmapi8 )
{
  return 0;
}

/** Function    oyCMMapi8_Copy__Members
 *  @memberof   oyCMMapi8_s
 *  @brief      Custom CMMapi8 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi8_s_ input object
 *  @param[out]  dst  the output oyCMMapi8_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi8_Copy__Members( oyCMMapi8_s_ * dst, oyCMMapi8_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */

  return 0;
}
