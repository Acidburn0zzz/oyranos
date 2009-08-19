/** @file oyranos_gamma.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    gamma loader
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/02/03
 *
 * It reads the default profile(s) from the Oyranos CMS and recalls this
 * profile(s)
 * as new default profile for a screen, including a possible curves upload to
 * the video card.
 * Currently You need xcalib installed to do the curves upload.
 * 
 */



#include "oyranos.h"
#include "oyranos_alpha.h"
#include "oyranos_debug.h"
#include "oyranos_internal.h"
#include "oyranos_config.h"
#include "oyranos_version.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void* oyAllocFunc(size_t size) {return malloc (size);}
void  oyDeAllocFunc ( oyPointer ptr) { if(ptr) free (ptr); }

int main( int argc , char** argv )
{
  char *display_name = getenv("DISPLAY");
  char *monitor_profile = 0;
  int error = 0;

  /* the functional switches */
  int erase = 0;
  int list = 0;
  int setup = 0;
  int database = 0;

  char *ptr = NULL;
  int x = 0, y = 0;
  char *oy_display_name = NULL;
  oyProfile_s * prof = 0;
  size_t size = 0;
  const char * filename = 0;
  char * data = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyI18NInit_();

#ifndef __APPLE__
  if(!display_name)
  {
    WARNc_S( _("DISPLAY variable not set: giving up.") );
    error = 1;
    return error;
  }
#endif

  if(getenv("OYRANOS_DEBUG"))
  {
    int value = atoi(getenv("OYRANOS_DEBUG"));
    if(value > 0)
      oy_debug = value;
  }

  /* cut off the screen information */
  if(display_name &&
     (ptr = strchr(display_name,':')) != 0)
    if( (ptr = strchr(ptr, '.')) != 0 )
      ptr[0] = '\000';

/* allow "-opt val" and "-opt=val" syntax */
#define OY_PARSE_INT_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = atoi( argv[pos+1] ); \
                          if( opt == 0 && strcmp(argv[pos+1],"0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = atoi( &argv[pos][i+2] ); \
                          if( opt == 0 && strcmp(&argv[pos][i+2],"0") ) \
                            wrong_arg = "-" #opt; \
                          i = 1000; \
                        } else wrong_arg = "-" #opt; \
                        if(oy_debug) printf(#opt "=%d\n",opt)

  if(argc != 1)
  {
    int pos = 1, i;
    char *wrong_arg = 0;
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'e': erase = 1; monitor_profile = 0; break;
              case 'b': database = 1; monitor_profile = 0; break;
              case 'l': list = 1; monitor_profile = 0; break;
              case 'x': OY_PARSE_INT_ARG( x ); break;
              case 'y': OY_PARSE_INT_ARG( y ); break;
              case 'v': oy_debug += 1; break;
              case 's': setup = 1; break; /* implicite -> setup */
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(strcmp(&argv[pos][2],"unset") == 0)
                        { erase = 1; monitor_profile = 0; i=100; break; }
                        else if(strcmp(&argv[pos][2],"setup") == 0)
                        { setup = 1; i=100; break; }
                        else if(strcmp(&argv[pos][2],"database") == 0)
                        { database = 1; monitor_profile = 0; i=100; break; }
                        else if(strcmp(&argv[pos][2],"list") == 0)
                        { list = 1; monitor_profile = 0; i=100; break; }
                        else if(strcmp(&argv[pos][2],"verbose") == 0)
                        { oy_debug += 1; i=100; break; }
                        }
              default:
                        printf("\n");
                        printf("oyranos-monitor v%d.%d.%d %s\n",
                        OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a colour profile administration tool for monitors"));
                        printf("%s\n",                 _("Usage"));
                        printf("  %s\n",               _("Set new profile:"));
                        printf("      %s -x pos -y pos %s\n", argv[0],
                                                       _("profile name"));
                        printf("\n");
                        printf("  %s\n",               _("Unset profile:"));
                        printf("      %s -e -x pos -y pos\n", argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("Activate profiles:"));
                        printf("      %s\n",           argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("Query server profile:"));
                        printf("      %s -x pos -y pos\n", argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("Query device data base profile:"));
                        printf("      %s -b -x pos -y pos\n", argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("List devices:"));
                        printf("      %s -l\n",        argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("General options:"));
                        printf("      %s\n",           _("-v verbose"));
                        printf("\n");
                        printf("For more informations read the man page:\n");
                        printf("      man oyranos-monitor\n");
                        exit (0);
                        break;
            }
            break;
        default:
            monitor_profile = argv[pos];
            /* activate all profiles at once */
            /*error = oyActivateMonitorProfiles (display_name); */

            erase = 0;
      }
      if( wrong_arg )
      {
        printf("%s %s\n", _("wrong argument to option:"), wrong_arg);
        exit(1);
      }
      ++pos;
    }
    if(oy_debug) printf( "%s\n", argv[1] );

    if(!erase && !list && !database && !setup)
      setup = 1;

    oy_display_name = oyGetDisplayNameFromPosition( display_name, x,y,
                                                    oyAllocFunc);

    if(!monitor_profile && !erase && !list && !setup)
    {
      if(database)
      {
        filename = oyGetMonitorProfileNameFromDB( oy_display_name, oyAllocFunc);
        prof = oyProfile_FromFile( filename, 0, 0 );
        data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
      } else {
        data = oyGetMonitorProfile(oy_display_name, &size, oyAllocFunc);
        prof = oyProfile_FromMem( size, data, 0, 0 );
        filename = oyProfile_GetFileName( prof, -1 );
      }
      if(size && data) oyDeAllocFunc( data ); data = 0;

      printf("%s:%d profile \"%s\" size: %d\n",__FILE__,__LINE__,
             filename?filename:OY_PROFILE_NONE, (int)size);

      oyProfile_Release( &prof );
    }

    if(list)
    {
      char * text = 0;
      uint32_t n = 0, i;
      oyConfigs_s * devices = 0;
      oyConfig_s * c = 0;

      error = oyDevicesGet( 0, "monitor", 0, &devices );
      n = oyConfigs_Count( devices );
      if(!error)
      {
        for(i = 0; i < n; ++i)
        {
          c = oyConfigs_Get( devices, i );

          if(oy_debug)
          printf("------------------------ %d ---------------------------\n",i);

          error = oyDeviceGetInfo( c, oyNAME_NICK, 0, &text, oyAllocFunc );
          printf("\"%s\" ", text? text:"???");
          error = oyDeviceGetInfo( c, oyNAME_NAME, 0, &text, oyAllocFunc );
          printf("%s\n", text? text:"???");

          if(oy_debug)
          {
            error = oyDeviceGetInfo( c, oyNAME_DESCRIPTION, 0, &text, oyAllocFunc );
            printf("%s\n", text?  text:"???");
          }

          if(text)
            free( text );

          /* verbose adds */
          if(oy_debug)
          {
            oyDeviceAskProfile( c, &prof );
            data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc);
            if(size && data)
              oyDeAllocFunc( data );
            filename = oyProfile_GetFileName( prof, -1 );
            printf( "%s:%d server profile \"%s\" size: %d\n",__FILE__,__LINE__,
                    filename?filename:OY_PROFILE_NONE, (int)size );

            oyDeviceProfileFromDB( c, &text, oyAllocFunc );
            printf( "%s:%d DB profile \"%s\"\n  DB registration key set: %s\n",
                    __FILE__,__LINE__,
                    text?text:OY_PROFILE_NONE,
                    oyConfig_FindString( c, "key_set_name", 0 ) ?
                      oyConfig_FindString( c, "key_set_name", 0 ) :
                      OY_PROFILE_NONE );

            oyProfile_Release( &prof );
            oyDeAllocFunc( text );
          }

          oyConfig_Release( &c );
        }
      }
      oyConfigs_Release( &devices );
    }

    /* make shure the display name is correct including the screen */
    if(monitor_profile)
      oySetMonitorProfile (oy_display_name, monitor_profile);
    if(monitor_profile || erase)
      oySetMonitorProfile (oy_display_name, 0);
  } else
    setup = 1;

  if(setup || monitor_profile)
    error = oyActivateMonitorProfiles (display_name);

  if(oy_display_name)
    oyDeAllocFunc(oy_display_name);

  return error;
}
