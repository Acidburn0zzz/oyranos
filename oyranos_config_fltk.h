// generated by Fast Light User Interface Designer (fluid) version 1.0107

#ifndef oyranos_config_fltk_h
#define oyranos_config_fltk_h
#include <FL/Fl.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Pack.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_config.h"
#include "config.h"
#include "fl_i18n/fl_i18n.H"
#include <iostream>
#include <fstream>
#include <cstdio>
using namespace oyranos;
#define H_SPACING 10
#define V_SPACING 3
#define BUTTON_HEIGHT 25
#define BOX_WIDTH 250
#define SELECT_WIDTH 176
void setPolicy( int i );
const char* getPolicyName();
const char** getPolicies( int *count );
void selectDefaultProfile_callback( Fl_Widget* w, void* );
void showDefaultProfile_callback( Fl_Widget* w, void* );
void addPathCallback( Fl_Widget* w, void* );
void rmPathCallback( Fl_Widget* w, void* );
void selectBehaviourCallback( Fl_Widget* w, void* x );
void debug_me( );

class OyFlPack : public Fl_Pack {
public:
  OyFlPack(int x, int y, int w, int h , const char *t = 0) ;
private:
  void draw();
};

class BoxChoiceCombo : public Fl_Pack {
public:
  Fl_Box   *box;
  Fl_Choice*choice;
  BoxChoiceCombo( int x, int y, int w, int h ) ;
};

class Option : public OyFlPack {
public:
  Fl_Box   *box;
  Fl_Choice*choice;

  oyWIDGET option;
  oyWIDGET_TYPE type;
  int       i;
  Option( int x, int y, int w, int h, const char *name,
  oyWIDGET option_,
  oyWIDGET_TYPE type_,
  int choices_n,
  const char **choices,
  int current,
  const char *tooltip) ;
};

class ProfilePath : public Fl_Pack {
public:
  Fl_Box   *box;
  int       n;
  Fl_Button*button_add, *button_remove;
  ProfilePath( int x, int y, int w, int h, int num )
    ;
};
void createUI();
void updateUI();
void updateUIIdle(void*);
#include <FL/Fl_Double_Window.H>
extern Fl_Double_Window *top_group;
#include <FL/Fl_Pack.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tabs.H>
extern Fl_Tabs *top_tabs;
#include <FL/Fl_Group.H>
#include <FL/Fl_Choice.H>
extern Fl_Choice *policy_choice;
#include <FL/Fl_Scroll.H>
extern Fl_Scroll *paths_scroll;
#include <FL/Fl_Button.H>
extern Fl_Box *policy_box;
extern Fl_Menu_Item menu_policy_choice[];
void listWindow( Fl_Widget *w );
#endif
