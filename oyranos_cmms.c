/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2006  Kai-Uwe Behrmann
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
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
 *  @brief CMM's framework
 */

/* Date:      30. 07. 2006 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_cmms.h"
#include "oyranos_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"
#include "oyranos_texts.h"
#include "oyranos_xml.h"

/* --- Helpers  --- */

/* --- structs, typedefs, enums --- */

/** @internal
    @brief the internal only used structure for external registred CMM functions
 */
typedef struct {
  char       *id;               /**< usually a 4 letter short name */
  char       *name;             /**< short name */
  char       *description;      /**< long description */ // TODO help license ..
  char       *libname;          /**< library to search for function */
  char       *funcname;         /**< function for dlsym */
  oyWIDGET    opts_start;       /**< options numbers for oyGetOptionUITitle */
  oyWIDGET    opts_end;
  oyOption_t_ *option;         /**< the CMM options */
} oyExternFunc_t_;


/** @internal
    @brief the internal only used structure for external registred CMM's
 */
typedef struct {
  char        id[5];            /**< 4 letter identifier */
  char       *name;             /**< short name */
  char       *description;      /**< long description */ // TODO help license ..
  int         groups_start;
  int         groups_end;       /**< the registred layouts frames */
  oyOption_t_*group;            /**< the oy_groups_description_ synonym */
  oyExternFunc_t_ *func;        /**< the registred functions of the CMM */
  int         funcs;            /**< number of provided functions */
  char       *xml;              /**< original xml text */
  const char *domain;           /**< textdomain */
  const char *domain_path;      /**< textdomain path */
} oyModule_t_;

/** @internal singleton */
struct {
  int          looked;
  oyModule_t_ *modul;
  int          n;
} oyModules_ = {0,NULL,0};

static int  oy_data_globale_id_;
/** @internal a object containing data, file information and inform observers */
typedef struct {
    /* Zeigermaschine */
    unsigned char** zeiger_;
    size_t*     groesse_;
    int*        ref_n_;
    int         id_;
    char        *name_;                   // z.B. Profilname
    double      *letze_aen_zeit_;         // letztes mal geaendert
    double      *letze_ben_zeit_;         // letztes mal benutzt
} oyData_t_;

void
oyDataInit_ (oyData_t_ *data)
{
    data->id_ = oy_data_globale_id_;
    ++oy_data_globale_id_;
    data->ref_n_=           (int*)         calloc(sizeof( int ),1);
    data->zeiger_=          (unsigned char**)calloc(sizeof( unsigned char* ),1);
    data->groesse_=         (size_t*)      calloc(sizeof( size_t ),1);
    data->name_ =           (char*)        calloc(sizeof( char* ),1);
    data->letze_aen_zeit_ = (double*)      calloc(sizeof( double ),1);
    data->letze_ben_zeit_ = (double*)      calloc(sizeof( double ),1);
    *data->ref_n_ = 1; *data->zeiger_ = 0; *data->groesse_ = 0;
    *data->letze_aen_zeit_ = *data->letze_ben_zeit_ = 0;
}


/* --- static variables   --- */

/* --- internal API definition --- */

/** @internal CMM API */
oyModule_t_* oyModulGet_              (const char *id);
int          oyModulAdd_              (oyModule_t_ *cmm);
int          oyModulGetFromXML_       (oyGROUP           group,
                                       char             *xml,
                                       oyModule_t_      *cmm);
oyWIDGET  oyModulsGetNewOptionRange_  (int count);



/* small helpers */
#define OY_FREE( ptr ) if(ptr) { free(ptr); ptr = 0; }

/* --- function definitions --- */


/* CMM support */


oyModule_t_*
oyModulGet_        (const char *id)
{
  int i;
  if(oyModules_.modul && oyModules_.n)
    for(i = 0; i < oyModules_.n; ++i)
    {
      if(memcmp( id, oyModules_.modul[i].id, 4 ) == 0)
      {
        return &oyModules_.modul[i];
      }
    }
  return NULL;
}

int
oyModulRemove_     (const char *id)
{
  int i,
      error = 0,
      pos = 0;
  oyModule_t_ *ptr = NULL;

  oyModules_.looked = 1;

  oyAllocHelper_m_(ptr, oyModule_t_, oyModules_.n-1, oyAllocateFunc_, return 1)
  for(i = 0; i < oyModules_.n; ++i)
  {
    if(strcmp(oyModules_.modul[i].id, id) == 0)
      ++i;
    else
      ptr[pos] = oyModules_.modul[i];
    ++pos;
  }

  if(oyModules_.modul) oyDeAllocateFunc_(oyModules_.modul);
  oyModules_.modul = ptr;
  --oyModules_.n;
  oyModules_.looked = 0;

  return error;
}

int
oyModulAdd_        (oyModule_t_ *cmm)
{
  int i,
      error = 0;
  oyModule_t_ *ptr = NULL;

  oyModules_.looked = 1;

  oyAllocHelper_m_(ptr, oyModule_t_, oyModules_.n+1, oyAllocateFunc_, return 1)
  for(i = 0; i < oyModules_.n; ++i)
    ptr[i] = oyModules_.modul[i];
  ptr[oyModules_.n] = *cmm;
  if(oyModules_.modul) oyDeAllocateFunc_(oyModules_.modul);
  oyModules_.modul = ptr;
  ++oyModules_.n;
  oyModules_.looked = 0;

  return error;
}

char**
oyModulsGetNames_( int        *count,
                   oyAllocFunc_t alloc_func )
{
  char **ids = NULL;
  int    i;

  *count = 0;
  oyAllocHelper_m_( ids, char*, oyModules_.n, alloc_func, return NULL)
  WARN_S(("oyModules_.n %d",oyModules_.n))
  for( i = 0; i < oyModules_.n; ++i)
  {
    oyAllocHelper_m_( ids[i], char, 5, alloc_func, return NULL);

    snprintf( ids[i], 5, oyModules_.modul[i].id );
  }
  *count = oyModules_.n;
  return ids;
}

oyGROUP
oyRegisterGroups_(char *cmm, char *id, char *name, char *tooltip)
{
  return oyGroupAdd_(cmm, id, name, tooltip);
}


int
oyModulGetFromXML_( oyGROUP           group,
                    char             *xml,
                    oyModule_t_      *cmm)
{

  /* allocate memory */
  char *value = 0,
       *cmm_reg = 0,
       *cmm_group = 0,
       *groups = 0,
      **groupa,
       *funcs,
      **func,
       *options,
      **option,
       *choices,
      **choice;
  int   count = 0, count2, count3;
  int   i, j, k;
  int   err = 0;
  int base_complete = 1;
  int first_group_n = 0;
  int group_modules = 0;

  DBG_PROG_START


  value = oyXMLgetValue_(xml, "oyDOMAIN");
  if(strlen(domain) &&
     value && strlen(value))
    cmm->domain = value;
  else
    cmm->domain = domain;
  domain = cmm->domain;
  value = oyXMLgetValue_(xml, "oyDOMAIN_PATH");
  if(value && strlen(value))
    cmm->domain_path = value;
  else
    cmm->domain_path = LOCALEDIR;
  domain_path = cmm->domain_path;

  oyI18NInit_();


  cmm_reg = oyXMLgetValue_(xml, "oyCMM_REGISTER");
  cmm->xml = xml;
  cmm_group= oyXMLgetValue_(cmm_reg, "oyCMM_GROUP");
  value = oyXMLgetValue_(cmm_group, "oyID");
  if(value && strlen(value) == 4)
    snprintf( cmm->id, 5, value );
  else
    base_complete = 0;
  value = oyXMLgetValue_(cmm_group, "oyNAME");
  if(value && strlen(value))
    cmm->name = value;
  else
    base_complete = 0;
  value = _( oyXMLgetValue_(cmm_group, "oyDESCRIPTION") );
  if(value && strlen(value))
    cmm->description = value;
  groups = oyXMLgetValue_(cmm_group, "oyGROUPS");
  groupa = oyXMLgetArray_(groups, "oyGROUP", &count);

  /*oy_debug = 1;*/

  group_modules = count;
  cmm->group = NULL;
  oyAllocHelper_m_( cmm->group, oyOption_t_, group_modules,
                    oyAllocateFunc_, return -1; );
  for(i = 0; i < group_modules; ++i)
  {
    oyGROUP oy_group;
    char *txt0, *txt1, *txt2;
    oyOption_t_ *opt = cmm->group;
    int pos = 0;

    txt0 =    oyXMLgetValue_(groupa[i], "oyCONFIG_STRING_XML");
    txt1 = _( oyXMLgetValue_(groupa[i], "oyNAME") );
    txt2 = _( oyXMLgetValue_(groupa[i], "oyDESCRIPTION") );

#   define oySET_OPTIONS_M_( t_, id_, ca_n, ca1, ca2, ca3, labl, desc, \
                             ch_n, ch0, ch1, ch2, ch3, \
                             conf, xml) { \
      pos = id_; \
      opt[pos]. type = t_; \
      opt[pos]. id = id_; \
      opt[pos]. category[0] = ca_n; \
      opt[pos]. category[1] = ca1; \
      opt[pos]. category[2] = ca2; \
      opt[pos]. category[3] = ca3; \
      opt[pos]. flags = 0; \
      opt[pos]. name = labl; \
      opt[pos]. description = desc; \
      opt[pos]. choices = ch_n; \
      opt[pos]. choice_list[0] = ch0; \
      opt[pos]. choice_list[1] = ch1; \
      opt[pos]. choice_list[2] = ch2; \
      opt[pos]. choice_list[3] = ch3; \
/*      opt[pos]. choice_desc[0] = cd0; \
      opt[pos]. choice_desc[1] = cd1; \
      opt[pos]. choice_desc[2] = cd2; \
      opt[pos]. choice_desc[3] = cd3;*/ \
      opt[pos]. config_string = conf; \
      opt[pos]. config_string_xml = xml; \
    }

    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, i, 0,
      0, 0, 0,
      txt1,
      txt2,
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      txt0)

    oy_group = oyRegisterGroups_( cmm->id, txt0, txt1, txt2 );

    if(i == 0)
      first_group_n = oy_group;
    
    //DBG_S(("oyGROUP[%d]: %s", i, groupa[i]));
    DBG_S(("   [%d]: %s", i, oyXMLgetValue_(groupa[i], "oyCONFIG_STRING_XML")));
    DBG_S(("   [%d]: %s", i, _( oyXMLgetValue_(groupa[i], "oyNAME")) ));
    DBG_S(("   [%d]: %s", i, _( oyXMLgetValue_(groupa[i], "oyDESCRIPTION")) ));
    DBG_S(("   [%d]: %s", i, oyXMLgetValue_(groupa[i], "oyNIX")));
  }
  cmm->groups_start = first_group_n;
  cmm->groups_end   = cmm->groups_start + group_modules - 1;

  funcs =   oyXMLgetValue_(cmm_reg, "oyFUNCTIONS");
  func =    oyXMLgetArray_(funcs, "oyFUNCTION", &count);
  cmm->func = NULL;
  oyAllocHelper_m_(cmm->func, oyExternFunc_t_, count,
                               oyAllocateFunc_, return -1; );
  cmm->funcs = count;
  for(i = 0; i < count; ++i)
  {
    cmm->func[i].id          =   oyXMLgetValue_(func[i], "oyID");
    cmm->func[i].name        = _(oyXMLgetValue_(func[i], "oyNAME"));
    cmm->func[i].description = _(oyXMLgetValue_(func[i], "oyDESCRIPTION"));
    cmm->func[i].libname     =   oyXMLgetValue_(func[i], "oyDYNLOAD_LIB");
    cmm->func[i].funcname    =   oyXMLgetValue_(func[i], "oyDYNLOAD_FUNC");
    DBG_S(("     : %s", cmm->func[i].id))
    DBG_S(("     : %s", cmm->func[i].name))
    DBG_S(("     : %s", cmm->func[i].description))
    DBG_S(("     : %s", cmm->func[i].libname))
    DBG_S(("     : %s", cmm->func[i].funcname))
    options = oyXMLgetValue_(func[i], "oyWIDGETS");
    option =  oyXMLgetArray_(options, "oyWIDGET", &count2);
    cmm->func[i].option = NULL;
    oyAllocHelper_m_(cmm->func[i].option, oyOption_t_, count2,
                                  oyAllocateFunc_, return -1; );
    for(j = 0; j < count2; ++j)
    {
      int group_n = 0;
      char **grs = NULL;
      char *type = NULL;
      //DBG_S(("oyWIDGET[%d]: %s", i, option[i]));
      DBG_S(("       : %s", oyXMLgetValue_(option[j], "oyID")));

      grs = oyXMLgetArray_(option[j], "oyGROUP", &group_n);
      if(group_n && grs)
      for( k = 0; k < MIN(group_n,group_modules); ++k)
      {
        int cat_intern = atoi(grs[k]) - oyGROUP_EXTERN;

        cmm->func[i].option[j].category[k] = atoi(grs[k]);
        if(cat_intern >= 0)
          DBG_S(("       => %s",
                 _( oyXMLgetValue_(groupa[ cat_intern ], "oyNAME") )));
      }
      if(group_n < 10)
        cmm->func[i].option[j].category[ group_n ] = oyGROUP_START;

      cmm->func[i].option[j].name = _( oyXMLgetValue_(option[j], "oyNAME") );
      cmm->func[i].option[j].description = _( oyXMLgetValue_(option[j], "oyDESCRIPTION") );
      DBG_S(("       : %s", _( oyXMLgetValue_(option[j], "oyNAME") )));
      DBG_S(("       : %s", _( oyXMLgetValue_(option[j], "oyDESCRIPTION") )));

      type = oyXMLgetValue_(option[j], "oyWIDGET_TYPE");
      if(type)
      {
        if      (strcmp(type,"oyTYPE_BEHAVIOUR") == 0)
          cmm->func[i].option[j].type = oyTYPE_BEHAVIOUR;
        else if (strcmp(type,"oyTYPE_DEFAULT_PROFILE") == 0)
          cmm->func[i].option[j].type = oyTYPE_DEFAULT_PROFILE;
        else if (strcmp(type,"oyTYPE_PROFILE") == 0)
          cmm->func[i].option[j].type = oyTYPE_PROFILE;
        else if (strcmp(type,"oyTYPE_INT") == 0)
          cmm->func[i].option[j].type = oyTYPE_INT;
        else if (strcmp(type,"oyTYPE_FLOAT") == 0)
          cmm->func[i].option[j].type = oyTYPE_FLOAT;
        else if (strcmp(type,"oyTYPE_CHOICE") == 0)
          cmm->func[i].option[j].type = oyTYPE_CHOICE;
        else if (strcmp(type,"oyTYPE_VOID") == 0)
          cmm->func[i].option[j].type = oyTYPE_VOID;
        else
          WARN_S(("Did not find a type for option: %s",
                  cmm->func[i].option[j].name));
      }

      switch (cmm->func[i].option[j].type)
      {
        case oyTYPE_START:
        case oyTYPE_GROUP_TREE:
        case oyTYPE_BEHAVIOUR:
        case oyTYPE_DEFAULT_PROFILE:
        case oyTYPE_PROFILE:
        case oyTYPE_INT:
        case oyTYPE_FLOAT:
             break;
        case oyTYPE_CHOICE:
             choices = oyXMLgetValue_(option[j], "oyCHOICES");
             choice = oyXMLgetArray_(choices, "oyNAME", &count3);
             cmm->func[i].option[j].choices = count3 <= 10 ? count3 : 10;
             for(k = 0; k < cmm->func[i].option[j].choices; ++k)
             {
               cmm->func[i].option[j].choice_list[k] = _( choice[k] );
               DBG_S(("         : %s", _(choice[k])));
             }
             break;
        case oyTYPE_VOID:
        case oyTYPE_END:
             break;
      }
      cmm->func[i].option[j].config_string = oyXMLgetValue_(option[j], "oyCONFIG_STRING");
      cmm->func[i].option[j].config_string_xml = oyXMLgetValue_(option[j], "oyCONFIG_STRING_XML");
      DBG_S(("       : %s", oyXMLgetValue_(option[j], "oyCONFIG_STRING")));
      DBG_S(("       : %s", oyXMLgetValue_(option[j], "oyCONFIG_STRING_XML")));
    }
    cmm->func[i].opts_start  =   oyModulsGetNewOptionRange_(count2);
    cmm->func[i].opts_end    =   cmm->func[i].opts_start + count2 - 1;
  }
  /*oy_debug = 0;*/

  domain = OY_TEXTDOMAIN;
  domain_path = LOCALEDIR;
  oyI18NInit_();

  DBG_PROG_ENDE
  return err;
}

int
oyModulRegisterXML_(oyGROUP           group,
                    const char       *xml)
{

  int   err = 0;
  oyModule_t_ cmm;
  char *xml_ = NULL;

  DBG_PROG_START

  oyAllocHelper_m_(xml_, char, strlen(xml)+1, oyAllocateFunc_, return -1; )
  sprintf(xml_,"%s",xml);

  oyModulGetFromXML_(group, xml_, &cmm);
  oyModulAdd_(&cmm);

  printf(oyModulPrint_(cmm.id));

  DBG_PROG_ENDE
  return err;
}

/** @internal print all information out */
char*
oyModulPrint_   ( const char       *modul )
{
  char *text = NULL,
       *tmp  = NULL;
  int   len  = 1024, i, j, k;
  oyModule_t_ *mod = oyModulGet_(modul);

  DBG_PROG_START

  oyAllocHelper_m_( text, char, len, oyAllocateFunc_, return NULL );
  oyAllocHelper_m_( tmp, char, len, oyAllocateFunc_, return NULL );

# define add_s() { len = oyMemBlockExtent_(&text, len, strlen(tmp) + 1); \
                   sprintf( &text[strlen(text)], tmp );  }

  if(!mod)
  {
    snprintf( tmp, 1024, "Could not find %s", modul ); add_s()
    return text;
  }

  snprintf( tmp, 1024, "Modul: %s\n", modul ); add_s()
  snprintf( tmp, 1024, " Name: %s\n", mod->name ); add_s()
  snprintf( tmp, 1024, "       %d - %d\n", mod->groups_start, mod->groups_end ); add_s()
  snprintf( tmp,   80, "  xml: %s", mod->xml ); add_s()
  snprintf( tmp, 1024, "\n" ); add_s()
  snprintf( tmp, 1024, "  domain: %s path: %s\n", mod->domain, mod->domain_path ); add_s()

  snprintf( tmp, 1024, "  \n" ); add_s()
  snprintf( tmp, 1024, "  \n" ); add_s()


  for(i = 0; i < mod->funcs; ++i)
  {
    int options_n = mod->func[i].opts_end - mod->func[i].opts_start + 1;

    snprintf( tmp, 1024, "    F %d[%d] %s (%s)\n", i, mod->funcs,
              mod->func[i].name, mod->func[i].description ); add_s()
    snprintf( tmp, 1024, "             %s::%s\n", mod->func[i].libname,
              mod->func[i].funcname ); add_s()
    for(j = 0; j < options_n; ++j)
    {
      snprintf( tmp, 1024, "             O %s (%s)\n",
                mod->func[i].option[j].name,
                mod->func[i].option[j].description ); add_s()
     
      snprintf( tmp, 1024, "             G" ); add_s()
      for( k = 0; k < 10; ++k )
      { oyGROUP g = mod->func[i].option[j].category[k];
        if(g)
        {
          int module_group = g - oyGROUP_EXTERN;
          if(module_group >= 0)
          {
            snprintf( tmp, 1024, " %d %s (%s)", g,
                      mod->group[module_group].name,
                      mod->group[module_group].description ); add_s()
          }
        }
      }
      snprintf( tmp, 1024, "\n" ); add_s()

      switch (mod->func[i].option[j].type)
      {
        case oyTYPE_START:
        case oyTYPE_GROUP_TREE:
        case oyTYPE_BEHAVIOUR:
        case oyTYPE_DEFAULT_PROFILE:
        case oyTYPE_PROFILE:
        case oyTYPE_INT:
        case oyTYPE_FLOAT:
             break;
        case oyTYPE_CHOICE:
             for(k = 0; k < mod->func[i].option[j].choices; ++k)
             {
               snprintf( tmp, 1024, "               C %d[%d] %s\n", k,
                         mod->func[i].option[j].choices,
                         mod->func[i].option[j].choice_list[k] ); add_s()
             }
             break;
        case oyTYPE_VOID:
        case oyTYPE_END:
             break;
      }
    }
  }

  oyDeAllocateFunc_(tmp);

  DBG_PROG_ENDE
  return text;
}

/** @internal
 *  ask for free oyWIDGET ID's to register the new ones.
 */
oyWIDGET
oyModulsGetNewOptionRange_(int count)
{
  int i, j;
  int last = OY_STATIC_OPTS_;

  DBG_PROG_START

  for(i = 0; i < oyModules_.n; ++i)
  {
    for ( j = 0; j < oyModules_.modul[i].funcs; ++j)
    {
      int options_n = oyModules_.modul[i].func[j].opts_end -
                      oyModules_.modul[i].func[j].opts_start + 1;
      if( oyModules_.modul[i].func[j].opts_start > (last + count + 1) )
      {
        DBG_PROG_ENDE
        /* just the first occurence */
        return last + 1;
      }
      last = oyModules_.modul[i].func[j].opts_start + options_n;
    }
  }

  DBG_PROG_ENDE
  return last + 1;
}

/** @internal
 *  map a oyWIDGET to a oyOption_t_ in dynamic oyModules_
 */
oyOption_t_*
oyModulsUIOptionSearch_ (oyWIDGET       id)
{
  int i, j;

  DBG_PROG_START

  for(i = 0; i < oyModules_.n; ++i)
  {
    for ( j = 0; j < oyModules_.modul[i].funcs; ++j)
    {
      if( oyModules_.modul[i].func[j].opts_start <= id &&
          id <= oyModules_.modul[i].func[j].opts_end )
      {
        DBG_PROG_ENDE
        /* just the first occurence */
        return &oyModules_.modul[i].func[j].option[
                  id - oyModules_.modul[i].func[j].opts_start ];
      }
#     if 0
      for( k = 0; k < options_n; ++k )
      {
      }
#     endif
    }
  }

  DBG_PROG_ENDE
  return NULL;
};

const char*
oyModulGetName_  (const char *cmm)
{
  DBG_PROG_START

  int i;
  char *result = NULL;

  for(i = 0; i < oyModules_.n; ++i)
  {
    if(strcmp(oyModules_.modul[i].id, cmm) == 0)
      result = oyModules_.modul[i].name ;
  }

  DBG_PROG_ENDE
  return result;
};

const char*
oyModulGetDescription_  (const char *cmm)
{
  DBG_PROG_START

  int i;
  char *result = NULL;

  for(i = 0; i < oyModules_.n; ++i)
  {
    if(strcmp(oyModules_.modul[i].id, cmm) == 0)
      result = oyModules_.modul[i].description ;
  }

  DBG_PROG_ENDE
  return result;
};

const char*
oyModulGetXml_  (const char *cmm)
{
  DBG_PROG_START

  int i;
  char *result = NULL;

  for(i = 0; i < oyModules_.n; ++i)
  {
    if(strcmp(oyModules_.modul[i].id, cmm) == 0)
      result = oyModules_.modul[i].xml ;
  }

  DBG_PROG_ENDE
  return result;
};

const char*
oyModulGetDomain_  (const char *cmm)
{
  DBG_PROG_START

  int i;
  const char *result = NULL;

  for(i = 0; i < oyModules_.n; ++i)
  {
    if(strcmp(oyModules_.modul[i].id, cmm) == 0)
      result = oyModules_.modul[i].domain ;
  }

  DBG_PROG_ENDE
  return result;
};

const char*
oyModulGetDomainPath_  (const char *cmm)
{
  DBG_PROG_START

  int i;
  const char *result = NULL;

  for(i = 0; i < oyModules_.n; ++i)
  {
    if(strcmp(oyModules_.modul[i].id, cmm) == 0)
      result = oyModules_.modul[i].domain_path ;
  }

  DBG_PROG_ENDE
  return result;
};

void
oyModulGetGroups_  (const char *cmm, int *start, int *count)
{
  DBG_PROG_START

  int i;

  for(i = 0; i < oyModules_.n; ++i)
  {
    if(strcmp(oyModules_.modul[i].id, cmm) == 0)
    {
      if(start)
        *start = oyModules_.modul[i].groups_start;
      if(count)
        *count = oyModules_.modul[i].groups_end - oyModules_.modul[i].groups_start + 1;
    }
  }

  DBG_PROG_ENDE
};

#if 0
const char*
oyModulGetGroupUITitle_ (oyGROUP     group, const char **tooltip,
                         const char**config_string_xml)
{
  DBG_PROG_START

  int i, start, count;

  for(i = 0; i < oyModules_.n; ++i)
  {
    {
      start = oyModules_.modul[i].groups_start;
      count = oyModules_.modul[i].groups_end - oyModules_.modul[i].groups_start + 1;
      if(start <= group && group <= start + count - 1)
      {
        int modul_group = group-start;
        if(tooltip)
          *tooltip = oyModules_.modul[i].group[modul_group].description;
        if(config_string_xml)
          *config_string_xml = oyModules_.modul[i].group[modul_group].config_string_xml;

        DBG_PROG_ENDE
        return oyModules_.modul[i].group[modul_group].name;
      }
    }
  }

  DBG_PROG_ENDE
  return NULL;
};
#endif



void
oyModulsRefreshI18N_  (void)
{
  DBG_PROG_START

  int i;

  /* refresh CMM's */
  for( i = 0; i < oyModules_.n; ++i)
  { 
    oyModulGetFromXML_ ( oyGROUP_START, oyModules_.modul[i].xml,
                       &oyModules_.modul[i] );
  }

  DBG_PROG_ENDE
};

void
oyModulRefreshI18N_  (const char *cmm)
{
  DBG_PROG_START

  int i;

  /* refresh CMM */
  for( i = 0; i < oyModules_.n; ++i)
  { 
    if(strcmp(oyModules_.modul[i].id, cmm) == 0)
    {
      oyModulGetFromXML_ ( oyGROUP_START, oyModules_.modul[i].xml,
                         &oyModules_.modul[i] );
    }
  }

  DBG_PROG_ENDE
};

