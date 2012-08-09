/** @file oyranos_convert.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    ICC conversion - on the command line
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2012/02/19
 *
 *  The program uses ICC profiles to perform colour transforms.
 *
 *  cc -Wall -g oyranos_convert.c -o oyranos-icc `pkg-config --libs --cflags oyranos` -I../../ -I../build_11.4 -I ../../API_generated/ -I ../../oforms/
 */


#include "oyranos.h"
#include "oyranos_alpha.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_helper_macros.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_config.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_version.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_forms.h"
#ifdef USE_LCMS
#include "lcms2.h"
#endif

void* oyAllocFunc(size_t size) {return malloc (size);}

void  printfHelp (int argc, char** argv)
{
  char * version = oyVersionString(1,0);
  char * id = oyVersionString(2,0);
  char * cfg_date =  oyVersionString(3,0);
  char * devel_time = oyVersionString(4,0);

  fprintf( stderr, "\n");
  fprintf( stderr, "oyranos-icc %s\n",
                                _("is a ICC color conversion tool"));
  fprintf( stderr, "  Oyranos v%s config: %s devel period: %s\n",
                  oyNoEmptyName_m_(version),
                  oyNoEmptyName_m_(cfg_date), oyNoEmptyName_m_(devel_time) );
  if(id)
  fprintf( stderr, "  Oyranos git id %s\n", oyNoEmptyName_m_(id) );
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",
                                           _("Hint: search paths are influenced by the XDG_CONFIG_HOME shell variable."));
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("Convert Image:"));
  fprintf( stderr, "      %s -p PROFILE [-o FILENAME] [-n MODULE] -i FILENAME\n", argv[0]);
  fprintf( stderr, "      -i FILENAME  %s\n", _("read from file"));
  fprintf( stderr, "      -p PROFILE   %s\n", _("Output Color Space"));
  fprintf( stderr, "      -s PROFILE   %s\n", _("Simulation/Proof Color Space"));
  fprintf( stderr, "      -e PROFILE   %s\n", _("Effect abtract Color Space"));
  fprintf( stderr, "      -o FILENAME  %s\n", _("write to file, currently only PPM format"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Generate CLUT Image:"));
  fprintf( stderr, "      %s -p PROFILE -f clut [-o FILENAME] [-n MODULE] -i FILENAME\n", argv[0]);
  fprintf( stderr, "      -i FILENAME  %s\n", _("read from file"));
  fprintf( stderr, "      -p PROFILE   %s\n", _("Output Color Space"));
  fprintf( stderr, "      -s PROFILE   %s\n", _("Simulation/Proof Color Space"));
  fprintf( stderr, "      -e PROFILE   %s\n", _("Effect abtract Color Space"));
  fprintf( stderr, "      -o FILENAME  %s\n", _("write to file, currently only PPM format"));
  fprintf( stderr, "      -f FORMAT    %s\n", _("select format, currently only clut"));
  fprintf( stderr, "                   %s\n", _("CLUT is a levels x levels*levels sized PPM, --levels defaults for clut to 64"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Extract ICC profile:"));
  fprintf( stderr, "      %s -f icc [-o FILENAME] [-n MODULE] -i FILENAME\n", argv[0]);
  fprintf( stderr, "      -o FILENAME  %s\n", _("write to file"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Generate Image:"));
  fprintf( stderr, "      %s -f [hald|slice|lab] [-o FILENAME] --levels 8\n", argv[0]);
  fprintf( stderr, "      -o FILENAME  %s\n", _("write to file"));
  fprintf( stderr, "      --levels NUMBER   %s\n", _("levels from 4-16 make sense"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h [-n MODULE] [-d]\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "      -v           %s\n",  _("verbose"));
  fprintf( stderr, "      -n MODULE    %s\n",  _("module name"));
  fprintf( stderr, "      -d           %s\n",  _("enable simple defaults"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s:\n",               _("Example"));
  fprintf( stderr, "    %s:\n",             _("Get ICC profile"));
  fprintf( stderr, "      oyranos-icc -f icc -i image.png | iccexamin -g -i\n");
  fprintf( stderr, "\n");
  fprintf( stderr, "    %s:\n",             _("Convert image to ICC Color Space"));
  fprintf( stderr, "      oyranos-icc -i image.png -n lcm2 -p Lab.icc -o image.ppm\n");
  fprintf( stderr, "\n");
  fprintf( stderr, "    %s:\n",             _("Create 3D CLUT"));
  fprintf( stderr, "      oyranos-icc -i Lab.icc -n lcm2 -p sRGB.icc -f clut -o clut.ppm\n");
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");

  if(version) oyDeAllocateFunc_(version);
  if(id) oyDeAllocateFunc_(id);
  if(cfg_date) oyDeAllocateFunc_(cfg_date);
  if(devel_time) oyDeAllocateFunc_(devel_time);
}


int main( int argc , char** argv )
{
  int error = 0;
  char * format = 0;
  char * output = 0;
  char * input = 0;
  char * node_name = 0;
  int help = 0;
  int verbose = 0;
  int icc_defaults_simple = 0;
  char * output_profile = 0;
  char * simulation_profile = 0;
  char * effect_profile = 0;
  oyProfiles_s * proofing = oyProfiles_New(0),
               * effects = oyProfiles_New(0);
  oyProfile_s * p = 0;
  oyOptions_s * module_options = 0;

  int levels = 0;

  int output_model = 0;
  const char * input_xml_file = 0;
  const char * output_model_file = 0,
             * result_xml = 0;
  char * text = 0, * t = 0;
  const char * opt_names = 0;
  oyFormsArgs_s * forms_args = oyFormsArgs_New( 0 );
  const char * data = 0, * ct = 0;
  char ** other_args = 0;
  int other_args_n = 0;
  int i;
  int print = 1;
  oyOptions_s * opts = 0;
  oyOption_s * o = 0;
  oyImage_s * image = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyExportStart_(EXPORT_CHECK_NO);

  if(argc >= 2)
  {
    int pos = 1, i;
    char *wrong_arg = 0;
    DBG_PROG1_S("argc: %d\n", argc);
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; pos < argc && i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'd': icc_defaults_simple = 1; break;
              case 'f': OY_PARSE_STRING_ARG(format); break;
              case 'o': OY_PARSE_STRING_ARG(output); break;
              case 'p': OY_PARSE_STRING_ARG(output_profile); break;
              case 's': OY_PARSE_STRING_ARG(simulation_profile);
                        p = oyProfile_FromFile( simulation_profile, 0,0 );
                        if(!p) wrong_arg = effect_profile;
                        oyProfiles_MoveIn( proofing, &p, -1 );
                        break;
              case 'e': OY_PARSE_STRING_ARG(effect_profile);
                        p = oyProfile_FromFile( effect_profile, 0,0 );
                        if(!p) wrong_arg = effect_profile;
                        oyProfiles_MoveIn( effects, &p, -1 );
                        break;
              case 'i': OY_PARSE_STRING_ARG(input); break;
              case 'n': OY_PARSE_STRING_ARG(node_name); break;
              case 'v': if(verbose) oy_debug += 1; verbose = 1; break;
              case 'h': help = 1; break;
              case '-':
                        if(OY_IS_ARG("help"))
                        { help = 1; i=100; break; }
                        else if(OY_IS_ARG("levels"))
                        { OY_PARSE_INT_ARG2(levels, "levels"); break; }
                        else if(OY_IS_ARG("output"))
                        { OY_PARSE_STRING_ARG2(output, "output"); break; }
                        else if(strcmp(&argv[pos][2],"verbose") == 0)
                        { oy_debug += 1; i=100; break;
                        } else if(argv[pos][2])
                        {
                          STRING_ADD( t, &argv[pos][2] );
                          text = oyStrrchr_(t, '=');
                          /* get the key only */
                          if(text)
                            text[0] = 0;
                          oyStringListAddStaticString_( &other_args,&other_args_n,
                                                        t,
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
                          if(text)
                            oyStringListAddStaticString_(
                                            &other_args,&other_args_n,
                                            oyStrrchr_(&argv[pos][2], '=') + 1,
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
                          else {
                            if(argv[pos+1])
                            {
                              oyStringListAddStaticString_( &other_args,
                                                            &other_args_n,
                                                            argv[pos+1],
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
                              ++pos;
                            } else wrong_arg = argv[pos];
                          }
                          if(t) oyDeAllocateFunc_( t );
                          t = 0;
                          i=100; break;
                        } else
                        {
                          wrong_arg = argv[pos];
                        }
                        break;
              default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
            }
            break;
        default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
      }
      if( wrong_arg )
      {
       fprintf(stderr, "%s %s\n", _("wrong argument to option:"), wrong_arg);
       printfHelp(argc, argv);
       exit(1);
      }
      ++pos;
    }
  } else
  {
                        printfHelp(argc, argv);
                        exit (0);
  }

  if(help)
  {
    printfHelp(argc, argv);
    if(node_name)
    {
      char * t = 0;
      STRING_ADD( t, "oyranos-xforms-modules -n " );
      STRING_ADD( t, node_name );
      if(!icc_defaults_simple)
        STRING_ADD( t, " -f" );
      if(verbose)
        STRING_ADD( t, " -v" );
      STRING_ADD( t, " | oyranos-xforms" );
      if(verbose)
        STRING_ADD( t, " -lh" );
      if(oy_debug)
        fprintf(stderr, "%s\n", t);
      system(t);
      exit(0);
    }
  }

  if(other_args)
  {
    forms_args->print = 0;

    error = oyXFORMsRenderUi( text, oy_ui_cmd_line_handlers, forms_args );
    result_xml = oyFormsArgs_ModelGet( forms_args );

    opts = oyOptions_FromText( result_xml, 0,0 );

    data = oyOptions_GetText( opts, oyNAME_NAME );
    opt_names = oyOptions_GetText( opts, oyNAME_DESCRIPTION );

      for( i = 0; i < other_args_n; i += 2 )
      {
        /* check for wrong args */
        if(opt_names && strstr( opt_names, other_args[i] ) == NULL)
        {
          fprintf(stderr, "Unknown option: %s", other_args[i]);
          printfHelp( argc, argv );
          exit( 1 );

        } else
        {
          o = oyOptions_Find( opts, other_args[i] );
          if(i + 1 < other_args_n)
          {
            ct = oyOption_GetText( o, oyNAME_NICK );
            if(oy_debug)
            fprintf( stderr, "%s => ",
                    ct?ct:"---" ); ct = 0;
            oyOption_SetFromText( o, other_args[i + 1], 0 );
            data = oyOption_GetText( o, oyNAME_NICK );

            if(oy_debug)
            fprintf( stderr, "%s\n",
                    data?oyStrchr_(data, ':') + 1:"" ); data = 0;
          }
          else
          {
            fprintf( stderr,
                    "%s: --%s  argument missed\n", _("Option"), other_args[i] );
            exit( 1 );
          }
          oyOption_Release( &o );
        }
      }
      print = 0;
  }

  if(output_profile)
  {
    uint32_t flags = 0;
    oyPixel_t pixel_layout;
    oyDATATYPE_e data_type = oyUINT8;

    if(!output)
      WARNc_S("No output file name provided");
    if(!icc_defaults_simple)
      flags |= oyOPTIONATTRIBUTE_ADVANCED;
    if(oyProfiles_Count(effects))
      error = oyOptions_MoveInStruct( &module_options,
                                     "//" OY_TYPE_STD "/config/profiles_effect",
                                       (oyStruct_s**) &effects,
                                       OY_CREATE_NEW );
    if(oyProfiles_Count(proofing))
      error = oyOptions_MoveInStruct( &module_options,
                                      "//" OY_TYPE_STD "/config/profiles_proof",
                                       (oyStruct_s**) &proofing,
                                       OY_CREATE_NEW );

    if(format && strcmp(format,"clut") == 0)
    {
      int width = levels,
          size, l,a,b,j;
      uint16_t * buf = 0;
      uint16_t in[3];
      char comment[80];

      if(!width)
        width = 64;

      size = width*width;

      if(!output)
        WARNc_S("No output file name provided");

      buf = calloc(sizeof(uint16_t), size*width*3);

#pragma omp parallel for private(in,a,b,j)
      for(l = 0; l < width; ++l)
      {
        in[0] = floor((double) l / (width - 1) * 65535.0 + 0.5);
        for(a = 0; a < width; ++a) {
          in[1] = floor((double) a / (width - 1) * 65535.0 + 0.5);
          for(b = 0; b < width; ++b)
          {
            in[2] = floor((double) b / (width - 1) * 65535.0 + 0.5);

            for(j = 0; j < 3; ++j)
              /* BGR */
              buf[b*size*3+a*+width*3+l*3+j] = in[j];
          }
        }
      }
      if(input)
      {
        p = oyProfile_FromFile( input, 0,0 );
        if(!p)
          WARNc1_S("Could not open profile: %s", input);
          error = 1;
      } else
        p = oyProfile_FromStd( oyASSUMED_WEB, 0 );
      image = oyImage_Create( width,width*width, buf, OY_TYPE_123_16,
                              p, 0 );
      oyProfile_Release( &p );
      sprintf( comment, "clut with %d levels", width );

      pixel_layout = oyImage_PixelLayoutGet( image );
      data_type = oyToDataType_m(pixel_layout);
      p = oyProfile_FromFile(output_profile, 0,0);
      oyConversion_s * cc = oyConversion_CreateFromImage (
                                image, node_name, module_options, 
                                p, data_type, flags, 0 );

      error = oyConversion_RunPixels( cc, 0 );
      image = oyConversion_GetImage( cc, OY_OUTPUT );

      error = oyImage_WritePPM( image, output, comment);

      oyImage_Release( &image );
    } else
    {
      error = oyImage_FromFile( input, &image, NULL );
      pixel_layout = oyImage_PixelLayoutGet( image );
      data_type = oyToDataType_m(pixel_layout);
      p = oyProfile_FromFile(output_profile, 0,0);
      if(!p)
      {
          WARNc1_S("Could not open profile: %s", input);
          error = 1;
      }
      oyConversion_s * cc = oyConversion_CreateFromImage (
                                image, node_name, module_options, 
                                p, data_type, flags, 0 );

      error = oyConversion_RunPixels( cc, 0 );
      image = oyConversion_GetImage( cc, OY_OUTPUT );
      oyConversion_Release( &cc );
      oyOptions_SetFromText( &opts, "//" OY_TYPE_STD "/file_write/comment",
                             input, OY_CREATE_NEW );
      error = oyImage_ToFile( image, output, opts );

      oyImage_Release( &image );
    }
    
  } else
  if(format && strcmp(format,"icc") == 0)
  {
    
    oyProfile_s * prof = 0;
    size_t size = 0;
    char * data = 0;
    fprintf(stderr, "%s\n", input);
    error = oyImage_FromFile( input, &image, NULL );
    prof = oyImage_ProfileGet( image );
    data = oyProfile_GetMem( prof, &size, 0, oyAllocateFunc_);
    if(size)
    {
      if(output)
      {
        error = oyWriteMemToFile_ ( output, data, size );
        if(error)
        {
          WARNc_S("Could not write to profile");
        }
      } else
      {
        fwrite( data, sizeof(char), size, stdout );
      }
      oyDeAllocateFunc_(data); size = 0; data = 0;
    } else
      WARNc_S("No profile found");
    
  } else
  if(format && (strcmp(format,"hald") == 0 ||
                strcmp(format,"slice") == 0 ||
                strcmp(format,"lab") == 0))
  {
    int width = levels,
        size = width*width,
        l,a,b,j;
    uint16_t * buf = 0;
    uint16_t in[3];
    char comment[80];

    if(!output)
      WARNc_S("No output file name provided");

    p = oyProfile_FromStd( oyEDITING_LAB, 0 );
    if(strcmp(format,"hald") == 0)
    {
      if(!width)
        width = 8;
      buf = calloc(sizeof(uint16_t), size*width*size*width*3);
#pragma omp parallel for private(in,a,b,j,)
      for(l = 0; l < size; ++l)
      {
        in[0] = floor((double) l / (size - 1) * 65535.0 + 0.5);
        for(a = 0; a < size; ++a) {
          in[1] = floor((double) a / (size - 1) * 65535.0 + 0.5);
          for(b = 0; b < size; ++b)
          {
            in[2] = floor((double) b / (size - 1) * 65535.0 + 0.5);
            for(j = 0; j < 3; ++j)
              buf[l*size*size*3+b*size*3+a*3+j] = in[j];
          }
        }
      }
      image = oyImage_Create( size*width, size*width, buf, OY_TYPE_123_16,
                              p, 0 );
      sprintf( comment, "CIE*Lab Hald with %d levels", width );

    } else if(strcmp(format,"lab") == 0)
    {
      if(!width)
        width = 64;
      buf = calloc(sizeof(uint16_t), size*width*3);

#pragma omp parallel for private(in,a,b,j)
      for(l = 0; l < width; ++l)
      {
        in[0] = floor((double) l / (width - 1) * 65535.0 + 0.5);
        for(a = 0; a < width; ++a) {
          in[1] = floor((double) a / (width - 1) * 65535.0 + 0.5);
          for(b = 0; b < width; ++b)
          {
            in[2] = floor((double) b / (width - 1) * 65535.0 + 0.5);
            for(j = 0; j < 3; ++j)
              buf[a*size*3+b*+width*3+l*3+j] = in[j];
          }
        }
      }
      image = oyImage_Create( width,width*width, buf, OY_TYPE_123_16,
                              p, 0 );
      sprintf( comment, "CIE*Lab LUT with %d levels", width );
    } else if(strcmp(format,"slice") == 0)
    {
      if(!width)
        width = 17;
      buf = calloc(sizeof(uint16_t), size*width*3);

#pragma omp parallel for private(in,a,b,j)
      for(l = 0; l < width; ++l)
      {
        in[1] = floor((double) l / (width - 1) * 65535.0 + 0.5);
        for(a = 0; a < width; ++a) {
          in[0] = floor((double) a / (width - 1) * 65535.0 + 0.5);
          for(b = 0; b < width; ++b)
          {
            in[2] = floor((double) b / (width - 1) * 65535.0 + 0.5);
            for(j = 0; j < 3; ++j)
              buf[a*size*3+b*+width*3+l*3+j] = in[j];
          }
        }
      }
      image = oyImage_Create( width,width*width, buf, OY_TYPE_123_16,
                              p, 0 );
      sprintf( comment, "CIE*Lab slice with %d levels", width );
    } else
      WARNc1_S("format is not supported %s", format);

    error = oyImage_WritePPM( image, output, comment);
    if(error)
    {
      WARNc_S("Could not write to file");
    }
  }
  else
  {
                        printfHelp(argc, argv);
                        exit (0);
  }

  return error;
}


