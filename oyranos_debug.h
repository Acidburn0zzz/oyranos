/** @file oyranos_debug.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2005-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    internal helpers
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/31/01
 */


/* Dont use in non Oyranos projects. */


#ifndef OYRANOS_DEBUG_H
#define OYRANOS_DEBUG_H


#include <time.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** debugging variable - set 0 to off (default), set 1 to switch debugging on */
extern int oy_debug;
extern clock_t oyranos_clock_;
extern int level_PROG;

#if defined(OY_CONFIG_H)

/* Debug system */
/*#define DEBUG 1*/

#define DBG_UHR_ (double)clock()/(double)CLOCKS_PER_SEC

#if DEBUG == 1
#define DBG oyMessageFunc_p(oyMSG_DBG,0,"%s:%d",__FILE__,__LINE__);
#define DBG_S(txt) oyMessageFunc_p(oyMSG_DBG,0,"%s:%d %s",__FILE__,__LINE__,txt);
#define DBG1_S(format,arg) oyMessageFunc_p( oyMSG_DBG,0,"%s:%d " format,__FILE__,__LINE__,arg);
#define DBG2_S(format,arg,arg2) oyMessageFunc_p( oyMSG_DBG,0,"%s:%d " format,__FILE__,__LINE__,arg,arg2);
#define DBG3_S(format,arg,arg2,arg3) oyMessageFunc_p( oyMSG_DBG,0,"%s:%d " format,__FILE__,__LINE__,arg,arg2,arg3);
#define DBG4_S(format,arg,arg2,arg3,arg4) oyMessageFunc_p( oyMSG_DBG,0,"%s:%d " format,__FILE__,__LINE__,arg,arg2,arg3,arg4);
#define DBG5_S(format,arg,arg2,arg3,arg4,arg5) oyMessageFunc_p( oyMSG_DBG,0,"%s:%d " format,__FILE__,__LINE__,arg,arg2,arg3,arg4,arg5);
#define DBG6_S(format,arg,arg2,arg3,arg4,arg5,arg6) oyMessageFunc_p( oyMSG_DBG,0,"%s:%d " format,__FILE__,__LINE__,arg,arg2,arg3,arg4,arg5,arg6);
#define DBG_V(var) oyMessageFunc_p(oyMSG_DBG,0,"%s:%d " #var ": %d",__FILE__,__LINE__,(int)var);
#define DBG_MEM DBG
#else
#define DBG
#define DBG_S(txt)
#define DBG_V(var)
#define DBG1_S(a,b)
#define DBG2_S(a,b,c)
#define DBG3_S(a,b,c,d)
#define DBG4_S(a,b,c,d,e)
#define DBG5_S(a,b,c,d,e,f)
#define DBG6_S(a,b,c,d,e,f,h)
#define DBG7_S(a,b,c,d,e,f,h,i)
#define DBG8_S(a,b,c,d,e,f,h,i,j)
#define DBG9_S(a,b,c,d,e,f,h,i,j,k)
#define DBG_MEM
#endif

#define DBG_NUM DBG
#define DBG_NUM_S DBG_S
#define DBG_NUM1_S DBG1_S
#define DBG_NUM2_S DBG2_S
#define DBG_NUM3_S DBG3_S
#define DBG_NUM4_S DBG4_S
#define DBG_NUM_V DBG_V

#define DBG_PROG DBG
#define DBG_PROG_S DBG_S
#define DBG_PROG1_S DBG1_S
#define DBG_PROG2_S DBG2_S
#define DBG_PROG3_S DBG3_S
#define DBG_PROG4_S DBG4_S
#define DBG_PROG5_S DBG5_S
#define DBG_PROG6_S DBG6_S
#define DBG_PROG_V DBG_V
#if DEBUG == 1
#define DBG_PROG_START oyMessageFunc_p( oyMSG_DBG,0,"Start: %s:%d",__FILE__,__LINE__ );
#define DBG_PROG_ENDE  oyMessageFunc_p( oyMSG_DBG,0,"  End: %s:%d",__FILE__,__LINE__ );
#else
#define DBG_PROG_START DBG
#define DBG_PROG_ENDE DBG
#endif

#endif /* defined OY_CONFIG_H */

#define WARNcc(ptr)    oyMessageFunc_p( oyMSG_WARN,ptr,"%s:%d",__FILE__,__LINE__ ); 
#define WARNcc_S(ptr,txt)   oyMessageFunc_p( oyMSG_WARN,ptr,"%s:%d %s",__FILE__,__LINE__,txt);
#define WARNcc1_S(ptr,format,arg) oyMessageFunc_p( oyMSG_WARN,ptr,"%s:%d " format,__FILE__,__LINE__,arg);
#define WARNcc2_S(ptr,format,arg,arg2) oyMessageFunc_p( oyMSG_WARN,ptr,"%s:%d " format,__FILE__,__LINE__,arg,arg2);
#define WARNcc3_S(ptr,format,arg,arg2,arg3) oyMessageFunc_p( oyMSG_WARN,ptr,"%s:%d " format,__FILE__,__LINE__,arg,arg2,arg3);
#define WARNcc4_S(ptr,format,arg,arg2,arg3,arg4) oyMessageFunc_p( oyMSG_WARN,ptr,"%s:%d " format,__FILE__,__LINE__,arg,arg2,arg3,arg4);

#define WARNc          oyMessageFunc_p( oyMSG_WARN,0,"%s:%d",__FILE__,__LINE__ ); 
#define WARNc_S(txt)   oyMessageFunc_p( oyMSG_WARN,0,"%s:%d %s",__FILE__,__LINE__,txt);
#define WARNc1_S(format,arg) oyMessageFunc_p( oyMSG_WARN,0,"%s:%d " format,__FILE__,__LINE__,arg);
#define WARNc2_S(format,arg,arg2) oyMessageFunc_p( oyMSG_WARN,0,"%s:%d " format,__FILE__,__LINE__,arg,arg2);
#define WARNc3_S(format,arg,arg2,arg3) oyMessageFunc_p( oyMSG_WARN,0,"%s:%d " format,__FILE__,__LINE__,arg,arg2,arg3);
#define WARNc4_S(format,arg,arg2,arg3,arg4) oyMessageFunc_p( oyMSG_WARN,0,"%s:%d " format,__FILE__,__LINE__,arg,arg2,arg3,arg4);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OYRANOS_DEBUG_H */
