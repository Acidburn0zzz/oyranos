/**
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2006  Kai-Uwe Behrmann
 *
 * @autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 */

/** @file @internal
 *  @brief i18n API
 */

/** @date      29. 07. 2006 */


#ifndef OYRANOS_I18N_H
#define OYRANOS_I18N_H

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

#ifdef USE_GETTEXT
# include <libintl.h>
# include <locale.h>
# define _(text) dgettext( domain, text )
#else
# define _(text) text
#endif
extern const char *domain;
extern const char *domain_path;

#define OY_TEXTDOMAIN "oyranos"

/* library sentinels */
void   oyI18NInit_               (void);
void   oyI18Nrefresh_();
void   oyI18NSet_                    ( int active,
                                       int reserved );


#ifdef __cplusplus
} // extern "C"
} // namespace oyranos
#endif /* __cplusplus */

#endif /* OYRANOS_I18N_H */
