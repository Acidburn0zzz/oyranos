/** @file oyObserver_s_.c

   [Template file inheritance graph]
   +-- Observer_s_.template.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/02/14
 */


#include "oyranos_object_internal.h"

#include "oyObserver_s.h"
#include "oyObserver_s_.h"

#include "oyObject_s.h"
#include "oyOption_s_.h"


/**
 *  @internal
 *  Function oyStructSignalForward_
 *  @memberof oyObserver_s
 *  @brief   observe all list members
 *
 *  This function is useful to forward signals and fill holes in a chain.
 *  Implements oyObserver_Signal_f.
 *
 *  @verbatim
    error = oyStruct_ObserverAdd( (oyStruct_s*)model, (oyStruct_s*)observer,
                                  0, oyStructSignalForward_ );
    @endverbatim
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/28 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
int      oyStructSignalForward_      ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data )
{
  int handled = 0;
  oyObserver_s * obs = observer;

  if(oy_debug_signals)
    WARNc6_S( "\n\t%s %s: %s[%d]->%s[%d]", _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );

  if(observer && observer->model &&
     observer->observer && observer->observer->type_ > oyOBJECT_NONE)
    handled = oyStruct_ObserverSignal( observer->observer,
                                       signal_type, signal_data );

  return handled;
}

/** @internal
 *  Function oyObserver_Copy_
 *  @memberof oyObserver_s
 *  @brief   real copy a Observer object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
oyObserver_s * oyObserver_Copy_
                                     ( oyObserver_s      * obj,
                                       oyObject_s          object )
{
  oyObserver_s * s = 0;
  int error = 0;

  if(!obj || !object)
    return s;

  s = oyObserver_New( object );
  error = !s;

  if(!error)
  {
    s->observer = obj->observer->copy( obj->observer, object );
    s->model = obj->model->copy( obj->model, object );
    s->user_data = obj->user_data->copy( obj->user_data, object );
    s->disable_ref = obj->disable_ref;
  }

  if(error)
    oyObserver_Release( &s );

  return s;
}

/** @internal
 *  Function oyStruct_ObserverListGet_
 *  @memberof oyObserver_s
 *  @brief   get the desired list of oyObserver_s'
 *
 *  @param[in]     obj                 the object to look in for the list
 *  @param[in]     reg                 the selector for the list
 *  @return                            the observers
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/02 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
oyStructList_s * oyStruct_ObserverListGet_(
                                       oyStruct_s        * obj,
                                       const char        * reg )
{
  oyOption_s * o = 0;
  oyOption_s_ * o_ = 0;
  int error = 0;
  oyStructList_s * list = 0;

  o = oyOptions_Find( obj->oy_->handles_, reg );

  if(!o)
  {
    list = oyStructList_New( 0 );
    o = oyOption_FromRegistration( reg, obj->oy_ );
    error = oyOption_StructMoveIn( o, (oyStruct_s**)&list );
    if(!error)
    {
      if(!obj->oy_->handles_)
        obj->oy_->handles_ = oyOptions_New( 0 );
      error = oyOptions_MoveIn( obj->oy_->handles_, &o, -1);
      o = oyOptions_Find( obj->oy_->handles_,
                          reg );
    }
  }

  o_ = oyOptionPriv_m(o);

  if(!error && o_ && o_->value_type == oyVAL_STRUCT && o_->value)
  {
    if(o_->value->oy_struct &&
       o_->value->oy_struct->type_ == oyOBJECT_STRUCT_LIST_S)
      list = (oyStructList_s*)o_->value->oy_struct;
    else
    {
      WARNcc3_S( obj, "%s: %s %s", _("found list of wrong type"),
                 reg,
                 oyStruct_TypeToText( o_->value->oy_struct ) );
    }

    oyOption_Release( &o );
  }
  return list;
}

/** @internal
 *  Function oyStruct_ObserverRemove_
 *  @memberof oyObserver_s
 *  @brief   remove a observer from the observer or model internal list
 *
 *  @param[in,out] list                the reference list
 *  @param[in]     obj                 comparision object
 *  @param[in]     observer            1 - remove observer; 0 - remove model
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/02 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
int        oyStruct_ObserverRemove_  ( oyStructList_s    * list,
                                       oyStruct_s        * obj,
                                       int                 observer,
                                       oyObserver_Signal_f signalFunc )
{
  int error = 0;
  oyObserver_s * obs = 0;
  int n,i;
  if(list)
  {
    n = oyStructList_Count( list );
    for(i = n-1; i >= 0; --i)
    {
      obs = (oyObserver_s*) oyStructList_GetType( list,
                                                  i, oyOBJECT_OBSERVER_S );

      if(obs &&
         ((observer && obj == obs->observer) ||
          (!observer && obj == obs->model)) &&
          (!signalFunc || obs->signal == signalFunc))
        oyStructList_ReleaseAt( list, i );
    }
  }
  return error;
}
