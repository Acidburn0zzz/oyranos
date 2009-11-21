/** @file oyranos_cmm_oyIM.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2008-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    registration definitions for Oyranos backends
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2008/12/16
 */

#ifndef OYRANOS_CMM_OYIM_H
#define OYRANOS_CMM_OYIM_H

#include "config.h"
#include "oyranos_alpha.h"
#include "oyranos_cmm.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_texts.h"

/* --- internal definitions --- */

#define CMM_NICK "oyIM"

extern oyMessage_f message;



int                oyIMCMMInit       ( );
int                oyIMIconv         ( const char        * input,
                                       size_t              len,
                                       char              * output,
                                       const char        * from_codeset );
int      oyIMCMMMessageFuncSet       ( oyMessage_f         message_func );
int      oyStructList_MoveInName     ( oyStructList_s    * texts,
                                       char             ** text,
                                       int                 pos );
int      oyStructList_AddName        ( oyStructList_s    * texts,
                                       const char        * text,
                                       int pos );
oyStructList_s * oyIMProfileTag_GetValues(
                                       oyProfileTag_s    * tag );
int      oyIMProfileTag_Create       ( oyProfileTag_s    * tag,
                                       oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version );
oyWIDGET_EVENT_e   oyIMWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event );

extern oyCMMapi3_s   oyIM_api3;
extern oyCMMapi5_s   oyIM_api5_meta_c;

#endif /* OYRANOS_CMM_OYIM_H */
