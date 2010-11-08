/** @file oyranos_cs.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    ColorSync internal API
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/24
 */


#ifndef OYRANOS_CS_H
#define OYRANOS_CS_H

#include "config.h"
#include "oyranos.h"
#include "oyranos_version.h"
#include "oyranos_alpha.h"
#include "oyranos_i18n.h"
#include "oyranos_texts.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifdef __APPLE__
#include <Carbon/Carbon.h>

int    oyGetProfileBlockOSX              ( CMProfileRef prof,
                                           char **block, size_t *size,
                                           oyAlloc_f     allocate_func);
char * oyGetProfileNameOSX               ( CMProfileRef prof,
                                           oyAlloc_f     allocate_func);
char * oyGetDefaultProfileNameOSX_   ( oyPROFILE_e         type );
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_CS_H */
