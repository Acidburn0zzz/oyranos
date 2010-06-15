/** @file oyranos_i18n.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2006-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    string translation
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/07/29
 */


#ifndef OYRANOS_I18N_H
#define OYRANOS_I18N_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef USE_GETTEXT
# include <libintl.h>
# include <locale.h>
# define _(text) dgettext( oy_domain, text )
#else
# define _(text) text
#endif
extern const char *oy_domain;
extern const char *oy_domain_path;
extern const char *oy_domain_codeset;

#define OY_TEXTDOMAIN "oyranos"

void   oyI18NInit_                   ( void );
void   oyI18Nrefresh_                ( void );
void   oyI18Nreset_                  ( void );
void   oyI18NSet_                    ( int active,
                                       int reserved );

const char *   oyLang_               ( void );
const char *   oyLanguage_           ( void );
const char *   oyCountry_            ( void );


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_I18N_H */
