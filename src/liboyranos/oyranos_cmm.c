/** @file oyranos_cmm.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2008-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    CMM core APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2009/04/28
 *
 *  basic and / or common functions for modules
 */


#include "oyranos_cmm.h"
#include "oyranos_elektra.h"
#include "oyranos_generic.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_object_internal.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include "oyConnectorImaging_s.h"
#include "oyRectangle_s_.h"
#include "oyCMMapi8_s_.h"

#if !defined(_WIN32)
#include <dlfcn.h>
#endif
#include <math.h>
#include <stdarg.h> /* va_list */

#define OY_ERR if(l_error != 0) error = l_error;

#ifdef DEBUG_
#define DEBUG_OBJECT 1
#endif

/** @func    oyFilterPlug_ImageRootRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/10 (Oyranos: 0.1.8)
 *  @date    2009/05/01
 */
int      oyFilterPlug_ImageRootRun   ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int x_pix = 0, y_pix = 0;
  int result = 0, error = 0;
  oyFilterSocket_s * socket = oyFilterPlug_GetSocket( requestor_plug );
  oyImage_s * image = (oyImage_s*)oyFilterSocket_GetData( socket ),
            * output_image;
  int width, output_width;
  oyRectangle_s * output_array_roi;

  DBGs_PROG3_S( ticket, "%s[%d] %s", "Work on remote socket image",
                oyStruct_GetId( (oyStruct_s*)image ),
                oyPixelAccess_Show(ticket) );

  oyFilterSocket_Release( &socket );

  /* Do not work on non existent data. */
  output_image = oyPixelAccess_GetOutputImage( ticket );
  if(!image || !output_image)
    return result;

  output_array_roi = oyPixelAccess_GetArrayROI( ticket );

  /* Set a unknown output image dimension to something appropriate. */
  if(!oyImage_GetWidth(output_image) && !oyImage_GetHeight(output_image))
  {
    DBGs_PROG7_S( ticket, "%s[%d] %s %.04gx%.04g %.04gx%.04g",
                 "Set dimensions on ticket->output_image",
                 oyStruct_GetId( (oyStruct_s*)output_image ),
                 oyRectangle_Show( output_array_roi ),
                 oyImage_GetWidth(output_image), oyImage_GetHeight(output_image),
                 oyImage_GetWidth(image), oyImage_GetHeight(image) );
    oyImage_SetCritical( output_image,
                         oyImage_GetPixelLayout(image, oyLAYOUT), 0, 0,
                         oyImage_GetWidth(image), oyImage_GetHeight(image) );
  }

  width = oyImage_GetWidth(image);
  output_width = oyImage_GetWidth(output_image);

  x_pix = oyPixelAccess_GetStart(ticket,0) * output_width;
  y_pix = oyPixelAccess_GetStart(ticket,1) * output_width;

  {
    char * t = 0;
    oyArray2d_s * array = oyPixelAccess_GetArray( ticket );
    int channels = oyToChannels_m( oyImage_GetPixelLayout(image, oyLAYOUT) );
    int array_pix_width = oyArray2d_GetWidth(array)/channels;

    /* adapt the rectangle of interesst to the new image dimensions */
    oyRectangle_s_ image_roi = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0},
                   output_array_roi_ = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0};
    oyRectangle_SetByRectangle( (oyRectangle_s*)&image_roi, output_array_roi );
    oyRectangle_SetByRectangle( (oyRectangle_s*)&output_array_roi_, output_array_roi );
    /* x and y source image offset */
    oyRectangle_SetGeo( (oyRectangle_s*)&image_roi,
                        x_pix / (double) width,
                        y_pix / (double) width,
                        oyRectangle_GetGeo1((oyRectangle_s*)&image_roi,2),
                        oyRectangle_GetGeo1((oyRectangle_s*)&image_roi,3) );

    STRING_ADD( t, oyRectangle_Show( (oyRectangle_s*)&image_roi ) );
    DBGs_PROG8_S( ticket, "%s %s[%d] image_roi: %s output_array_roi:%s array[%d](%dx%d)",
                  "Fill ticket->array from", _("Image"),
                  oyStruct_GetId( (oyStruct_s*)image ), t,
                  oyRectangle_Show( (oyRectangle_s*)output_array_roi ),
                  oyStruct_GetId((oyStruct_s*)array),oyArray2d_GetWidth(array),oyArray2d_GetHeight(array) );

    /* the array rectangle passed to oyImage_FillArray() is by definition relative to image units */
      DBGs_PROG2_S( ticket, "array_pix_width: %d / width: %d", array_pix_width, width);
    if(array_pix_width)
      oyRectangle_Scale( (oyRectangle_s*)&output_array_roi_, array_pix_width/(double)width );
      DBGs_PROG1_S( ticket, "after array/image scaling output_array_roi_: %s", oyRectangle_Show((oyRectangle_s*)&output_array_roi_));

    error = oyImage_FillArray( image, (oyRectangle_s*)&image_roi, 1,
                               &array, (oyRectangle_s*)&output_array_roi_, 0 );
      DBGs_PROG4_S( ticket, "%s[%d] output_array: %dx%d", "filled ticket->array",
                  oyStruct_GetId( (oyStruct_s*)array ),
                  oyArray2d_GetWidth(array),oyArray2d_GetHeight(array) );
    oyPixelAccess_SetArray( ticket, array );
    oyArray2d_Release( &array );
    if(error)
      result = error;
    oyFree_m_( t );
  }

  return result;
}

int oyDeviceCMMInit                  ( oyStruct_s        * filter,
                                       const char        * rank_file_pattern )
{
  int error = !filter || !rank_file_pattern;
  char ** rank_name = NULL;
  const char * rfilter = rank_file_pattern;
  oyCMMapi8_s_ * s = (oyCMMapi8_s_*) filter;

  if(!error)
  {
    error = oyRankMapList( rfilter, NULL, &rank_name, oyAllocateFunc_ );
    if(error > 0 || !rank_name || !rank_name[0])
    {
      WARNc2_S("Problems loading rank map: %s %d", rfilter, error);

    } else
    {
      oyRankMap * rank_map = NULL;
      char * json_text = NULL;
      size_t json_size = 0;

      json_text = oyReadFileToMem_( rank_name[0], &json_size, oyAllocateFunc_ );
      if(!json_text || !json_text[0])
        oyMessageFunc_p( oyMSG_WARN, filter, "%s() %s: %s", __func__,
                         _("File not loaded!"), rank_name[0] );

      error = oyRankMapFromJSON ( json_text, NULL, &rank_map, oyAllocateFunc_ );

      if(!rank_map || error || !rank_map[0].key)
        oyMessageFunc_p( oyMSG_WARN, filter, "%s() %s: %s  %d", __func__,
                         _("Creation of rank_map filed from"), rank_name[0], error );
      else
        s->rank_map = rank_map;

      if(json_text) oyFree_m_( json_text );
    }

    if(rank_name) oyStringListRelease_( &rank_name, 1, oyDeAllocateFunc_ );
  }

  return error;
}

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2010/09/06
 */
const char * oyCMMgetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _("NoNo");
    else if(type == oyNAME_NAME)
      return _("Oyranos modules");
    else
      return _("Oyranos supplied modules");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return "Kai-Uwe";
    else if(type == oyNAME_NAME)
      return "Kai-Uwe Behrmann";
    else
      return _("Oyranos project; www: http://www.oyranos.org; support/email: ku.b@gmx.de; sources: http://www.oyranos.org/downloads/");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return "newBSD";
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2005-2014 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/BSD-3-Clause");
  }
  return 0;
}
const char *oyCMM_texts[4] = {"name","copyright","manufacturer",0};

/**
 *  This function implements oyCMMinfoGetText_f for image connector plugs.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
const char * oyCMMgetImageConnectorPlugText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select, "name")==0)
  {
    if(type == oyNAME_NICK)
      return _("Img");
    else if(type == oyNAME_NAME)
      return _("Image");
    else if(type == oyNAME_DESCRIPTION)
      return _("Image Plug");
  }
  return NULL;
}
const char * oy_image_connector_texts[] = {"name", 0};
/**
 *  This function implements oyCMMinfoGetText_f for image connector sockets.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
const char * oyCMMgetImageConnectorSocketText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select, "name")==0)
  {
    if(type == oyNAME_NICK)
      return _("Img");
    else if(type == oyNAME_NAME)
      return _("Image");
    else if(type == oyNAME_DESCRIPTION)
      return _("Image Socket");
  }
  return NULL;
}

oyObjectInfoStatic_s oy_connector_imaging_static_object = {
  oyOBJECT_INFO_STATIC_S, 0,0,0,
  {2,oyOBJECT_CONNECTOR_S,oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,0,0}
};
